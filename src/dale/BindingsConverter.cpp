/* todo: this is a hacked-on version of clay-bindgen - clay is
 * released under 2-clause BSD, so make sure to include that copyright
 * notice. */

#include <iostream>
#include <sstream>
#include <vector>
using namespace std;

#include "llvm/Config/config.h"
#include "llvm/Support/raw_ostream.h"

#include "clang/AST/Expr.h"
#include "clang/AST/ASTConsumer.h"
#include "clang/AST/ASTContext.h"
#include "clang/AST/Decl.h"
#include "clang/AST/DeclGroup.h"
#include "clang/Basic/SourceManager.h"
#include "llvm/ADT/Triple.h"

#include "BindingsConverter.h"

#include <cstdio>

using namespace clang;
using namespace llvm;

/* Assuming 64-bit for the moment. */

int anon_structs = 0;

BindingsConverter::BindingsConverter(
    ostream& out,
    clang::DiagnosticOptions const &diagOpts,
    std::string const &lang,
    std::vector<string> const &match,
    std::vector<string> const &import
)   :   TextDiagnosticPrinter(llvm::errs(), diagOpts),
    out(out), succeeded(true),
    language(lang), matchNames(match), importNames(import),
    ast(NULL)
{ }

string BindingsConverter::allocateName(const string &base)
{
    if (!allocatedNames.count(base)) {
        allocatedNames.insert(base);
        return base;
    }
    unsigned i = 2;
    string s;
    while (true) {
        ostringstream out;
        out << base << i;
        s = out.str();
        if (!allocatedNames.count(s))
            break;
        ++i;
    }
    allocatedNames.insert(s);
    return s;
}

string BindingsConverter::convertBuiltinType(const BuiltinType *type) {
    switch (type->getKind()) {
    case BuiltinType::Void :
        return "void";
    case BuiltinType::Bool :
        return "bool";
    case BuiltinType::Char_U :
        return "char";
    case BuiltinType::UChar :
        return "char";
    case BuiltinType::UShort :
        return "uint16";
    case BuiltinType::UInt :
        return "uint";
    case BuiltinType::ULong :
        return "uint64";
    case BuiltinType::ULongLong :
        return "uint64";

    case BuiltinType::Char_S :
        return "char";
    case BuiltinType::SChar :
        return "char";
    case BuiltinType::Short :
        return "int16";
    case BuiltinType::Int :
        return "int";
    case BuiltinType::Long :
        return "int64";
    case BuiltinType::LongLong :
        return "int64";

    case BuiltinType::Float :
        return "float";
    case BuiltinType::Double :
        return "double";
    case BuiltinType::LongDouble :
        return "long-double";

    default : {
        ostringstream out;
        out << type->getKind();
        out << " " << type << "QWER";
        return "UnsupportedCBuiltinType" + out.str();
    }
    }
}

/* Not used at the moment. */
static const char *getCodePointerConstructor(const FunctionType *ft)
{

    switch (ft->getCallConv()) {
    case CC_X86StdCall :
        return "StdCallCodePointer";
    case CC_X86FastCall :
        return "FastCallCodePointer";
    default :
        return "CCodePointer";
    }
}

string BindingsConverter::convertFPType(const FunctionNoProtoType *type)
{
    const Type *rt = type->getResultType().getTypePtr();
    ostringstream sout;
    sout << "(p (fn ";
    string s = convertType(rt);
    sout << s << " (void)))";
    return sout.str();
}

string BindingsConverter::convertFPType(const FunctionProtoType *type)
{
    ostringstream sout;
    sout << "(p (fn ";
    const Type *rt = type->getResultType().getTypePtr();
    string s = convertType(rt);
    sout << s << " ";

    FunctionProtoType::arg_type_iterator i, e;
    bool first = true;
    sout << "(";
    int has_args = 0;
    int arg_count = 0;
    char buf[255];
    for (i = type->arg_type_begin(), e = type->arg_type_end(); i != e; ++i) {
        has_args = 1;
        if (!first) {
            sout << " ";
        } else {
            first = false;
        }
        sprintf(buf, "a%d", arg_count++);
        const Type *argType = i->getTypePtr();
        sout << "(" << buf << " " << convertType(argType) << ")";
    }
    if (!has_args) {
        sout << "void";
    }
    sout << ")))";
    return sout.str();
}

