#ifndef DALE_GENERATOR
#define DALE_GENERATOR

#include <vector>
#include <set>
#include <map>
#include <string>

#include "../Element/Label/Label.h"
#include "../Context/Context.h"
#include "../Element/Function/Function.h"
#include "../Element/Variable/Variable.h"
#include "../Element/Type/Type.h"
#include "../Element/Struct/Struct.h"
#include "../ParseResult/ParseResult.h"
#include "../Parser/Parser.h"
#include "../Node/Node.h"
#include "../ErrorReporter/ErrorReporter.h"
#include "../Element/Enum/Enum.h"
#include "../TypeRegister/TypeRegister.h"
#include "../UnitStack/UnitStack.h"

#define DECIMAL_RADIX 10

#define STRTOUL_FAILED(ret, str, end) \
    (((((ret) == ULONG_MAX || ((ret) == 0)) && (errno == ERANGE)) \
        || (((ret) == 0) && ((str) == (end)))))

namespace llvm {
    class Linker;
    class Module;
}

struct DNode
{
    int    is_list;
    char  *token_str;
    DNode *list_node;
    DNode *next_node;
    int    begin_line;
    int    begin_column;
    int    end_line;
    int    end_column;
    int    macro_begin_line;
    int    macro_begin_column;
    int    macro_end_line;
    int    macro_end_column;
    const char  *filename;
};

struct PoolNode
{
    void     *value;
    PoolNode *next_node;
    PoolNode *last_node;
};

int pool_free(PoolNode *pool_node);

struct MContext
{
    int      arg_count;
    PoolNode *pool_node;
    void     *generator;
};

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
    std::set<std::string> *included_once_tags;
    std::set<std::string> *included_modules;
    std::set<std::string> *cto_modules;
    std::string module_name;
    int cto;
    int set_module_name;
    int debug;

    std::vector<std::string> *so_paths_g;

    void parseDefine(Node *top);
    void addVoidPointerType(void);
    void addVarargsFunctions(void);
    void parseModuleName(Node *top);
    void parseImport(Node *top);
    int removeUnneededForms(
        Context *mynewcontext,
        std::set<std::string> *forms_set,
        std::set<std::string> *found_forms
    );

    void parseGlobalVariable(const char *name, Node *n);
    void parseMacroDefinition(const char *name, Node *n);
    void parseEnumDefinition(const char *name, Node *n);
    int parseFunctionBody(Element::Function *dfn,
                          llvm::Function *fn, Node *n, int skip, int is_anonymous);
    llvm::Value *coerceValue(llvm::Value *from_value,
                             Element::Type *from_type,
                             Element::Type *to_type,
                             llvm::BasicBlock *block);
    llvm::GlobalValue::LinkageTypes daleToLLVMLinkage(int linkage);
    llvm::Constant *parseLiteral(Element::Type *type, Node *n, int *size);

    void regetPointersForFVDM(Context *newctx);
    void regetPointersForDM(Context *newctx);

    Node *parseSetv(Node *n);

    Node *typeToIntNode(Element::Type *type);


    void removeMacroTemporaries(void);

    void deleteDNode(DNode *dnode);

    Parser *newParser(FILE *new_fp, const char *filename);

    bool doCast(llvm::BasicBlock *block,
                        llvm::Value *value,
                        Element::Type *from_type,
                        Element::Type *to_type,
                        Node *n,
                        int implicit, ParseResult *pr);




    int addOpaqueStruct(const char *name, Node *top,
                        int linkage, int must_init);
    void popErrors(int original_count);
    bool parseExistsMacro(DNode *dnode);

    void reportErrorExternal(DNode *dnode, char *str);

    Node *WrapNode(Node *n);
    void addMacroPosition(Node *n, Node *mac_node);

    bool typeToStringExternal(DNode *dnode, char *buf);

    int parseStructLinkage(Node *n);

    llvm::Constant *parseLiteralElement(Node *top,
                                        char *thing,
                                        Element::Type
                                        *type, int *size);


    int addVariable(const char *name,
                    Element::Type *type,
                    llvm::Constant *init,
                    bool ignore_if_present = false);


