#ifndef DALE_GENERATOR
#define DALE_GENERATOR

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cassert>
#include <cerrno>
#include <vector>
#include <set>
#include <map>
#include <string>
#include <sys/stat.h>

#include "../Element/Label/Label.h"
#include "../Lexer/Lexer.h"
#include "../Context/Context.h"
#include "../Type/Type.h"
#include "../Linkage/Linkage.h"
#include "../Element/Function/Function.h"
#include "../Element/Variable/Variable.h"
#include "../Element/Type/Type.h"
#include "../Element/Struct/Struct.h"
#include "../ParseResult/ParseResult.h"
#include "../Parser/Parser.h"
#include "../Node/Node.h"
#include "../Utils/Utils.h"
#include "../ErrorReporter/ErrorReporter.h"
#include "../Element/Enum/Enum.h"
#include "../BasicTypes/BasicTypes.h"

#include "llvm/LLVMContext.h"
#include "llvm/Module.h"
#include "llvm/LinkAllPasses.h"
#include "llvm/Linker.h"
#include "llvm/Function.h"
#include "llvm/PassManager.h"
#include "llvm/CallingConv.h"
#include "llvm/Analysis/Verifier.h"
#include "llvm/Assembly/PrintModulePass.h"
#include "llvm/Support/IRBuilder.h"
#include "llvm/Support/TypeBuilder.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/ExecutionEngine/ExecutionEngine.h"
#include "llvm/ExecutionEngine/JIT.h"
#include "llvm/ExecutionEngine/Interpreter.h"
#include "llvm/Support/TargetSelect.h"
#include "llvm/Target/TargetData.h"
#include "llvm/ValueSymbolTable.h"
#include "llvm/ADT/StringRef.h"

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
    Parser                *prsr;
    Context               *ctx;
    ErrorReporter         *erep;
    llvm::Module          *mod;

    std::vector<llvm::Module *> *modules;
    std::map<std::string, llvm::Module*> *dtm_modules;
    std::map<std::string, llvm::Module*> *dtm_nm_modules;
    std::vector<Parser *>       *parsers;
    std::vector<Context *>      *contexts;
    std::vector<llvm::Linker *> *linkers;

    llvm::ExecutionEngine *ee;
    llvm::Linker          *linker;

    std::vector<DeferredGoto *> *defgotos;
    std::multiset<ino_t> *included_inodes;
    std::set<std::string> *included_once_tags;
    std::set<std::string> *included_modules;
    std::set<std::string> *cto_modules;
    std::string current_once_tag;
    std::string module_name;
    int cto;
    int set_module_name;
    int debug;

    std::vector<std::string> *so_paths_g;

    int  parseTopLevel(Node *top);
    void parseDefine(Node *top);
    void parseFunction(const char *name, Node *n,
                       Element::Function **new_function,
                       int override_linkage,
                       int is_anonymous);
    void addCommonDeclarations(void);
    void addVoidPointerType(void);
    void addVarargsFunctions(void);
    void parseInclude(Node *top);
    void parseModuleName(Node *top);
    void parseImport(Node *top);
    int removeUnneededForms(
        Context *mynewcontext,
        std::set<std::string> *forms_set,
        std::set<std::string> *found_forms
    );

    int addDaleModule(Node *n, const char *module_name,
                      std::vector<const char*> *import_forms);
    void parseArgument(Element::Variable *var, Node *n,
                       bool allow_anon_structs,
                       bool allow_bitfields);
    int parseLinkage(Node *n);
    int parseInteger(Node *n);
    int getUnusedVarname(std::string *mystr);
    void parseGlobalVariable(const char *name, Node *n);
    void parseStructDefinition(const char *name, Node *n);
    void parseMacroDefinition(const char *name, Node *n);
    void parseEnumDefinition(const char *name, Node *n);
    Element::Type *parseType(Node *top, bool
                             allow_anon_structs,
                             bool allow_bitfields);
    int parseFunctionBody(Element::Function *dfn,
                          llvm::Function *fn, Node *n, int skip, int is_anonymous);
    llvm::Type *toLLVMType(Element::Type *type, Node *n, bool allow_non_first_class, bool externally_defined = false);
    llvm::Type *toLLVMType(Element::Type *type, Node *n);
    llvm::Value *coerceValue(llvm::Value *from_value,
                             Element::Type *from_type,
                             Element::Type *to_type,
                             llvm::BasicBlock *block);
    llvm::GlobalValue::LinkageTypes daleToLLVMLinkage(int linkage);
    llvm::Constant *parseLiteral(Element::Type *type, Node *n, int *size);
    llvm::Constant *parseLiteral1(Element::Type *type, Node *n, int *size);

    ParseResult *parseReturn(Element::Function *dfn, llvm::BasicBlock *block,
                             Node *n, bool getAddress, bool
                             prefixed_with_core);
    ParseResult *parseFunctionBodyInstrInternal(Element::Function *dfn,
            llvm::BasicBlock *block,
            Node *n,
            bool getAddress,
            Element::Type *wanted_type);
    ParseResult *parseFunctionBodyInstr(Element::Function *dfn,
                                        llvm::BasicBlock *block,
                                        Node *n,
                                        bool getAddress,
                                        Element::Type *wanted_type);

    ParseResult *parseSetf(Element::Function *dfn, llvm::BasicBlock *block, Node *n,
                           bool getAddress, bool
                           prefixed_with_core);
    ParseResult *parseDereference(Element::Function *dfn, llvm::BasicBlock *block, Node *n,
                                  bool getAddress, bool
                                  prefixed_with_core);
    ParseResult *parseSubtract(Element::Function *dfn, llvm::BasicBlock *block, Node *n,
                               bool getAddress, bool
                               prefixed_with_core);
    ParseResult *parseAddressOf(Element::Function *dfn, llvm::BasicBlock *block, Node *n,
                                bool getAddress, bool
                                prefixed_with_core);
    ParseResult *parseLabel(Element::Function *dfn, llvm::BasicBlock *block, Node *n,
                            bool getAddress, bool
                            prefixed_with_core);
    ParseResult *parseGoto(Element::Function *dfn, llvm::BasicBlock *block, Node *n,
                           bool getAddress, bool
                           prefixed_with_core);
    ParseResult *parseIf(Element::Function *dfn, llvm::BasicBlock *block, Node *n,
                         bool getAddress, bool
                         prefixed_with_core);
    ParseResult *parseInFunctionDefine(Element::Function *dfn,
                                       llvm::BasicBlock
                                       *block, Node *n, bool
                                       getAddress, bool
                                       prefixed_with_core);
    ParseResult *parseFunctionCall(Element::Function *dfn,
                                   llvm::BasicBlock *block, Node *n, const char* fn_name,
                                   bool getAddress, bool
                                   prefixed_with_core,
                                   Element::Function
                                   **macro_to_call);
    ParseResult *parseSref(Element::Function *dfn,
                           llvm::BasicBlock *block, Node *n,
                           bool getAddress, bool
                           prefixed_with_core);
    ParseResult *parseAref(Element::Function *dfn,
                           llvm::BasicBlock *block, Node *n,
                           bool getAddress, bool
                           prefixed_with_core);
    ParseResult *parseNull(Element::Function *dfn,
                           llvm::BasicBlock *block, Node *n,
                           bool getAddress, bool
                           prefixed_with_core);

    ParseResult *parseDo(Element::Function *dfn,
                         llvm::BasicBlock *block,
                         Node *n,
                         bool getAddress, bool
                         prefixed_with_core);

    ParseResult *parseFuncall(Element::Function *dfn,
                              llvm::BasicBlock *block,
                              Node *n,
                              bool getAddress, bool
                              prefixed_with_core);
    ParseResult *parseCast(Element::Function *dfn,
                           llvm::BasicBlock *block,
                           Node *n,
                           bool getAddress, bool
                           prefixed_with_core);

    ParseResult *parsePtrEquals(Element::Function *dfn,
                                llvm::BasicBlock *block, Node *n, bool getAddress, bool
                                prefixed_with_core);
    ParseResult *parsePtrAdd(Element::Function *dfn,
                             llvm::BasicBlock *block, Node *n, bool getAddress, bool
                             prefixed_with_core);
    ParseResult *parsePtrSubtract(Element::Function *dfn,
                                  llvm::BasicBlock *block, Node *n, bool getAddress, bool
                                  prefixed_with_core);
    ParseResult *parsePtrLessThan(Element::Function *dfn,
                                  llvm::BasicBlock *block, Node *n, bool getAddress, bool
                                  prefixed_with_core);
    ParseResult *parsePtrLessThanOrEqualTo(Element::Function
                                           *dfn, llvm::BasicBlock *block, Node *n, bool getAddress,
                                           bool prefixed_with_core);
    ParseResult *parsePtrMoreThan(Element::Function *dfn,
                                  llvm::BasicBlock *block, Node *n, bool getAddress, bool
                                  prefixed_with_core);
    ParseResult *parsePtrMoreThanOrEqualTo(Element::Function
                                           *dfn, llvm::BasicBlock *block, Node *n, bool getAddress,
                                           bool prefixed_with_core);


    Node *parseMacroCall(
        Node *n,
        const char *name,
        Element::Function *macro_to_call);

    void parseNamespace(Node *top);
    void parseUsingNamespaceTopLevel(Node *top);

    ParseResult *parseVaArg(Element::Function *dfn,
                            llvm::BasicBlock *block,
                            Node *n,
                            bool getAddress, bool
                            prefixed_with_core);


    ParseResult *parseSizeof(Element::Function *dfn,
                             llvm::BasicBlock *block,
                             Node *n,
                             bool getAddress, bool
                             prefixed_with_core);


    ParseResult *parseUsingNamespace(Element::Function *dfn,
                                     llvm::BasicBlock *block,
                                     Node *n,
                                     bool getAddress, bool
                                     prefixed_with_core);
    ParseResult *parseNewScope(Element::Function *dfn,
                               llvm::BasicBlock *block,
                               Node *n,
                               bool getAddress, bool
                               prefixed_with_core);

    void regetPointersForFVDM(Context *newctx);
    void regetPointersForDM(Context *newctx);

    Node *parseSetv(Node *n);

    Node *typeToIntNode(Element::Type *type);

    Node *parseOptionalMacroCall(Node *n);

    void removeFluff(void);

    void deleteDNode(DNode *dnode);

    Parser *newParser(FILE *new_fp, const char *filename);

    ParseResult *doCast(llvm::BasicBlock *block,
                        llvm::Value *value,
                        Element::Type *from_type,
                        Element::Type *to_type,
                        Node *n,
                        int implicit = 0);

    ParseResult *getAlignmentofType(llvm::BasicBlock *block,
                                    Element::Type *type);

    ParseResult *getSizeofType(llvm::BasicBlock *block,
                               Element::Type *type);
    int assertArgNums(const char *form_name,
                      Node *n,
                      int min_args,
                      int max_args);
    int assertArgIsList(const char *form_name,
                        Node *n,
                        const char *arg_number);
    int assertArgIsAtom(const char *form_name,
                        Node *n,
                        const char *arg_number);

    int assertTypeEquality(const char *form_name,
                           Node *n,
                           Element::Type *got,
                           Element::Type *expected,
                           int ignore_arg_constness = 0);


    int assertAtomIsSymbol(const char *form_name,
                           Node *n,
                           const char *arg_number);
    int assertAtomIsStringLiteral(const char *form_name,
                                  Node *n,
                                  const char *arg_number);

    int assertIsPointerType(const char *form_name,
                            Node *n,
                            Element::Type *type,
                            const char *arg_number);

    int assertIsPointerOrIntegerType(const char *form_name,
                                     Node *n,
                                     Element::Type *type,
                                     const char *arg_number);
    int assertIsIntegerType(const char *form_name,
                            Node *n,
                            Element::Type *type,
                            const char *arg_number);

    int addOpaqueStruct(const char *name, Node *top,
                        int linkage, int must_init);
    void popErrors(int original_count);
    bool parseExistsMacro(DNode *dnode);

    ParseResult *parseArrayLiteral(Element::Function *dfn,
                                   llvm::BasicBlock *block,
                                   Node *n,
                                   const char *name,
                                   Element::Type *array_type,
                                   bool getAddress,
                                   int *size);

    ParseResult *parseStructLiteral(Element::Function *dfn,
                                    llvm::BasicBlock *block,
                                    Node *n,
                                    const char *name,
                                    Element::Struct *str,
                                    Element::Type *structtype,
                                    bool getAddress);

    void reportErrorExternal(DNode *dnode, char *str);

    Node *WrapNode(Node *n);
    void addMacroPosition(Node *n, Node *mac_node);

    ParseResult *parseEnumLiteral(
        llvm::BasicBlock *block,
        Node *n,
        Element::Enum *myenum,
        Element::Type
        *myenumtype,
        Element::Struct
        *myenumstructtype,
        bool getAddress);

    ParseResult *parseGetDNodes(Element::Function *dfn,
                                llvm::BasicBlock *block,
                                Node *n,
                                bool getAddress, bool
                                prefixed_with_core);


    ParseResult *parseNullPtr(Element::Function *dfn,
                              llvm::BasicBlock *block,
                              Node *n,
                              bool getAddress, bool
                              prefixed_with_core);



    bool typeToStringExternal(DNode *dnode, char *buf);

    llvm::ConstantInt *getNativeInt(int n);

    llvm::ConstantInt *getConstantInt(
        llvm::IntegerType *type,
        const char *numstr
    );
    size_t getSizeofTypeImmediate(Element::Type *type);

    int mySizeToRealSize(int n);

    int parseStructLinkage(Node *n);
    int parseEnumLinkage(Node *n);

    ParseResult *getOffsetofType(llvm::BasicBlock *block,
                                 Element::Type *type,
                                 const char *field_name,
                                 int index);

    llvm::Value *IntNodeToStaticDNode(Node *node, llvm::Value *next_node);
    ParseResult *parseOffsetof(Element::Function *dfn,
                               llvm::BasicBlock *block,
                               Node *n,
                               bool getAddress, bool
                               prefixed_with_core);

    size_t getOffsetofTypeImmediate(Element::Type *type,
                                    const char *field_name, int index);

    llvm::Constant *parseLiteralElement(Node *top,
                                        char *thing,
                                        Element::Type
                                        *type, int *size);


    ParseResult *parseAlignmentof(Element::Function *dfn,
                                  llvm::BasicBlock *block,
                                  Node *n,
                                  bool getAddress, bool
                                  prefixed_with_core);

    ParseResult *parseArrayOf(Element::Function *dfn,
                              llvm::BasicBlock *block,
                              Node *n,
                              bool getAddress, bool
                              prefixed_with_core);
    Node *parseArrayDeref(Node *n);
    Node *parseStructDeref(Node *n);
    Node *parseDerefStruct(Node *n);
    Node *parseDerefStructDeref(Node *n);

    void removeMacrosAndCTOFunctions(Context *myctx, int reget_pointers);
    int addVariable(const char *name,
                    Element::Type *type,
                    llvm::Constant *init,
                    bool ignore_if_present = false);

    ParseResult *parseFuncallInternal(
        Element::Function *dfn,
        Node *n,
        bool getAddress,
        ParseResult *fn_ptr,
        int skip,
        std::vector<llvm::Value*> *extra_call_args
    );

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
    bool existsFunction(DNode *dnode);
    bool hasErrors(DNode *dnode);
    bool mustInit(DNode *dnode);
    bool isConst(DNode *dnode);
    int fnByArgsCount(DNode *dnode);
    const char* fnByArgsName(DNode *dnode, int acount);
    DNode *typeOf(DNode *dnode);
    const char *printfLength(DNode *dnode);
    bool existsVariable(DNode *dnode);
    bool existsType(DNode *dnode);
    bool existsMacro(DNode *dnode);
    bool existsMacroExact(DNode *dnode);
    bool typeToString(DNode *dnode, char *buf);
    void reportError(DNode *dnode, char *str);
    DNode *codomain(DNode *dnode);
    int structMemberCount(DNode *struct_name_node);
    int struct_2D_member_2D_count(DNode *struct_name_node);
    DNode *input_2D_type(DNode *fn_name, int arg_count);
    DNode *inputType(DNode *fn_name_nd, int arg_count);
    DNode *structMemberType(DNode *struct_name_node,
                            int arg_count);
    DNode *struct_2D_member_2D_type(DNode *struct_name_node,
                                    int arg_count);
    const char *structMemberName(DNode *struct_name_node, int arg_count);
    const char *struct_2D_member_2D_name(DNode *struct_name_node, int arg_count);
    bool typeToDisplayString(DNode *dnode, char *buf);
    int type_2D_to_2D_display_2D_string(DNode *dnode, char
                                        *buf);
    int arity(DNode *fn_name);

    ParseResult *destructIfApplicable(ParseResult *pr, llvm::IRBuilder<> *builder);
    ParseResult *copyWithSetfIfApplicable(
        Element::Function *dfn,
        ParseResult *pr);

    void setPdnode();

    int scopeClose(Element::Function *dfn,
                   llvm::BasicBlock *block,
                   llvm::Value *no_destruct);
    int is_2D_char_2D_type(DNode *dnode);
    int is_2D_integer_2D_type(DNode *dnode);
    int is_2D_signed_2D_integer_2D_type(DNode *dnode);
    int is_2D_unsigned_2D_integer_2D_type(DNode *dnode);
    int is_2D_floating_2D_point_2D_type(DNode *dnode);
    int is_2D_pointer_2D_type(DNode *dnode);
    int is_2D_pointer_2D_to_2D_type(DNode *dnode, DNode *pointee_type);
    int types_2D_equal(DNode *T1, DNode *T2);
    DNode *pointee_2D_type(DNode *dnode);
    bool isCharType(DNode *dnode);
    bool isIntegerType(DNode *dnode);
    bool isSignedIntegerType(DNode *dnode);
    bool isUnsignedIntegerType(DNode *dnode);
    bool isFloatingPointType(DNode *dnode);
    bool isPointerType(DNode *dnode);
    bool isPointerToType(DNode *dnode, DNode *pointee_type);
    bool typesEqual(DNode *T1, DNode *T2);
    DNode *pointeeType(DNode *dnode);
    int prefunction_ctx_index;
    bool hasRelevantDestructor(ParseResult *pr);
};
};

#endif
