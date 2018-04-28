#ifndef DALE_MODULE_READER
#define DALE_MODULE_READER

#include <map>
#include <set>
#include <string>
#include <vector>

#include "../../Context/Context.h"
#include "../../llvm_Linker.h"
#include "../../llvm_Module.h"

namespace dale {
namespace Module {
/*! Reader

    A class for reading Dale modules from disk, and maintaining the
    list of processed modules.
*/
class Reader {
    private:
    char *cwd;
    std::set<std::string> static_module_names;
    bool static_modules_all;
    bool remove_macros;
    std::set<std::string> cto_module_names;
    /*! Find the specified module.
     *  @param lib_module_name The name of the module.
     *  @param fh Storage for the file pointer.
     *  @param prefix The path prefix for the module.
     *
     *  This will populate fh and prefix on success.
     */
    bool findModule(Context *ctx, Node *n, std::string *lib_module_name,
                    FILE **fh, std::string *prefix);

    public:
    std::vector<std::string> *so_paths;
    std::vector<const char *> module_directory_paths;
    std::map<std::string, std::vector<std::string> *> included_modules;
    std::set<std::string> included_once_tags;
    std::vector<const char *> include_directory_paths;
    std::map<std::string, llvm::Module *> dtm_modules;
    std::map<std::string, std::string> dtm_nm_modules;

    /*! Construct a new Module::Reader.
     *  @param module_directory_paths Module search paths.
     *  @param so_paths Shared object paths.
     *
     *  This does not take ownership of any of its arguments.
     */
    Reader(std::vector<const char *> *module_directory_paths,
           std::vector<std::string> *so_paths,
           std::vector<const char *> *include_directory_paths,
           std::vector<const char *> *static_module_names,
           bool static_modules_all, bool remove_macros);
    ~Reader();

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
     *  @param linker The current LLVM linker.
     *  @param mod The current LLVM module.
     *  @param n The reference node for errors.
     *  @param module_name The module name.
     *  @param import_forms The forms to be imported from the module.
     */
    bool run(Context *ctx, llvm::Linker *linker, llvm::Module *mod,
             Node *n, const char *module_name,
             std::vector<const char *> *import_forms);
};
}
}

#endif
