#include "Introspection.h"

#include "../Element/Type/Type.h"

extern "C" {
    int types_2D_equal(MContext *mc, DNode *T1, DNode *T2)
    {
        dale::Generator *g = (dale::Generator*) mc->generator;
        return g->typesEqual(T1, T2);
    }

    int is_2D_char_2D_type(MContext *mc, DNode *dnode)
    {
        dale::Generator *g = (dale::Generator*) mc->generator;
        return g->isCharType(dnode);
    }

    int is_2D_integer_2D_type(MContext *mc, DNode *dnode)
    {
        dale::Generator *g = (dale::Generator*) mc->generator;
        return g->isIntegerType(dnode);
    }

    int is_2D_signed_2D_integer_2D_type(MContext *mc, DNode *dnode)
    {
        dale::Generator *g = (dale::Generator*) mc->generator;
        return g->isSignedIntegerType(dnode);
    }

    int is_2D_unsigned_2D_integer_2D_type(MContext *mc, DNode *dnode)
    {
        dale::Generator *g = (dale::Generator*) mc->generator;
        return g->isUnsignedIntegerType(dnode);
    }

    int is_2D_floating_2D_point_2D_type(MContext *mc, DNode *dnode)
    {
        dale::Generator *g = (dale::Generator*) mc->generator;
        return g->isFloatingPointType(dnode);
    }

    int is_2D_pointer_2D_type(MContext *mc, DNode *dnode)
    {
        dale::Generator *g = (dale::Generator*) mc->generator;
        return g->isPointerType(dnode);
    }

    int is_2D_pointer_2D_to_2D_type(MContext *mc, DNode *dnode,
                                    DNode *pointee_type)
    {
        dale::Generator *g = (dale::Generator*) mc->generator;
        return g->isPointerToType(dnode, pointee_type);
    }

    DNode *pointee_2D_type(MContext *mc, DNode *dnode)
    {
        dale::Generator *g = (dale::Generator*) mc->generator;
        return g->pointeeType(dnode);
    }

    bool has_2D_errors(MContext *mc, DNode *dnode)
    {
        dale::Generator *g = (dale::Generator*) mc->generator;
        return g->hasErrors(dnode);
    }

    bool must_2D_init(MContext *mc, DNode *dnode)
    {
        dale::Generator *g = (dale::Generator*) mc->generator;
        return g->mustInit(dnode);
    }

    bool is_2D_const(MContext *mc, DNode *dnode)
    {
        dale::Generator *g = (dale::Generator*) mc->generator;
        return g->isConst(dnode);
    }

    int fn_2D_by_2D_args_2D_count(MContext *mc, DNode *dnode,
                                  const char *prefix)
    {
        dale::Generator *g = (dale::Generator*) mc->generator;
        return g->fnByArgsCount(dnode, prefix);
    }

    const char* fn_2D_by_2D_args_2D_name(MContext *mc, DNode *dnode,
                                         int acount)
    {
        dale::Generator *g = (dale::Generator*) mc->generator;
        return g->fnByArgsName(dnode, acount);
    }

    bool register_2D_type(MContext *mc, const char *buf1, const char *buf2)
    {
        dale::addTypeMapEntry(buf1, buf2);
        return true;
    }

    DNode *type_2D_of(MContext *mc, DNode *dnode)
    {
        dale::Generator *g = (dale::Generator*) mc->generator;
        return g->typeOf(dnode);
    }

    const char *printf_2D_length(MContext *mc, DNode *dnode)
    {
        dale::Generator *g = (dale::Generator*) mc->generator;
        return g->printfLength(dnode);
    }

    int type_2D_to_2D_string(MContext *mc, DNode *dnode, char *buf)
    {
        dale::Generator *g = (dale::Generator*) mc->generator;
        return g->typeToString(dnode, buf);
    }

    int type_2D_to_2D_display_2D_string(MContext *mc, DNode *dnode, char *buf)
    {
        dale::Generator *g = (dale::Generator*) mc->generator;
        return g->typeToDisplayString(dnode, buf);
    }

    int report_2D_error(MContext *mc, DNode *dnode, char *str)
    {
        dale::Generator *g = (dale::Generator*) mc->generator;
        g->reportError(dnode, str);
        return 1;
    }

    int exists_2D_fn(MContext *mc, DNode *dnode)
    {
        dale::Generator *g = (dale::Generator*) mc->generator;
        return g->existsFunction(dnode);
    }

    int exists_2D_type(MContext *mc, DNode *dnode)
    {
        dale::Generator *g = (dale::Generator*) mc->generator;
        return g->existsType(dnode);
    }

    int exists_2D_macro(MContext *mc, DNode *dnode)
    {
        dale::Generator *g = (dale::Generator*) mc->generator;
        return g->existsMacro(dnode);
    }

    int exists_2D_macro_2D_exact(MContext *mc, DNode *dnode)
    {
        dale::Generator *g = (dale::Generator*) mc->generator;
        return g->existsMacroExact(dnode);
    }

    int exists_2D_variable(MContext *mc, DNode *dnode)
    {
        dale::Generator *g = (dale::Generator*) mc->generator;
        return g->existsVariable(dnode);
    }

    DNode *codomain(MContext *mc, DNode *dnode)
    {
        dale::Generator *g = (dale::Generator*) mc->generator;
        return g->codomain(dnode);
    }

    DNode *input_2D_type(MContext *mc, DNode *fn_name, int arg_count)
    {
        dale::Generator *g = (dale::Generator*) mc->generator;
        return g->inputType(fn_name, arg_count);
    }

    DNode *struct_2D_member_2D_type(MContext *mc,
                                    DNode *struct_name_node, int arg_count)
    {
        dale::Generator *g = (dale::Generator*) mc->generator;
        return g->structMemberType(struct_name_node, arg_count);
    }

    const char *struct_2D_member_2D_name(MContext *mc,
                                         DNode *struct_name_node,
                                         int arg_count)
    {
        dale::Generator *g = (dale::Generator*) mc->generator;
        return g->structMemberName(struct_name_node, arg_count);
    }

    int arity(MContext *mc, DNode *fn_name)
    {
        dale::Generator *g = (dale::Generator*) mc->generator;
        return g->arity(fn_name);
    }

    int struct_2D_member_2D_count(MContext *mc, DNode *struct_name_node)
    {
        dale::Generator *g = (dale::Generator*) mc->generator;
        return g->structMemberCount(struct_name_node);
    }

    void *find_introspection_function(const char *name)
    {
        if (!strcmp(name, "exists-fn")) {
            return (void *) exists_2D_fn;
        } else if (!strcmp(name, "exists-type")) {
            return (void *) exists_2D_type;
        } else if (!strcmp(name, "type-of")) {
            return (void *) type_2D_of;
        } else if (!strcmp(name, "printf-length")) {
            return (void *) printf_2D_length;
        } else if (!strcmp(name, "exists-variable")) {
            return (void *) exists_2D_variable;
        } else if (!strcmp(name, "exists-macro")) {
            return (void *) exists_2D_macro;
        } else if (!strcmp(name, "exists-macro-exact")) {
            return (void *) exists_2D_macro_2D_exact;
        } else if (!strcmp(name, "report-error")) {
            return (void *) report_2D_error;
        } else if (!strcmp(name, "type-to-string")) {
            return (void *) type_2D_to_2D_string;
        } else if (!strcmp(name, "type-to-display-string")) {
            return (void *) type_2D_to_2D_display_2D_string;
        } else if (!strcmp(name, "register-type")) {
            return (void *) register_2D_type;
        } else if (!strcmp(name, "struct-member-count")) {
            return (void *) struct_2D_member_2D_count;
        } else if (!strcmp(name, "struct-member-type")) {
            return (void *) struct_2D_member_2D_type;
        } else if (!strcmp(name, "struct-member-name")) {
            return (void *) struct_2D_member_2D_name;
        } else if (!strcmp(name, "input-type")) {
            return (void *) input_2D_type;
        } else if (!strcmp(name, "is-char-type")) {
            return (void *) is_2D_char_2D_type;
        } else if (!strcmp(name, "is-integer-type")) {
            return (void *) is_2D_integer_2D_type;
        } else if (!strcmp(name, "is-signed-integer-type")) {
            return (void *) is_2D_signed_2D_integer_2D_type;
        } else if (!strcmp(name, "is-unsigned-integer-type")) {
            return (void *) is_2D_unsigned_2D_integer_2D_type;
        } else if (!strcmp(name, "is-floating-point-type")) {
            return (void *) is_2D_floating_2D_point_2D_type;
        } else if (!strcmp(name, "is-pointer-type")) {
            return (void *) is_2D_pointer_2D_type;
        } else if (!strcmp(name, "is-pointer-to-type")) {
            return (void *) is_2D_pointer_2D_to_2D_type;
        } else if (!strcmp(name, "pointee-type")) {
            return (void *) pointee_2D_type;
        } else if (!strcmp(name, "fn-by-args-count")) {
            return (void *) fn_2D_by_2D_args_2D_count;
        } else if (!strcmp(name, "fn-by-args-name")) {
            return (void *) fn_2D_by_2D_args_2D_name;
        } else if (!strcmp(name, "has-errors")) {
            return (void *) has_2D_errors;
        } else if (!strcmp(name, "is-const")) {
            return (void *) is_2D_const;
        } else if (!strcmp(name, "must-init")) {
            return (void *) must_2D_init;
        }
        return NULL;
    }
}
