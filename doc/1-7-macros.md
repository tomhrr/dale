# Dale

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
list. If `is-list` is zero, then the node is a non-list node, and
`token-str` will contain the node's content. `next-node` is applicable
to both list and non-list nodes, and points to the next node after the
current node.

The return type of all macros is a pointer to a `DNode`. This value
will replace the call to the macro. If the value is a null pointer,
then the call to the macro will be removed (i.e. the call will be a
no-op).

### Macro context

The body of a macro has access to two arguments implicitly provided by
the compiler: `pool` and `arg-count`. 

`pool` is a pointer to a struct of type `PoolNode`. It is used within
the macro body to allocate memory, by way of the `pool-malloc`
function, which has the following prototype:

        (def pool-malloc
          (fn extern (p void) ((pool-node (p PoolNode)) (n size))))

Memory allocated by way of this function will be freed by the compiler
after it has finished evaluating the macro.

`arg-count` is an `int` containing the total number of arguments
passed to the macro. This is present so that varargs macros can be
supported without requiring macro users to also provide the number of
arguments/forms being passed to the macro. Varargs macros otherwise
operate in the same way as varargs functions.

### Examples

The identity macro is like so:

        (def identity (macro extern (N) N))

        ; ...

        (identity (+ 1 1)) ; becomes (+ 1 1)

As per functions, the last value evaluated within the body of the
macro is returned implicitly, in the absence of an explicit `return`
instruction.

There are two principal standard libraries that deal with macros:
[`macros-core`](./2-4-macros-core.md) and [`macros`](./2-6-macros.md).
The former provides many functions that ease macro authorship. The
latter provides the quasiquotation (`qq`) macro, which is one of the
more useful macro-writing macros. For example, an `unless` (opposite
of `if`) macro would look like so, when using `qq`:

        (def unless (macro extern (expr tc fc)
          (qq if (not (uq expr)) (uq tc) (uq fc))))

`uq` is a form that 'unquotes' the specified `DNode`, when used within
a `qq` form. There is a corresponding `uql` form ('unquote-list'),
that takes the list from the specified `DNode` and 'splices' it into
place.

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

### Idempotence

Macros may be evaluated more than once per call site by the compiler,
so it is important that they are idempotent. One exception to this is
that any errors reported by the macro to the compiler, by way of the
`report-error` function (see
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