string BindingsConverter::convertType(const Type *type)
{
    const Type *ctype = type->getCanonicalTypeUnqualified().getTypePtr();
    if ((type->getTypeClass() != Type::Typedef) &&
            ctype->getTypeClass() == Type::Builtin) {
        return convertBuiltinType((const BuiltinType *)ctype);
    }

    if (ctype->isObjCObjectPointerType() || ctype->isObjCSelType()) {
        fprintf(stderr, "does not support objective-c\n");
        abort();
    }

    switch (type->getTypeClass()) {
    case Type::Typedef : {
        const TypedefType *t = (const TypedefType *)type;
        TypedefDecl *decl = t->getDecl();
        string name = decl->getName().str();
        if (name == "size_t") {
            return "size";
        } else if (name == "ptrdiff_t") {
            return "ptrdiff";
        } else if (ctype->getTypeClass() == Type::Builtin) {
            return convertBuiltinType((const BuiltinType *)ctype);
        }
        const string &outName = typedefNames[name];
        assert(!outName.empty());
        string temp;
        temp.append("(").append(outName).append(")");
        return temp;
    }
    case Type::Record : {
        const RecordType *t = (const RecordType *)type;
        RecordDecl *decl = t->getDecl();
        if (decl->isUnion()) {
            fprintf(stderr, "Unions are not supported, skipping\n");
        } else {
            string name = decl->getName().str();
            if (name.empty()) {
                if (!anon_structs) {
                    if (!anonRecordNames.count(decl)) {
                        pendingAnonRecords.push_back(decl);
                        string outName = allocateName("UnnamedRecord");
                        anonRecordNames[decl] = outName;
                    }
                    return anonRecordNames[decl];
                } else {
                    std::string sd;
                    sd.append("(struct (");

                    RecordDecl::field_iterator i, e;
                    int index = 0;
                    for (i = decl->field_begin(), e = decl->field_end();
                            i != e; ++i) {
                        FieldDecl *y = *i;
                        string fname = y->getName().str();
                        const Type *t = y->getType().getTypePtr();
                        sd.append("    (");
                        if (fname.empty()) {
                            fprintf(stderr, "Unnamed field\n");
                            abort();
                        }
                        sd.append(convertIdent(fname));
                        if (y->isBitField()) {
                            clang::Expr *wid =
                                y->getBitWidth();
                            int width = wid->EvaluateAsInt(y->getASTContext()).getZExtValue();
                            char buf[255];
                            sprintf(buf, "%d", width);
                            out << " (bf "
                                << convertType(t)
                                << " "
                                << buf
                                << "))";
                        } else {
                            sd.append(" ").append(convertType(t))
                            .append(")\n");
                        }
                        ++index;
                    }
                    sd.append("))\n");

                    return sd;
                }
            }
            if (recordBodies.count(name)) {
                return recordNames[name];
            }
            return "Opaque";
        }
    }
    case Type::Enum :
        return "int";
    case Type::Pointer : {
        const PointerType *t = (const PointerType *)type;
        const Type *pt = t->getPointeeType().getTypePtr();
        const Type *pt2 = pt->getCanonicalTypeUnqualified().getTypePtr();
        switch (pt2->getTypeClass()) {
        case Type::FunctionNoProto :
            return convertFPType((const FunctionNoProtoType *)pt2);
        case Type::FunctionProto :
            return convertFPType((const FunctionProtoType *)pt2);
        default :
            break;
        }
        if (pt->getTypeClass() == Type::Record) {
            const RecordType *rt = (const RecordType *)pt;
            const RecordDecl *decl = rt->getDecl();
            string name = decl->getName().str();
            if (!name.empty() && !recordBodies.count(name)) {
                return "(p void)";
            }
        }
        string inner = convertType(pt);
        return "(p " + inner + ")";
    }
    case Type::ConstantArray : {
        const ConstantArrayType *t = (const ConstantArrayType *)type;
        ostringstream sout;
        sout << "(array-of " << t->getSize().getZExtValue() << " ";
        sout << convertType(t->getElementType().getTypePtr()) << ")";
        return sout.str();
    }
    case Type::IncompleteArray : {
        const IncompleteArrayType *t = (const IncompleteArrayType *)type;
        ostringstream sout;
        sout << "(array-of 0 ";
        sout << convertType(t->getElementType().getTypePtr()) << ")";
        return sout.str();
    }
    case Type::VariableArray : {
        const VariableArrayType *t = (const VariableArrayType *)type;
        ostringstream sout;
        sout << "(array-of 0 ";
        sout << convertType(t->getElementType().getTypePtr()) << ")";
        return sout.str();
    }
    case Type::DependentSizedArray : {
        const DependentSizedArrayType *t = (const DependentSizedArrayType *)type;
        ostringstream sout;
        sout << "(array-of 0 ";
        sout << convertType(t->getElementType().getTypePtr()) << ")";
        return sout.str();
    }
    default : {
        string str = type->getCanonicalTypeInternal().getAsString();
        cerr << "warning: unknown type: " << str << ": " << type->getTypeClass() << '\n';
        return "(unknown-type)";
    }
    }
}

