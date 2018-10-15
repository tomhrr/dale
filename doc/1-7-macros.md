# Dale

[Previous](./1-6-control-flow.md) | [Next](./1-8-init-destroy.md)

## 1.7 Macros

Macros are declared like so:

        (def {name} 
          (macro {linkage} 
                 ({param1} {param2} ... {paramN})
            {body}))

`{linkage}` is one of `intern` and `extern`.

Each `{param}` is either a parameter name alone, or a name-type pair.
Macros may be overloaded in the same way as functions. However, the
overloading only affects dispatch: all of the macro arguments within
the body of the macro are pointers to `DNode`s, which are described
below.

To see the input form and output expansion of the macro calls on
compilation, pass the `--print-expansions` flag to `dalec`.

### `DNode`

The `DNode` struct definition, provided by the compiler, is as
follows:

        (def DNode
          (struct extern ((is-list   int)
                          (token-str (p char))
                          (list-node (p DNode))
                          (next-node (p DNode)))))

(There are actually many other members in this struct, but they should
generally only be used by standard libraries, and may be
removed/changed without notice.)

`is-list` will be set to a non-zero value for nodes that represent
lists. For such nodes, `list-node` will point to the first node of the
list, unless the list is empty, in which case `list-node` will be
null. If `is-list` is zero, then the node is a non-list node, and
`token-str` will contain the node's content. `next-node` is applicable
to both list and non-list nodes, and points to the next node after the
current node.

The return type of all macros is a pointer to a `DNode`. This value
will replace the call to the macro. If the value is a null pointer,
then the call to the macro will be removed (i.e. the call will be a
no-op).

### Macro context

The body of a macro has access to a macro context argument named 'mc'.
This argument is of type `(p MContext)`, where `MContext` is an opaque
struct type. Many internal macro functions and introspection functions
take the macro context as their initial argument.

Heap allocations within macros should be performed using the
`pool-malloc` function:

        (def pool-malloc
          (fn extern (p void) ((mc (p MContext)) (n size))))

Memory allocated by way of this function will be freed by the compiler
after it has finished evaluating the macro.

The number of arguments passed to the macro can be retrieved by way of
the `arg-count` function, which takes the context as its single
argument. This function is present so that varargs macros can be
supported without requiring macro users to also provide the number of
arguments/forms being passed to the macro. Varargs macros are defined
different from varargs functions: to mark a macro as varargs, add a
final parameter with the name `rest`.  Within the macro, `rest` will
be bound to a pointer to a DNode pointer, containing the rest of the
arguments that were passed to the macro.

### Examples

The identity macro is like so:

        (def identity (macro extern (N) N))

        ; ...

        (identity (+ 1 1)) ; becomes (+ 1 1)

As per functions, the last value evaluated within the body of the
macro is returned implicitly, in the absence of an explicit `return`
instruction.

The quote (`q`) core form allows for converting an arbitrary form into
a pointer to a `DNode` (static). For example:

        (def const-string (macro extern (void) (q (p (const char)))))

The standard library for dealing with macros is (unsurprisingly)
[`macros`](./2-4-macros.md).  It provides many functions that ease
macro authorship, as well as the quasiquotation (`qq`) macro, which is
one of the more useful macro-writing macros. For example, an `unless`
(opposite of `if`) macro would look like so, when using `qq`:

        (def unless (macro extern (expr tc fc)
          (qq if (not (uq expr)) (uq tc) (uq fc))))

`uq` is a form that 'unquotes' the specified `DNode`, when used within
a `qq` form. There is a corresponding `uql` form ('unquote-list'),
that takes the list from the specified `DNode` and 'splices' it into
place.

Note that `qq` 'wraps' its arguments, whereas `q` does not:

        (qq p (const char))  -> (p const char)
        (q (p (const char))) -> (p const char)

### Typed macros

A macro may specify one or more typed parameters, starting from the
beginning of its parameter list. Typed and untyped parameters may not
be interleaved. So, a macro definition like:

        (def unless (macro extern ((expr bool) tc fc)
          (qq if (not (uq expr)) (uq tc) (uq fc))))

will work properly, while:

        (def unless (macro extern (expr (tc int) (fc int))
          (qq if (not (uq expr)) (uq tc) (uq fc))))

will not work properly.

A macro may not have the same set of parameter types as a function of
the same name, and vice-versa, because it would make dispatch
ambiguous. However, if a macro's set of parameters only differs from
that of a function insofar as one or more of the macro's parameters is
untyped, then both may be defined.

For dispatch, the procedure to call is selected from the set of
procedures that can be called using the following logic:

   * non-varargs functions are preferred to varargs functions;
   * varargs functions are preferred to macros;
   * macros with fewer `(p DNode)` parameters are preferred to other
     macros;
   * if there are multiple macros with the same number of `(p DNode)`
     parameters, the one with the earliest typed parameter is
     preferred.

### Evaluation and side effects

For procedure dispatch, the type of each argument needs to be known,
so each argument needs to be fully evaluated.  Since an argument can
itself be a macro call, there may be instances where a macro call only
occurs because of the evaluation necessary for dispatch:

        (def mprint (macro intern (void)
          (printf "test\n")
          (nullptr DNode)))

        (def identity (macro intern (ignored returned)
          returned))

        (def main (fn extern-c int (void)
          (identity (mprint) (printf "hello\n"))))

During compilation of the above, the string "test" will be printed,
even though `(mprint)` isn't evaluated directly.  This is because
`(mprint)` is evaluated as part of determining dispatch for
`identity`.  (The `ignored` argument in the call to `identity` will
still be `(mprint)`, though: the evaluation is purely for dispatch,
and macros always receive unevaluated arguments.)

In addition to the previous consideration, macros in general may be
evaluated more than once per call site by the compiler.  As a result,
it is typically safest for macros to avoid side effects, though there
may be some contexts where that is acceptable.  One exception to this
is that any errors reported by the macro to the compiler, by way of
the `report-error` function (see
[`introspection`](./2-1-introspection.md)), will be cleared by the
compiler in the event that it is unable to evaluate or otherwise use
the macro.

### Functions used by macros

By default, macros will not be included in a compiled program, so the
fact that they reference symbols that are only available at
compile-time is not a problem. However, functions that are used
exclusively by macros, and similarly rely on compile-time-only
symbols, are not removed by the compiler before the final compilation
steps. It is necessary to mark such functions with the `cto`
attribute, so that the compiler removes them before any linking errors
occur.

[Previous](./1-6-control-flow.md) | [Next](./1-8-init-destroy.md)
