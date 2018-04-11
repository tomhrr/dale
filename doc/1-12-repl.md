# Dale

[Previous](./1-11-form-reference.md) | [Next](./1-13-tools.md)

## 1.12 REPL

`daleci` is the REPL executable.  It works like so:

 - Top-level expressions, like variable and procedure definitions and
   module import statements, are evaluated in the same way as for a
   compiled program.
 - Each other expression is evaluated as if it were run within a new
   anonymous function.  The result of the expression is bound to the
   variable '\_', unless the expression has a type of `void`, in which
   case no binding occurs.
 - On submitting a non-top-level expression, the type of the
   expression is printed to standard output, followed by the output
   (if any) of executing the expression.
 - No modules are imported by default.  The only bindings that are
   available are those provided implicitly to all programs, such as
   the base set of arithmetical and relational operators.

An example session:

    > (+ 1 2)
    int
    > (+ 3 _)
    int
    > (import cstdio)
    > (printf "%d\n" _)
    int
    6
    > (+ 1.0 _)
    /dev/stdin:7:1: error: overloaded function/macro not in scope: '+'
    (parameters are float int, closest candidate expects (const float)
    (const float))
    > (+ 1.0 2.0)
    float

The REPL does not provide any readline/editline-like functionality, so
using something like `rlwrap` is recommended.

[Previous](./1-11-form-reference.md) | [Next](./2-1-introspection.md)
