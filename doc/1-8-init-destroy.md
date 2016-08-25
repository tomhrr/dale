# Dale

[Previous](./1-7-macros.md) | [Next](./1-9-namespaces.md)

## 1.8 Initialisers and destructors

### `init`

An `init` function is defined like so:

        (def init (fn {linkage} bool ((val (ref {source-type})))
          {body}))

If a variable of the specified type is defined without being
initialised, this function will be run with that variable as its
argument.

### `setf-copy-init`

A `setf-copy-init` function is defined like so:

        (def setf-copy-init (fn {linkage} bool ((dst (p {source-type}))
                                                (src (p (const {source-type}))))
          {body}))

Such functions will be used in lieu of the default `setf` behaviour
whenever `dst` is uninitialised.

### `setf-copy-assign`

`setf-copy-assign` functions are defined in the same manner as
`setf-copy-init` functions. They are used whenever `dst` has already
been initialised.

### `setf-move-init`

`setf-move-init` functions are defined in the same manner as
`setf-copy-init` functions, save that the `src` parameter must be an
rvalue reference parameter.

### `setf-move-assign`

`setf-move-assign` functions are defined in the same manner as
`setf-move-init` functions. They are used whenever `dst` has already
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
