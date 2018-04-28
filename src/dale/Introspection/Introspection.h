#ifndef DALE_INTROSPECTION
#define DALE_INTROSPECTION

#include "../ErrorType/ErrorType.h"
#include "../MacroProcessor/MacroProcessor.h"

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
bool is_2D_pointer_2D_to_2D_type(MContext *mc, DNode *t,
                                 DNode *pointee);

/*! Get the pointee of a pointer type.
 *  @param mc The current macro context.
 *  @param t The type node.
 */
DNode *pointee_2D_type(MContext *mc, DNode *t);

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

/*! Retrieve the number of functions that have the specified
 *  parameters.
 *  @param mc The current macro context.
 *  @param form The list of parameters.
 *  @param prefix If not null, only include functions that begin
 *  with this prefix.
 */
int fn_2D_by_2D_args_2D_count(MContext *mc, DNode *form,
                              const char *prefix);
/*! Retrieve the name of the function at the specified index.
 *  @param mc The current macro context.
 *  @param form The list of parameters.
 *  @param index The function index.
 *
 *  The index must be more than or equal to zero, and less than or
 *  equal to the result of an earlier call to
 *  fn_2D_by_2D_args_2D_count with the form.
 */
const char *fn_2D_by_2D_args_2D_name(MContext *mc, DNode *form,
                                     int index);

/*! Register a type mapping.
 *  @param mc The current macro context.
 *  @param from The internal name of the type.
 *  @param to The display string for the type.
 */
bool register_2D_type(MContext *mc, const char *from, const char *to);

/*! Get the type of a given form.
 *  @param mc The current macro context.
 *  @param form The form.
 *  @param report_errors Whether to report or suppress errors.
 */
DNode *type_2D_of(MContext *mc, DNode *form, bool report_errors);

/*! Get the printf format specifier for the given type.
 *  @param mc The current macro context.
 *  @param t The type node.
 */
const char *printf_2D_length(MContext *mc, DNode *t);

/*! Determine whether values of the given type can be copied.
 *  @param mc The current macro context.
 *  @param t The type node.
 *  @param report Whether to report an error on copy not being
 *                permitted.
 */
bool is_2D_copy_2D_permitted(MContext *mc, DNode *t, bool report);

/*! Stringify a type.
 *  @param mc The current macro context.
 *  @param t The type node.
 *  @param buf A buffer for the stringified type.
 */
bool type_2D_to_2D_string(MContext *mc, DNode *t, char *buf);
/*! Stringify a type for display.
 *  @param mc The current macro context.
 *  @param t The type node.
 *  @param buf A buffer for the stringified type.
 */
bool type_2D_to_2D_display_2D_string(MContext *mc, DNode *t, char *buf);

/*! Report an error to the compiler.
 *  @param mc The current macro context.
 *  @param form The reference node.
 *  @param str The error message.
 */
void report_2D_error(MContext *mc, DNode *form, char *str);

/*! Get the arity of a given function.
 *  @param mc The current macro context.
 *  @param name The name of the function.
 */
int arity(MContext *mc, DNode *name);
/*! Get the codomain of a given function.
 *  @param mc The current macro context.
 *  @param form The function form node.
 *
 *  The function form node comprises the function name and its
 *  parameter types.
 */
DNode *codomain(MContext *mc, DNode *form);
/*! Get the type of the argument at a given index for a function.
 *  @param mc The current macro context.
 *  @param name The name of the function.
 *  @param index The argument index.
 */
DNode *input_2D_type(MContext *mc, DNode *name, int index);

/*! Get the number of members in a struct.
 *  @param mc The current macro context.
 *  @param name The struct name node.
 */
int struct_2D_member_2D_count(MContext *mc, DNode *name);
/*! Get the type of a given struct member.
 *  @param mc The current macro context.
 *  @param name The struct name node.
 *  @param index The member index within the struct.
 */
DNode *struct_2D_member_2D_type(MContext *mc, DNode *name, int index);
/*! Get the name of a given struct member.
 *  @param mc The current macro context.
 *  @param name The struct name node.
 *  @param index The member index within the struct.
 */
const char *struct_2D_member_2D_name(MContext *mc, DNode *name,
                                     int index);

/*! Initialise introspection function lookup.
 *
 *  This must be called at least once before
 *  find_introspection_function may be called.
 */
void init_introspection_functions();
/*! Find an introspection function.
 *  @param name The external name of the function (e.g. "type-of").
 *
 *  Returns a pointer to the relevant function, or null if none
 *  exists.
 */
void *find_introspection_function(const char *name);
}

#endif