public:
    int addIncludePath(char *filename);
    int addModulePath(char *filename);

    int makeTemporaryGlobalFunction(
        std::vector<DeferredGoto*> *dgs,
        std::map<std::string, Element::Label*> *mls
    );
    void removeTemporaryGlobalFunction(int,
                                       std::vector<DeferredGoto*> *dgs,
                                       std::map<std::string, Element::Label*> *mls
                                      );
    Node  *DNodeToIntNode(DNode *dnode);
    DNode *IntNodeToDNode(Node  *node);
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

    bool copyWithSetfIfApplicable(
        Element::Function *dfn,
        ParseResult *pr, ParseResult *pr2);

    void setPoolfree();

    int prefunction_ctx_index;
   
    bool scopeClose(Element::Function *dfn,
                   llvm::BasicBlock *block,
                   llvm::Value *no_destruct);
    bool destructIfApplicable(ParseResult *pr, llvm::IRBuilder<> *builder, 
                              ParseResult *pr2, bool value_is_ptr = false);
    Context               *ctx;
    Node *parseOptionalMacroCall(Node *n);
    llvm::Module          *mod;
    bool is_x86_64;
    llvm::Value *IntNodeToStaticDNode(Node *node, llvm::Value *next_node);
    Element::Type *type_pdnode;
    void parseStructDefinition(const char *name, Node *n);
    bool parseFuncallInternal(
        Element::Function *dfn,
        Node *n,
        bool getAddress,
        ParseResult *fn_ptr,
        int skip,
        std::vector<llvm::Value*> *extra_call_args
    , ParseResult *pr);

    bool parseArrayLiteral(Element::Function *dfn,
                                   llvm::BasicBlock *block,
                                   Node *n,
                                   const char *name,
                                   Element::Type *array_type,
                                   bool getAddress,
                                   int *size, ParseResult *pr);

    int parseInteger(Node *n);

    std::vector<dale::Element::Function*> global_functions;
    std::vector<llvm::BasicBlock*>        global_blocks;

    dale::Element::Function *global_function;
    llvm::BasicBlock        *global_block;

    /* Function name lists will be stored here by fnByArgsCount, keyed on
     * the stringification of of the parameter types. This map will, in
     * turn, be used by fnByArgsName. */
    std::map<std::string, std::vector<std::string>*> fn_by_args;

    void parseArgument(Element::Variable *var, Node *n,
                       bool allow_anon_structs,
                       bool allow_bitfields);
    bool parseEnumLiteral(
        llvm::BasicBlock *block,
        Node *n,
        Element::Enum *myenum,
        Element::Type
        *myenumtype,
        Element::Struct
        *myenumstructtype,
        bool getAddress, ParseResult *pr);
    llvm::Constant *parseLiteral1(Element::Type *type, Node *n, int *size);
    int getUnusedVarname(std::string *mystr);

    Namespace *prefunction_ns;
    void parseFunction(const char *name, Node *n,
                       Element::Function **new_function,
                       int override_linkage,
                       int is_anonymous);
    bool parseStructLiteral(Element::Function *dfn,
                                    llvm::BasicBlock *block,
                                    Node *n,
                                    const char *name,
                                    Element::Struct *str,
                                    Element::Type *structtype,
                                    bool getAddress, ParseResult *pr);
    Node *parseMacroCall(
        Node *n,
        const char *name,
        Element::Function *macro_to_call);
    bool parseFunctionCall(Element::Function *dfn,
                                   llvm::BasicBlock *block, Node *n, const char* fn_name,
                                   bool getAddress, bool
                                   prefixed_with_core,
                                   Element::Function
                                   **macro_to_call, ParseResult *pr);
    int  parseTopLevel(Node *top);
    Parser                *prsr;
    llvm::Linker          *linker;
    std::string current_once_tag;
    llvm::ExecutionEngine *ee;
    bool no_add_common_declarations;
    bool no_drt;
    void setPdnode();
    char *inc_paths[100];
    int inc_path_count;
    void addCommonDeclarations(void);
    UnitStack *unit_stack;
    int addDaleModule(Node *n, const char *module_name,
                      std::vector<const char*> *import_forms);

};
}

#endif
