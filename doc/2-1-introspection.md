# Dale

[Previous](./1-13-tools.md) | [Next](./2-2-ctype.md)

## 2.1 introspection

### Details

Module: introspection

### Description

Exports declarations for various functions provided by the compiler.
These are only available at compile time, i.e. within macros. Almost
all of them are interrogative in nature, the two exceptions being
`register-type` and `report-error`. For the most part, the functions
that take `(p DNode)` arguments will expand macros in those arguments
before performing their core task.



### Functions

#### `exists-fn`

Linkage: `extern-c`
Returns: `bool`
Parameters:

  * `(mc (p MContext))`: An MContext.
  * `(frm (p DNode))`: The form describing the function.


Determine whether the specified function exists. The `frm` argument
should be a list node comprising the return type, the function name
and the argument types: e.g. `(bool < int int)`.


#### `exists-variable`

Linkage: `extern-c`
Returns: `bool`
Parameters:

  * `(mc (p MContext))`: An MContext.
  * `(frm (p DNode))`: The token node containing the variable name.


Determine whether the specified variable exists. The `frm` argument
should be a token node containing the name of the variable.


#### `exists-type`

Linkage: `extern-c`
Returns: `bool`
Parameters:

  * `(mc (p MContext))`: An MContext.
  * `(frm (p DNode))`: The form describing the type.


Determine whether the specified type exists.


#### `exists-macro`

Linkage: `extern-c`
Returns: `bool`
Parameters:

  * `(mc (p MContext))`: An MContext.
  * `(frm (p DNode))`: The form describing the macro.


Determine whether the specified macro exists. The `frm` argument
should be a list node comprising the macro name and the argument
types. It is not necessary to provide the exact types specified by the
macro for its arguments; for example, if a macro `testmacro` takes
three untyped arguments, then a call to `exists-macro` with a form
like `(testmacro int bool int)` will return `true`.


#### `type-of`

Linkage: `extern-c`
Returns: `(p DNode)`
Parameters:

  * `(mc (p MContext))`: An MContext.
  * `(frm (p DNode))`: The form for which the type should be determined.
  * `(report-errors bool)`: Whether to report or suppress errors.


Evaluates the form and returns its type. For example, if `frm` is `(+
1 1)`, the result node will be `int` (single token node).


#### `codomain`

Linkage: `extern-c`
Returns: `(p DNode)`
Parameters:

  * `(mc (p MContext))`: An MContext.
  * `(frm (p DNode))`: The form describing the function.


Returns the codomain (the return type) of the specified function. The
argument form has the same structure as for `exists-fn`, except that
there is no return type at the beginning. Returns a null pointer if
the function doesn't exist.


#### `arity`

Linkage: `extern-c`
Returns: `int`
Parameters:

  * `(mc (p MContext))`: An MContext.
  * `(frm (p DNode))`: The token node containing the function name.


Returns the arity of the specified function (the number of parameters
that it requires). Only works for functions with `extern-c` linkage.
The `frm` argument should contain only the name of the function.


#### `input-type`

Linkage: `extern-c`
Returns: `(p DNode)`
Parameters:

  * `(mc (p MContext))`: An MContext.
  * `(frm (p DNode))`: The token node containing the function name.
  * `(n int)`: The parameter index (begins from 0).


Returns the type of the specified function's `n`th parameter. Only
works for functions with `extern-c` linkage. As per `arity`, the `frm`
argument should contain only the name of the function.


#### `struct-member-count`

Linkage: `extern-c`
Returns: `int`
Parameters:

  * `(mc (p MContext))`: An MContext.
  * `(st (p DNode))`: The token node containing the struct name.


Returns the number of members present in the specified struct.


#### `struct-member-type`

Linkage: `extern-c`
Returns: `(p DNode)`
Parameters:

  * `(mc (p MContext))`: An MContext.
  * `(st (p DNode))`: The token node containing the struct name.
  * `(n int)`: The member index (begins from 0).


Returns the type of the specified struct's `n`th member.


#### `struct-member-name`

Linkage: `extern-c`
Returns: `(p char)`
Parameters:

  * `(mc (p MContext))`: An MContext.
  * `(st (p DNode))`: The token node containing the struct name.
  * `(n int)`: The member index (begins from 0).


Returns the name of the specified struct's `n`th member.


#### `report-error`

Linkage: `extern-c`
Returns: `void`
Parameters:

  * `(mc (p MContext))`: An MContext.
  * `(frm (p DNode))`: The node that 'caused' the error.
  * `(err (p (const char)))`: The error string.