void BindingsConverter::Initialize(ASTContext &astc) {
    ast = &astc;
}

bool BindingsConverter::declMatches(Decl *decl)
{
    if (matchNames.empty())
        return true;

    clang::SourceLocation sloc = decl->getLocation();
    clang::PresumedLoc ploc = ast->getSourceManager().getPresumedLoc(sloc);
    string filename = ploc.getFilename();

    for (vector<string>::const_iterator i = matchNames.begin();
            i != matchNames.end();
            ++i) {
        if (filename.find(*i) != string::npos)
            return true;
    }
    return false;
}

// Handle top level declarations observed in the program
void BindingsConverter::HandleTopLevelDecl(DeclGroupRef DG)
{
    for (DeclGroupRef::iterator it = DG.begin(); it != DG.end(); ++it) {
        Decl *decl = *it;

        bool matches = declMatches(decl);

        if (matches)
            decls.push_back(decl);

        switch (decl->getKind()) {
        case Decl::Record : {
            RecordDecl *x = (RecordDecl *)decl;
            string name = x->getName().str();
            if (!x->isUnion()) {
                if (name.empty()) {
                    string outName = allocateName("UnnamedStruct");
                    anonRecordNames[x] = outName;
                }
                else {
                    assert(!recordBodies.count(name));
                    recordBodies[name] = x;
                    string outName = allocateName(name);
                    recordNames[name] = outName;
                }
            }
            break;
        }
        case Decl::Typedef : {
            TypedefDecl *x = (TypedefDecl *)decl;
            const Type *t = x->getTypeSourceInfo()->getType().getTypePtr();
            if (!t->isFunctionType()) {
                string name = x->getName().str();
                string outName = allocateName(name);
                typedefNames[name] = outName;
            }
            break;
        }
        default :
            break;
        }
    }
}

void BindingsConverter::generate()
{
    if (!succeeded)
        return;

    fprintf(stderr, "%d\n", decls.size());

    generateHeader();
    unsigned i = 0;
    while ((pendingAnonRecords.size() > 0) || (i < decls.size())) {
        if (pendingAnonRecords.size() > 0) {
            vector<RecordDecl*> pending = pendingAnonRecords;
            pendingAnonRecords.clear();
            for (unsigned j = 0; j < pending.size(); ++j)
                generateDecl(pending[j]);
        }
        else if (i < decls.size()) {
            generateDecl(decls[i]);
            ++i;
        }
    }
}

bool BindingsConverter::isDaleKeyword(const string &cIdent) {
    // list from compiler/src/lexer.cpp:121
    static char const * const keywords[] =
    {   "",
        "record", "variant", "instance",
        "procedure", "overload", "external", "alias",
        "static", "callbyname", "lvalue", "rvalue",
        "inline", "enum", "var", "ref", "forward",
        "and", "or", "not", "new",
        "if", "else", "goto", "return", "while",
        "switch", "case", "default", "break", "continue", "for", "in",
        "true", "false", "try", "catch", "throw", NULL
    };

    for (char const * const *k = keywords; *k; ++k)
        if (cIdent == *k)
            return true;
    return false;
}

