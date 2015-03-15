#include "Reader.h"
#include "Config.h"

#include "llvm/Support/DynamicLibrary.h"
#include "llvm/Support/Host.h"
#include "llvm/Support/Signals.h"
#if D_LLVM_VERSION_MINOR <= 4
#include "llvm/Support/system_error.h"
#else
#include "llvm/Object/Error.h"
#endif
#include "../../llvm_LLVMContext.h"
#include "../../llvm_Module.h"
#include "llvm/LinkAllPasses.h"
#include "../../llvm_Linker.h"
#include "../../llvm_Function.h"
#include "../../llvm_CallingConv.h"
#include "../../llvm_AssemblyPrintModulePass.h"
#include "llvm/ExecutionEngine/ExecutionEngine.h"
#include "llvm/ExecutionEngine/JIT.h"
#include "llvm/ExecutionEngine/Interpreter.h"
#include "../../llvm_ValueSymbolTable.h"
#include "llvm/ADT/StringRef.h"
#include "llvm/PassManager.h"
#include "llvm/Analysis/Passes.h"
#include "../../llvm_AnalysisVerifier.h"
#include "llvm/Transforms/Scalar.h"
#include "llvm/Transforms/IPO.h"
#include "llvm/Transforms/IPO/PassManagerBuilder.h"
#include "llvm/Bitcode/ReaderWriter.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/ADT/Triple.h"
#include "llvm/Support/TargetRegistry.h"
#include "llvm/Target/TargetMachine.h"
#include "llvm/Support/TargetSelect.h"
#include "llvm/CodeGen/LinkAllAsmWriterComponents.h"
#include "llvm/CodeGen/LinkAllCodegenComponents.h"
#include "llvm/Support/FileUtilities.h"
#include "llvm/Support/MemoryBuffer.h"
#include "llvm/Support/FormattedStream.h"
#include "llvm/Support/Debug.h"
#include "llvm/Transforms/Utils/Cloning.h"

#if D_LLVM_VERSION_MINOR >= 3
#include "llvm/Support/SourceMgr.h"
#include "llvm/IRReader/IRReader.h"
#endif

#include "../../Serialise/Serialise.h"
#include "../../Utils/Utils.h"

#include <sys/types.h>
#include <sys/stat.h>

#include <unistd.h>

