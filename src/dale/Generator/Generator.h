#ifndef DALE_GENERATOR
#define DALE_GENERATOR

#include <vector>
#include <set>
#include <map>
#include <string>

#include "../Label/Label.h"
#include "../Context/Context.h"
#include "../Function/Function.h"
#include "../Variable/Variable.h"
#include "../Type/Type.h"
#include "../Struct/Struct.h"
#include "../ParseResult/ParseResult.h"
#include "../Parser/Parser.h"
#include "../Node/Node.h"
#include "../ErrorReporter/ErrorReporter.h"
#include "../Enum/Enum.h"
#include "../TypeRegister/TypeRegister.h"
#include "../Units/Units.h"
#include "../llvm_IRBuilder.h"

namespace llvm {
    class Linker;
    class Module;
}

namespace dale
{
enum Produce {
    IR,
    BitCode,
    ASM
};

typedef std::vector<Node*> symlist;

class Generator
{
private:
    NativeTypes           *nt;
    TypeRegister          *tr;
    ErrorReporter         *erep;

    std::map<std::string, llvm::Module*> *dtm_modules;
    std::map<std::string, std::string>   *dtm_nm_modules;

    std::set<std::string> *included_modules;
    std::set<std::string> *cto_modules;
    int debug;

public:
    Generator();
    ~Generator();
    int run(std::vector<const char *> *filenames,
            std::vector<const char *> *bc_files,
            FILE *outfile, int produce, int optlevel,
            int remove_macros, char *module_name,
            int no_acd,
            std::vector<std::string> *so_paths,
            int nostrip,
            int static_mods_all,
            std::vector<const char *> *static_modules,
            std::vector<const char *> *cto_modules,
            int enable_cto,
            int debug,
            int noalways,
            std::vector<const char *> *compile_libs_sv,
            std::vector<const char *> *include_paths_sv,
            std::vector<const char *> *module_paths_sv);

    llvm::ExecutionEngine *ee;
    bool no_add_common_declarations;
    bool no_drt;
    Units *units;
    Unit *getUnit(void);
};
}

#endif
