# Dale

## 1.1 Introduction

Dale is an unsafe (no GC), homoiconic programming language. It uses
S-expressions for its syntax, and is essentially an experiment in
seeing how a language at this level can work when it has 'proper'
macros. The basic language is similar to C; the notable additions are:

  * local type deduction;
  * overloaded functions;
  * anonymous functions;
  * function structs;
  * overridden copy/assign operations;
  * destructors;
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
    single types; i.e. `(+ int int)` is defined by default, but `(+ int
    float)` is not.

  * Linkage is not implicit in the core language. This extends to
    local variables in functions and macros. (In practice, the `let`
    macro is generally used to introduce local variable bindings, so
    this isn't a significant problem.)

Throughout the documentation, the term 'procedure' is used to refer to
functions and macros as a single group.

The language is still very much at the experimental/toy stage. There
are likely to be significant bugs and other problems, as well as
documentation inconsistencies. The compiler itself is very slow and
leaky, the generated code is not very good and the code (both for the
compiler and the libraries) is best avoided.
