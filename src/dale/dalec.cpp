#include "Generator/Generator.h"

#include <cstring>
#include <cstdlib>
#include <unistd.h>
#include <sys/stat.h>
#include <getopt.h>
#include <cstdio>
#include "Config.h"

/*! dalec

    The main compiler executable, responsible for organising the
    arguments for Generator, running Generator, and assembling/linking
    the results together using the system's compiler.
*/

using namespace dale;

static const char *options = "M:m:O:a:I:L:l:o:s:b:cdrR";
static const int COPY_SIZE = 8192;
static const char *progname = NULL;

static void
error(const char *error_msg, bool show_perror = false)
{
    if (show_perror) {
        perror(progname);
    }
    fprintf(stderr, "%s: %s.\n", progname, error_msg);
    exit(1);
}

static bool
appearsToBeLib(const char *str)
{
    int len = strlen(str);
    if (len >= 2) {
        const char *end = str + (len - 2);
        if ((!strcmp(end, ".o")) || (!strcmp(end, ".a"))) {
            return true;
        }
    }
    return false;
}

static void
joinWithPrefix(std::vector<const char*> *strings, const char *prefix,
               std::string *buffer)
{
    for (std::vector<const char*>::iterator b = strings->begin(),
                                            e = strings->end();
            b != e;
            ++b) {
        buffer->append(" ");
        buffer->append(prefix);
        buffer->append(" ");
        buffer->append(*b);
    }
}

static bool
copyFile(const char *to_path, FILE *from)
{
    FILE *to = fopen(to_path, "w");
    char buf[COPY_SIZE];
    memset(buf, 0, COPY_SIZE);
    size_t bytes;
    size_t wbytes;
    bool success = true;

    fseek(from, SEEK_SET, 0);

    while ((bytes = fread(buf, (size_t) 1, (size_t) COPY_SIZE, from))) {
        if (ferror(from)) {
            fclose(from);
            fclose(to);
            error("unable to read temporary file", true);
            success = false;
            break;
        }
        wbytes = fwrite(buf, (size_t) 1, bytes, to);
        if (wbytes != bytes) {
            if (ferror(from)) {
                error("unable to copy temporary file content", true);
                success = false;
                break;
            }
            if (ferror(to)) {
                error("unable to copy temporary file content", true);
                success = false;
                break;
            }
        }
        if (bytes != COPY_SIZE) {
            if (feof(from)) {
                break;
            } else {
                error("unable to copy temporary file content", true);
                success = false;
                break;
            }
        }
        memset(buf, 0, COPY_SIZE);
    }

    fflush(to);
    fclose(from);
    fclose(to);

    return success;
}