Adds an error message to the compiler's internal error buffer. The
node provided to this function will be used to set the line/column
numbers and the filename in the error message.


#### `register-type`

Linkage: `extern-c`
Returns: `bool`
Parameters:

  * `(mc (p MContext))`: An MContext.
  * `(from (p char))`: The concrete type, as a string.
  * `(to (p char))`: The display name for the type, as a string.


Registers a name (the 'display name') against a concrete type. Such
registrations are used by the compiler for things like error messages,
so this function is primarily useful for macros that create generic
types. In the case of `(Pair int int)`, for example, the concrete type
is a struct with a name like `pairii`, but that type is registered
against the form `(Pair int int)`, so error messages concerning that
type use the latter form.


#### `type-to-display-string`

Linkage: `extern-c`
Returns: `bool`
Parameters:

  * `(mc (p MContext))`: An MContext.
  * `(frm (p DNode))`: The type form.
  * `(buf (p char))`: A buffer into which the display string will be written.


Writes the specified type's display string to `buf`. This string is
either the display name provided by way of `register-type`, or the
stringification of the provided form.


#### `type-to-string`

Linkage: `extern-c`
Returns: `bool`
Parameters:

  * `(mc (p MContext))`: An MContext.
  * `(frm (p DNode))`: The concrete type, as a form.
  * `(buf (p char))`: A buffer into which the encoded type name will be written.


Writes the specified type's concrete type string to `buf`. For
example, `(Pair int int)` has a concrete type of `pairii`, so passing
the former as the `frm` will cause the latter to be written to `buf`.


#### `is-char-type`

Linkage: `extern-c`
Returns: `bool`
Parameters:

  * `(mc (p MContext))`: An MContext.
  * `(frm (p DNode))`: The type form.


Returns a boolean indicating whether the specified type is a character
type.


#### `is-integer-type`

Linkage: `extern-c`
Returns: `bool`
Parameters:

  * `(mc (p MContext))`: An MContext.
  * `(frm (p DNode))`: The type form.


Returns a boolean indicating whether the specified type is an integer
type (either signed or unsigned).


#### `is-signed-integer-type`

Linkage: `extern-c`
Returns: `bool`
Parameters:

  * `(mc (p MContext))`: An MContext.
  * `(frm (p DNode))`: The type form.


Returns a boolean indicating whether the specified type is a signed
integer type.


#### `is-unsigned-integer-type`

Linkage: `extern-c`
Returns: `bool`
Parameters:

  * `(mc (p MContext))`: An MContext.
  * `(frm (p DNode))`: The type form.


Returns a boolean indicating whether the specified type is an unsigned
integer type.


#### `is-floating-point-type`

Linkage: `extern-c`
Returns: `bool`
Parameters:

  * `(mc (p MContext))`: An MContext.
  * `(frm (p DNode))`: The type form.


Returns a boolean indicating whether the specified type is a
floating-point type.


#### `is-pointer-type`

Linkage: `extern-c`
Returns: `bool`
Parameters:

  * `(mc (p MContext))`: An MContext.
  * `(frm (p DNode))`: The type form.


Returns a boolean indicating whether the specified type is a
pointer type.


#### `is-pointer-to-type`

Linkage: `extern-c`
Returns: `bool`
Parameters:

  * `(mc (p MContext))`: An MContext.
  * `(ptr-type (p DNode))`: The pointer type form.
  * `(pte-type (p DNode))`: The pointee type form.


Returns a boolean indicating whether the first type form is a pointer
to the type represented by the second form.


#### `pointee-type`

Linkage: `extern-c`
Returns: `(p DNode)`
Parameters:

  * `(mc (p MContext))`: An MContext.
  * `(ptr-type (p DNode))`: The pointer type form.


Returns the type to which the type form points.


#### `is-const`

Linkage: `extern-c`
Returns: `bool`
Parameters:

  * `(mc (p MContext))`: An MContext.
  * `(frm (p DNode))`: The type form.


Returns a boolean indicating whether the type is a `const` type.


#### `fn-by-args-count`

Linkage: `extern-c`
Returns: `int`
Parameters:

  * `(mc (p MContext))`: An MContext.
  * `(frm (p DNode))`: A list of parameter types.
  * `(prefix (p (const char)))`: An optional function name prefix.


Takes a form (list node) of parameter types and a function prefix
string (may be null). Returns the number of functions that have those
parameter types as their own. If the function prefix string is
provided, then only functions that begin with that prefix will be
taken into account. Calling this function initialises an internal list
containing the names of the functions with the specified parameter
types, which list is used by `fn-by-args-name`.


