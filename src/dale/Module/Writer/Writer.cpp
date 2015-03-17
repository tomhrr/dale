#include "Writer.h"
#include "Config.h"

#include "../../Serialise/Serialise.h"
#include "../../Utils/Utils.h"

namespace dale
{
namespace Module
{
Writer::Writer(std::string module_name, dale::Context *ctx,
               llvm::Module *mod, llvm::PassManager *pm,
               std::set<std::string> *included_once_tags,
               std::set<std::string> *included_modules,
               bool cto)
{
    this->module_name = module_name;
    this->ctx = ctx;
    this->mod = mod;
    this->pm = pm;
    this->included_once_tags = included_once_tags;
    this->included_modules = included_modules;
    this->cto = cto;

    this->module_prefix.append(module_name);
}

Writer::~Writer()
{
}

bool
Writer::writeBitcode(const char *suffix)
{
    std::string bc_path(module_prefix);
    bc_path.append(suffix)
           .append(".bc");

    FILE *bc = fopen(bc_path.c_str(), "w");
    if (!bc) {
        perror("Cannot create module bitcode file.");
        return false;
    }
    llvm::raw_fd_ostream bc_out(fileno(bc), false);
    pm->run(*mod);
    llvm::WriteBitcodeToFile(mod, bc_out);
    bc_out.flush();
    fflush(bc);
    fclose(bc);

    return true;
}

bool
Writer::writeSharedObject(const char *suffix)
{
    std::string asm_path(module_prefix);
    asm_path.append(suffix);

    std::string bc_path(asm_path);
    asm_path.append(".s");
    bc_path.append(".bc");

    std::string cmd;
    cmd.append(LLVM_BIN_DIR "/llc -relocation-model=pic ")
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
    cmd.append("cc -shared ")
       .append(asm_path)
       .append(" -o ")
       .append(lib_path);

    res = system(cmd.c_str());
    assert(!res && "unable to make library");

    res = remove(asm_path.c_str());
    assert(!res && "unable to remove temporary assembly file");

    return true;
}

bool
Writer::writeContext()
{
    ctx->removeDeserialised();

    std::string ctx_module_path(module_prefix);
    module_prefix.append(".dtm");

    FILE *mod_data = fopen(module_prefix.c_str(), "w");
    assert(mod_data && "cannot create module file");
    serialise(mod_data, ctx);
    serialise(mod_data, included_once_tags);
    serialise(mod_data, included_modules);
    serialise(mod_data, cto);
    serialise(mod_data, &dale_typemap);

    fflush(mod_data);
    fclose(mod_data);

    return true;
}

bool
Writer::run()
{
    writeBitcode("");
    writeSharedObject("");
    ctx->regetPointers(mod);
    ctx->eraseLLVMMacrosAndCTOFunctions();
    writeBitcode("-nomacros");
    writeSharedObject("-nomacros");
    writeContext();
    return true;
}
}
}
