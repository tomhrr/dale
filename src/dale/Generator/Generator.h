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
#include "../UnitStack/UnitStack.h"
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


    std::vector<DeferredGoto *> *defgotos;
    std::multiset<ino_t> *included_inodes;
    std::set<std::string> *included_modules;
    std::set<std::string> *cto_modules;
    int debug;

    std::vector<std::string> *so_paths_g;

    void addVoidPointerType(void);
    void addVarargsFunctions(void);
    void parseModuleName(Node *top);
    void parseImport(Node *top);
    int removeUnneededForms(
        Context *mynewcontext,
        std::set<std::string> *forms_set,
        std::set<std::string> *found_forms
    );

    void regetPointersForFVDM(Context *newctx);
    void regetPointersForDM(Context *newctx);

    void removeMacroTemporaries(void);

    void popErrors(int original_count);
    void processRetval(Type *et,
                       llvm::BasicBlock *block,
                       ParseResult *pr,
                       std::vector<llvm::Value*> *call_args);

public:
    int addIncludePath(char *filename);
    int addModulePath(char *filename);

    Node  *DNodeToIntNode(DNode *dnode);
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
            int noalways);
    bool addLib(const char *lib_path, int add_to_so_paths,
                int add_nm_to_so_paths);

    int prefunction_ctx_index;
   
    Context               *ctx;
    llvm::Module          *mod;
    bool is_x86_64;
    llvm::Value *IntNodeToStaticDNode(Node *node, llvm::Value *next_node);

    /* Function name lists will be stored here by fnByArgsCount, keyed on
     * the stringification of of the parameter types. This map will, in
     * turn, be used by fnByArgsName. */
    std::map<std::string, std::vector<std::string>*> fn_by_args;

    int getUnusedVarname(std::string *mystr);

    Namespace *prefunction_ns;
    Parser                *prsr;
    llvm::Linker          *linker;
    std::string current_once_tag;
    llvm::ExecutionEngine *ee;
    bool no_add_common_declarations;
    bool no_drt;
    void setPdnode();
    char *inc_paths[100];
    int inc_path_count;
    UnitStack *unit_stack;
    Unit *getUnit(void);
    int addDaleModule(Node *n, const char *module_name,
                      std::vector<const char*> *import_forms);
    std::set<std::string> *included_once_tags;
    int cto;
    int has_defined_extern_macro;
    std::string module_name;
    int set_module_name;
};
}

#endif