#### `fn-by-args-name`

Linkage: `extern-c`
Returns: `(p (const char))`
Parameters:

  * `(mc (p MContext))`: An MContext.
  * `(frm (p DNode))`: A list of parameter types.
  * `(n int)`: The function list index.


Takes a form (list node) of parameter types and an index. Returns the
name of the `n`th function that has those parameters types as its own.
The call to this function must be preceded, at some point, by a call
to `fn-by-args-count` that uses the same set of parameter types.


#### `fn-by-name-count`

Linkage: `extern-c`
Returns: `int`
Parameters:

  * `(mc (p MContext))`: An MContext.
  * `(name (p (const char)))`: A function name.


Takes a function name as its single argument.  Returns the number of
functions with that name that are currently in scope.  Calling this
function initialises an internal list containing the instances of the
specified function, which list is used by `fn-by-name-types-count` and
`fn-by-name-types-type`.


#### `fn-by-name-types-count`

Linkage: `extern-c`
Returns: `int`
Parameters:

  * `(mc (p MContext))`: An MContext.
  * `(name (p (const char)))`: A function name.
  * `(n int)`: The function list index.


Takes a function name and an index as its arguments.  Returns the
parameter count of the function at this index of the generated
list.


#### `fn-by-name-types-type`

Linkage: `extern-c`
Returns: `(p DNode)`
Parameters:

  * `(mc (p MContext))`: An MContext.
  * `(name (p (const char)))`: A function name.
  * `(n int)`: The function list index.
  * `(m int)`: The parameter list index.


Takes a function name, a function index, and a parameter index as its
arguments.  Returns the type of the parameter at this index of the
relevant function.


#### `has-errors`

Linkage: `extern-c`
Returns: `bool`
Parameters:

  * `(mc (p MContext))`: An MContext.
  * `(frm (p DNode))`: The form to be evaluated.


Evaluates the provided form and returns a boolean indicating whether
any errors were encountered.


#### `types-equal`

Linkage: `extern-c`
Returns: `bool`
Parameters:

  * `(mc (p MContext))`: An MContext.
  * `(type-1 (p DNode))`: The first type form.
  * `(type-2 (p DNode))`: The second type form.


Returns a boolean indicating whether the two type forms are equal.


#### `printf-length`

Linkage: `extern-c`
Returns: `(const (p char))`
Parameters:

  * `(mc (p MContext))`: An MContext.
  * `(frm (p DNode))`: The type of the printf argument.


Returns a 'length' string that can be put between the '%' and type
specifier in a printf formatter string, based on the size of the
provided type. For example, if the argument form is a token node
containing the string "size", then then returned string will be "z".


#### `is-copy-permitted`

Linkage: `extern-c`
Returns: `bool`
Parameters:

  * `(mc (p MContext))`: An MContext.
  * `(frm (p DNode))`: The type form.
  * `(report bool)`: Whether to report an error on copy not being permitted.


Returns a boolean indicating whether values of this type can be
copied.


#### `eval-expression`

Linkage: `extern-c`
Returns: `bool`
Parameters:

  * `(mc (p MContext))`: An MContext.
  * `(type (p DNode))`: The type form.
  * `(frm (p DNode))`: The literal form.
  * `(buf (p void))`: The buffer for the literal's memory.


Attempts to evaluate the provided form as a value of the specified
type, and writes the memory of the literal into the buffer.  If the
form does not evaluate successfully, returns false.  If the type is
not supported, reports an error and returns false.

Currently, the only supported type is `int`.


#### `eval-macro-call`

Linkage: `extern-c`
Returns: `(p DNode)`
Parameters:

  * `(mc (p MContext))`: An MContext.
  * `(frm (p DNode))`: The form.
  * `(mandatory bool)`
  * `(once bool)`


Attempts to evaluate the provided form as a macro call, and returns
the resulting form.  This function corresponds to Common Lisp's
`macroexpand` and `macroexpand-1` functions.

If `mandatory` is true, then any errors related to the attempted
expansion will be retained, and null will be returned.  If it is
false, then any errors will be suppressed, and the original node will
be returned.

If `once` is true, then only a single macro expansion will occur, if
applicable.  If it is false, then macro expansion will continue until
a form that is not a macro call is returned.


#### `is-lvalue`

Linkage: `extern-c`
Returns: `bool`
Parameters:

  * `(mc (p MContext))`: An MContext.
  * `(frm (p DNode))`: The form.


Determine whether a form is an lvalue.


[Previous](./1-13-tools.md) | [Next](./2-2-ctype.md)

