#ifndef DALE_INTROSPECTION
#define DALE_INTROSPECTION

#include "../Generator/Generator.h"

extern "C" {
    int types_2D_equal(MContext *mc, DNode *T1, DNode *T2);
    int is_2D_char_2D_type(MContext *mc, DNode *dnode);
    int is_2D_integer_2D_type(MContext *mc, DNode *dnode);
    int is_2D_signed_2D_integer_2D_type(MContext *mc, DNode *dnode);
    int is_2D_unsigned_2D_integer_2D_type(MContext *mc, DNode *dnode);
    int is_2D_floating_2D_point_2D_type(MContext *mc, DNode *dnode);
    int is_2D_pointer_2D_type(MContext *mc, DNode *dnode);
    int is_2D_pointer_2D_to_2D_type(MContext *mc, DNode *dnode,
                                    DNode *pointee_type);
    DNode *pointee_2D_type(MContext *mc, DNode *dnode);
    bool has_2D_errors(MContext *mc, DNode *dnode);
    bool must_2D_init(MContext *mc, DNode *dnode);
    bool is_2D_const(MContext *mc, DNode *dnode);
    int fn_2D_by_2D_args_2D_count(MContext *mc, DNode *dnode,
                                  const char *prefix);
    const char* fn_2D_by_2D_args_2D_name(MContext *mc, DNode *dnode,
                                         int acount);
    bool register_2D_type(MContext *mc, const char *buf1, 
                                        const char *buf2);
    DNode *type_2D_of(MContext *mc, DNode *dnode);
    const char *printf_2D_length(MContext *mc, DNode *dnode);
    int type_2D_to_2D_string(MContext *mc, DNode *dnode, char *buf);
    int type_2D_to_2D_display_2D_string(MContext *mc, 
                                        DNode *dnode, char *buf);
    int report_2D_error(MContext *mc, DNode *dnode, char *str);
    int exists_2D_fn(MContext *mc, DNode *dnode);
    int exists_2D_type(MContext *mc, DNode *dnode);
    int exists_2D_macro(MContext *mc, DNode *dnode);
    int exists_2D_macro_2D_exact(MContext *mc, DNode *dnode);
    int exists_2D_variable(MContext *mc, DNode *dnode);
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
