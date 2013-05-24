# Dale

## 1.8 Copy/destroy

The `setf` core form may be overridden for a particular type, in
effect, by defining functions named `setf-copy` and `setf-assign`.  A
corresponding destructor for the type may be defined by implementing a
function named `destroy`, which function takes a pointer to the
relevant type as its single argument.

### `setf-copy`

A `setf-copy` function is defined like so:

        (def setf-copy (fn {linkage} bool ((dst {pointer-to-type})
                                           (src {source-type}))
          {body}))

Such functions will be used in lieu of the default `setf` behaviour
whenever `dst` is uninitialised.

### `setf-assign`

`setf-assign` functions are defined in the same manner as `setf-copy`
functions, save for the name. They are used whenever `dst` has already
been initialised.

Since the compiler cannot determine whether a particular variable or
value has actually been initialised in all cases, it is important that
all declared variables are initialised properly, and as early as
possible.

Multiple `setf` functions, supporting different source types, may be
defined over a single destination type.

### `destroy`

Destructors are defined like so:

        (def destroy (fn {linkage} void ((val {pointer-to-type}))
          {body}))

They are run when a value of the relevant type goes out of scope.

If `destroy` is not defined over a particular type, then any attempt
to call `destroy` on a pointer to that type will become a no-op.
