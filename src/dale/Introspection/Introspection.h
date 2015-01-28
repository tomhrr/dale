#ifndef DALE_INTROSPECTION
#define DALE_INTROSPECTION

#include "../ErrorType/ErrorType.h"
#include "../Generator/Generator.h"

/*! Introspection

    These functions map to those from the Dale introspection library.
    These have "_2D_" where the Dale functions have '-' so as to
    account for Dale's encoding of non-LDU characters in names.
*/

extern "C" {
    /*! Check whether two type nodes are equal.
     *  @param mc The current macro context.
     *  @param t1 The first type node.
     *  @param t2 The second type node.
     */
    bool types_2D_equal(MContext *mc, DNode *t1, DNode *t2);
    /*! Check whether a type is a character type.
     *  @param mc The current macro context.
     *  @param t The type node.
     */
    bool is_2D_char_2D_type(MContext *mc, DNode *t);
    /*! Check whether a type is an integer type.
     *  @param mc The current macro context.
     *  @param t The type node.
     */
    bool is_2D_integer_2D_type(MContext *mc, DNode *t);
    /*! Check whether a type is a signed integer type.
     *  @param mc The current macro context.
     *  @param t The type node.
     */
    bool is_2D_signed_2D_integer_2D_type(MContext *mc, DNode *t);
    /*! Check whether a type is an unsigned integer type.
     *  @param mc The current macro context.
     *  @param t The type node.
     */
    bool is_2D_unsigned_2D_integer_2D_type(MContext *mc, DNode *t);
    /*! Check whether a type is a floating point type.
     *  @param mc The current macro context.
     *  @param t The type node.
     */
    bool is_2D_floating_2D_point_2D_type(MContext *mc, DNode *t);
    /*! Check whether a type is a pointer type.
     *  @param mc The current macro context.
     *  @param t The type node.
     */
    bool is_2D_pointer_2D_type(MContext *mc, DNode *t);
    /*! Check whether a type is a pointer to another type.
     *  @param mc The current macro context.
     *  @param t The type node.
     *  @param pointee The pointee type.
     */
    bool is_2D_pointer_2D_to_2D_type(MContext *mc, DNode *t, DNode *pointee);

    /*! Get the pointee of a pointer type.
     *  @param mc The current macro context.
     *  @param t The type node.
     */
    DNode *pointee_2D_type(MContext *mc, DNode *t);

    /*! Check whether a particular struct must be initialised on declaration.
     *  @param mc The current macro context.
     *  @param t The struct type node.
     */
    bool must_2D_init(MContext *mc, DNode *t);
    /*! Check whether a particular type is a const type.
     *  @param mc The current macro context.
     *  @param t The type node.
     */
    bool is_2D_const(MContext *mc, DNode *t);

    /*! Check whether evaluating a form will cause errors.
     *  @param mc The current macro context.
     *  @param t The form node.
     */
    bool has_2D_errors(MContext *mc, DNode *form);

    /*! Check whether the given function definition exists.
     *  @param mc The current macro context.
     *  @param form The function form node.
     *
     *  The form node comprises the return type, the name, and the
     *  argument types (e.g. (bool < int int)).
     */
    bool exists_2D_fn(MContext *mc, DNode *form);
    /*! Check whether the given type exists.
     *  @param mc The current macro context.
     *  @param form The type node.
     */
    bool exists_2D_type(MContext *mc, DNode *t);
    /*! Check whether the given macro definition exists.
     *  @param mc The current macro context.
     *  @param form The macro form node.
     *
     *  As per exists_2D_fn, except that the return type is not
     *  included.
     */
    bool exists_2D_macro(MContext *mc, DNode *form);
    /*! Check whether the given variable exists.
     *  @param mc The current macro context.
     *  @param form The variable node.
     */
    bool exists_2D_variable(MContext *mc, DNode *form);


    int fn_2D_by_2D_args_2D_count(MContext *mc, DNode *dnode,
                                  const char *prefix);
    const char* fn_2D_by_2D_args_2D_name(MContext *mc, DNode *dnode,
                                         int acount);
    bool register_2D_type(MContext *mc, const char *buf1, 
                                        const char *buf2);
    DNode *type_2D_of(MContext *mc, DNode *dnode);
    const char *printf_2D_length(MContext *mc, DNode *dnode);
    bool type_2D_to_2D_string(MContext *mc, DNode *dnode, char *buf);
    bool type_2D_to_2D_display_2D_string(MContext *mc, 
                                        DNode *dnode, char *buf);
    void report_2D_error(MContext *mc, DNode *dnode, char *str);
    DNode *codomain(MContext *mc, DNode *dnode);
    DNode *input_2D_type(MContext *mc, DNode *fn_name, int arg_count);
    DNode *struct_2D_member_2D_type(MContext *mc,
                                    DNode *struct_name_node, int
                                    arg_count);
    const char *struct_2D_member_2D_name(MContext *mc,
                                         DNode *struct_name_node,
                                         int arg_count);
    int arity(MContext *mc, DNode *fn_name);
    int struct_2D_member_2D_count(MContext *mc, DNode
    *struct_name_node);

    void *find_introspection_function(const char *name);
}

#endif
