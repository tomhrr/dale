#include "Token/Token.h"
#include "Generator/Generator.h"

#include <cstring>
#include <cstdlib>
#include <unistd.h>
#include <sys/stat.h>
#include <getopt.h>
#include <cstdio>

const int MAX_LIBS        = 100;
const int MAX_INPUT_FILES = 100;

using namespace dale;

int appears_to_be_lib(char *str)
{
    int len = strlen(str);
    if (len >= 2) {
        char *end = str + (len - 2);
        if ((!strcmp(end, ".o")) || (!strcmp(end, ".a"))) {
            return 1;
        }
    }
    return 0;
}

int main(int argc, char **argv)
{
    const char *options = "M:m:O:a:I:L:l:o:s:b:cdrR";
    int opt;
    char optc;

    char *input_files[MAX_INPUT_FILES];
    char *input_link_files[MAX_INPUT_FILES];
    const char *compile_libs[MAX_LIBS];
    const char *run_libs[MAX_LIBS];
    char *include_paths[MAX_LIBS];
    char *run_paths[MAX_LIBS];
    char *bitcode_paths[MAX_LIBS];
    char *static_modules[MAX_LIBS];
    char *cto_modules[MAX_LIBS];
    char *output_path_arg = NULL;
    char *module_paths[MAX_LIBS];
    int no_linking    = 0;
    int produce       = ASM;
    int produce_set   = 0;
    int optlevel      = 0;

    int cto_module_count = 0;
    int static_module_count = 0;
    int compile_lib_count = 0;
    int input_link_file_count = 0;
    int run_lib_count = 0;
    int include_path_count = 0;
    int run_path_count = 0;
    int input_file_count = 0;
    int j = 0;
    int debug = 0;
    char output_path[256] = "";
    int no_dale_stdlib = 0;
    int no_stdlib = 0;
    int bitcode_path_count = 0;
    int remove_macros = 0;
    int no_acd = 0;
    char *module_name = NULL;
    int no_strip = 0;
    int static_mods_all = 0;
    int found_sm = 0;
    int found_ctom = 0;
    int enable_cto = 0;
    int module_path_count = 0;
    int noalways = 0;

    if (argc < 2) {
        fprintf(stderr, "dalec: no input files.\n");
        exit(1);
    }

    static struct option long_options[] = {
        { "no-dale-stdlib", no_argument,       &no_dale_stdlib,  1 },
        { "no-common",      no_argument,       &no_acd,          1 },
        { "no-stdlib",      no_argument,       &no_stdlib,       1 },
        { "no-strip",       no_argument,       &no_strip,        1 },
        { "static-modules", no_argument,       &static_mods_all, 1 },
        { "static-module",  required_argument, &found_sm,        1 },
        { "cto-module",     required_argument, &found_ctom,      1 },
        { "enable-cto",     no_argument,       &enable_cto,      1 },
        { 0, 0, 0, 0 }
    };

    int option_index = 0;
    int forced_remove_macros = 0;
    while ((opt = getopt_long(argc, argv, options,
                              long_options, &option_index)) != -1) {
        optc = (char) opt;

        if (optc == 'o') {
            if (output_path_arg) {
                fprintf(stderr, "dalec: an output path has already "
                        "been specified.\n");
                exit(1);
            }
            output_path_arg = optarg;
        } else if (optc == 'd') {
            debug = 1;
        } else if (optc == 'O') {
            optlevel = (optarg[0] - '0');
            if ((optlevel < 0) || (optlevel > 4)) {
                fprintf(stderr, "dalec: invalid optimisation level.\n");
                exit(1);
            }
        } else if (optc == 's') {
            char *type = optarg;
            if (!strcmp(type, "as")) {
                produce = ASM;
            } else if (!strcmp(type, "ir")) {
                produce = IR;
            } else if (!strcmp(type, "bc")) {
                produce = BitCode;
            } else {
                fprintf(stderr, "dalec: unrecognised output "
                        "option: %s\n",
                        type);
                exit(1);
            }
            produce_set = 1;
        } else if (optc == 'I') {
            if (include_path_count == MAX_LIBS) {
                fprintf(stderr, "dalec: hit include path limit (change "
                        "the limit in dale-to-bc.\n");
                exit(1);
            }
            include_paths[include_path_count++] = optarg;
        } else if (optc == 'a') {
            if (compile_lib_count == MAX_LIBS) {
                fprintf(stderr, "dalec: hit library limit (change "
                        "the limit in dale-to-bc.\n");
                exit(1);
            }
            compile_libs[compile_lib_count++] = optarg;
        } else if (optc == 'L') {
            if (run_path_count == MAX_LIBS) {
                fprintf(stderr, "dalec: hit run-include path limit (change "
                        "the limit in dale-to-bc.\n");
                exit(1);
            }
            run_paths[run_path_count++] = optarg;
        } else if (optc == 'l') {
            if (run_lib_count == MAX_LIBS) {
                fprintf(stderr, "dalec: hit run-library limit (change "
                        "the limit in dale-to-bc.\n");
                exit(1);
            }
            run_libs[run_lib_count++] = optarg;
        } else if (optc == 'b') {
            if (run_lib_count == MAX_LIBS) {
                fprintf(stderr, "dalec: hit bitcode path limit (change "
                        "the limit in dale-to-bc.\n");
                exit(1);
            }
            bitcode_paths[bitcode_path_count++] = optarg;
        } else if (optc == 'M') {
            if (module_path_count == MAX_LIBS) {
                fprintf(stderr, "dalec: hit module path limit (change "
                        "the limit in dale-to-bc.\n");
                exit(1);
            }
            module_paths[module_path_count++] = optarg;
        } else if (optc == 'c') {
            no_linking = 1;
        } else if (optc == 'r') {
            remove_macros = 1;
            forced_remove_macros = 1;
        } else if (optc == 'R') {
            remove_macros = 0;
            forced_remove_macros = 1;
        } else if (optc == 'm') {
            module_name = optarg;
        } else if (found_sm == 1) {
            found_sm = 0;
            static_modules[static_module_count++] = optarg;
        } else if (found_ctom == 1) {
            found_sm = 0;
            cto_modules[cto_module_count++] = optarg;
        }
    }

    /* If the user wants an executable and has not specified either
     * way with respect to removing macros, then remove macros. */
    if (!no_linking && !produce_set && !forced_remove_macros) {
        remove_macros = 1;
    }

    input_file_count = argc - optind;

    if (input_file_count > MAX_INPUT_FILES) {
        fprintf(stderr, "dalec: input file count (%d) exceeds the "
                "maximum number of input files that "
                "may be specified.\n", input_file_count);
        exit(1);
    }

    for (j = 0; j < input_file_count; ++j) {
        input_files[j] = argv[optind + j];
    }

    /* Input files that end with .o or .a should go straight to the
     * linker. */
    int actual_input_file_count = input_file_count;
    for (int k = 0; k < input_file_count; k++) {
        int len = strlen(input_files[k]);
        if (len >= 2) {
            if (appears_to_be_lib(input_files[k])) {
                input_link_files[input_link_file_count++] =
                    input_files[k];
                actual_input_file_count--;
            }
        }
    }
    if (actual_input_file_count < 1) {
        fprintf(stderr, "dalec: no input files.\n");
        exit(1);
    }

    if (!no_linking) {
        if (output_path_arg) {
            strncpy(output_path, output_path_arg, 256);
        } else {
            if (produce == ASM) {
                sprintf(output_path, "a.out");
            } else if (produce_set) {
                strncpy(output_path, input_files[0], 253);
                strcat(output_path,
                       (produce == IR)      ? ".ll"
                       : (produce == ASM)     ? ".s"
                       : (produce == BitCode) ? ".bc"
                       : ".unknown"
                      );
            }
        }
    } else {
        if (output_path_arg) {
            strncpy(output_path, output_path_arg, 256);
        } else {
            strncpy(output_path, input_files[0], 253);
            strcat(output_path, ".o");
        }
    }
    if (no_dale_stdlib) {
        noalways = 1;
    }

    char compile_lib_str[1024] = "";
    for (int i = 0; i < compile_lib_count; ++i) {
        strcat(compile_lib_str, " -l ");
        strcat(compile_lib_str, compile_libs[i]);
    }

    char include_path_str[1024] = "";
    for (int i = 0; i < include_path_count; ++i) {
        strcat(include_path_str, " -I ");
        strcat(include_path_str, include_paths[i]);
    }

    char run_path_str[1024] = "";
    for (int i = 0; i < run_path_count; ++i) {
        strcat(run_path_str, " -L ");
        strcat(run_path_str, run_paths[i]);
    }

    char input_file_str[1024] = "";
    for (int i = 0; i < input_file_count; ++i) {
        if (!appears_to_be_lib(input_files[i])) {
            strcat(input_file_str, " ");
            strcat(input_file_str, input_files[i]);
        }
    }

    std::vector<const char*> vinput_files;

    for (j = 0; j < input_file_count; ++j) {
        if (!appears_to_be_lib(argv[optind + j])) {
            vinput_files.push_back(argv[optind + j]);
        }
    }

    char input_link_file_str[1024] = "";
    for (int i = 0; i < input_link_file_count; ++i) {
        strcat(input_link_file_str, " ");
        strcat(input_link_file_str, input_link_files[i]);
    }

    FILE *tempout = tmpfile();
    if (!tempout) {
        perror("Unable to open temporary output file");
        exit(1);
    }

    Generator *g = new Generator();
    bool res;

    std::vector<const char*> compile_libs_sv;
    for (j = 0; j < compile_lib_count; ++j) {
        compile_libs_sv.push_back(compile_libs[j]);
    }

    std::vector<const char*> include_paths_sv;
    for (j = 0; j < include_path_count; ++j) {
        include_paths_sv.push_back(include_paths[j]);
    }

    std::vector<const char*> module_paths_sv;
    for (j = 0; j < module_path_count; ++j) {
        module_paths_sv.push_back(module_paths[j]);
    }

    std::vector<const char*> bc_files;
    for (j = 0; j < bitcode_path_count; ++j) {
        bc_files.push_back(bitcode_paths[j]);
    }

    std::vector<const char*> vstatic_modules;
    for (j = 0; j < static_module_count; ++j) {
        vstatic_modules.push_back(static_modules[j]);
    }

    std::vector<const char*> vcto_modules;
    for (j = 0; j < cto_module_count; ++j) {
        vcto_modules.push_back(cto_modules[j]);
    }

    std::vector<std::string> so_paths;
    //so_paths.push_back("/usr/local/lib/libffi.so");

    int rest = g->run(&vinput_files, &bc_files, tempout,
                      produce, optlevel, remove_macros,
                      module_name, no_acd, &so_paths, no_strip,
                      static_mods_all, &vstatic_modules,
                      &vcto_modules, enable_cto, debug, noalways,
                      &compile_libs_sv, &include_paths_sv,
                      &module_paths_sv);
    if (!rest) {
        exit(1);
    }
    delete g;

    char run_lib_str[1024] = "";
    for (int i = 0; i < run_lib_count; ++i) {
        strcat(run_lib_str, " -l");
        strcat(run_lib_str, run_libs[i]);
    }
    for (std::vector<std::string>::reverse_iterator b = so_paths.rbegin(),
            e = so_paths.rend();
            b != e;
            ++b) {
        strcat(input_link_file_str, " ");
        strcat(input_link_file_str, (*b).c_str());
    }

    char temp_output_path[256] = "";
    strcpy(temp_output_path, output_path);
    if (!produce_set) {
        strcat(temp_output_path, ".s");
    }

    if (!rest) {
        exit(1);
    }

    if (rest) {
        fflush(tempout);
        FILE *out = fopen(temp_output_path, "w");
        fseek(tempout, 0, SEEK_SET);
        char buf[8192];
        memset(buf, 0, 8192);
        size_t bytes;
        size_t wbytes;
        while ((bytes = fread(buf, (size_t) 1, (size_t) 8192, tempout))) {
            if (ferror(tempout)) {
                perror("Unable to read output file");
                fclose(tempout);
                fclose(out);
                break;
            }
            wbytes = fwrite(buf, (size_t) 1, bytes, out);
            if (wbytes != bytes) {
                if (ferror(tempout)) {
                    perror("Unable to copy file content");
                    break;
                }
                if (ferror(out)) {
                    perror("Unable to copy file content");
                    break;
                }
            }
            if (bytes != 8192) {
                if (feof(tempout)) {
                    break;
                } else {
                    perror("Unable to copy file content");
                    break;
                }
            }
            memset(buf, 0, 8192);
        }
        fflush(tempout);
        fflush(out);
        fclose(tempout);
        fclose(out);
    }

    if (produce_set) {
        exit(0);
    }

    char final[8192] = "";
    final[0] = '\0';
    if (no_linking) {
        sprintf(final, "cc %s -c %s "
                "%s %s -o %s",
                (no_stdlib) ? "--nostdlib" : "",
                run_path_str,
                run_lib_str,
                temp_output_path,
                // not needed, obviously enough
                //input_link_file_str,
                output_path);
        int res = system(final);
        if (res) {
            perror("Unable to run cc");
            fprintf(stderr, "dalec: cc failed: %d (%s)\n", res, final);
            exit(1);
        }
    } else {
        sprintf(final, "cc %s -Wl,--gc-sections %s "
                "%s %s %s -o %s",
                (no_stdlib) ? "--nostdlib" : "",
                run_path_str,
                temp_output_path,
                input_link_file_str,
                run_lib_str,
                output_path);
        int res = system(final);
        if (res) {
            perror("Unable to run cc");
            fprintf(stderr, "dalec: cc failed: %d (%s)\n", res, final);
            exit(1);
        }
    }
    int rres = remove(temp_output_path);
    if (rres) {
        fprintf(stderr, "Internal error: unable to remove "
                "temporary file (%s).\n",
                temp_output_path);
    }

    return 0;
}
