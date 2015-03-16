#ifndef DALE_MODULE_READER
#define DALE_MODULE_READER

#include "../../Context/Context.h"
#include "llvm/Bitcode/ReaderWriter.h"
#include "../../llvm_Module.h"

#include <string>

namespace dale
{
namespace Module
{
/*! Reader

    A class for reading Dale modules from disk, and maintaining the
    list of processed modules.
*/
class Reader
{
private:
    std::vector<const char*> *module_directory_paths; // mod_paths
    std::set<std::string> cto_module_names; // cto_modules

public:
    std::vector<std::string> *so_paths;
    std::set<std::string> included_modules;
    std::set<std::string> included_once_tags;
    std::vector<const char*> *include_directory_paths;
    std::map<std::string, llvm::Module*> dtm_modules;
    std::map<std::string, std::string> dtm_nm_modules;

    /*! Construct a new Module::Reader.
     *  @param module_directory_paths Module search paths.
     *  @param so_paths Shared object paths.
     *
     *  This does not take ownership of any of its arguments.
     */
    Reader(std::vector<const char*> *module_directory_paths,
           std::vector<std::string> *so_paths,
           std::vector<const char*> *include_directory_paths);
    ~Reader();

    /*! Load the module from the specified path.
     *  @param path The path to the module.
     */
    llvm::Module *loadModule(std::string *path);
    /*! Load a dynamic library.
     *  @param path The path to the library.
     *  @param add_to_so_paths Whether the dynamic library should be
     *                         added to the shared object path list.
     *  @param add_nm_to_so_paths Whether the no-macros version of
     *                            the library should be added to the
     *                            shared object path list.
     */
    bool addDynamicLibrary(const char *path, bool add_to_so_paths,
                           bool add_nm_to_so_paths);
    /*! Load and read a module.
     *  @param ctx The current context.
     *  @param mod The current LLVM module.
     *  @param n The reference node for errors.
     *  @param module_name The module name.
     *  @param import_forms The forms to be imported from the module.
     */
    bool run(Context *ctx, llvm::Module *mod, Node *n,
             const char *module_name,
             std::vector<const char*> *import_forms);
};
}
}

#endif
