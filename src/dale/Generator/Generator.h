#ifndef DALE_GENERATOR
#define DALE_GENERATOR

#include <vector>
#include <string>

namespace llvm {
    class Linker;
    class Module;
}

namespace dale
{
enum Produce
{
    IR,
    BitCode,
    ASM
};

/*! Generator

    Previously, this class contained nearly everything.  Now, it
    serves as a very simple frontend to the rest of the compiler, and
    probably shouldn't be a class at all.
*/
class Generator
{
public:
    Generator();
    ~Generator();
    /*! Compile a set of files into a specific output format.
     *  @file_paths The Dale files to compile.
     *  @bc_file_paths The LLVM bitcode files to compile.
     *  @compile_lib_paths Paths to libraries for use at compile time.
     *  @include_paths Paths to inspect for include files.
     *  @module_paths Paths to modules to import.
     *  @static_module_names The names of modules that should be
     *                       linked statically.
     *  @cto_module_names The names of modules that will only be used
     *                    at compile time.
     *  @module_name If a module is being compiled, the name of the module.
     *  @debug Enable debug mode.
     *  @produce The output format (see Produce).
     *  @optlevel The optimisation level (1-4).
     *  @remove_macros Whether macro definitions should be removed from the
     *                 final product.
     *  @no_common Whether common declarations should not be included.
     *  @no_dale_stdlib Whether the drt library should not be imported.
     *  @static_mods_all Whether all modules should be linked
     *                   statically.
     *  @enable_cto Whether the module being compiled is a
     *              compile-time-only module (equivalent to (attr
     *              cto)).
     *  @shared_object_paths The paths to shared objects against which
     *                       the output file has to be linked
     *                       (populated by this function).
     *  @output_file The compilation output file.
     */
    int run(std::vector<const char *> *file_paths,
            std::vector<const char *> *bc_file_paths,
            std::vector<const char *> *compile_lib_paths,
            std::vector<const char *> *include_paths,
            std::vector<const char *> *module_paths,
            std::vector<const char *> *static_module_names,
            std::vector<const char *> *cto_module_names,
            const char *module_name,
            int debug,
            int produce,
            int optlevel,
            int remove_macros,
            int no_common,
            int no_dale_stdlib,
            int static_mods_all,
            int enable_cto,
            std::vector<std::string> *shared_object_paths,
            FILE *output_file);

};
}

#endif
