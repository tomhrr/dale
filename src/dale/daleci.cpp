#include <getopt.h>
#include <sys/stat.h>
#include <unistd.h>

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <string>
#include <vector>

#include "Config.h"
#include "REPL/REPL.h"
#include "Utils/Utils.h"

/*! daleci

    The REPL executable.
*/

using namespace dale;

std::string joinWithPrefix(std::vector<const char *> strings,
                           const std::string prefix,
                           std::string buffer) {
    for (std::vector<const char *>::iterator b = strings.begin(),
                                             e = strings.end();
         b != e; buffer += " " + prefix + " " + (*b++)) {
    }

    return buffer;
}

int main(int argc, char **argv) {
    srand(time(NULL) + getpid());

    progname = argv[0];

    std::vector<const char *> compile_libs;
    std::vector<const char *> run_libs;
    std::vector<const char *> include_paths;
    std::vector<const char *> run_paths;
    std::vector<const char *> bitcode_paths;
    std::vector<const char *> module_paths;

    int debug = 0;
    int no_dale_stdlib = 0;
    int no_stdlib = 0;
    int no_common = 0;
    int version = 0;
    int print_expansions = 0;

    int option_index = 0;

    static const char *options = "M:a:I:d";
    static struct option long_options[] = {
        {"no-dale-stdlib", no_argument, &no_dale_stdlib, 1},
        {"no-common", no_argument, &no_common, 1},
        {"no-stdlib", no_argument, &no_stdlib, 1},
        {"version", no_argument, &version, 1},
        {"print-expansions", no_argument, &print_expansions, 1},
        {0, 0, 0, 0}};

    for (int opt; (opt = getopt_long(argc, argv, options, long_options,
                                     &option_index)) != -1;) {
        switch (static_cast<char>(opt)) {
            case 'd':
                debug = 1;
                break;
            case 'I':
                include_paths.push_back(optarg);
                break;
            case 'a':
                compile_libs.push_back(optarg);
                break;
            case 'M':
                module_paths.push_back(optarg);
                break;
        }
    }

    if (version) {
        printVersion();
        printf("\n");
        exit(0);
    }

    printf("daleci ");
    printVersion();
    printf(", ctrl-c to exit\n");

    REPL repl;
    repl.run(&compile_libs, &include_paths, &module_paths, debug,
             no_common, no_dale_stdlib, print_expansions);
}
