/* emacs: -*- C++ -*- */
/* vim: set sw=4 ts=4: */
/*
 * BindingsConverter.h
 * Implements an AST Consumer that outputs clay bindings
 *
 */

#include <iostream>
#include <vector>
#include <map>
#include <set>
using namespace std;

#include "clang/AST/ASTConsumer.h"
#include "clang/AST/DeclObjC.h"
#include "clang/Basic/Diagnostic.h"
#include "clang/Frontend/TextDiagnosticPrinter.h"
#include "clang/Frontend/DiagnosticOptions.h"
using namespace clang;


class BindingsConverter : public ASTConsumer, public TextDiagnosticPrinter {
public:
    BindingsConverter(
        ostream& out,
        clang::DiagnosticOptions const &diagOpts,
        std::string const &lang,
        std::vector<string> const &match,
        std::vector<string> const &import
    );

private:
    string allocateName(const string &base);
    string convertBuiltinType(const BuiltinType *type);
    string convertFPType(const FunctionNoProtoType *type);
    string convertFPType(const FunctionProtoType *type);
    string convertObjcType(const clang::Type *type);
    string convertType(const clang::Type *type);
    string convertIdent(const string &cIdent);

    bool isDaleKeyword(const string &cIdent);

    void allocateObjcMethodSelector(ObjCMethodDecl *method);
    void allocateObjcPropertySelector(ObjCPropertyDecl *property);

    template<typename MethodIterator>
    void generateObjCClassMethods(string const& className, MethodIterator begin, MethodIterator end);

public :
    virtual void Initialize(ASTContext &astc);
    virtual void HandleTopLevelDecl(DeclGroupRef DG);
    void generate();

    //virtual void HandleDiagnostic(clang::Diagnostic::Level level, const clang::DiagnosticInfo &info);

private :
    void generateDecl(Decl *decl);
    void generateHeader();
    void generateObjC();

    bool declMatches(Decl *decl);

    bool combineObjcType(QualType *inout_a, QualType b);
private :
    ostream& out;
    vector<Decl*> decls;
    map<string, RecordDecl*> recordBodies;
    set<string> allocatedNames;

    map<RecordDecl*, string> anonRecordNames;
    vector<RecordDecl*> pendingAnonRecords;
    map<string, string> recordNames;
    map<string, string> typedefNames;

    struct class_info {
        ObjCInterfaceDecl *classDecl;
        vector<ObjCCategoryDecl *> categoryDecls;

        class_info() : classDecl(NULL) {}
    };

    map<string, class_info> objcClasses;

    struct selector_info {
        QualType resultType;
        bool isVariadic;
        vector<QualType> paramTypes;

        selector_info() : isVariadic(false) {}

        explicit selector_info(ObjCMethodDecl *decl)
            : resultType(decl->getResultType()),
              isVariadic(decl->isVariadic()) {
            for (ObjCMethodDecl::param_iterator i = decl->param_begin();
                    i != decl->param_end();
                    ++i) {
                paramTypes.push_back((*i)->getType());
            }
        }
    };

    map<string, selector_info> objcSelectors;

    set<string> externsDeclared;

    bool succeeded;
    string language;
    vector<string> matchNames;
    vector<string> importNames;

    ASTContext *ast;
};
