#include "Inst.h"
#include "../../../Generator/Generator.h"
#include "../../../Node/Node.h"
#include "../../../ParseResult/ParseResult.h"
#include "../../../Element/Function/Function.h"
#include "../Token/Token.h"
#include "../../Type/Type.h"
#include "llvm/Function.h"

#include "../Goto/Goto.h"
#include "../If/If.h"
#include "../Label/Label.h"
#include "../Return/Return.h"
#include "../Setf/Setf.h"
#include "../Dereference/Dereference.h"
#include "../Sref/Sref.h"
#include "../AddressOf/AddressOf.h"
#include "../Aref/Aref.h"
#include "../PtrEquals/PtrEquals.h"
#include "../PtrAdd/PtrAdd.h"
#include "../PtrSubtract/PtrSubtract.h"
#include "../PtrLessThan/PtrLessThan.h"
#include "../PtrGreaterThan/PtrGreaterThan.h"
#include "../VaStart/VaStart.h"
#include "../VaEnd/VaEnd.h"
#include "../VaArg/VaArg.h"
#include "../Null/Null.h"
#include "../GetDNodes/GetDNodes.h"
#include "../Def/Def.h"
#include "../NullPtr/NullPtr.h"
#include "../Do/Do.h"
#include "../Cast/Cast.h"
#include "../Sizeof/Sizeof.h"
#include "../Offsetof/Offsetof.h"
#include "../Alignmentof/Alignmentof.h"
#include "../Funcall/Funcall.h"
#include "../UsingNamespace/UsingNamespace.h"
#include "../NewScope/NewScope.h"
#include "../ArrayOf/ArrayOf.h"
#include "../../Macro/ArrayDeref/ArrayDeref.h"
#include "../../Macro/StructDeref/StructDeref.h"
#include "../../Macro/DerefStructDeref/DerefStructDeref.h"
#include "../../Macro/DerefStruct/DerefStruct.h"
#include "../../Macro/Setv/Setv.h"
#include "../../Function/Function.h"
#include "../../Literal/Struct/Struct.h"
#include "../../Literal/Enum/Enum.h"
#include "../../Literal/Array/Array.h"

#define eq(str) !strcmp(t->str_value.c_str(), str)

#define DALE_DEBUG 0