namespace dale
{
namespace Module
{
Reader::Reader(std::vector<const char*> *module_directory_paths,
               std::vector<std::string> *so_paths,
               std::vector<const char*> *include_directory_paths)
{
    this->module_directory_paths = module_directory_paths;
    this->include_directory_paths = include_directory_paths;
    this->so_paths = so_paths;
}

Reader::~Reader()
{
}

llvm::Module *
Reader::loadModule(std::string *path, bool materialize)
{
#if D_LLVM_VERSION_MINOR <= 4
    llvm::OwningPtr<llvm::MemoryBuffer> buffer;
#endif

#if D_LLVM_VERSION_MINOR <= 3
    const llvm::sys::Path sys_path(*path);
    llvm::MemoryBuffer::getFileOrSTDIN(sys_path.c_str(), buffer);
#elif D_LLVM_VERSION_MINOR <= 4
    llvm::MemoryBuffer::getFileOrSTDIN(*path, buffer);
#else
    llvm::ErrorOr<std::unique_ptr<llvm::MemoryBuffer>> eo =
        llvm::MemoryBuffer::getFileOrSTDIN(*path);
    assert(!eo.getError() && "cannot load module");
    std::unique_ptr<llvm::MemoryBuffer> buffer = std::move(eo.get());
#endif

#if D_LLVM_VERSION_MINOR <= 4
    std::string errmsg;
    llvm::Module *module = 
        llvm::getLazyBitcodeModule(buffer.get(),
                                   llvm::getGlobalContext(),
                                   &errmsg);
#else
    std::string errmsg;
    llvm::ErrorOr<llvm::Module *> err =
        llvm::getLazyBitcodeModule(buffer.get(),
                                   llvm::getGlobalContext());
    llvm::Module *module = err.get();
    if (!module) {
        errmsg = err.getError().message();
    }
    buffer.release();
#endif

    assert(module && "cannot load module");

    std::string ma_error;
#if D_LLVM_VERSION_MINOR <= 4
    bool ma = module->MaterializeAll(&ma_error);
#else
    std::error_code ec = module->materializeAllPermanently();
    bool ma = (bool) ec;
    if (ec) {
        ma_error = ec.message();
    }
#endif
    assert(!ma && "failed to materialize module");
    _unused(ma);

    return module;
}

bool
Reader::addLib(const char *lib_path,
               int add_to_so_paths,
               int add_nm_to_so_paths)
{
    std::string temp;

    bool res =
        llvm::sys::DynamicLibrary::LoadLibraryPermanently(
            lib_path,
            &temp
        );
    if (res) {
        /* If this is Darwin, and .so is at the end of lib_path, try
         * replacing it with dylib. */
        if (!strcmp(SYSTEM_NAME, "Darwin")) {
            int len = strlen(lib_path);
            if ((len >= 3) && !strcmp((lib_path + len - 3), ".so")) {
                char lib_path_dylib[256];
                strcpy(lib_path_dylib, lib_path);
                strcpy((lib_path_dylib + len - 2),
                       "dylib");
                bool res =
                    llvm::sys::DynamicLibrary::LoadLibraryPermanently(
                        lib_path_dylib,
                        &temp
                    );
                if (!res) {
                    /* This only generates SOs - the dylib stuff
                     * is only for internal Mac libraries, not for
                     * dale modules. */
                    goto done;
                }
            }
        }
        assert(true && "unable to load library");
        return false;
    } else {
        if (add_nm_to_so_paths) {
            std::string temp;
            temp.append(lib_path);
            temp.erase((temp.size() - 3), 3);
            temp.append("-nomacros.so");
            so_paths->push_back(temp);
        } else if (add_to_so_paths) {
            std::string temp;
            temp.append(lib_path);
            so_paths->push_back(temp);
        }
    }
done:

    return !res;
}



bool
Reader::run(Context *ctx, llvm::Module *mod, Node *n, const char *my_module_name,
    std::vector<const char*> *import_forms)
{
    std::vector<const char*> temp;
    if (import_forms == NULL) {
        import_forms = &temp;
    }

    std::string real_module_name;
    if (!(strstr(my_module_name, "lib") == my_module_name)) {
        real_module_name.append("lib");
    }
    real_module_name.append(my_module_name);

    /* If the module has already been added, then skip it. */

    if (included_modules.find(real_module_name)
            != included_modules.end()) {
        return 1;
    }

    /* Look for the module in the current directory, then the
     * specified directories, then the modules directory. */

    std::string tmn(real_module_name);
    std::string tmn2(real_module_name);
    std::string tmn4;
    const char *bc_suffix    = ".bc";
    const char *bc_nm_suffix = "-nomacros.bc";
    const char *so_suffix    = ".so";
    tmn.append(".dtm");
    FILE *test = fopen(tmn.c_str(), "r");
    if (!test) {
        for (std::vector<const char*>::iterator b = module_directory_paths->begin(),
                                                e = module_directory_paths->end();
                b != e;
                ++b) {
            std::string whole_name(*b);
            whole_name.append("/")
            .append(real_module_name)
            .append(".dtm");
            test = fopen(whole_name.c_str(), "r");
            if (test) {
                tmn2 = std::string(*b);
                tmn2.append("/")
                .append(real_module_name)
                .append(bc_suffix);

                tmn4 = std::string(*b);
                tmn4.append("/")
                .append(real_module_name)
                .append(so_suffix);
                break;
            }
        }

        if (!test) {
            std::string whole_name(DALE_MODULE_PATH);
            whole_name.append("/")
            .append(real_module_name)
            .append(".dtm");
            test = fopen(whole_name.c_str(), "r");
            if (!test) {
                Error *e = new Error(
                    ErrorInst::Generator::FileError,
                    n,
                    whole_name.c_str(),
                    strerror(errno)
                );
                ctx->er->addError(e);
                return 0;
            }
            tmn2 = std::string(DALE_MODULE_PATH);
            tmn2.append("/")
            .append(real_module_name)
            .append(bc_suffix);

            tmn4 = std::string(DALE_MODULE_PATH);
            tmn4.append("/")
            .append(real_module_name)
            .append(so_suffix);
        }
    } else {
        tmn2.append(bc_suffix);

        char *cwd = getcwd(NULL, 0);
        tmn4 = std::string(cwd);
        free(cwd);

        tmn4.append("/")
        .append(real_module_name)
        .append(so_suffix);
    }

    Context *mynewcontext = new Context(ctx->er, ctx->nt, ctx->tr);

    int fd = fileno(test);
    struct stat buf;
    int fstat_res = fstat(fd, &buf);
    assert(!fstat_res && "unable to fstat file");
    _unused(fstat_res);
    int size = buf.st_size;
    char *data = (char*) malloc(size);
    char *original_data = data;
    size_t res = fread(data, 1, size, test);
    assert((res == (size_t) size) && "unable to read module file");
    _unused(res);

    data = deserialise(ctx->tr, data, mynewcontext);
    std::set<std::string> temponcetags;
    data = deserialise(ctx->tr, data, &temponcetags);
    std::set<std::string> tempmodules;
    data = deserialise(ctx->tr, data, &tempmodules);
    int my_cto;
    data = deserialise(ctx->tr, data, &my_cto);
    std::map<std::string, std::string> new_typemap;
    data = deserialise(ctx->tr, data, &new_typemap);
    for (std::map<std::string, std::string>::iterator
            b = new_typemap.begin(),
            e = new_typemap.end();
            b != e;
            ++b) {
        std::string x = (*b).first;
        std::string y = (*b).second;
        addTypeMapEntry(x.c_str(), y.c_str());
    }
    free(original_data);

    std::string module_path(tmn2);
    std::string module_path_nomacros(tmn2);

    module_path_nomacros.replace(module_path_nomacros.find(".bc"), 
                                 3, bc_nm_suffix);

    llvm::Module *new_module = loadModule(&module_path, false);

    included_modules.insert(real_module_name);

    /* Load each dependent module, before loading this one. */

    for (std::set<std::string>::iterator b = tempmodules.begin(),
            e = tempmodules.end();
            b != e;
            ++b) {
        int res = run(ctx, mod, n, (*b).c_str(), NULL);
        if (!res) {
            return 0;
        }
    }

    if (my_cto) {
        cto_module_names.insert(real_module_name);
    }

    int add_to_so_paths =
        (cto_module_names.find(std::string(real_module_name)) ==
         cto_module_names.end());

    /* Never add to so_paths if you are making a module (it's
     * pointless - it only matters when you are linking an
     * executable). */
    res = addLib(tmn4.c_str(), 0, add_to_so_paths);
    assert(res && "unable to add library");

    /* Get the union of temponcetags and included_once_tags.
     * Remove from the module any structs/enums that have a once
     * tag from this set, remove the bodies of any
     * functions/variables that have a once tag from this set, and
     * remove from the context any structs/enums that have a once
     * tag from this set (the functions and variables can stay,
     * they won't cause any trouble.) todo: comment doesn't make
     * any sense given what's below. */

    std::set<std::string> common;
    std::set_union(included_once_tags.begin(),
                   included_once_tags.end(),
                   temponcetags.begin(),
                   temponcetags.end(),
                   std::insert_iterator<std::set<std::string> >(
                       common,
                       common.end()));

    mynewcontext->eraseOnceForms(&common, new_module);

    std::set<std::string> current;
    std::merge(included_once_tags.begin(),
               included_once_tags.end(),
               temponcetags.begin(),
               temponcetags.end(),
               std::insert_iterator<std::set<std::string> >(
                   current,
                   current.end()));
    included_once_tags.erase(included_once_tags.begin(),
                              included_once_tags.end());
    included_once_tags.insert(current.begin(), current.end());

    /* Add the module name to the set of included modules. */
    included_modules.insert(real_module_name);

    dtm_modules.insert(std::pair<std::string, llvm::Module *>(
                            std::string(real_module_name),
                            new_module
                        ));

    dtm_nm_modules.insert(std::pair<std::string, std::string>(
                               std::string(real_module_name), 
                               module_path_nomacros
                           ));

    /* Remove from mynewctx things not mentioned in import_forms,
     * but only if at least one import form has been specified.
     * */

    if (import_forms->size() > 0) {
        std::set<std::string> forms_set;
        for (std::vector<const char*>::iterator
                b = import_forms->begin(),
                e = import_forms->end();
                b != e;
                ++b) {
            forms_set.insert(std::string(*b));
        }

        std::set<std::string> found;
        mynewcontext->removeUnneeded(&forms_set, &found);

        std::set<std::string> not_found;
        set_difference(forms_set.begin(), forms_set.end(),
                       found.begin(),     found.end(),
                       std::insert_iterator<std::set<std::string> >(
                           not_found,
                           not_found.end()));
        if (not_found.size() > 0) {
            std::string all;
            for (std::set<std::string>::iterator b = not_found.begin(),
                    e = not_found.end();
                    b != e;
                    ++b) {
                all.append(*b).append(", ");
            }
            all.erase(all.size() - 2, all.size() - 1);
            std::string temp_mod_name(real_module_name);
            // Get rid of "^lib".
            temp_mod_name.replace(0, 3, "");
            Error *e = new Error(
                ErrorInst::Generator::ModuleDoesNotProvideForms,
                n,
                temp_mod_name.c_str(),
                all.c_str()
            );
            ctx->er->addError(e);
            return 0;
        }

    }

    ctx->merge(mynewcontext);
    ctx->regetPointersForNewModule(mod);
    ctx->relink();

    return 1;
}
}
}
