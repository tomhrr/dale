# Dale

[Previous](./1-7-macros.md) | [Next](./1-9-namespaces.md)

## 1.8 Initialisers and destructors

### `init`

An `init` function is defined like so:

        (def init (fn {linkage} bool ((val (ref {type})))
          {body}))

If a lexically-scoped variable of the specified type is defined within
a procedure without being initialised, this function will be run with
that variable as its argument.

If `init` is not defined over a type, then a variable of that type
that is not explicitly initialised will have indeterminate state.
This is also the case for variables defined outside of procedures,
even if `init` is defined over the type: in those instances, `init`
must be called explicitly for the variable at runtime.

### `setf-copy-init`

A `setf-copy-init` function is defined like so:

        (def setf-copy-init (fn {linkage} bool ((dst (p {type}))
                                                (src (p (const {type}))))
          {body}))

Such functions will be used in lieu of the default `setf` behaviour
whenever `dst` is uninitialised.  The source type may be any type:
this allows for a variable to be initialised from a value of a
different type.  Multiple bindings may be defined for the same
destination type.  On defining a variable, if the initial value has a
type for which there is a `setf-copy-init` binding with that type for
the source, then that binding will be used to initialise the variable.

`setf-copy-init` is also used when a value needs to be copied
implicitly, such as when a value is returned from a function.  If it
is not defined over a type, then implicit copy operations are
equivalent to a `memcpy` from source to destination.  Only the binding
which has `(p (const {type}))` as its source type will be used for
implicit copying.

### `setf-copy-assign`

`setf-copy-assign` functions are defined in the same manner as
`setf-copy-init` functions.  They are used whenever `dst` has already
been initialised.  As with `setf-copy-init`, the source type may be
any type, and multiple bindings may be defined for the same
destination type.

### `setf-move-init`

`setf-move-init` functions are defined in the same manner as
`setf-copy-init` functions, save that the `src` parameter must be an
rvalue reference parameter.  This constraint means that there is only
ever one `setf-move-init` binding for a given destination type.

Where possible, `setf-move-init` is used instead of `setf-copy-init`
for implicit operations that would otherwise have to be copies.

### `setf-move-assign`

`setf-move-assign` functions are defined in the same manner as
`setf-move-init` functions.  They are used whenever `dst` has already
been initialised.

If a type should only be moved, and not copied, declare a function
named `setf-copy-disabled` over the type. If that declaration exists,
any attempt to implicitly or explicitly copy a value of that type will
fail with the error message 'copying is disabled for this type'.

Multiple `setf-` functions, supporting different source types, may be
defined over a single destination type.

### `destroy`

Destructors are defined like so:

        (def destroy (fn {linkage} void ((val (ref {source-type})))
          {body}))

They are run when a value of the relevant type goes out of scope.

If `destroy` is not defined over a particular type, then any attempt
to call `destroy` on a value of that type will become a no-op.

[Previous](./1-7-macros.md) | [Next](./1-9-namespaces.md)
