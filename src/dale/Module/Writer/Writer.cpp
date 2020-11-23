#include "Writer.h"

#include <map>
#include <set>
#include <string>
#include <vector>

#include "../../Serialise/Serialise.h"
#include "../../Utils/Utils.h"
#include "../../llvmUtils/llvmUtils.h"
#include "Config.h"

namespace dale {
namespace Module {
Writer::Writer(
    std::string module_name, dale::Context *ctx, llvm::Module *mod,
    PassManager *pm, std::set<std::string> *included_once_tags,
    std::map<std::string, std::vector<std::string> *> *included_modules,
    bool cto) {
    this->module_name = module_name;
    this->ctx = ctx;
    this->mod = mod;
    this->pm = pm;
    this->included_once_tags = included_once_tags;
    this->included_modules = included_modules;
    this->cto = cto;

    this->module_prefix.append(module_name);
}

Writer::~Writer() {}

bool Writer::writeBitcode(const char *suffix) {
    std::string bc_path(module_prefix);
    bc_path.append(suffix).append(".bc");

    FILE *bc = fopen(bc_path.c_str(), "w");
    if (!bc) {
        error("unable to open %s for writing", bc_path.c_str(), true);
    }

    llvm::raw_fd_ostream bc_out(fileno(bc), false);

#if D_LLVM_VERSION_ORD <= 35
    pm->run(*mod);
    llvm::WriteBitcodeToFile(mod, bc_out);
#elif D_LLVM_VERSION_ORD <= 60
    std::unique_ptr<llvm::Module> module_ptr(llvm::CloneModule(mod));

    pm->run(*module_ptr);
    llvm::WriteBitcodeToFile(module_ptr.get(), bc_out);
#else
    std::unique_ptr<llvm::Module> module_ptr(llvm::CloneModule(*mod));
    llvm::Module &mod_ref = static_cast<llvm::Module&>(*module_ptr);

    pm->run(*module_ptr);
    llvm::WriteBitcodeToFile(mod_ref, bc_out);
#endif

    bc_out.flush();
    int res = fflush(bc);
    if (res != 0) {
        error("unable to flush %s", bc_path.c_str(), true);
    }
    res = fclose(bc);
    if (res != 0) {
        error("unable to close %s", bc_path.c_str(), true);
    }

    return true;
}

bool Writer::writeSharedObject(const char *suffix) {
    std::string asm_path(module_prefix);
    asm_path.append(suffix);

    std::string bc_path(asm_path);
    bc_path.append(".bc");

    asm_path.append(".s");

    std::string cmd;
    cmd.append(LLVM_BIN_DIR "/llc -relocation-model=pic -filetype=asm ")
        .append(bc_path)
        .append(" -o ")
        .append(asm_path);

    int res = system(cmd.c_str());
    assert(!res && "unable to assemble bitcode");
    _unused(res);

    std::string lib_path(module_prefix);
    lib_path.append(suffix);
    lib_path.append(".so");

    cmd.clear();
    cmd.append(DALE_CC " -shared ");
    if (!strcmp(SYSTEM_NAME, "Darwin")) {
        cmd.append(
            " -undefined dynamic_lookup"
            " -install_name @rpath/");
        cmd.append(lib_path);
        cmd.append(" ");
    }
    cmd.append(asm_path).append(" -o ").append(lib_path);

    res = system(cmd.c_str());
    assert(!res && "unable to make library");

    res = remove(asm_path.c_str());
    assert(!res && "unable to remove temporary assembly file");

    return true;
}

bool Writer::writeContext() {
    ctx->removeDeserialised();

    std::string ctx_module_path(module_prefix);
    module_prefix.append(".dtm");

    FILE *mod_data = fopen(module_prefix.c_str(), "w");
    if (!mod_data) {
        error("unable to open %s for writing", module_prefix.c_str(),
              true);
    }

    assert(mod_data && "cannot create module file");
    serialise(mod_data, ctx);
    serialise(mod_data, included_once_tags);
    serialise(mod_data, included_modules);
    serialise(mod_data, cto);
    serialise(mod_data, &dale_typemap);

    int res = fflush(mod_data);
    if (res != 0) {
        error("unable to flush %s", module_prefix.c_str(), true);
    }
    res = fclose(mod_data);
    if (res != 0) {
        error("unable to close %s", module_prefix.c_str(), true);
    }

    return true;
}

bool Writer::run() {
    writeBitcode("");
    writeSharedObject("");
#if D_LLVM_VERSION_ORD <= 35
    ctx->regetPointers(mod);
#endif
    ctx->eraseLLVMMacrosAndCTOFunctions();
    writeBitcode("-nomacros");
    writeSharedObject("-nomacros");
    writeContext();
    return true;
}
}
}
