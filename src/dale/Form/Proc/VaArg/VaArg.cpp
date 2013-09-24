#include "../../../Generator/Generator.h"
#include "../../../Node/Node.h"
#include "../../../ParseResult/ParseResult.h"
#include "../../../Element/Function/Function.h"
#include "llvm/Function.h"

namespace dale
{
namespace Form
{
namespace Proc
{
namespace VaArg
{
bool parse(Generator *gen,
           Element::Function *fn,
           llvm::BasicBlock *block,
           Node *node,
           bool get_address,
           bool prefixed_with_core,
           ParseResult *pr)
{
    Context *ctx = gen->ctx;

    assert(node->list && "must receive a list!");

    if (!ctx->er->assertArgNums("va-arg", node, 2, 2)) {
        return false;
    }

    symlist *lst = node->list;

    /* Get the arglist. */

    ParseResult pr_arglist;
    bool res = gen->parseFunctionBodyInstr(fn, block, (*lst)[1], false,
                                      NULL, &pr_arglist);
    if (!res) {
        return false;
    }

    /* Get the type to which it is being cast. */

    Element::Type *type = gen->parseType((*lst)[2], false, false);

    if (!type) {
        return false;
    }

    llvm::IRBuilder<> builder(pr_arglist.block);

    /* The va_arg intrinsic is not implemented for x86-64, which
     * is why it's done here.
     */

    llvm::Type *llvm_type =
        ctx->toLLVMType(type, NULL, false, false);
    if (!llvm_type) {
        return false;
    }

    if (gen->is_x86_64) {
        /*
            (See the AMD64 ABI draft of 13/01/2010 for more
            information, most of this is verbatim from there, it
            even details the algorithm to use. Own comments are
            further indented.)

            va_list on x86-64 looks like so:

            typedef struct {
                unsigned int gp_offset;
                unsigned int fp_offset;
                void *overflow_arg_area;
                void *reg_save_area;
            } va_list[1];

            reg_save_area: points to the start of the register
            save area.

            overflow_arg_area: points to the address of the first
            argument passed on the stack, if any. When an argument
            is fetched, it should be updated to point to the start
            of the next argument on the stack.

            gp_offset: holds the offset in bytes from
            reg_save_area to the place where the next available
            general purpose argument register is saved. In case
            all argument registers have been exhausted, it is set
            to the value 48 (6 * 8).

            fp_offset: holds the offset in bytes from
            reg_save_area to the place where the next available
            floating point argument register is saved. In case all
            argument registers have been exhausted, it is set to
            the value 304 ((6 * 8) + (16 * 16)).

                So add the struct in common declarations, cast
                va_list to the struct, get all the values out of
                it.

            Algorithm:
                1. Determine whether 'type' may be passed in the
                    registers. If not, go to 7.
                2. Compute 'num_gp' to hold the number of general
                    purpose registers needed to pass 'type' and
                    'num_fp' to hold the number of floating point
                    registers needed.
                3. Verify whether arguments fit into registers. In
                    the event that:

                    gp_offset > (48 - (num_gp * 8))
                        ||
                    fp_offset > (304 - (num_fp * 16))

                    go to 7.
                4. Fetch 'type' from 'reg_save_area' with an
                    offset of 'gp_offset' and/or 'fp_offset'. This
                    may require copying to a temporary location in
                    case the parameter is passed in different
                    register classes or requires an alignment of
                    greater than 8 for general purpose registers
                    and 16 for XMM registers.
                5. Set:

                    gp_offset += num_gp * 8
                    fp_offset += num_fp * 16

                6. Return the fetched type.

                7. Align 'overflow_arg_area' upwards to a 16-byte
                    boundary if alignment needed by 'type' exceeds
                    8-byte boundary.
                8. Fetch 'type' from 'overflow_arg_area'.
                9. Set:

                    overflow_arg_area += sizeof(type)

                10. Align overflow_arg_area upwards to an 8-byte
                    boundary.
                11. Return the fetched type.

        ----

            Only handling INTEGER class at the moment (being the
            normal number types (int, char, uint etc.) and
            pointers. Structs are a bit trickier.

            Cast arglist to a struct of the correct type.

            Get the fields from it as per the specification.

            Each register is 8 bytes, so the assumption for the
            moment is that everything fits in a register. num_gp is
            1, num_fp is 1.

            Get the value - gp_offset from reg_save_area.

            Update gp_offset and fp_offset as per the previous
            part.

            Return the fetched type.

            typedef struct {
                unsigned int gp_offset;
                unsigned int fp_offset;
                void *overflow_arg_area;
                void *reg_save_area;
            } va_list[1];
        */
        /* builder, pr_arglist, type */

        llvm::Value *pvlstr = llvm::cast<llvm::Value>(
                                  builder.CreateBitCast(
                                      pr_arglist.value,
                                      llvm::PointerType::getUnqual(
                                          ctx->getStruct("va-list")->type
                                      )
                                  )
                              );

        std::vector<llvm::Value *> indices_gp_offset;
        stl::push_back2(&indices_gp_offset, ctx->nt->getLLVMZero(),
                        ctx->nt->getLLVMZero());

        std::vector<llvm::Value *> indices_overflow_arg_area;
        stl::push_back2(&indices_overflow_arg_area,
                        ctx->nt->getLLVMZero(),
                        ctx->nt->getNativeInt(2));

        std::vector<llvm::Value *> indices_reg_save_area;
        stl::push_back2(&indices_reg_save_area,
                        ctx->nt->getLLVMZero(),
                        ctx->nt->getNativeInt(3));

        llvm::Value *ptr_gpo =
            builder.CreateGEP(pvlstr,
                              llvm::ArrayRef<llvm::Value*>(indices_gp_offset));

        llvm::Value *gpo =
            llvm::cast<llvm::Value>(builder.CreateLoad(ptr_gpo));

        llvm::Value *ptr_oaa =
            builder.CreateGEP(pvlstr,
                              llvm::ArrayRef<llvm::Value*>(indices_overflow_arg_area));

        llvm::Value *oaa =
            llvm::cast<llvm::Value>(builder.CreateLoad(ptr_oaa));

        llvm::Value *ptr_rsa =
            builder.CreateGEP(pvlstr,
                              llvm::ArrayRef<llvm::Value*>(indices_reg_save_area));

        llvm::Value *rsa =
            llvm::cast<llvm::Value>(builder.CreateLoad(ptr_rsa));

        /* if gp_offset > (48 - (num_gp * 8))
           for the time being - if gp_offset > 40 */

        llvm::BasicBlock *then_block = llvm::BasicBlock::Create(llvm::getGlobalContext(), "then", fn->llvm_function);
        llvm::BasicBlock *else_block = llvm::BasicBlock::Create(llvm::getGlobalContext(), "else", fn->llvm_function);

        llvm::Value *cond =
            builder.CreateICmpUGT(
                gpo,
                llvm::ConstantInt::get(ctx->nt->getNativeUIntType(), 40)
            );

        builder.CreateCondBr(cond, then_block, else_block);

        llvm::IRBuilder<> builder_then(then_block);
        llvm::IRBuilder<> builder_else(else_block);

        /* Then component (after the first 5) */

        /*
                            7. Align 'overflow_arg_area' upwards to a 16-byte
                                boundary if alignment needed by 'type' exceeds
                                8-byte boundary.
                            8. Fetch 'type' from 'overflow_arg_area'.
                            9. Set:

                                overflow_arg_area += sizeof(type)

                            10. Align overflow_arg_area upwards to an 8-byte
                                boundary.
                            11. Return the fetched type.
        */

        std::vector<llvm::Value *> indices_po1;
        indices_po1.push_back(llvm::ConstantInt::get(ctx->nt->getNativeUIntType(),
                              0));

        llvm::Value *then_ptr_obj =
            builder_then.CreateGEP(oaa,
                                   llvm::ArrayRef<llvm::Value*>(indices_po1));

        llvm::Value *then_value=
            builder_then.CreateLoad(
                builder_then.CreateBitCast(
                    then_ptr_obj,
                    llvm::PointerType::getUnqual(llvm_type)
                )
            );

        builder_then.CreateStore(
            builder_then.CreateIntToPtr(
                builder_then.CreateAdd(
                    builder_then.CreatePtrToInt(
                        oaa,
                        llvm::Type::getInt64Ty(llvm::getGlobalContext())
                    ),
                    llvm::ConstantInt::get(
                        llvm::Type::getInt64Ty(llvm::getGlobalContext()),
                        8
                    )
                ),
                llvm::PointerType::getUnqual(
                    llvm::Type::getInt8Ty(llvm::getGlobalContext())
                )
            ),
            ptr_oaa
        );

        /* Else component (first 5) */

        std::vector<llvm::Value *> indices_po2;
        indices_po2.push_back(gpo);

        llvm::Value *ptr_obj =
            builder_else.CreateGEP(rsa,
                                   llvm::ArrayRef<llvm::Value*>(indices_po2));

        llvm::Value *else_value=
            builder_else.CreateLoad(
                builder_else.CreateBitCast(
                    ptr_obj,
                    llvm::PointerType::getUnqual(llvm_type)
                )
            );

        builder_else.CreateStore(
            builder_else.CreateAdd(gpo,
                                   ctx->nt->getNativeInt(8)),
            ptr_gpo
        );

        /* End components */

        llvm::BasicBlock *done_block =
            llvm::BasicBlock::Create(llvm::getGlobalContext(),
                                     "va_done", fn->llvm_function);

        builder_then.CreateBr(done_block);
        builder_else.CreateBr(done_block);

        llvm::IRBuilder<> builder_done(done_block);

        llvm::PHINode *pn =
            builder_done.CreatePHI(llvm_type, 0);

        pn->addIncoming(then_value, then_block);
        pn->addIncoming(else_value, else_block);

        pr->set(done_block, type, llvm::cast<llvm::Value>(pn));

        return true;

    } else {
        llvm::Value *res =
            builder.CreateVAArg(pr_arglist.value, llvm_type);
        pr->set(pr_arglist.block, type, res);

        return true;
    }
}
}
}
}
}
