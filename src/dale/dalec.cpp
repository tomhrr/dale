#include "Token/Token.h"
#include "Generator/Generator.h"

#include <cstring>
#include <cstdlib>
#include <unistd.h>
#include <sys/stat.h>
#include <getopt.h>
#include <cstdio>

using namespace dale;

int appears_to_be_lib(const char *str)
{
    int len = strlen(str);
    if (len >= 2) {
        const char *end = str + (len - 2);
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

    char *output_path_arg = NULL;
    int no_linking    = 0;
    int produce       = ASM;
    int produce_set   = 0;
    int optlevel      = 0;

    int input_file_count;
    int j = 0;
    int debug = 0;
    char output_path[256] = "";
    int no_dale_stdlib = 0;
    int no_stdlib = 0;
    int remove_macros = 0;
    int no_common = 0;
    char *module_name = NULL;
    int no_strip = 0;
    int static_mods_all = 0;
    int found_sm = 0;
    int found_ctom = 0;
    int enable_cto = 0;

    if (argc < 2) {
        fprintf(stderr, "dalec: no input files.\n");
        exit(1);
    }

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
            include_paths.push_back(optarg);
        } else if (optc == 'a') {
            compile_libs.push_back(optarg);
        } else if (optc == 'L') {
            run_paths.push_back(optarg);
        } else if (optc == 'l') {
            run_libs.push_back(optarg);
        } else if (optc == 'b') {
            bitcode_paths.push_back(optarg);
        } else if (optc == 'M') {
            module_paths.push_back(optarg);
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
            static_modules.push_back(optarg);
        } else if (found_ctom == 1) {
            found_sm = 0;
            cto_modules.push_back(optarg);
        }
    }

    /* If the user wants an executable and has not specified either
     * way with respect to removing macros, then remove macros. */
    if (!no_linking && !produce_set && !forced_remove_macros) {
        remove_macros = 1;
    }

    input_file_count = argc - optind;

    for (j = 0; j < input_file_count; ++j) {
        input_files.push_back(argv[optind + j]);
    }

    /* Input files that end with .o or .a should go straight to the
     * linker. */
    int actual_input_file_count = input_file_count;
    for (int k = 0; k < input_file_count; k++) {
        int len = strlen(input_files[k]);
        if (len >= 2) {
            if (appears_to_be_lib(input_files[k])) {
                input_link_files.push_back(input_files[k]);
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

    std::string compile_lib_str;
    for (std::vector<const char*>::iterator b = compile_libs.begin(),
                                            e = compile_libs.end();
            b != e;
            ++b) {
        compile_lib_str.append(" -l ");
        compile_lib_str.append(*b);
    }

    std::string include_path_str;
    for (std::vector<const char*>::iterator b = include_paths.begin(),
                                            e = include_paths.end();
            b != e;
            ++b) {
        include_path_str.append(" -I ");
        include_path_str.append(*b);
    }

    std::string run_path_str;
    for (std::vector<const char*>::iterator b = run_paths.begin(),
                                            e = run_paths.end();
            b != e;
            ++b) {
        run_path_str.append(" -L ");
        run_path_str.append(*b);
    }

    std::string input_file_str;
    for (std::vector<const char*>::iterator b = input_files.begin(),
                                            e = input_files.end();
            b != e;
            ++b) {
        if (!appears_to_be_lib(*b)) {
            input_file_str.append(" ");
            input_file_str.append(*b);
        }
    }

    std::vector<const char*> non_lib_input_files;
    for (std::vector<const char*>::iterator b = input_files.begin(),
                                            e = input_files.end();
            b != e;
            ++b) {
        if (!appears_to_be_lib(*b)) {
            non_lib_input_files.push_back(*b);
        }
    }

    std::string input_link_file_str;
    for (std::vector<const char*>::iterator b = input_link_files.begin(),
                                            e = input_link_files.end();
            b != e;
            ++b) {
        input_link_file_str.append(" ");
        input_link_file_str.append(*b);
    }

    FILE *tempout = tmpfile();
    if (!tempout) {
        perror("Unable to open temporary output file");
        exit(1);
    }

    Generator g;
    std::vector<std::string> so_paths;

    int rest = g.run(&non_lib_input_files, &bitcode_paths, tempout,
                     produce, optlevel, remove_macros,
                     module_name, no_common, &so_paths, no_strip,
                     static_mods_all, &static_modules,
                     &cto_modules, enable_cto, debug, no_dale_stdlib,
                     &compile_libs, &include_paths,
                     &module_paths);
    if (!rest) {
        exit(1);
    }

    std::string run_lib_str;
    for (std::vector<const char*>::iterator b = run_libs.begin(),
                                            e = run_libs.end();
            b != e;
            ++b) {
        run_lib_str.append(" -l ");
        run_lib_str.append(*b);
    }

    for (std::vector<std::string>::reverse_iterator b = so_paths.rbegin(),
            e = so_paths.rend();
            b != e;
            ++b) {
        input_link_file_str.append(" ");
        input_link_file_str.append((*b).c_str());
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
                run_path_str.c_str(),
                run_lib_str.c_str(),
                temp_output_path,
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
                run_path_str.c_str(),
                temp_output_path,
                input_link_file_str.c_str(),
                run_lib_str.c_str(),
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