namespace dale
{
namespace Form
{
namespace Proc
{
namespace Inst
{
static int anoncount = 0;

bool parseInternal(Generator *gen,
                   Element::Function *fn,
                   llvm::BasicBlock *block,
                   Node *n,
                   bool get_address,
                   bool prefixed_with_core,
                   Element::Type *wanted_type,
                   ParseResult *pr)
{
    Context *ctx = gen->ctx;

    if (DALE_DEBUG) {
        printf("Called Form::Proc::Inst::parse: ");
        n->print();
        printf("\n");
    }

    gen->global_block = block;

    if (n->is_token) {
        return Form::Proc::Token::parse(
            gen, fn, block, n, get_address, prefixed_with_core,
            wanted_type, pr
        );
    }

    symlist *lst = n->list;

    if (lst->size() == 0) {
        Error *e = new Error(
            ErrorInst::Generator::NoEmptyLists,
            n
        );
        ctx->er->addError(e);
        return false;
    }

    Node *first = (*lst)[0];

    if (!first->is_token) {
        first = gen->parseOptionalMacroCall(first);
        if (!first) {
            return false;
        }
    }

    /* If the first node is a token, and it equals "fn", then
     * create an anonymous function and return a pointer to it. */

    if (first->is_token and !first->token->str_value.compare("fn")) {
        int preindex = ctx->lv_index;

        std::vector<NSNode *> active_ns_nodes = ctx->active_ns_nodes;
        std::vector<NSNode *> used_ns_nodes   = ctx->used_ns_nodes;
        ctx->popUntilNamespace(gen->prefunction_ns);

        char buf[255];
        sprintf(buf, "_anon_%d", anoncount++);
        Element::Function *myanonfn = NULL;
        int error_count = ctx->er->getErrorTypeCount(ErrorType::Error);

        Form::Function::parse(gen, n, buf, &myanonfn, Linkage::Intern, 1);

        int diff = ctx->er->getErrorTypeCount(ErrorType::Error)
                   - error_count;

        if (diff) {
            ctx->active_ns_nodes = active_ns_nodes;
            ctx->used_ns_nodes = used_ns_nodes;
            return false;
        }

        Element::Type *fntype = new Element::Type();
        fntype->is_function = 1;
        fntype->return_type = myanonfn->return_type;

        std::vector<Element::Type *> *parameter_types =
            new std::vector<Element::Type *>;

        std::vector<Element::Variable *>::iterator iter;

        iter = myanonfn->parameter_types->begin();

        while (iter != myanonfn->parameter_types->end()) {
            parameter_types->push_back((*iter)->type);
            ++iter;
        }

        fntype->parameter_types = parameter_types;

        pr->set(
            block,
            ctx->tr->getPointerType(fntype),
            llvm::cast<llvm::Value>(myanonfn->llvm_function)
        );

        std::vector<Element::Variable *> myvars;
        ctx->ns()->getVarsAfterIndex(preindex, &myvars);
        for (std::vector<Element::Variable *>::iterator
                b = myvars.begin(),
                e = myvars.end();
                b != e;
                ++b) {
            (*b)->index = 0;
        }

        ctx->active_ns_nodes = active_ns_nodes;
        ctx->used_ns_nodes = used_ns_nodes;

        return true;
    }

    /* If wanted_type is present and is a struct, then use
     * Form::Literal::Struct::parse, if the first list element is a
     * list. */

    if (wanted_type
            && (wanted_type->struct_name)
            && (!first->is_token)) {

        Element::Struct *str =
            ctx->getStruct(wanted_type->struct_name->c_str(),
                           wanted_type->namespaces);

        if (!str) {
            fprintf(stderr,
                    "Internal error: cannot load struct '%s'.\n",
                    wanted_type->struct_name->c_str());
            abort();
        }

        bool res =
            Form::Literal::Struct::parse(
                               gen, fn, block, n,
                               wanted_type->struct_name->c_str(),
                               str,
                               wanted_type,
                               get_address,
                               pr);
        return res;
    }

    if (!first->is_token) {
        Error *e = new Error(
            ErrorInst::Generator::FirstListElementMustBeAtom,
            n
        );
        ctx->er->addError(e);
        return false;
    }

    dale::Token *t = first->token;

    if (t->type != TokenType::String) {
        Error *e = new Error(
            ErrorInst::Generator::FirstListElementMustBeSymbol,
            n
        );
        ctx->er->addError(e);
        return false;
    }

    /* If the first element matches an enum name, then make an
     * enum literal (a struct literal) from the remainder of the
     * form. */

    Element::Enum *myenum =
        ctx->getEnum(t->str_value.c_str());

    if (myenum) {
        if (lst->size() != 2) {
            goto past_en_parse;
        }
        Element::Struct *myenumstruct =
            ctx->getStruct(t->str_value.c_str());

        if (!myenumstruct) {
            fprintf(stderr,
                    "Internal error: no struct associated "
                    "with enum.\n");
            abort();
        }

        Element::Type *myenumtype =
            Form::Type::parse(gen, (*lst)[0], false, false);

        if (!myenumtype) {
            fprintf(stderr,
                    "Internal error: no type associated "
                    "with enum.\n");
            abort();
        }

        int original_error_count =
            ctx->er->getErrorTypeCount(ErrorType::Error);

        bool res = Form::Literal::Enum::parse(gen, block, (*lst)[1],
                                             myenum,
                                             myenumtype,
                                             myenumstruct,
                                             get_address,
                                             pr);
        if (!res) {
            ctx->er->popErrors(original_error_count);
            goto past_en_parse;
        }
        return true;
    }
past_en_parse:

    /* If the first element matches a struct name, then make a
     * struct literal from the remainder of the form. */

    Element::Struct *mystruct =
        ctx->getStruct(t->str_value.c_str());

    if (mystruct) {
        if (lst->size() != 2) {
            goto past_sl_parse;
        }

        Node *struct_name = (*lst)[0];
        Element::Struct *str =
            ctx->getStruct(t->str_value.c_str());
        if (!str) {
            fprintf(stderr,
                    "Internal error: cannot load struct '%s'.\n",
                    struct_name->token->str_value.c_str());
            abort();
        }

        Element::Type *structtype =
            Form::Type::parse(gen, (*lst)[0], false, false);

        if (!structtype) {
            fprintf(stderr,
                    "Internal error: struct ('%s') type does "
                    "not exist.\n",
                    struct_name->token->str_value.c_str());
            abort();
        }

        int original_error_count =
            ctx->er->getErrorTypeCount(ErrorType::Error);

        bool res = Form::Literal::Struct::parse(gen, fn, block, (*lst)[1],
                                                "asdf",
                                                str,
                                                structtype,
                                                get_address,
                                                pr);
        if (!res) {
            ctx->er->popErrors(original_error_count);
            goto past_sl_parse;
        }
        return true;
    }
past_sl_parse:

    /* If the first element is 'array', and an array type has been
     * requested, handle that specially. */

    if (wanted_type
            && wanted_type->is_array
            && (!strcmp(t->str_value.c_str(), "array"))) {
        int size;
        bool res = Form::Literal::Array::parse(
                                gen,
                                fn, block, n,
                                "array literal",
                                wanted_type,
                                get_address,
                                &size,
                                pr
                            );
        return res;
    }

    /* Check that a macro/function exists with the relevant name.
       This can be checked by passing NULL in place of the types.
       If the form begins with 'core', though, skip this part. If
       any errors occur here, then pop them from the reporter and
       keep going - only if the rest of the function fails, should
       the errors be restored. */

    int error_count =
        ctx->er->getErrorTypeCount(ErrorType::Error);

    Error *backup_error = NULL;

    prefixed_with_core = !(t->str_value.compare("core"));

    if (!prefixed_with_core) {
        Element::Function *fn_exists =
            ctx->getFunction(t->str_value.c_str(), NULL, NULL, 0);
        Element::Function *mac_exists =
            ctx->getFunction(t->str_value.c_str(), NULL, NULL, 1);

        if (fn_exists || mac_exists) {
            /* A function (or macro) with this name exists. Call
             * parseFunctionCall: if it returns a PR, then great.
             * If it returns no PR, but sets macro_to_call, then
             * pass off to parseMacroCall. If it returns no PR,
             * then pop the errors and continue, but only if there
             * is one error, and it's related to an overloaded
             * function not being present. */

            Element::Function *macro_to_call_real;
            Element::Function **macro_to_call = &macro_to_call_real;
            *macro_to_call = NULL;

            bool res = gen->parseFunctionCall(fn, block, n,
                                   t->str_value.c_str(), get_address,
                                   false, macro_to_call, pr);
            if (res) {
                return true;
            }

            if (*macro_to_call) {
                Node *mac_node =
                    gen->parseMacroCall(n, t->str_value.c_str(),
                                   *macro_to_call);
                if (!mac_node) {
                    return false;
                }
                bool res =
                    parse(
                        gen, fn, block, mac_node, get_address, false, wanted_type, pr
                    );

                delete mac_node;

                return res;
            }

            int error_count2 =
                ctx->er->getErrorTypeCount(ErrorType::Error);

            if (error_count2 != (error_count + 1)) {
                return false;
            }
            backup_error = ctx->er->popLastError();
            if (backup_error->getType() != ErrorType::Error) {
                ctx->er->addError(backup_error);
                return false;
            }
            if ((backup_error->instance !=
                    ErrorInst::Generator::OverloadedFunctionOrMacroNotInScope)
                    && (backup_error->instance !=
                        ErrorInst::Generator::OverloadedFunctionOrMacroNotInScopeWithClosest)) {
                ctx->er->addError(backup_error);
                return false;
            }
        }
    }

    if (prefixed_with_core) {
        std::vector<Node *> *temp = new std::vector<Node *>;
        temp->insert(temp->begin(),
                     lst->begin() + 1,
                     lst->end());
        lst = temp;
        n = new Node(temp);
        first = (*lst)[0];

        if (!first->is_token) {
            first = gen->parseOptionalMacroCall(first);
            if (!first) {
                return false;
            }
        }
        if (!first->is_token) {
            Error *e = new Error(
                ErrorInst::Generator::FirstListElementMustBeSymbol,
                n
            );
            ctx->er->addError(e);
            return false;
        }

        t = first->token;
        if (t->type != TokenType::String) {
            Error *e = new Error(
                ErrorInst::Generator::FirstListElementMustBeSymbol,
                n
            );
            ctx->er->addError(e);
            return false;
        }
    }

    /* Core forms (at least at this point). */

    bool (* core_fn)(Generator *gen,
                     Element::Function *fn,
                     llvm::BasicBlock *block,
                     Node *node,
                     bool get_address,
                     bool prefixed_with_core,
                     ParseResult *pr);

    core_fn =
          (eq("goto"))            ? &Form::Proc::Goto::parse
        : (eq("if"))              ? &Form::Proc::If::parse
        : (eq("label"))           ? &Form::Proc::Label::parse
        : (eq("return"))          ? &Form::Proc::Return::parse
        : (eq("setf"))            ? &Form::Proc::Setf::parse
        : (eq("@"))               ? &Form::Proc::Dereference::parse
        : (eq(":"))               ? &Form::Proc::Sref::parse
        : (eq("#"))               ? &Form::Proc::AddressOf::parse
        : (eq("$"))               ? &Form::Proc::Aref::parse
        : (eq("p="))              ? &Form::Proc::PtrEquals::parse
        : (eq("p+"))              ? &Form::Proc::PtrAdd::parse
        : (eq("p-"))              ? &Form::Proc::PtrSubtract::parse
        : (eq("p<"))              ? &Form::Proc::PtrLessThan::parse
        : (eq("p>"))              ? &Form::Proc::PtrGreaterThan::parse
        : (eq("va-arg"))          ? &Form::Proc::VaArg::parse
        : (eq("va-start"))        ? &Form::Proc::VaStart::parse
        : (eq("va-end"))          ? &Form::Proc::VaEnd::parse
        : (eq("null"))            ? &Form::Proc::Null::parse
        : (eq("get-dnodes"))      ? &Form::Proc::GetDNodes::parse
        : (eq("def"))             ? &Form::Proc::Def::parse
        : (eq("nullptr"))         ? &Form::Proc::NullPtr::parse
        : (eq("do"))              ? &Form::Proc::Do::parse
        : (eq("cast"))            ? &Form::Proc::Cast::parse
        : (eq("sizeof"))          ? &Form::Proc::Sizeof::parse
        : (eq("offsetof"))        ? &Form::Proc::Offsetof::parse
        : (eq("alignmentof"))     ? &Form::Proc::Alignmentof::parse
        : (eq("funcall"))         ? &Form::Proc::Funcall::parse
        : (eq("using-namespace")) ? &Form::Proc::UsingNamespace::parse
        : (eq("new-scope"))       ? &Form::Proc::NewScope::parse
        : (eq("array-of"))        ? &Form::Proc::ArrayOf::parse
                                  : NULL;

    if (core_fn) {
        return core_fn(gen, fn, block, n,
                       get_address, prefixed_with_core, pr);
    }

    /* Not core form - look for core macro. */

    Node* (*core_mac)(Context *ctx, Node *n);

    core_mac =   (eq("setv"))   ? &Form::Macro::Setv::parse
               : (eq("@$"))     ? &Form::Macro::ArrayDeref::parse
               : (eq(":@"))     ? &Form::Macro::DerefStruct::parse
               : (eq("@:"))     ? &Form::Macro::StructDeref::parse
               : (eq("@:@"))    ? &Form::Macro::DerefStructDeref::parse
               : NULL;

    if (core_mac) {
        /* Going to assume similarly here, re the error messages. */
        Node *new_node = core_mac(ctx, n);
        if (!new_node) {
            return false;
        }
        return parse(gen, fn, block, new_node,
                                      get_address, false, wanted_type, pr);
    }

    /* Not core form/macro, nor function. If the string token is
     * 'destroy', then treat this as a no-op (because it's
     * annoying to have to check, in macros, whether destroy
     * happens to be implemented over a particular type). If it is
     * not, call pfbi on the first element. If it is a function
     * pointer, then go to funcall. If it is a struct, see if it
     * is a function object and go from there. Don't do any of
     * this if backup_error is set. */

    if (!(t->str_value.compare("destroy"))) {
        pr->set(block, ctx->tr->type_void, NULL);
        return true;
    }

    if (backup_error) {
        ctx->er->addError(backup_error);
        return false;
    }

    int last_error_count =
        ctx->er->getErrorTypeCount(ErrorType::Error);
    ParseResult try_fnp;
    bool res = parse(gen,
                               fn, block, (*lst)[0], get_address,
                               false, wanted_type, &try_fnp
                           );
    if (!res) {
        /* If this fails, and there is one extra error, and the
         * error is 'variable not in scope', then change it to
         * 'not in scope' (it could be intended as either a
         * variable, a macro or a fn). */
        int new_error_count =
            ctx->er->getErrorTypeCount(ErrorType::Error);
        if (new_error_count == (last_error_count + 1)) {
            Error *e = ctx->er->popLastError();
            if (e->instance ==
                    ErrorInst::Generator::VariableNotInScope) {
                e->instance = ErrorInst::Generator::NotInScope;
            }
            ctx->er->addError(e);
        }
        return false;
    }
    block = try_fnp.block;
    if (try_fnp.type->points_to
            && try_fnp.type->points_to->is_function) {
        dale::Token *funcall_str_tok = 
            new dale::Token(TokenType::String, 0,0,0,0);
        funcall_str_tok->str_value.clear();
        funcall_str_tok->str_value.append("funcall");
        Node *funcall_str_node = new Node(funcall_str_tok);
        funcall_str_node->filename = ctx->er->current_filename;
        lst->insert(lst->begin(), funcall_str_node);
        bool res =
            Form::Proc::Funcall::parse(
                         gen,
                         fn,
                         block,
                         n,
                         get_address,
                         false,
                         pr);
        return res;
    }
    res = parse(gen,
                  fn, try_fnp.block, (*lst)[0], true, false, wanted_type,
                  &try_fnp
              );
    if (!res) {
        return false;
    }
    if (try_fnp.type->points_to
            && try_fnp.type->points_to->struct_name) {
        /* Struct must implement 'apply' to be considered a
         * function object. */
        Element::Type *try_fnp_inner_type = try_fnp.type->points_to;
        Element::Struct *mystruct =
            ctx->getStruct(
                try_fnp_inner_type->struct_name->c_str(),
                try_fnp_inner_type->namespaces
            );
        if (mystruct) {
            Element::Type *apply =
                mystruct->nameToType("apply");
            if (apply
                    && apply->points_to
                    && apply->points_to->is_function) {
                /* The first argument of this function must be a
                 * pointer to this particular struct type. */
                Element::Type *applyfn = apply->points_to;
                if (!(applyfn->parameter_types->size())) {
                    Error *e = new Error(
                        ErrorInst::Generator::ApplyMustTakePointerToStructAsFirstArgument,
                        (*lst)[0]
                    );
                    ctx->er->addError(e);
                    return false;
                }
                if (!(applyfn->parameter_types->at(0)->isEqualTo(
                            try_fnp.type))) {
                    Error *e = new Error(
                        ErrorInst::Generator::ApplyMustTakePointerToStructAsFirstArgument,
                        (*lst)[0]
                    );
                    ctx->er->addError(e);
                    return false;
                }
                /* Get the function pointer value. */
                std::vector<llvm::Value *> indices;
                stl::push_back2(&indices,
                                ctx->nt->getLLVMZero(),
                                ctx->nt->getNativeInt(
                                    mystruct->nameToIndex("apply")));

                llvm::IRBuilder<> builder(block);
                llvm::Value *res =
                    builder.CreateGEP(
                        try_fnp.value,
                        llvm::ArrayRef<llvm::Value*>(indices)
                    );

                ParseResult supertemp;
                supertemp.type  = apply;
                supertemp.block = block;
                llvm::Value *pvalue =
                    llvm::cast<llvm::Value>(builder.CreateLoad(res));
                supertemp.value = pvalue;

                /* So a pointer to the struct is your first
                 * argument. Skip 1 element of the list when
                 * passing off (e.g. (adder 1)). */

                std::vector<llvm::Value*> extra_args;
                extra_args.push_back(try_fnp.value);
                return gen->parseFuncallInternal(
                           fn,
                           n,
                           get_address,
                           &supertemp,
                           1,
                           &extra_args,
                           pr
                       );
            }
        }
    }

    Error *e = new Error(
        ErrorInst::Generator::NotInScope,
        n,
        t->str_value.c_str()
    );
    ctx->er->addError(e);
    return false;
}

bool parse(Generator *gen,
           Element::Function *fn,
           llvm::BasicBlock *block,
           Node *node,
           bool get_address,
           bool prefixed_with_core,
           Element::Type *wanted_type,
           ParseResult *pr,
           bool no_copy)
{
    bool res =
        parseInternal(gen, fn, block, node,
                      get_address,
                      prefixed_with_core,
                      wanted_type,
                      pr);

    if (!res) {
        return false;
    }
    if (fn->is_setf_fn) {
        return true;
    }
    /* todo: if there's never a use case for a separate
     * parseresult, then fix this function accordingly. */
    if (!no_copy) {
        gen->copyWithSetfIfApplicable(fn, pr, pr);
    }

    return true;
}
}
}
}
}
