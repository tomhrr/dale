#include <getopt.h>
#include <sys/stat.h>
#include <unistd.h>

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <string>
#include <vector>

#include "Config.h"
#include "Generator/Generator.h"
#include "Utils/Utils.h"

/*! dalec

    The main compiler executable, responsible for organising the
    arguments for Generator, running Generator, and assembling/linking
    the results together using the system's compiler.
*/

using namespace dale;

/* Determine whether the given string ends with the given ending. */
static bool isEndingOn(const char *string, const char *ending) {
    size_t sl = strlen(string);
    size_t el = strlen(ending);

    return (sl >= el) && (strcmp(string + (sl - el), ending) == 0);
}

/* Determine whether a path is for a file that should be included in
 * the final linking command without any further processing. */
static bool appearsToBeLib(const char *str) {
    return isEndingOn(str, ".o") || isEndingOn(str, ".a");
}

/* Prefix each string from the vector with the given prefix, followed
 * by a space, and then add the final string to the buffer. */
std::string joinWithPrefix(std::vector<const char *> strings,
                           const std::string prefix,
                           std::string buffer) {
    for (std::vector<const char *>::iterator b = strings.begin(),
                                             e = strings.end();
         b != e; buffer += " " + prefix + " " + (*b++)) {
    }

    return buffer;
}

/* Add the runtime libraries as compile-time libraries, so that LLVM
 * is able to resolve external symbols during compilation.  (There is
 * probably a better way to do this.) */
void copyRuntimeToCompileTime(std::vector<const char *> *run_libs,
                              std::vector<const char *> *compile_libs) {
    for (std::vector<const char *>::iterator b = run_libs->begin(),
                                             e = run_libs->end();
         b != e; ++b) {
        FILE *fp;
        char libname[256];
        snprintf(libname, sizeof(libname), "-l%s", *b);
        char command[256];
        snprintf(command, sizeof(command), "ld -t %s 2>/dev/null",
                 libname);
        fp = popen(command, "r");
        if (fp == NULL) {
            fprintf(stderr, "Unable to resolve library path");
            abort();
        }
        char line[256];
        while (fgets(line, sizeof(line) - 1, fp) != NULL) {
            if (!strncmp(line, libname, strlen(libname))) {
                char path[256];
                char *start = strchr(line, '/');
                char *end = strchr(line, ')');
                strncpy(path, start, (end - start));
                path[end - start] = '\0';
                compile_libs->push_back(strdup(path));
            }
        }
    }
}

/* Determine the output path. */
void getOutputPath(const char *output_path_arg, const char *input_file,
                   bool no_linking, int produce_set, int produce,
                   std::string *output_path) {
    if (output_path_arg) {
        /* Is given. */
        *output_path = output_path_arg;
    } else {
        /* Otherwise, construct it. */
        *output_path = input_file;

        if (no_linking) {
            *output_path += ".o";
        } else if (produce_set) {
            /* .unknown should never be reached. */
            *output_path +=
                ((produce == IR)
                     ? ".ll"
                     : (produce == ASM)
                           ? ".s"
                           : (produce == BitCode) ? ".bc" : ".unknown");
        } else {
            /* Overwrite what was there. */
            *output_path = "a.out";
        }
    }
}

