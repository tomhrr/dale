#include "Token/Token.h"
#include "Generator/Generator.h"

#include <cstring>
#include <cstdlib>
#include <unistd.h>
#include <sys/stat.h>
#include <getopt.h>
#include <cstdio>

using namespace dale;

static const char *options = "M:m:O:a:I:L:l:o:s:b:cdrR";
static const int COPY_SIZE = 8192;

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
            perror("Unable to read file");
            fclose(from);
            fclose(to);
            success = false;
            break;
        }
        wbytes = fwrite(buf, (size_t) 1, bytes, to);
        if (wbytes != bytes) {
            if (ferror(from)) {
                perror("Unable to copy file content");
                success = false;
                break;
            }
            if (ferror(to)) {
                perror("Unable to copy file content");
                success = false;
                break;
            }
        }
        if (bytes != COPY_SIZE) {
            if (feof(from)) {
                break;
            } else {
                perror("Unable to copy file content");
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
    int no_strip        = 0;
    int static_mods_all = 0;
    int found_sm        = 0;
    int found_ctom      = 0;
    int enable_cto      = 0;

    int option_index         = 0;
    int forced_remove_macros = 0;

    static struct option long_options[] = {
        { "no-dale-stdlib", no_argument,       &no_dale_stdlib,  1 },
        { "no-common",      no_argument,       &no_common,       1 },
        { "no-stdlib",      no_argument,       &no_stdlib,       1 },
        { "no-strip",       no_argument,       &no_strip,        1 },
        { "static-modules", no_argument,       &static_mods_all, 1 },
        { "static-module",  required_argument, &found_sm,        1 },
        { "cto-module",     required_argument, &found_ctom,      1 },
        { "enable-cto",     no_argument,       &enable_cto,      1 },
        { 0, 0, 0, 0 }
    };

    if (argc < 2) {
        fprintf(stderr, "dalec: no input files.\n");
        exit(1);
    }

    while ((opt = getopt_long(argc, argv, options,
                              long_options, &option_index)) != -1) {
        optc = (char) opt;

        switch (optc) {
            case 'o': {
                if (output_path_arg) {
                    fprintf(stderr, "dalec: an output path has already "
                                    "been specified.\n");
                    exit(1);
                }
                output_path_arg = optarg;
                break;
            }
            case 'O': {
                optlevel = (optarg[0] - '0');
                if ((optlevel < 0) || (optlevel > 4)) {
                    fprintf(stderr, "dalec: invalid optimisation level.\n");
                    exit(1);
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
                    fprintf(stderr, "dalec: unrecognised output "
                            "option.\n");
                    exit(1);
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
        fprintf(stderr, "dalec: no input files.\n");
        exit(1);
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
        fprintf(stderr, "dalec: unable to open temporary output file.\n");
        exit(1);
    }
    std::vector<std::string> so_paths;
    Generator generator;

    bool generated =
        generator.run(&input_files, &bitcode_paths, output_file,
                      produce, optlevel, remove_macros,
                      module_name, no_common, &so_paths, no_strip,
                      static_mods_all, &static_modules,
                      &cto_modules, enable_cto, debug, no_dale_stdlib,
                      &compile_libs, &include_paths,
                      &module_paths);
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
        fprintf(stderr, "dalec: cc command is too long!\n");
        exit(1);
    }

    int status = system(compile_cmd);
    if (status != 0) {
        fprintf(stderr, "dalec: cc failed.\n", res, compile_cmd);
        if (debug) {
            fprintf(stderr, "%s\n", compile_cmd);
        }
        exit(1);
    }

    status = remove(intermediate_output_path.c_str());
    if (status != 0) {
        fprintf(stderr, "dalec: unable to remove temporary file '%s'.\n",
                        intermediate_output_path.c_str());
        exit(1);
    }

    return 0;
}
