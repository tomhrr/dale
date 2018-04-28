#ifndef DALE_SERIALISE
#define DALE_SERIALISE

#include <cstdio>
#include <map>
#include <set>
#include <string>
#include <utility>
#include <vector>

#include "../Context/Context.h"
#include "../Namespace/Namespace.h"
#include "../Type/Type.h"
#include "../TypeRegister/TypeRegister.h"
#include "../Variable/Variable.h"

namespace dale {
/*! Serialise

    Provides module serialisation and deserialisation functions.
    These are not documented individually, since they all follow the
    same pattern: serialise functions take a file pointer and an
    arbitrary type, and deserialise functions take a TypeRegister, a
    char pointer (the input stream), and an arbitrary type pointer.
    Serialise functions do not return anything, whereas deserialise
    functions return a pointer to the next position in the input
    stream.
*/
template <typename T>
void serialise(FILE *out, std::vector<T> *x);

char *deserialise_type_vector(TypeRegister *tr, char *in,
                              std::vector<Type *> *x);

template <typename T>
char *deserialise(TypeRegister *tr, char *in, std::vector<T> *x);

template <typename T>
void serialise(FILE *out, std::vector<T> **x);

template <typename T>
char *deserialise(TypeRegister *tr, char *in, std::vector<T> **x);

template <typename T1, typename T2>
void serialise(FILE *out, std::map<T1, T2> *x);

template <typename T1, typename T2>
char *deserialise(TypeRegister *tr, char *in, std::map<T1, T2> *x);

void xfwrite(const void *a, size_t b, size_t c, FILE *d);

void serialise(FILE *out, bool a);

void serialise(FILE *out, bool *a);

char *deserialise(TypeRegister *tr, char *in, bool *a);

void serialise(FILE *out, int64_t a);

void serialise(FILE *out, int64_t *a);

char *deserialise(TypeRegister *tr, char *in, int64_t *a);

void serialise(FILE *out, char a);

void serialise(FILE *out, char *a);

char *deserialise(TypeRegister *tr, char *in, char *a);

void serialise(FILE *out, int a);

void serialise(FILE *out, int *a);

char *deserialise(TypeRegister *tr, char *in, int *a);

void serialise(FILE *out, size_t s);

void serialise(FILE *out, size_t *s);

char *deserialise(TypeRegister *tr, char *in, size_t *s);

void serialise(FILE *out, std::string x);

void serialise(FILE *out, std::string *x);

char *deserialise(TypeRegister *tr, char *in, std::string *x);

void serialise(FILE *out, Type *t);

char *deserialise(TypeRegister *tr, char *in, Type **t);

void serialise(FILE *out, Variable *v);

void serialise(FILE *out, Variable **v);

char *deserialise(TypeRegister *tr, char *in, Variable *v);

char *deserialise(TypeRegister *tr, char *in, Variable **v);

void serialise(FILE *out, Function *fn);

void serialise(FILE *out, Function **fn);

char *deserialise(TypeRegister *tr, char *in, Function *fn);

char *deserialise(TypeRegister *tr, char *in, Function **fn);

void serialise(FILE *out, Struct *st);

void serialise(FILE *out, Struct **st);

char *deserialise(TypeRegister *tr, char *in, Struct *st);

char *deserialise(TypeRegister *tr, char *in, Struct **st);

void serialise(FILE *out, Namespace *ns);

void serialise(FILE *out, Namespace **ns);

char *deserialise(TypeRegister *tr, char *in, Namespace *ns);

void serialise(FILE *out, NSNode *nsnode);

void serialise(FILE *out, NSNode **nsnode);

char *deserialise(TypeRegister *tr, char *in, NSNode *nsnode);

char *deserialise(TypeRegister *tr, char *in, NSNode **nsnode);

void serialise(FILE *out, Context *ctx);

void serialise(FILE *out, Context **ctx);

char *deserialise(TypeRegister *tr, char *in, Context *ctx);

char *deserialise(TypeRegister *tr, char *in, Context **ctx);

template <typename T>
void serialise(FILE *out, std::vector<T> *x) {
    size_t s = x->size();
    serialise(out, s);
    for (size_t i = 0; i < s; i++) {
        serialise(out, (*x)[i]);
    }
}

template <typename T>
void serialise(FILE *out, std::vector<T> **x) {
    serialise(out, *x);
}

char *deserialise_type_vector(TypeRegister *tr, char *in,
                              std::vector<Type *> *x);

template <typename T>
char *deserialise(TypeRegister *tr, char *in, std::vector<T> *x) {
    size_t s;
    in = deserialise(tr, in, &s);
    x->reserve(s);
    for (size_t i = 0; i < s; i++) {
        T item;
        in = deserialise(tr, in, &item);
        x->push_back(item);
    }
    return in;
}

template <typename T>
char *deserialise(TypeRegister *tr, char *in, std::vector<T> **x) {
    std::vector<T> *b = new std::vector<T>;
    *x = b;
    return deserialise(tr, in, b);
}

template <typename T>
void serialise(FILE *out, std::set<T> *x) {
    size_t s = x->size();
    serialise(out, s);
    for (typename std::set<T>::iterator b = x->begin(), e = x->end();
         b != e; ++b) {
        serialise(out, *b);
    }
}

template <typename T>
char *deserialise(TypeRegister *tr, char *in, std::set<T> *x) {
    size_t s;
    in = deserialise(tr, in, &s);
    for (size_t i = 0; i < s; i++) {
        T item;
        in = deserialise(tr, in, &item);
        x->insert(item);
    }
    return in;
}

template <typename T>
char *deserialise(TypeRegister *tr, char *in, std::set<T> **x) {
    std::set<T> *b = new std::set<T>;
    *x = b;
    return deserialise(tr, in, b);
}

template <typename T>
void serialise(FILE *out, std::set<T> **x) {
    serialise(out, *x);
}

template <typename T1, typename T2>
void serialise(FILE *out, std::map<T1, T2> *x) {
    size_t s = x->size();
    serialise(out, s);
    for (typename std::map<T1, T2>::iterator b = x->begin(),
                                             e = x->end();
         b != e; ++b) {
        serialise(out, b->first);
        serialise(out, b->second);
    }
}

template <typename T1, typename T2>
char *deserialise(TypeRegister *tr, char *in, std::map<T1, T2> *x) {
    size_t s;
    in = deserialise(tr, in, &s);
    for (size_t i = 0; i < s; i++) {
        T1 key;
        in = deserialise(tr, in, &key);
        T2 value;
        in = deserialise(tr, in, &value);
        x->insert(std::pair<T1, T2>(key, value));
    }
    return in;
}
}

#endif
