#ifndef DALE_REPL
#define DALE_REPL

#include <string>
#include <vector>

namespace dale {
/*! REPL

    An adaptation of Generator for the REPL use case.
*/
class REPL {
    public:
    REPL();
    ~REPL();
    /*! Evaluate commands entered via standard input and execute them.
     *  @param compile_lib_paths Paths to libraries for use at compile
     * time.
     *  @param include_paths Paths to inspect for include files.
     *  @param module_paths Paths to modules to import.
     *  @param debug Enable debug mode.
     *  @param no_common Whether common declarations should not be
     * included.
     *  @param no_dale_stdlib Whether the drt library should not be
     * imported.
     *  @param print_expansions Whether to print macro expansions.
     */
    void run(std::vector<const char *> *compile_lib_paths,
             std::vector<const char *> *include_paths,
             std::vector<const char *> *module_paths, int debug,
             int no_common, int no_dale_stdlib, int print_expansions);
};
}

#endif
