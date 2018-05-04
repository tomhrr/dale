# Dale

[Next](./1-2-setup.md)

## 1.1 Introduction

Dale is a system (no GC) programming language that uses S-expressions
for syntax and supports syntactic macros. The basic language is
similar to C, with the following additional features:

  * local type deduction;
  * overloaded functions;
  * anonymous functions;
  * reference parameters;
  * initialisers and destructors;
  * variants;
  * namespaces;
  * modules;
  * concepts; and
  * compiler introspection.

There are also a number of core differences in basic functionality, as
compared with C:

  * The only implicit casting that occurs is:
    * from non-const to const, generally;
    * for integral type arguments to `extern-c` functions; and
    * for values used to initialise explicitly-typed variable 
      declarations.

  * Arithmetical operators (`+`, `-`, etc.) are exposed as functions,
    rather than core forms. Functions like `+` are only defined for
    single types; i.e. `(+ int int)` is defined by default, but `(+
    int float)` is not. However, there are macros in the `stdlib`
    module corresponding to the core operators/functions (e.g. `+'`,
    `-'`) that cast the second argument to that of the first, to make
    this a bit easier.

  * Linkage is not implicit in the core language. This extends to
    local variables in functions and macros. (In practice, the `let`
    macro is generally used to introduce local variable bindings, so
    this isn't a significant problem.)

Throughout the documentation, the term 'procedure' is used to refer to
functions and macros as a single group.

Note that the language is still very much at the experimental/toy
stage, so there are likely to be bugs, documentation inconsistencies
and so on.

### Motivation

The aim with Dale is to see how a language with a minimal C-like core
plus syntactic macros works in practice, and whether things usually
implemented within languages can be moved out to libraries if they
have the appropriate level of support from the core.  As an ongoing
project, it will retain the following properties:

  * relatively simple core, with new features implemented as libraries
    wherever possible;
  * easy interoperation with C code;
  * no features that impose mandatory performance penalties (e.g. GC);
    and
  * one language (no separate macro language).

[Next](./1-2-setup.md)
