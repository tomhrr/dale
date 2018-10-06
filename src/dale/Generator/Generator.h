#ifndef DALE_GENERATOR
#define DALE_GENERATOR

#include <string>
#include <vector>

namespace llvm {
class Linker;
class Module;
}

namespace dale {
enum Produce { IR, BitCode, ASM };

/*! Generator

    Previously, this class contained nearly everything.  Now, it
    serves as a very simple frontend to the rest of the compiler, and
    probably shouldn't be a class at all.
*/
class Generator {
    public:
    Generator();
    ~Generator();
    /*! Compile a set of files into a specific output format.
     *  @param file_paths The Dale files to compile.
     *  @param bc_file_paths The LLVM bitcode files to compile.
     *  @param compile_lib_paths Paths to libraries for use at compile
     * time.
     *  @param include_paths Paths to inspect for include files.
     *  @param module_paths Paths to modules to import.
     *  @param static_module_names The names of modules that should be
     *                             linked statically.
     *  @param module_name If a module is being compiled, the name of
     * the
     *                     module.
     *  @param debug Enable debug mode.
     *  @param produce The output format (see Produce).
     *  @param optlevel The optimisation level (1-4).
     *  @param remove_macros Whether macro definitions should be removed
     * from
     *                       the final product.
     *  @param no_common Whether common declarations should not be
     * included.
     *  @param no_drt Whether the drt library should not be imported.
     *  @param no_arithmetic Whether the arithmetic library should not
     * be imported.
     *  @param static_mods_all Whether all modules should be linked
     *                         statically.
     *  @param enable_cto Whether the module being compiled is a
     *                    compile-time-only module (equivalent to (attr
     *                    cto)).
     *  @param print_expansions Whether to print macro expansions.
     *  @param shared_object_paths The paths to shared objects against
     * which
     *                             the output file has to be linked
     *                             (populated by this function).
     *  @param output_file The compilation output file.
     */
    int run(std::vector<const char *> *file_paths,
            std::vector<const char *> *bc_file_paths,
            std::vector<const char *> *compile_lib_paths,
            std::vector<const char *> *include_paths,
            std::vector<const char *> *module_paths,
            std::vector<const char *> *static_module_names,
            const char *module_name, int debug, int produce,
            int optlevel, int remove_macros, int no_common,
            int no_drt, int no_arithmetic,
            int static_mods_all, int enable_cto,
            int print_expansions,
            std::vector<std::string> *shared_object_paths,
            FILE *output_file);
};
}

#endif
