#ifndef DALE_PARSERESULT
#define DALE_PARSERESULT

#include "../llvm_AnalysisVerifier.h"
#include "../llvm_AssemblyPrintModulePass.h"
#include "../llvm_CallingConv.h"
#include "../llvm_Function.h"
#include "../llvm_IRBuilder.h"
#include "../llvm_Module.h"
#include "../llvm_PassManager.h"
#include "llvm/ExecutionEngine/ExecutionEngine.h"
#include "llvm/Support/raw_ostream.h"

#include "../Context/Context.h"
#include "../Type/Type.h"

namespace dale {
/*! ParseResult

    Represents the result of a computation.  Used extensively
    throughout the form-processing code and the Generator.
*/
class ParseResult {
    private:
    /*! Set the address of the value.
     *  @param ctx The current context.
     *
     *  If the address has not been set explicitly by the time this is
     *  called, this will use an alloca instruction to create a
     *  "dummy" holder for the value, so that an address may be
     *  returned.  This is just a failsafe.
     */
    bool setAddressOfValue(Context *ctx);
    /*! Reset the state of the parse result.
     */
    void init();
    /*! The value. */
    llvm::Value *value;

    public:
    /*! The address of the value. */
    llvm::Value *address_of_value;
    /*! The type of the value. */
    Type *type;
    /*! The type of the address of the value. */
    Type *type_of_address_of_value;
    /*! The block to which new code should be appended. */
    llvm::BasicBlock *block;

    /*! Whether this parse result should be treated as a terminator.
     *  Branch and return instructions are examples of terminators. */
    bool treat_as_terminator;
    /*! Whether to skip destruction of the value once the parse result
     *  is no longer needed. */
    bool do_not_destruct;
    /*! Whether to skip copying with overridden setf methods, if the
     *  parse result needs to be copied. */
    bool do_not_copy_with_setf;
    /*! Whether the parse result was created by way of an overridden
     *  setf method. */
    bool freshly_copied;
    /*! Whether the value is an lvalue. */
    bool value_is_lvalue;

    /*! The retval for this parse result.  This is set prior to the
     *  parse result being passed downwards, so that retval-setting
     *  operations can use it. */
    llvm::Value *retval;
    /*! The retval type. */
    Type *retval_type;
    /*! Whether the retval has been used. */
    bool retval_used;
    /*! Whether the retval requires initialisation before being passed
     *  to a function. */
    bool retval_requires_init;

    ParseResult();
    /*! Construct a new ParseResult.
     *  @param block The block.
     *  @param type The type.
     *  @param value The value.
     */
    ParseResult(llvm::BasicBlock *block, Type *type,
                llvm::Value *value);
    ~ParseResult();

    /*! Copy the details of this parse result to another.
     *  @param other The other parse result.
     *
     *  This is a shallow copy.
     */
    void copyTo(ParseResult *other);

    /*! Set the details of this parse result.
     *  @param block The block.
     *  @param type The type.
     *  @param value The value.
     */
    void set(llvm::BasicBlock *block, Type *type, llvm::Value *value);
    /*! Get the address of the value.
     *  @param ctx The current context.
     *  @param pr The parse result for the address of the value.
     *
     *  See setAddressOfValue.
     */
    bool getAddressOfValue(Context *ctx, ParseResult *pr);
    /*! Return the value of the parse result.
     *  @param ctx The current context.
     *
     *  This takes retval into account: if it is set, this will return
     *  the underlying value therefrom.
     */
    llvm::Value *getValue(Context *ctx);
};
}

#endif
