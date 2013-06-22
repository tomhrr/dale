#ifndef DALE_SERIALISE
#define DALE_SERIALISE

#include <cstdio>
#include <string>
#include <vector>
#include "../Element/Type/Type.h"
#include "../Element/Variable/Variable.h"
#include "../Context/Context.h"
#include "../Namespace/Namespace.h"

namespace dale
{
template<typename T>
void serialise(FILE *out, std::vector<T> *x);

template<typename T>
char *deserialise(char *in, std::vector<T> *x);

template<typename T>
void serialise(FILE *out, std::vector<T> **x);

template<typename T>
char *deserialise(char *in, std::vector<T> **x);

template<typename T1, typename T2>
void serialise(FILE *out, std::map<T1, T2> *x);

template<typename T1, typename T2>
char *deserialise(char *in, std::map<T1, T2> *x);

void xfwrite(const void *a, size_t b, size_t c, FILE *d)
{
    size_t res = fwrite(a, b, c, d);
    if (res != c) {
        perror("fwrite failed");
        abort();
    }
}

void serialise(FILE *out, int64_t a)
{
    xfwrite(&a, sizeof(int64_t), 1, out);
}

void serialise(FILE *out, int64_t *a)
{
    serialise(out, *a);
}

char *deserialise(char *in, int64_t *a)
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

char *deserialise(char *in, char *a)
{
    *a = *in;
    return in + 1;
}

void serialise(FILE *out, int a)
{
    if ((a < 0) || (a > 255)) {
        fprintf(stderr, "Internal error: assuming ints for "
                "serialising are between 0 and 255 "
                "inclusive (got %d).", a);
        abort();
    }

    uint8_t aa = (uint8_t) a;
    xfwrite(&aa, sizeof(uint8_t), 1, out);
}

void serialise(FILE *out, int *a)
{
    serialise(out, *a);
}

char *deserialise(char *in, int *a)
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

char *deserialise(char *in, size_t *s)
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

char *deserialise(char *in, std::string *x)
{
    size_t s;
    in = deserialise(in, &s);
    x->reserve(s);
    char temp[256];
    strncpy(temp, in, s);
    temp[s] = '\0';
    x->clear();
    x->append(temp);
    return in + s;
}

void serialise(FILE *out, Element::Type *t)
{
    /* Shortcut for simple types. */
    char c;
    if (!t->is_array
            && !t->array_size
            && !t->array_type
            && !t->is_function
            && !t->struct_name
            && !t->namespaces
            && !t->points_to
            && !t->return_type
            && !t->parameter_types
            && !t->bitfield_size) {
        c = 'S';
        serialise(out, &c);
        serialise(out, &(t->base_type));
        serialise(out, &(t->linkage));
        return;
    }
    c = 'N';
    serialise(out, &c);

    serialise(out, &(t->base_type));
    serialise(out, &(t->is_array));
    serialise(out, &(t->array_size));
    serialise(out, &(t->bitfield_size));
    serialise(out, &(t->is_const));
    if (!t->array_type) {
        serialise(out, 0);
    } else {
        serialise(out, 1);
        serialise(out, t->array_type);
    }
    serialise(out, &(t->linkage));
    serialise(out, &(t->is_function));
    if (!t->struct_name) {
        serialise(out, 0);
    } else {
        serialise(out, 1);
        serialise(out, t->struct_name);
    }
    if (!t->namespaces) {
        serialise(out, 0);
    } else {
        serialise(out, 1);
        serialise(out, t->namespaces);
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
    if (!t->parameter_types) {
        serialise(out, 0);
    } else {
        serialise(out, 1);
        serialise(out, t->parameter_types);
    }
}

char *deserialise(char *in, Element::Type *t)
{
    char c;
    in = deserialise(in, &c);
    if (c == 'S') {
        in = deserialise(in, &(t->base_type));
        in = deserialise(in, &(t->linkage));
        t->is_array = 0;
        t->array_size = 0;
        t->array_type = NULL;
        t->is_function = 0;
        t->namespaces = NULL;
        t->struct_name = NULL;
        t->points_to = NULL;
        t->return_type = NULL;
        t->parameter_types = NULL;
        return in;
    }
    if (c != 'N') {
        fprintf(stderr,
                "got wrong char, expected N, got '%c' (%d)\n",
                c, c);
        abort();
    }

    in = deserialise(in, &(t->base_type));
    in = deserialise(in, &(t->is_array));
    in = deserialise(in, &(t->array_size));
    in = deserialise(in, &(t->bitfield_size));
    in = deserialise(in, &(t->is_const));
    int is_present;
    in = deserialise(in, &is_present);
    if (is_present) {
        Element::Type *at = new Element::Type();
        in = deserialise(in, at);
        t->array_type = at;
    }
    in = deserialise(in, &(t->linkage));
    in = deserialise(in, &(t->is_function));
    in = deserialise(in, &is_present);
    if (is_present) {
        std::string *sn = new std::string();
        in = deserialise(in, sn);
        t->struct_name = sn;
    }
    in = deserialise(in, &is_present);
    if (is_present) {
        std::vector<std::string> *ns =
            new std::vector<std::string>();
        in = deserialise(in, ns);
        t->namespaces = ns;
    }
    in = deserialise(in, &is_present);
    if (is_present) {
        Element::Type *pt = new Element::Type();
        in = deserialise(in, pt);
        t->points_to = pt;
    }
    in = deserialise(in, &is_present);
    if (is_present) {
        Element::Type *rt = new Element::Type();
        in = deserialise(in, rt);
        t->return_type = rt;
    }
    in = deserialise(in, &is_present);
    if (is_present) {
        std::vector<Element::Type*> *vt =
            new std::vector<Element::Type*>();
        in = deserialise(in, vt);
        t->parameter_types = vt;
    }
    return in;
}

char *deserialise(char *in, Element::Type **t)
{
    Element::Type *mnt = new Element::Type();
    *t = mnt;
    return deserialise(in, mnt);
}

void serialise(FILE *out, Element::Variable *v)
{
    serialise(out, v->type);
    serialise(out, v->name);
    if (!v->internal_name) {
        serialise(out, 0);
    } else {
        serialise(out, 1);
        serialise(out, v->internal_name);
    }
    serialise(out, v->once_tag);
}

void serialise(FILE *out, Element::Variable **v)
{
    serialise(out, *v);
}

char *deserialise(char *in, Element::Variable *v)
{
    Element::Type *vt = new Element::Type();
    in = deserialise(in, vt);
    v->type = vt;

    std::string *name = new std::string();
    in = deserialise(in, name);
    v->name = name;

    int is_present;
    in = deserialise(in, &is_present);
    if (!is_present) {
        v->internal_name = NULL;
    } else {
        std::string *vname = new std::string();
        in = deserialise(in, vname);
        v->internal_name = vname;
    }
    in = deserialise(in, &(v->once_tag));

    v->value = NULL;

    return in;
}

char *deserialise(char *in, Element::Variable **v)
{
    Element::Variable *vv = new Element::Variable();
    vv->serialise = false;
    *v = vv;
    return deserialise(in, vv);
}

void serialise(FILE *out, Element::Function *fn)
{
    serialise(out, fn->return_type);
    serialise(out, fn->parameter_types);
    serialise(out, fn->is_macro);
    serialise(out, fn->internal_name);
    serialise(out, fn->always_inline);
    serialise(out, fn->once_tag);
    serialise(out, fn->cto);

    return;
}

void serialise(FILE *out, Element::Function **fn)
{
    serialise(out, *fn);
}

char *deserialise(char *in, Element::Function *fn)
{
    Element::Type *rt = new Element::Type();
    in = deserialise(in, rt);
    fn->return_type = rt;

    std::vector<Element::Variable *> *params =
        new std::vector<Element::Variable *>;
    in = deserialise(in, params);
    fn->parameter_types = params;

    fn->llvm_function = NULL;

    in = deserialise(in, &(fn->is_macro));

    std::string *name = new std::string();
    in = deserialise(in, name);
    fn->internal_name = name;

    in = deserialise(in, &(fn->always_inline));
    in = deserialise(in, &(fn->once_tag));
    in = deserialise(in, &(fn->cto));

    return in;
}

char *deserialise(char *in, Element::Function **fn)
{
    Element::Function *ff = new Element::Function();
    ff->serialise = false;
    *fn = ff;
    return deserialise(in, ff);
}

void serialise(FILE *out, Element::Struct *st)
{
    serialise(out, &(st->is_opaque));
    serialise(out, st->element_types);
    serialise(out, st->names_to_numbers);
    serialise(out, st->internal_name);
    serialise(out, st->once_tag);
    serialise(out, st->linkage);
    serialise(out, st->must_init);

    return;
}

void serialise(FILE *out, Element::Struct **st)
{
    serialise(out, *st);
}

char *deserialise(char *in, Element::Struct *st)
{
    st->type = NULL;
    in = deserialise(in, &(st->is_opaque));
    std::vector<Element::Type *> *et =
        new std::vector<Element::Type *>;
    in = deserialise(in, et);
    st->element_types = et;
    std::map<std::string, int> *nn =
        new std::map<std::string, int>;
    in = deserialise(in, nn);
    st->names_to_numbers = nn;
    std::string *intname = new std::string();
    in = deserialise(in, intname);
    st->internal_name = intname;
    in = deserialise(in, &(st->once_tag));
    in = deserialise(in, &(st->linkage));
    in = deserialise(in, &(st->must_init));

    return in;
}

char *deserialise(char *in, Element::Struct **st)
{
    Element::Struct *stt = new Element::Struct();
    stt->serialise = false;
    *st = stt;
    return deserialise(in, stt);
}

void serialise(FILE *out, Element::Enum *en)
{
    serialise(out, &(en->last_index));
    serialise(out, en->names_to_numbers);
    serialise(out, en->once_tag);
    serialise(out, en->linkage);

    return;
}

void serialise(FILE *out, Element::Enum **en)
{
    serialise(out, *en);
}

char *deserialise(char *in, Element::Enum *en)
{
    en->type = NULL;
    in = deserialise(in, &(en->last_index));
    std::map<std::string, int64_t> *nn =
        new std::map<std::string, int64_t>;
    in = deserialise(in, nn);
    en->names_to_numbers = nn;
    in = deserialise(in, &(en->once_tag));
    in = deserialise(in, &(en->linkage));

    return in;
}

char *deserialise(char *in, Element::Enum **en)
{
    Element::Enum *enn = new Element::Enum();
    enn->serialise = false;
    *en = enn;
    return deserialise(in, enn);
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

char *deserialise(char *in, Namespace *ns)
{
    std::map<std::string, std::vector<Element::Function *>*> *fns =
        new std::map<std::string, std::vector<Element::Function
    *>*>;
    in = deserialise(in, fns);
    ns->functions = *fns;

    std::map<std::string, Element::Variable *> *vars =
        new std::map<std::string, Element::Variable*>;
    in = deserialise(in, vars);
    ns->variables = *vars;

    std::map<std::string, Element::Struct *> *sts =
        new std::map<std::string, Element::Struct *>;
    in = deserialise(in, sts);
    ns->structs = *sts;

    std::map<std::string, Element::Enum *> *ens =
        new std::map<std::string, Element::Enum *>;
    in = deserialise(in, ens);
    ns->enums = *ens;

    std::string name;
    in = deserialise(in, &name);
    ns->name = name;

    std::string symbol_prefix;
    in = deserialise(in, &symbol_prefix);
    ns->symbol_prefix = symbol_prefix;

    if (ns->symbol_prefix.size()) {
        ns->has_symbol_prefix = true;
    }

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

char *deserialise(char *in, NSNode *nsnode)
{
    Namespace *ns = new Namespace();
    in = deserialise(in, ns);
    nsnode->ns = ns;

    std::map<std::string, NSNode *> *children = new
        std::map<std::string, NSNode *>;
    in = deserialise(in, children);
    nsnode->children = *children;

    return in;
}

char *deserialise(char *in, NSNode **nsnode)
{
    NSNode *nsn = new NSNode();
    *nsnode = nsn;
    return deserialise(in, nsn);
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

char *deserialise(char *in, Context *ctx)
{
    NSNode *nsnode = new NSNode();
    in = deserialise(in, nsnode);
    ctx->namespaces = nsnode;

    return in;
}

char *deserialise(char *in, Context **ctx)
{
    Context *mc = new Context();
    *ctx = mc;
    return deserialise(in, mc);
}

template<typename T>
void serialise(FILE *out, std::vector<T> *x)
{
    size_t s = x->size();
    serialise(out, s);
    for (size_t i = 0; i < s; i++) {
        serialise(out, x->at(i));
    }
}

template<typename T>
void serialise(FILE *out, std::vector<T> **x)
{
    serialise(out, *x);
}

template<typename T>
char *deserialise(char *in, std::vector<T> *x)
{
    size_t s;
    in = deserialise(in, &s);
    x->reserve(s);
    for (size_t i = 0; i < s; i++) {
        T item;
        in = deserialise(in, &item);
        x->push_back(item);
    }
    return in;
}

template<typename T>
char *deserialise(char *in, std::vector<T> **x)
{
    std::vector<T> *b = new std::vector<T>;
    *x = b;
    return deserialise(in, b);
}

template<typename T>
void serialise(FILE *out, std::set<T> *x)
{
    size_t s = x->size();
    serialise(out, s);
    for (typename std::set<T>::iterator b = x->begin(),
            e = x->end();
            b != e;
            ++b) {
        serialise(out, *b);
    }
}

template<typename T>
char *deserialise(char *in, std::set<T> *x)
{
    size_t s;
    in = deserialise(in, &s);
    for (size_t i = 0; i < s; i++) {
        T item;
        in = deserialise(in, &item);
        x->insert(item);
    }
    return in;
}

template<typename T>
char *deserialise(char *in, std::set<T> **x)
{
    std::set<T> *b = new std::set<T>;
    *x = b;
    return deserialise(in, b);
}

template<typename T>
void serialise(FILE *out, std::set<T> **x)
{
    serialise(out, *x);
}

template<typename T1, typename T2>
void serialise(FILE *out, std::map<T1, T2> *x)
{
    size_t s = x->size();
    serialise(out, s);
    for (typename std::map<T1, T2>::iterator b = x->begin(), e = x->end();
            b != e;
            ++b) {
        serialise(out, b->first);
        serialise(out, b->second);
    }
}

template<typename T1, typename T2>
char *deserialise(char *in, std::map<T1, T2> *x)
{
    size_t s;
    in = deserialise(in, &s);
    for (size_t i = 0; i < s; i++) {
        T1 key;
        in = deserialise(in, &key);
        T2 value;
        in = deserialise(in, &value);
        x->insert(std::pair<T1, T2>(key, value));
    }
    return in;
}
}

#endif