string BindingsConverter::convertIdent(const string &cIdent) {
    if (isDaleKeyword(cIdent))
        return cIdent + "_";
    else
        return cIdent;
}

void BindingsConverter::generateDecl(Decl *decl)
{
    switch (decl->getKind()) {
    case Decl::Typedef : {
        TypedefDecl *x = (TypedefDecl *)decl;
        string name = x->getName().str();
        const Type *t = x->getTypeSourceInfo()->getType().getTypePtr();
        if (!t->isFunctionType()) {
            string type = convertType(t);
            out << '\n';
            out << "; " << typedefNames[name] << " typedef\n";
            out << "(def " << typedefNames[name] << " \n";
            out << "  (macro extern (void) ";
            std::string stype = convertType(t);
            if (stype.at(0) == '(') {
                stype[0] = ' ';
                stype[stype.length() - 1] = ' ';
                out << "(qq3 " << stype << ")))\n";
            } else {
                out << "(mnfs pool \"" << stype << "\")))\n";
            }
        }
        break;
    }
    case Decl::Enum : {
        EnumDecl *x = (EnumDecl *)decl;
        EnumDecl::enumerator_iterator i = x->enumerator_begin();
        EnumDecl::enumerator_iterator e = x->enumerator_end();
        out << '\n';
        for (; i != e; ++i) {
            EnumConstantDecl *y = *i;
            out << "(def " << y->getName().str() << "\n";
            out << "  (macro extern (void) ";
            out << "    (mnfs pool \"" <<
                y->getInitVal().getZExtValue() << "\")))\n";
        }
        break;
    }
    case Decl::Record : {
        RecordDecl *x = (RecordDecl *)decl;
        string name = x->getName().str();
        if (!x->isUnion()) {
            if (x->isDefinition()) {
                string outName = name.empty() ?
                                 anonRecordNames[x] : recordNames[name];
                out << '\n';
                out << "(def " << outName << " \n";
                out << "  (struct extern (\n";
                RecordDecl::field_iterator i, e;
                int index = 0;
                for (i = x->field_begin(), e = x->field_end(); i != e; ++i) {
                    FieldDecl *y = *i;
                    string fname = y->getName().str();
                    const Type *t = y->getType().getTypePtr();
                    out << "    (";
                    if (fname.empty()) {
                        fprintf(stderr, "Unnamed field\n");
                        abort();
                    }
                    out << convertIdent(fname);
                    anon_structs = 1;
                    if (y->isBitField()) {
                        clang::Expr *wid =
                            y->getBitWidth();
                        int width = wid->EvaluateAsInt(y->getASTContext()).getZExtValue();
                        char buf[255];
                        sprintf(buf, "%d", width);
                        out << " (bf "
                            << convertType(t)
                            << " "
                            << buf
                            << "))";
                    } else {
                        out << " " << convertType(t) << ")\n";
                    }
                    anon_structs = 0;
                    ++index;
                }
                out << ")))\n";
            } else {
                string outName = name.empty() ?
                                 anonRecordNames[x] : recordNames[name];
                out << '\n';
                out << "(def " << outName << " \n";
                out << "  (struct opaque)) \n";
            }
        }
        break;
    }
    case Decl::Function : {
        FunctionDecl *x = (FunctionDecl *)decl;
        string name = x->getName().str();
        if (!x->isThisDeclarationADefinition() &&
                !x->isInlineSpecified() &&
                (x->getStorageClass() == SC_Extern ||
                 x->getStorageClass() == SC_None) &&
                !externsDeclared.count(name))
        {
            externsDeclared.insert(name);
            vector<string> attributes;
            if (x->hasAttr<DLLImportAttr>()) {
                fprintf(stderr, "DLL attributes not handled\n");
                abort();
            }
            if (x->hasAttr<DLLExportAttr>()) {
                fprintf(stderr, "DLL attributes not handled\n");
                abort();
            }
            const Type *t = x->getType().getTypePtr();
            assert(t->isFunctionType());
            const FunctionType *ft = (const FunctionType *)t;
            if (ft->getCallConv() == CC_X86StdCall) {
                fprintf(stderr, "sc not handled\n");
                abort();
            }
            if (ft->getCallConv() == CC_X86FastCall) {
                fprintf(stderr, "fastcall not handled\n");
                abort();
            }
            if (x->hasAttr<AsmLabelAttr>()) {
                const AsmLabelAttr *attr = x->getAttr<AsmLabelAttr>();
                string asmLabel(attr->getLabel());
                //attributes.push_back("\"" + asmLabel + "\"");
                fprintf(stderr, "Ignoring ASM label: %s\n",
                        asmLabel.c_str());
            }

            out << '\n';
            out << "(def " << name << "\n";
            out << "  (fn ";

            if (!attributes.empty()) {
                out << "(attr ";
                for (unsigned i = 0; i < attributes.size(); ++i) {
                    if (i != 0)
                        out << " ";
                    out << attributes[i];
                }
                out << ") ";
            }

            out << "extern-c ";
            const Type *rt = x->getResultType().getTypePtr();
            string s = convertType(rt);
            out << s << " (";

            FunctionDecl::param_iterator i, e;
            int index = 0;
            for (i = x->param_begin(), e = x->param_end(); i != e; ++i) {
                ParmVarDecl *y = *i;
                string pname = y->getName().str();
                if (index != 0) {
                    out << " ";
                }
                out << "(";
                if (pname.empty()) {
                    out << "argument" << index;
                }
                else {
                    out << convertIdent(pname);
                }
                const Type *t = y->getType().getTypePtr();
                out << " " << convertType(t) << ")";
                ++index;
            }
            if (ft->getTypeClass() == Type::FunctionProto) {
                const FunctionProtoType *fpt = (const FunctionProtoType *)ft;
                if (fpt->isVariadic()) {
                    if (index != 0) {
                        out << " ";
                    }
                    out << "...";
                }
            }
            if (index == 0) {
                out << "void";
            }
            out << ")))";
            out << "\n";
        }
        break;
    }
    case Decl::Var : {
        VarDecl *x = (VarDecl *)decl;
        string name = x->getName().str();
        if ((x->getStorageClass() == SC_Extern) &&
                !x->hasInit() &&
                !externsDeclared.count(name))
        {
            externsDeclared.insert(name);
            const Type *t = x->getType().getTypePtr();
            out << '\n';
            out << "(def ";
            vector<string> attributes;
            if (x->hasAttr<DLLImportAttr>()) {
                fprintf(stderr, "DLL attributes not supported\n");
                abort();
            }
            if (x->hasAttr<DLLExportAttr>()) {
                fprintf(stderr, "DLL attributes not supported\n");
                abort();
            }
            if (!attributes.empty()) {
                /* not needed at the moment
                out << "(";
                for (unsigned i = 0; i < attributes.size(); ++i) {
                    if (i != 0)
                        out << ", ";
                    out << attributes[i];
                }
                out << ") ";
                */
            }
            out << name << " (var extern-c " << convertType(t) << "))\n";
        }
        break;
    }
    default :
        break;
    }
}

void BindingsConverter::generateHeader()
{
    out << "; Automatically generated by dale-bindgen\n";
    out << "; language: " << language << "\n";
    out << '\n';
    out << "(import macros)\n";
    if (!importNames.empty()) {
        for (vector<string>::const_iterator i = importNames.begin();
                i != importNames.end();
                ++i) {
            out << "import " << *i << ".*;\n";
        }
        out << '\n';
    }
    /* default things go here */
    out << '\n';
}

/*
void BindingsConverter::HandleDiagnostic(clang::Diagnostic::Level level, const clang::DiagnosticInfo &info)
{
    if (level == Diagnostic::Error || level == Diagnostic::Fatal)
        succeeded = false;
    this->TextDiagnosticPrinter::HandleDiagnostic(level, info);
}
*/

