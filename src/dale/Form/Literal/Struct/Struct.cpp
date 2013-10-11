#include "Struct.h"
#include "../../../Linkage/Linkage.h"
#include "../../../Error/Error.h"
#include "../../../Operation/Cast/Cast.h"
#include "../../Proc/Inst/Inst.h"

namespace dale { namespace Form { namespace Literal { namespace Struct {
bool 
parse(Generator *gen,
      Element::Function *dfn,
      llvm::BasicBlock *block,
      Node *n,
      const char *struct_name,
      Element::Struct *str,
      Element::Type *structtype,
      bool get_address,
      ParseResult *pr)
{
    Context *ctx = gen->ctx;

    Node *struct_list = n;

    if (!struct_list->is_list) {
        Error *e = new Error(
            ErrorInst::Generator::UnexpectedElement, struct_list,
            "list", "struct initialisers", "atom"
        );
        ctx->er->addError(e);
        return false;
    }

    symlist *slst = struct_list->list;

    std::vector<Node *>::iterator siter = slst->begin();

    llvm::IRBuilder<> builder(block);

    llvm::Value *sp = llvm::cast<llvm::Value>(
                          builder.CreateAlloca(str->type)
                      );

    while (siter != slst->end()) {
        Node *sel = (*siter);
        if (!sel->is_list) {
            Error *e = new Error(
                ErrorInst::Generator::UnexpectedElement, sel,
                "list", "struct initialiser", "atom"
            );
            ctx->er->addError(e);
            return false;
        }
        symlist *sellst = sel->list;
        if (sellst->size() != 2) {
            Error *e = new Error(
                ErrorInst::Generator::UnexpectedElement, sel,
                "list", "struct initialiser", "atom"
            );
            ctx->er->addError(e);
            return false;
        }
        Node *name      = (*sellst)[0];
        Node *namevalue = (*sellst)[1];

        if (!name->is_token) {
            Error *e = new Error(
                ErrorInst::Generator::UnexpectedElement, sel,
                "atom", "struct field name", "list"
            );
            ctx->er->addError(e);
            return false;
        }

        Element::Type *nametype =
            str->nameToType(name->token->str_value.c_str());

        if (!nametype) {
            Error *e = new Error(
                ErrorInst::Generator::FieldDoesNotExistInStruct,
                name, name->token->str_value.c_str(),
                struct_name
            );
            ctx->er->addError(e);
            return false;
        }

        int index = str->nameToIndex(name->token->str_value.c_str());

        std::vector<llvm::Value *> indices;
        stl::push_back2(&indices, ctx->nt->getLLVMZero(),
                        ctx->nt->getNativeInt(index));

        llvm::Value *res =
            builder.CreateGEP(sp,
                              llvm::ArrayRef<llvm::Value*>(indices));

        ParseResult newvalue;
        bool mres =
            Form::Proc::Inst::parse(gen, dfn, block, namevalue, false,
                                   false, NULL, &newvalue);

        if (!mres) {
            return false;
        }
        if (!newvalue.type->isEqualTo(nametype)) {
            if ((nametype->isIntegerType()
                    && newvalue.type->isIntegerType())
                    || (nametype->isFloatingPointType()
                        && newvalue.type->isFloatingPointType())) {
                ParseResult casttemp;
                bool res =
                    Operation::Cast::execute(
                           ctx,
                           newvalue.block,
                           newvalue.value,
                           newvalue.type,
                           nametype,
                           sel,
                           0,
                           &casttemp
                          );
                if (!res) {
                    return false;
                }
                casttemp.copyTo(&newvalue);
            } else {
                std::string expstr;
                std::string gotstr;
                nametype->toStringProper(&expstr);
                newvalue.type->toStringProper(&gotstr);
                Error *e = new Error(
                    ErrorInst::Generator::IncorrectType,
                    name,
                    expstr.c_str(), gotstr.c_str()
                );
                ctx->er->addError(e);
                return false;
            }
        }

        block = newvalue.block;
        builder.SetInsertPoint(block);

        builder.CreateStore(newvalue.value,
                            res);

        ++siter;
    }

    if (get_address) {
        pr->set(block, ctx->tr->getPointerType(structtype), sp);
    } else {
        llvm::Value *final_value = builder.CreateLoad(sp);
        pr->set(block, structtype, final_value);
    }
    return true;
}
}}}}