int
main(int argc, char **argv)
{
    srand(time(NULL) + getpid());

    progname = argv[0];

    int opt;
    char optc;

    std::vector<const char*> input_files;
    std::vector<const char*> input_link_files;
    std::vector<const char*> compile_libs;
    std::vector<const char*> run_libs;
    std::vector<const char*> include_paths;
    std::vector<const char*> run_paths;
    std::vector<const char*> bitcode_paths;
    std::vector<const char*> static_modules;
    std::vector<const char*> cto_modules;
    std::vector<const char*> module_paths;

    std::string output_path;
    const char *output_path_arg = NULL;
    const char *module_name     = NULL;

    int produce  = ASM;
    int optlevel = 0;

    int produce_set     = 0;
    int no_linking      = 0;
    int debug           = 0;
    int no_dale_stdlib  = 0;
    int no_stdlib       = 0;
    int remove_macros   = 0;
    int no_common       = 0;
    int static_mods_all = 0;
    int found_sm        = 0;
    int found_ctom      = 0;
    int enable_cto      = 0;
    int version         = 0;

    int option_index         = 0;
    int forced_remove_macros = 0;

    static struct option long_options[] = {
        { "no-dale-stdlib", no_argument,       &no_dale_stdlib,  1 },
        { "no-common",      no_argument,       &no_common,       1 },
        { "no-stdlib",      no_argument,       &no_stdlib,       1 },
        { "static-modules", no_argument,       &static_mods_all, 1 },
        { "static-module",  required_argument, &found_sm,        1 },
        { "cto-module",     required_argument, &found_ctom,      1 },
        { "enable-cto",     no_argument,       &enable_cto,      1 },
        { "version",        no_argument,       &version,         1 },
        { 0, 0, 0, 0 }
    };

    if (argc < 2) {
        error("no input files");
    }

    while ((opt = getopt_long(argc, argv, options,
                              long_options, &option_index)) != -1) {
        optc = (char) opt;

        switch (optc) {
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
                produce_set = true;
                break;
            }
            case 'd': debug = 1;                                   break;
            case 'c': no_linking = 1;                              break;
            case 'r': remove_macros = 1; forced_remove_macros = 1; break;
            case 'R': remove_macros = 0; forced_remove_macros = 1; break;
            case 'I': include_paths.push_back(optarg);             break;
            case 'a': compile_libs.push_back(optarg);              break;
            case 'L': run_paths.push_back(optarg);                 break;
            case 'l': run_libs.push_back(optarg);                  break;
            case 'b': bitcode_paths.push_back(optarg);             break;
            case 'M': module_paths.push_back(optarg);              break;
            case 'm': module_name = optarg;                        break;
        };

        if (found_sm) {
            found_sm = 0;
            static_modules.push_back(optarg);
        } else if (found_ctom) {
            found_ctom = 0;
            cto_modules.push_back(optarg);
        }
    }

    if (version) {
        printf("%d.%d\n", DALE_VERSION_MAJOR, DALE_VERSION_MINOR);
        exit(0);
    }

    /* If the user wants an executable and has not specified either
     * way with respect to removing macros, then remove macros. */
    if (!no_linking && !produce_set && !forced_remove_macros) {
        remove_macros = 1;
    }

    /* Every argument after the options is treated as an input file.
     * Input files that end with .o or .a should go straight to the
     * linker. */
    int input_file_count = argc - optind;
    for (int i = 0; i < input_file_count; ++i) {
        const char *input_file = argv[optind + i];
        if (appearsToBeLib(input_file)) {
            input_link_files.push_back(input_file);
        } else {
            input_files.push_back(input_file);
        }
    }

    if (!input_files.size()) {
        error("no input files");
    }

    /* Set output_path. */
    if (!no_linking) {
        if (output_path_arg) {
            output_path.append(output_path_arg);
        } else {
            if (produce == ASM) {
                output_path.append("a.out");
            } else if (produce_set) {
                output_path.append(input_files[0]);
                output_path.append(
                      (produce == IR)      ? ".ll"
                    : (produce == ASM)     ? ".s"
                    : (produce == BitCode) ? ".bc"
                                           : ".unknown"
                );
            }
        }
    } else {
        if (output_path_arg) {
            output_path.append(output_path_arg);
        } else {
            output_path.append(input_files[0]);
            output_path.append(".o");
        }
    }

    std::string compile_lib_str;
    joinWithPrefix(&compile_libs, "-l", &compile_lib_str);

    std::string include_path_str;
    joinWithPrefix(&include_paths, "-I", &include_path_str);

    std::string run_path_str;
    joinWithPrefix(&run_paths, "-L", &run_path_str);

    std::string input_file_str;
    joinWithPrefix(&input_files, " ", &input_file_str);

    std::string input_link_file_str;
    joinWithPrefix(&input_link_files, " ", &input_link_file_str);

    FILE *output_file = tmpfile();
    if (!output_file) {
        error("unable to open temporary output file", true);
    }
    std::vector<std::string> so_paths;
    Generator generator;

    bool generated =
        generator.run(&input_files,
                      &bitcode_paths,
                      &compile_libs,
                      &include_paths,
                      &module_paths,
                      &static_modules,
                      &cto_modules,
                      module_name,
                      debug,
                      produce,
                      optlevel,
                      remove_macros,
                      no_common,
                      no_dale_stdlib,
                      static_mods_all,
                      enable_cto,
                      &so_paths,
                      output_file);
    if (!generated) {
        exit(1);
    }
    fflush(output_file);

    std::string run_lib_str;
    joinWithPrefix(&run_libs, " -l ", &run_lib_str);

    for (std::vector<std::string>::reverse_iterator b = so_paths.rbegin(),
                                                    e = so_paths.rend();
            b != e;
            ++b) {
        input_link_file_str.append(" ");
        input_link_file_str.append((*b).c_str());
    }

    std::string intermediate_output_path = output_path;
    if (!produce_set) {
        intermediate_output_path.append(".s");
    }
    bool result = copyFile(intermediate_output_path.c_str(), output_file);
    if (!result) {
        exit(1);
    }
    if (produce_set) {
        exit(0);
    }

    char compile_cmd[8192];
    int bytes = 0;
    if (no_linking) {
        bytes = snprintf(compile_cmd, (8192 - 1),
                         "cc %s -c %s %s %s -o %s",
                         (no_stdlib) ? "--nostdlib" : "",
                         run_path_str.c_str(),
                         run_lib_str.c_str(),
                         intermediate_output_path.c_str(),
                         output_path.c_str());
    } else {
        bytes = snprintf(compile_cmd, (8192 - 1),
                         "cc %s -Wl,--gc-sections %s %s %s %s -o %s",
                         (no_stdlib) ? "--nostdlib" : "",
                         run_path_str.c_str(),
                         intermediate_output_path.c_str(),
                         input_link_file_str.c_str(),
                         run_lib_str.c_str(),
                         output_path.c_str());
    }
    if (bytes >= 8192) {
        error("cc command is too long");
    }

    int status = system(compile_cmd);
    if (status != 0) {
        if (debug) {
            fprintf(stderr, "%s\n", compile_cmd);
        }
        error("cc failed");
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