int main(int argc, char **argv) {
    srand(time(NULL) + getpid());

    progname = argv[0];

    std::vector<const char *> input_files;
    std::vector<const char *> input_link_files;
    std::vector<const char *> compile_libs;
    std::vector<const char *> run_libs;
    std::vector<const char *> include_paths;
    std::vector<const char *> run_paths;
    std::vector<const char *> bitcode_paths;
    std::vector<const char *> static_modules;
    std::vector<const char *> module_paths;

    const char *output_path_arg = NULL;
    const char *module_name = NULL;

    int produce = ASM;
    int optlevel = 0;

    int produce_set = 0;
    int no_linking = 0;
    int debug = 0;
    int no_arithmetic = 0;
    int no_drt = 0;
    int no_stdlib = 0;
    int remove_macros = 0;
    int no_common = 0;
    int static_mods_all = 0;
    int found_sm = 0;
    int enable_cto = 0;
    int version = 0;
    int print_expansions = 0;

    int option_index = 0;
    int forced_remove_macros = 0;

    static const char *options = "M:m:O:a:I:L:l:o:s:b:cdrR";
    static struct option long_options[] = {
        {"no-drt", no_argument, &no_drt, 1},
        {"no-arithmetic", no_argument, &no_arithmetic, 1},
        {"no-common", no_argument, &no_common, 1},
        {"no-stdlib", no_argument, &no_stdlib, 1},
        {"static-modules", no_argument, &static_mods_all, 1},
        {"static-module", required_argument, &found_sm, 1},
        {"enable-cto", no_argument, &enable_cto, 1},
        {"version", no_argument, &version, 1},
        {"print-expansions", no_argument, &print_expansions, 1},
        {0, 0, 0, 0}};

    for (int opt; (opt = getopt_long(argc, argv, options, long_options,
                                     &option_index)) != -1;) {
        switch (static_cast<char>(opt)) {
            case 'o': {
                if (output_path_arg) {
                    error("an output path has already been specified");
                }
                output_path_arg = optarg;
                break;
            }
            case 'O': {
                optlevel = (optarg[0] - '0');
                if ((optlevel < 0) || (optlevel > 4)) {
                    error("invalid optimisation level");
                }
                break;
            }
            case 's': {
                produce_set = true;
                const char *type = optarg;
                if (!strcmp(type, "as")) {
                    produce = ASM;
                } else if (!strcmp(type, "ir")) {
                    produce = IR;
                } else if (!strcmp(type, "bc")) {
                    produce = BitCode;
                } else {
                    error("unrecognised output option");
                }
                break;
            }
            case 'd':
                debug = 1;
                break;
            case 'c':
                no_linking = 1;
                break;
            case 'r':
                remove_macros = 1;
                forced_remove_macros = 1;
                break;
            case 'R':
                remove_macros = 0;
                forced_remove_macros = 1;
                break;
            case 'I':
                include_paths.push_back(optarg);
                break;
            case 'a':
                compile_libs.push_back(optarg);
                break;
            case 'L':
                run_paths.push_back(optarg);
                break;
            case 'l':
                run_libs.push_back(optarg);
                break;
            case 'b':
                bitcode_paths.push_back(optarg);
                break;
            case 'M':
                module_paths.push_back(optarg);
                break;
            case 'm':
                module_name = optarg;
                break;
        }
        if (found_sm) {
            found_sm = 0;
            static_modules.push_back(optarg);
        }
    }

    if (version) {
        printVersion();
        printf("\n");
        exit(0);
    }

    if (static_modules.size() || static_mods_all) {
        copyRuntimeToCompileTime(&run_libs, &compile_libs);
    }

    /* If the user wants an executable and has not specified either
     * way with respect to removing macros, then remove macros. */
    if (!no_linking && !produce_set && !forced_remove_macros) {
        remove_macros = 1;
    }

    /* Every argument after the options is treated as an input file.
     * Input files that end with .o or .a should go straight to the
     * linker. */
    while (optind != argc) {
        const char *input_file = argv[optind++];

        (appearsToBeLib(input_file) ? input_link_files : input_files)
            .push_back(input_file);
    }
    if (input_files.empty()) {
        error("no input files");
    }

    std::string output_path;
    getOutputPath(output_path_arg, input_files[0], no_linking,
                  produce_set, produce, &output_path);

    /* Generate an intermediate file, to be compiled and linked later
     * with the system compiler, by building the executable in memory
     * and then exporting it into the requested intermediate format. */
    std::vector<std::string> so_paths;
    Generator generator;

    std::string intermediate_output_path =
        output_path + (produce_set ? "" : ".s");

    FILE *output_file = fopen(intermediate_output_path.c_str(), "w");
    if (output_file == NULL) {
        error("unable to open %s for writing",
              intermediate_output_path.c_str(), true);
    }
    int res = generator.run(&input_files, &bitcode_paths, &compile_libs,
                            &include_paths, &module_paths,
                            &static_modules, module_name, debug,
                            produce, optlevel, remove_macros, no_common,
                            no_drt, no_arithmetic,
                            static_mods_all, enable_cto,
                            print_expansions, &so_paths, output_file);
    if (!res) {
        exit(1);
    }
    res = fflush(output_file);
    if (res != 0) {
        error("unable to flush the intermediate output file", true);
    }
    if (produce_set) {
        /* We're done. */
        exit(0);
    }

    /* Prepare the strings to sew the compile command with. */
    std::string run_path_str = joinWithPrefix(run_paths, "-L", "");
    std::string run_lib_str = joinWithPrefix(run_libs, "-l", "");
    std::string rpath_str =
        strcmp(SYSTEM_NAME, "Darwin")
            ? ""
            : joinWithPrefix(module_paths, "-rpath", "");

    std::string input_link_file_str =
        joinWithPrefix(input_link_files, " ", "");
    for (std::vector<std::string>::iterator b = so_paths.begin(),
                                            e = so_paths.end();
         b != e; input_link_file_str += " " + (*b++)) {
    }

    /* Compose the compiler/linker command and execute it. */
    /* DALEC_CC_FLAGS is an undocumented environment variable that can
     * be used to provide additional flags to the compiler call here.
     * It is undocumented because this call may be removed in
     * the future in favour of using LLVM more directly. */
    const char *aux = getenv("DALE_CC_FLAGS");
    std::string compile_cmd = DALE_CC;
    if (no_stdlib) {
        compile_cmd += " --nostdlib";
    }
    if (no_linking) {
        compile_cmd += " -c";
    } else {
        compile_cmd +=
            input_link_file_str + " -lm" +
            (strcmp(SYSTEM_NAME, "Darwin") ? " -Wl,--gc-sections" : "");
    }
    compile_cmd += run_lib_str + run_path_str + rpath_str + " -o " +
                   output_path + " " + intermediate_output_path;
    if (aux) {
        compile_cmd += " ";
        compile_cmd += aux;
        fprintf(stderr, "Going to run: %s\n", compile_cmd.c_str());
    }

    int status = system(compile_cmd.c_str());
    if (status != 0) {
        if (debug) {
            fprintf(stderr, "%s\n", compile_cmd.c_str());
        }
        error(DALE_CC " failed");
    }

    status = remove(intermediate_output_path.c_str());
    if (status != 0) {
        if (debug) {
            fprintf(stderr, "%s\n", intermediate_output_path.c_str());
        }
        error("unable to remove temporary file");
    }

    return 0;
}
