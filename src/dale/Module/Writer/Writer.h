#ifndef DALE_MODULE_WRITER
#define DALE_MODULE_WRITER

#include <map>
#include <set>
#include <string>
#include <vector>

#include "../../Context/Context.h"
#include "../../Utils/Utils.h"
#include "../../llvmUtils/llvmUtils.h"
#include "../../llvm_Module.h"

namespace dale {
namespace Module {
/*! Writer

    A class for writing Dale modules to disk.
*/
class Writer {
    private:
    /*! The name of the module. */
    std::string module_name;
    /*! The path prefix for the module. */
    std::string module_prefix;
    /*! The context for the module. */
    dale::Context *ctx;
    /*! The LLVM module for the module. */
    llvm::Module *mod;
    /*! The LLVM pass manager for the module. */
    PassManager *pm;
    /*! The once tags for the module. */
    std::set<std::string> *included_once_tags;
    /*! The modules included by way of this module. */
    std::map<std::string, std::vector<std::string> *> *included_modules;
    /*! Whether the module is a compile-time-only module. */
    bool cto;
    /*! Write the module's bitcode to disk.
     *  @param suffix A string to append to the module name. */
    bool writeBitcode(const char *suffix);
    /*! Write the module's shared object to disk.
     *  @param suffix A string to append to the module name. */
    bool writeSharedObject(const char *suffix);
    /*! Write the module's context to disk. */
    bool writeContext();

    public:
    /*! The standard constructor.
     *  @param module_name The module name.
     *  @param ctx The module context.
     *  @param mod The LLVM module.
     *  @param pm The LLVM pass manager.
     *  @param included_once_tags The once tags for the module.
     *  @param included_modules The modules included by way of the
     * module.
     *  @param cto Whether the module is a compile-time only module.
     *
     *  This does not take ownership of any of its arguments.
     */
    Writer(std::string module_name, dale::Context *ctx,
           llvm::Module *mod, PassManager *pm,
           std::set<std::string> *included_once_tags,
           std::map<std::string, std::vector<std::string> *>
               *included_modules,
           bool cto);
    ~Writer();

    /*! Write the standard module artifacts to disk. */
    bool run();
};
}
}

#endif
