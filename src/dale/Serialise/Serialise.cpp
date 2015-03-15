#include "Serialise.h"

#include <cassert>

namespace dale
{
void xfwrite(const void *a, size_t b, size_t c, FILE *d)
{
    size_t res = fwrite(a, b, c, d);
    if (res != c) {
        perror("fwrite failed");
        abort();
    }
}

void serialise(FILE *out, bool a)
{
    xfwrite(&a, sizeof(bool), 1, out);
}

void serialise(FILE *out, bool *a)
{
    serialise(out, *a);
}

char *deserialise(TypeRegister *tr, char *in, bool *a)
{
    *a = *(bool*) in;
    return in + sizeof(bool);
}

void serialise(FILE *out, int64_t a)
{
    xfwrite(&a, sizeof(int64_t), 1, out);
}

void serialise(FILE *out, int64_t *a)
{
    serialise(out, *a);
}

char *deserialise(TypeRegister *tr, char *in, int64_t *a)
{
    *a = *(int64_t*) in;
    return in + sizeof(int64_t);
}

void serialise(FILE *out, char a)
{
    xfwrite(&a, sizeof(char), 1, out);
}

void serialise(FILE *out, char *a)
{
    serialise(out, *a);
}

char *deserialise(TypeRegister *tr, char *in, char *a)
{
    *a = *in;
    return in + 1;
}

void serialise(FILE *out, int a)
{
    assert(((a >= 0) && (a <= 255)) &&
           "serialised ints must be between 0 and 255 inclusive");
    uint8_t aa = (uint8_t) a;
    xfwrite(&aa, sizeof(uint8_t), 1, out);
}

void serialise(FILE *out, int *a)
{
    serialise(out, *a);
}

char *deserialise(TypeRegister *tr, char *in, int *a)
{
    uint8_t aa;
    aa = *(uint8_t*) in;
    *a = aa;
    return in + sizeof(uint8_t);
}

void serialise(FILE *out, size_t s)
{
    uint16_t ss = (uint16_t) s;
    xfwrite(&ss, sizeof(uint16_t), 1, out);
}

void serialise(FILE *out, size_t *s)
{
    serialise(out, *s);
}

char *deserialise(TypeRegister *tr, char *in, size_t *s)
{
    uint16_t ss;
    ss = *(uint16_t*) in;
    *s = ss;
    return in + sizeof(uint16_t);
}

void serialise(FILE *out, std::string x)
{
    serialise(out, x.length());
    xfwrite(x.c_str(), sizeof(char), x.length(), out);
}

void serialise(FILE *out, std::string *x)
{
    serialise(out, x->length());
    xfwrite(x->c_str(), sizeof(char), x->length(), out);
}

char *deserialise(TypeRegister *tr, char *in, std::string *x)
{
    size_t s;
    in = deserialise(tr, in, &s);
    x->reserve(s);
    char temp[256];
    strncpy(temp, in, s);
    temp[s] = '\0';
    x->clear();
    x->append(temp);
    return in + s;
}

void serialise(FILE *out, Type *t)
{
    /* Shortcut for simple types. */
    char c;
    if (!t->is_array
            && !t->array_size
            && !t->array_type
            && !t->is_function
            && !(t->struct_name.size())
            && !(t->namespaces.size())
            && !t->points_to
            && !t->return_type
            && !(t->parameter_types.size())
            && !t->is_const
            && !t->is_reference
            && !t->bitfield_size
            && !t->is_retval) {
        c = 'S';
        serialise(out, &c);
        serialise(out, &(t->base_type));
        return;
    }
    c = 'N';
    serialise(out, &c);

    serialise(out, &(t->base_type));
    serialise(out, &(t->is_array));
    serialise(out, &(t->array_size));
    serialise(out, &(t->bitfield_size));
    serialise(out, &(t->is_const));
    serialise(out, &(t->is_reference));
    serialise(out, &(t->is_retval));
    if (!t->array_type) {
        serialise(out, 0);
    } else {
        serialise(out, 1);
        serialise(out, t->array_type);
    }
    serialise(out, &(t->is_function));
    if (!t->struct_name.size()) {
        serialise(out, 0);
    } else {
        serialise(out, 1);
        serialise(out, &(t->struct_name));
    }
    if (!t->namespaces.size()) {
        serialise(out, 0);
    } else {
        serialise(out, 1);
        serialise(out, &(t->namespaces));
    }
    if (!t->points_to) {
        serialise(out, 0);
    } else {
        serialise(out, 1);
        serialise(out, t->points_to);
    }
    if (!t->return_type) {
        serialise(out, 0);
    } else {
        serialise(out, 1);
        serialise(out, t->return_type);
    }
    if (!t->parameter_types.size()) {
        serialise(out, 0);
    } else {
        serialise(out, 1);
        serialise(out, &(t->parameter_types));
    }
}

char *deserialise(TypeRegister *tr, char *in, Type **t)
{
    char c;
    in = deserialise(tr, in, &c);
    if (c == 'S') {
        int base_type;
        in = deserialise(tr, in, &base_type);
        *t = tr->getBasicType(base_type);
        return in;
    }
    assert((c == 'N') && "got invalid char on deserialising");

    Type temp;

    in = deserialise(tr, in, &(temp.base_type));
    in = deserialise(tr, in, &(temp.is_array));
    in = deserialise(tr, in, &(temp.array_size));
    in = deserialise(tr, in, &(temp.bitfield_size));
    in = deserialise(tr, in, &(temp.is_const));
    in = deserialise(tr, in, &(temp.is_reference));
    in = deserialise(tr, in, &(temp.is_retval));
    int is_present;
    in = deserialise(tr, in, &is_present);
    if (is_present) {
        Type *at;
        in = deserialise(tr, in, &at);
        temp.array_type = at;
    }
    in = deserialise(tr, in, &(temp.is_function));
    in = deserialise(tr, in, &is_present);
    if (is_present) {
        in = deserialise(tr, in, &temp.struct_name);
    }
    in = deserialise(tr, in, &is_present);
    if (is_present) {
        in = deserialise(tr, in, &temp.namespaces);
    }
    in = deserialise(tr, in, &is_present);
    if (is_present) {
        Type *pt;
        in = deserialise(tr, in, &pt);
        temp.points_to = pt;
    }
    in = deserialise(tr, in, &is_present);
    if (is_present) {
        Type *rt;
        in = deserialise(tr, in, &rt);
        temp.return_type = rt;
    }
    in = deserialise(tr, in, &is_present);
    if (is_present) {
        in = deserialise_type_vector(tr, in, &temp.parameter_types);
    }

    Type *final = tr->getType(&temp);
    *t = final;

    return in;
}

void serialise(FILE *out, Variable *v)
{
    serialise(out, v->type);
    serialise(out, &(v->name));
    serialise(out, v->linkage);
    if (!(v->internal_name.compare(""))) {
        serialise(out, 0);
    } else {
        serialise(out, 1);
        serialise(out, &(v->internal_name));
    }
    serialise(out, v->once_tag);
}

void serialise(FILE *out, Variable **v)
{
    serialise(out, *v);
}

char *deserialise(TypeRegister *tr, char *in, Variable *v)
{
    Type *vt;
    in = deserialise(tr, in, &vt);
    v->type = vt;

    in = deserialise(tr, in, &(v->name));

    in = deserialise(tr, in, &(v->linkage));

    int is_present;
    in = deserialise(tr, in, &is_present);
    if (is_present) {
        in = deserialise(tr, in, &(v->internal_name));
    }
    in = deserialise(tr, in, &(v->once_tag));

    v->value = NULL;

    return in;
}

char *deserialise(TypeRegister *tr, char *in, Variable **v)
{
    Variable *vv = new Variable();
    vv->serialise = false;
    *v = vv;
    return deserialise(tr, in, vv);
}

void serialise(FILE *out, Function *fn)
{
    serialise(out, fn->return_type);
    serialise(out, &(fn->parameter_types));
    serialise(out, fn->is_macro);
    serialise(out, fn->internal_name);
    serialise(out, fn->always_inline);
    serialise(out, fn->once_tag);
    serialise(out, fn->cto);
    serialise(out, fn->linkage);

    return;
}

void serialise(FILE *out, Function **fn)
{
    serialise(out, *fn);
}

char *deserialise(TypeRegister *tr, char *in, Function *fn)
{
    Type *rt;
    in = deserialise(tr, in, &rt);
    fn->return_type = rt;
    fn->llvm_function = NULL;

    in = deserialise(tr, in, &(fn->parameter_types));
    in = deserialise(tr, in, &(fn->is_macro));
    in = deserialise(tr, in, &(fn->internal_name));
    in = deserialise(tr, in, &(fn->always_inline));
    in = deserialise(tr, in, &(fn->once_tag));
    in = deserialise(tr, in, &(fn->cto));
    in = deserialise(tr, in, &(fn->linkage));

    return in;
}

char *deserialise(TypeRegister *tr, char *in, Function **fn)
{
    Function *ff = new Function();
    ff->serialise = false;
    *fn = ff;
    return deserialise(tr, in, ff);
}

void serialise(FILE *out, Struct *st)
{
    serialise(out, &(st->is_opaque));
    serialise(out, &(st->member_types));
    serialise(out, &(st->member_to_index));
    serialise(out, &(st->internal_name));
    serialise(out, st->once_tag);
    serialise(out, st->linkage);
    serialise(out, st->must_init);

    return;
}

void serialise(FILE *out, Struct **st)
{
    serialise(out, *st);
}

char *deserialise(TypeRegister *tr, char *in, Struct *st)
{
    st->type = NULL;
    in = deserialise(tr, in, &(st->is_opaque));
    in = deserialise(tr, in, &(st->member_types));
    in = deserialise(tr, in, &(st->member_to_index));
    in = deserialise(tr, in, &(st->internal_name));
    in = deserialise(tr, in, &(st->once_tag));
    in = deserialise(tr, in, &(st->linkage));
    in = deserialise(tr, in, &(st->must_init));

    return in;
}

char *deserialise(TypeRegister *tr, char *in, Struct **st)
{
    Struct *stt = new Struct();
    stt->serialise = false;
    *st = stt;
    return deserialise(tr, in, stt);
}

void serialise(FILE *out, Enum *en)
{
    serialise(out, &(en->last_index));
    serialise(out, &(en->member_to_index));
    serialise(out, en->once_tag);
    serialise(out, en->linkage);

    return;
}

void serialise(FILE *out, Enum **en)
{
    serialise(out, *en);
}

char *deserialise(TypeRegister *tr, char *in, Enum *en)
{
    en->type = NULL;
    in = deserialise(tr, in, &(en->last_index));
    in = deserialise(tr, in, &(en->member_to_index));
    in = deserialise(tr, in, &(en->once_tag));
    in = deserialise(tr, in, &(en->linkage));

    return in;
}

char *deserialise(TypeRegister *tr, char *in, Enum **en)
{
    Enum *enn = new Enum();
    enn->serialise = false;
    *en = enn;
    return deserialise(tr, in, enn);
}

void serialise(FILE *out, Namespace *ns)
{
    serialise(out, &(ns->functions));
    serialise(out, &(ns->variables));
    serialise(out, &(ns->structs));
    serialise(out, &(ns->enums));
    serialise(out, &(ns->name));
    serialise(out, &(ns->symbol_prefix));
    return;
}

void serialise(FILE *out, Namespace **ns)
{
    serialise(out, *ns);
}

char *deserialise(TypeRegister *tr, char *in, Namespace *ns)
{
    in = deserialise(tr, in, &(ns->functions));
    in = deserialise(tr, in, &(ns->variables));
    in = deserialise(tr, in, &(ns->structs));
    in = deserialise(tr, in, &(ns->enums));

    std::string name;
    in = deserialise(tr, in, &name);
    ns->name = name;
    
    in = deserialise(tr, in, &(ns->symbol_prefix));

    if (ns->symbol_prefix.size()) {
        ns->has_symbol_prefix = true;
    }
    ns->tr = tr;

    return in;
}

void serialise(FILE *out, NSNode *nsnode)
{
    serialise(out, nsnode->ns);
    serialise(out, &(nsnode->children));

    return;
}

void serialise(FILE *out, NSNode **nsnode)
{
    serialise(out, *nsnode);

    return;
}

char *deserialise(TypeRegister *tr, char *in, NSNode *nsnode)
{
    Namespace *ns = new Namespace();
    in = deserialise(tr, in, ns);
    nsnode->ns = ns;

    std::map<std::string, NSNode *> *children = new
        std::map<std::string, NSNode *>;
    in = deserialise(tr, in, children);
    nsnode->children = *children;

    return in;
}

char *deserialise(TypeRegister *tr, char *in, NSNode **nsnode)
{
    NSNode *nsn = new NSNode();
    *nsnode = nsn;
    return deserialise(tr, in, nsn);
}

void serialise(FILE *out, Context *ctx)
{
    serialise(out, ctx->namespaces);

    return;
}

void serialise(FILE *out, Context **ctx)
{
    serialise(out, *ctx);
}

char *deserialise(TypeRegister *tr, char *in, Context *ctx)
{
    NSNode *nsnode = new NSNode();
    in = deserialise(tr, in, nsnode);
    ctx->namespaces = nsnode;

    return in;
}

char *deserialise(TypeRegister *tr, char *in, Context **ctx)
{
    Context *mc = new Context();
    *ctx = mc;
    return deserialise(tr, in, mc);
}

char *deserialise_type_vector(TypeRegister *tr, char *in,
                              std::vector<Type *> *x)
{
    size_t s;
    in = deserialise(tr, in, &s);
    x->reserve(s);
    for (size_t i = 0; i < s; i++) {
        Type *t;
        in = deserialise(tr, in, &t);
        x->push_back(t);
    }
    return in;
}
}
