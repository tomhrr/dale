# Dale

[Previous](./1-4-variables.md) | [Next](./1-6-control-flow.md)

## 1.5 Functions

Functions are declared like so:

        (def {name} (fn {linkage}
                        [(attr {attr1} {attr2} ... {attrN})]
                        {return-type}
                        [({param1} {param2} ... {paramN})]
          {body}))
    
`{linkage}` is one of `intern`, `extern` and `extern-c`. As per C++,
symbols for `extern` and `intern` functions are 'mangled' based on the
function's parameter types, so as to support overloading. Symbols for
`extern-c` functions are not mangled.

Each `{attr}` is a function attribute type. There are currently two
attributes that can be set:

  * `inline` causes the function to be inlined in all call sites; and
  * `cto` (compile-time only) indicates that the function should be
    removed after compilation.

Each `{param}` is a name-type pair. The last `{param}` may also be the
string '...', which denotes a `varargs` function. A function that
takes no parameters should have the atom `void` as the sole element
in its parameter list.

A function that adds three integers looks like so:

        (def add-3-ints (fn intern int ((a int) (b int) (c int))
          (return (+ (+ a b) c))))

If the last expression in a function is being returned by the
function, it is not necessary to add an explicit `return`.

### Overloading

Functions may be overloaded on their parameter types:

        (def + (fn intern int ((a int) (b int) (c int))
          (+ (+ a b) c)))

        (def + (fn intern int ((a int) (b int) (c int) (d int))
          (+ (+ a b c) d)))

Return types are not taken into account for the purposes of
overloading.

### Varargs

Varargs functions are written in nearly the same way as in C. The
`va-list` type and associated functions are provided by the compiler.
    
        (def va-test (fn intern int ((args int) ...)
          (def arglist (var auto va-list))
          (va-start (cast (# arglist) (p void)))

          ; within a loop:

            (va-arg (# arglist) {va-arg-type})

          ; at the end of the function:

          (va-end (cast (# arglist) (p void)))
          0))

### Overriding core forms

Certain core forms may be overridden by user-level functions, namely
`@`, `#` and `$`. `setf` may also be 'overridden', in effect, by
defining functions named `setf-copy-init`, `setf-copy-assign`,
`setf-move-init`, and `setf-move-assign`.  These are discussed in more
detail in [Initialisers and destructors](./1-8-init-destroy.md).

The `core` core form may, in turn, be used to ignore overridden core
forms. For example:

        (core @ my-pointer-type)

will return the raw dereferenced value of `my-pointer-type`.

### Function pointers

A function pointer is declared like so:

        (def fp (var auto (p {function-type}) {address-of-function}))

For example:

        (def fp (var auto (p (fn int ((a int) (b int)))) (# + int int)))

When using the address-of (`#`) operator on a function, it is
necessary to include the function argument types so as to distinguish
between overloaded functions. However, if the function only has one
definition, and that definition has `extern-c` linkage, the argument
types may be omitted.

Local type deduction makes function pointer definitions much simpler:

        (def fp (var auto \ (# + int int)))

Function pointers may be used in place of normal functions, or by
using the `funcall` core form:

        (printf "%d\n" (fp 1 2))         ; prints "3\n"
        (printf "%d\n" (funcall fp 1 2)) ; also prints "3\n"

### Anonymous functions

Anonymous functions are declared like so:

        (fn {return-type} ({param1} {param2} ... {paramN}) {body})

The type of that expression is 'pointer to underlying function type':
it is not necessary to use the address-of (`#`) operator to take the
address of the newly-'constructed' function.

Note that these functions are not closures, and do not have access to
the surrounding environment.

### Reference parameters

Function parameters may be passed by reference. Within the body of the
function, the arguments for those parameters have a type of 'pointer
to referenced type'; a reference is not a separate type as such.  For
example:

        (def add-2-ints (fn intern int ((a (ref int)) (b (ref int)))
          (return (+ (@ a) (@ b)))))

Values passed by reference must be addressable, unless the reference
is to a constant type.

### Rvalue reference parameters

These operate in the same way as reference parameters, except that the
arguments must be rvalues, and the parameter is marked with `rv-ref`
instead of `ref`. An lvalue may be converted into an rvalue by using
the `move` core form.

### Retvals

A function's return type may be marked as `retval`. Such a function,
rather than returning a value using `return`, should write the result
to the binding `retval`, which is provided implicitly by the compiler.
That binding has the type 'pointer to actual return type', and its
value is used as the return value of the function. `retval` allows for
avoiding unnecessary allocations/copies. For example:

        (def add-2-ints (fn intern (retval int) ((a int) (b int))
          (setf retval (+ a b))
          (return)))

        ...

        (def n (var auto int (add-2-ints 5 10)))
        (printf "%d\n" n) ; prints "15\n"

(As with reference parameters, `int` is not a type for which this
would be used in practice.)

### invoke

A form that is not a procedure call can be treated as though it were
one by defining a function or macro named `invoke`, which is able to
accept the element (or elements) of that form as its arguments.  For
example:

        (def invoke (fn intern int ((a int) (b int))
          (+ a b)))

        ...

        (def n (var auto int 1))
        (printf "%d\n" (n 2)) ; prints "3\n"

[Previous](./1-4-variables.md) | [Next](./1-6-control-flow.md)
