#include "Generator/Generator.h"

#include "Config.h"
#include "Utils/Utils.h"
#include <cstring>
#include <cstdlib>
#include <unistd.h>
#include <sys/stat.h>
#include <getopt.h>
#include <cstdio>
#include <iostream>

/*! dalec

    The main compiler executable, responsible for organising the
    arguments for Generator, running Generator, and assembling/linking
    the results together using the system's compiler.
*/

using namespace dale;

static const char *options = "M:m:O:a:I:L:l:o:s:b:cdrR";

static bool
is_ending_on (const char *string, const char *ending)
{
  size_t sl = strlen (string), el = strlen (ending);

  return (sl >= el) && (strcmp (string + (sl - el), ending) == 0);
}

static bool
appearsToBeLib (const char *str)
{
  return is_ending_on (str, ".o") || is_ending_on (str, ".a");
}

std::string
joinWithPrefix (std::vector<const char*> strings,
                const std::string prefix, std::string buffer)
{
  for (std::vector<const char*>::iterator b = strings.begin (),
                                          e = strings.end ();
       b != e; buffer += " " + prefix + " " + (* b ++));

  return buffer;
}

int
main(int argc, char **argv)
{
    srand(time(NULL) + getpid());

    progname = argv[0];

    int opt;
    char optc;

    std::vector<const char*>
      input_files,
      input_link_files,
      compile_libs,
      run_libs,
      include_paths,
      run_paths,
      bitcode_paths,
      static_modules,
      module_paths;

    const char *output_path_arg = NULL;
    const char *module_name     = NULL;

    int produce  = ASM;
    int optlevel = 0;

    int produce_set      = 0;
    int no_linking       = 0;
    int debug            = 0;
    int no_dale_stdlib   = 0;
    int no_stdlib        = 0;
    int remove_macros    = 0;
    int no_common        = 0;
    int static_mods_all  = 0;
    int found_sm         = 0;
    int enable_cto       = 0;
    int version          = 0;
    int print_expansions = 0;

    int option_index         = 0;
    int forced_remove_macros = 0;

    static struct option long_options[] = {
        { "no-dale-stdlib",   no_argument,       &no_dale_stdlib,   1 },
        { "no-common",        no_argument,       &no_common,        1 },
        { "no-stdlib",        no_argument,       &no_stdlib,        1 },
        { "static-modules",   no_argument,       &static_mods_all,  1 },
        { "static-module",    required_argument, &found_sm,         1 },
        { "enable-cto",       no_argument,       &enable_cto,       1 },
        { "version",          no_argument,       &version,          1 },
        { "print-expansions", no_argument,       &print_expansions, 1 },
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
                produce_set = true;

                const char *type = optarg;
                if (!strcmp (type, "as")) produce = ASM;     else
                if (!strcmp (type, "ir")) produce = IR;      else
                if (!strcmp (type, "bc")) produce = BitCode; else
                  error ("unrecognised output file format");

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
        }
    }

    if (version)
      std::cout << DALE_VERSION_MAJOR << "."
                << DALE_VERSION_MINOR << std::endl, exit (0);

    /* If the user wants an executable and has not specified either
     * way with respect to removing macros, then remove macros. */
    if (!no_linking && !produce_set && !forced_remove_macros) {
        remove_macros = 1;
    }

    /* Every argument after the options is treated as an input file.
     * Input files that end with .o or .a should go straight to the
     * linker. */
    while (optind != argc)
      {
        const char *input_file = argv [optind ++];

        (appearsToBeLib (input_file) ?
         input_link_files : input_files) .push_back (input_file);
      }
    if (input_files.empty ()) error ("no input files");

    /* Set output_path. */
    std::string output_path;
    if (output_path_arg) output_path = output_path_arg;  // is given
    else  // otherwise construct it
      {
        output_path = input_files [0];  // leave the extension as is

        if (no_linking) output_path += ".o";
        else if (produce_set) output_path +=
                            ((produce == IR)      ? ".ll" :
                             (produce == ASM)     ? ".s"  :
                             (produce == BitCode) ? ".bc" :
                             ".unknown" );  // impossible, an error
        else output_path = "a.out";  // overwrite what was there
      }

    std::vector<std::string> so_paths;
    Generator generator;

    std::string intermediate_output_path =
      output_path + (produce_set ? "" : ".s");
    {
      FILE *output_file =
        fopen (intermediate_output_path.c_str (), "w");
      if (output_file == NULL) error ("unable to open %s for writing",
                                      intermediate_output_path.c_str (),
                                      true);
      if (! generator.run (&input_files,
                           &bitcode_paths,
                           &compile_libs,
                           &include_paths,
                           &module_paths,
                           &static_modules,
                           module_name,
                           debug,
                           produce,
                           optlevel,
                           remove_macros,
                           no_common,
                           no_dale_stdlib,
                           static_mods_all,
                           enable_cto,
                           print_expansions,
                           &so_paths,
                           output_file)) exit (1);
      if (fflush (output_file) != 0)
        error ("unable to flush the intermediate output file", true);
    }
    if (produce_set) exit (0);  // we're done

    // prepare the strings to sew the compile command with
    std::string input_file_str =
      joinWithPrefix (input_files, " ", "");
    std::string compile_lib_str =
      joinWithPrefix (compile_libs, "-l", "");
    std::string include_path_str =
      joinWithPrefix (include_paths, "-I", "");
    std::string run_path_str =
      joinWithPrefix (run_paths, "-L", "");
    std::string run_lib_str =
      joinWithPrefix (run_libs, "-l", "");
    std::string rpath_str = strcmp (SYSTEM_NAME, "Darwin") ?
      "" : joinWithPrefix (module_paths, "-rpath", "");

    std::string input_link_file_str =
      joinWithPrefix (input_link_files, " ", "");
    for (std::vector<std::string>::iterator b = so_paths.begin (),
                                            e = so_paths.end ();
         b != e; input_link_file_str += " " + (* b ++));

    // compose the compiler/linker command and execute it
    const char *aux = getenv ("DALE_CC_FLAGS");  // auxiliary options
    std::string compile_cmd = DALE_CC;
    if (no_stdlib) compile_cmd += " --nostdlib";
    if (no_linking) compile_cmd += " -c";
    else compile_cmd += strcmp (SYSTEM_NAME, "Darwin")
           ? " -Wl,--gc-sections" : "";
    compile_cmd += run_path_str + rpath_str;
    if (no_linking) compile_cmd += run_lib_str;
    compile_cmd += " " + intermediate_output_path;
    if (! no_linking)
      compile_cmd += input_link_file_str + run_lib_str + " -lm";
    compile_cmd += " -o " + output_path;
    if (aux) compile_cmd += " ", compile_cmd += aux;

    if (aux) std::cerr << "Going to run: " << compile_cmd << std::endl,
               fflush (stderr);  // show it immediately

    int status = system (compile_cmd.c_str ());
    if (status != 0) {
        if (debug) {
          std::cerr << compile_cmd << std::endl;
        }
        error(DALE_CC " failed");
    }

    status = remove(intermediate_output_path.c_str());
    if (status != 0) {
        if (debug) {
          std::cerr << intermediate_output_path << std::endl;
        }
        error("unable to remove temporary file");
    }

    return 0;
}
