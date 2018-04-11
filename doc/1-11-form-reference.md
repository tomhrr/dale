# Dale

[Previous](./1-10-modules.md) | [Next](./1-12-repl.md)

## 1.11 Form reference

In addition to the core forms, there are a number of core macros,
functions, structs and variables provided by the compiler to each
program by default. This reference is limited to forms usable from
within procedures.

### Core forms

#### (`setf` {`destination`} {`source`})

Copies `source` to `destination`.

May be overridden by way of `setf-copy-init`, `setf-copy-assign`,
`setf-move-init`, and `setf-move-assign`.

#### (`@` {`pointer`})

Returns the value pointed to by `pointer`.

May be overridden.

#### (`#` {`value`})

Returns the address of `value`.

May be overridden.

#### (`$` {`array-value`} {`array-index`})

Returns a pointer to the specified array index in `array-value`.

May be overridden.

#### (`:` {`struct-value`} {`struct-member-name`})

Returns a pointer to the specified struct member in `struct-value`.

#### (`null` {`expression`})

Returns a boolean indicating whether `expression` evaluates to a null
pointer.

#### (`nullptr` {`type`})

Returns a null pointer to `type`.

#### (`cast` {`from-value`} {`to-type`})

Casts `from-value` to the specified type and returns it.

Depending on the types involved, casting may change the underlying
representation of the data.  The instances where casting may occur are
listed below, with instances that may change representation marked with an
asterisk:

 - where the source and target types are integer types (includes
   `bool`) ( * );
 - where the source and target types are floating-point types ( * );
 - where the source type is a floating-point type and the targer type
   is an integer type, or vice-versa ( * );
 - where the source and target types are pointer types;
 - where the source type is a non-const struct type and the target
   type is a const struct type; and
 - where the source type is a pointer type and the target type is an
   integer type, or vice-versa ( * );

Casts may also occur implicitly during the compilation process.  The
instances where this occurs are:

 - during struct literal parsing, but only when both the source and
   target types are integer types or floating-point types; and
 - during `extern-c` function processing, for each argument.

#### (`funcall` {`function-pointer`} {`arg1`} {`arg2`} ... {`argN`})

Executes `function-pointer` using the provided arguments, and returns
its result.

#### (`do` {`form1`} {`form2`} ... {`formN`})

Evaluates each form in order. Returns the result of evaluating the
final form.

#### (`new-scope` {`form1`} {`form2`} ... {`formN`})

Introduces a new lexical scope, and evaluates each form within that
scope. As per `do`, the result is that of the final form.

#### (`label` {`name`})

Marks a point in the code, for later use by `goto`.

#### (`goto` {`label-name`})

Branches unconditionally to the specified label and continues
execution.

#### (`if` {`condition`} {`true-case`} {`false-case`})

Executes `true-case` if `condition` is true; otherwise, executes
`false-case`.

#### (`return` {`value`})

Returns `value` from the current procedure to the caller.

#### (`move` {`value`})

Converts an lvalue into an rvalue.

#### (`va-start` {`pointer-to-va-list`})

Initialises the `va-list`, for later use with `va-arg`.

#### (`va-arg` {`pointer-to-va-list`} {`argument-type`})

Returns the next argument from the `va-list`, cast to the specified
type.

#### (`va-end` {`pointer-to-va-list`})

Performs any necessary cleanup required by way of the use of the
`va-list`. 

#### (`array-of` {`count`} {`type`} (`array` {`a1`} {`a2`} ... {`aN`}))

Constructs and returns a new array literal of the specified type.

#### (`sizeof` {`type`})

Returns the size (in bytes) of the specified type.

#### (`offsetof` {`struct-type-name`} {`struct-member-name`})

Returns the offset (in bytes) of the specified struct member within
the struct.

#### (`alignmentof` {`type`})

Returns the required alignment (in bytes) of the specified type.

#### (`p=` {`pointer`} {`pointer`})
#### (`p<` {`pointer`} {`pointer`})
#### (`p>` {`pointer`} {`pointer`})
#### (`p+` {`pointer`} {`pointer-or-integer`})
#### (`p-` {`pointer`} {`pointer-or-integer`})

Pointer comparison and modification forms. The first three return
`bool` values, and the remaining two return pointers that have the
type of the first argument.

#### (`using-namespace` {`name`} {`form1`} {`form2`} ... {`formN`})

Adds the specified namespace to the list of namespaces used for
lookup, and evaluates each of the forms in that context. As per `do`,
the result is that of the final form.

#### (`q` {`form`})

Converts the form into a pointer to `DNode` (static), and returns that
pointer.

#### Other

Variables and structs may be defined within procedures in the same way
as they are defined at the top-level.

### Core macros

These are indistinguishable from core forms from a user's perspective,
but are implemented internally as macros.

#### (`setv` {`destination`} {`source`})

Expands to (`setf` (`#` {`destination`}) {`source`}).

#### (`@$` {`array-value`} {`array-index`})

Expands to (`@` (`$` {`array-value`} {`array-index`})).

#### (`@:` {`struct-value`} {`struct-member-name`})

Expands to (`@` (`:` {`struct-value`} {`struct-member-name`})).

#### (`:@` {`struct-value`} {`struct-member-name`})

Expands to (`:` (`@` {`struct-value`}) {`struct-member-name`}).

#### (`@:@` {`struct-value-pointer`} {`struct-member-name`})

Expands to (`@` (`:` (`@` {`struct-value-pointer`}) {`struct-member-name`})).

### Functions

The following functions are provided for the numeric types:

  * `+`, `-`, `*`, `/`, `%`, `=`, `!=`, `<`, `<=`, `>`, `>=` (all)
  * `<<`, `>>`, `&`, `|`, `^` (integral types only)

They operate in the same way as normal functions, i.e. they can be
addressed, cf. the core forms and macros. The last five operations are
'left shift', 'right shift', 'bitwise and', 'bitwise or' and 'bitwise
xor', respectively.  `-` is defined for both one argument (unary
negation) and two (standard subtraction).

Each shift function takes an `int` value as its second argument, and
returns a value of the type of its first argument.

#### (`pool-malloc` {`pointer-to-mcontext`} {`size`})

Allocates and returns uninitialised memory, as per `malloc`. For use
within macros. The memory will be freed by the compiler after the
macro has been evaluated.

#### (`arg-count` {`pointer-to-mcontext`})

Returns the number of arguments passed to the macro.

#### (`not` {`bool-expression`})

If the argument evaluates to false, returns true, and vice-versa.

### Structs

#### `DNode`

        (def DNode
          (struct extern ((is-list   int)
                          (token-str (p char))
                          (list-node (p DNode))
                          (next-node (p DNode)))))

See [Macros](1-7-macros.md).

#### `va-list` (opaque)

See [Functions](1-5-functions.md) for more information about this
struct.

### Variables

The following variables, defined by default, have the same meaning as
their analogues in C.

  * JMP_BUF_SIZE
  * FPOS_T
  * TIME_T
  * CLOCK_T
  * SIZEOF_LONG
  * SIZEOF_LONG_LONG
  * FLT_RADIX
  * FLT_MANT_DIG
  * FLT_DIG
  * FLT_EPSILON
  * FLT_ROUNDS
  * FLT_MIN_EXP
  * FLT_MAX_EXP
  * FLT_MIN
  * FLT_MAX
  * DBL_MANT_DIG
  * DBL_DIG
  * DBL_EPSILON
  * DBL_MIN_EXP
  * DBL_MAX_EXP
  * DBL_MIN
  * DBL_MAX
  * LDBL_MANT_DIG
  * LDBL_DIG
  * LDBL_EPSILON
  * LDBL_MIN_EXP
  * LDBL_MAX_EXP
  * LDBL_MIN
  * LDBL_MAX
  * HUGE_VAL
  * HUGE_VALF
  * HUGE_VALL
  * L_tmpnam
  * TMP_MAX
  * FILENAME_MAX
  * FOPEN_MAX
  * RAND_MAX
  * EXIT_FAILURE
  * EXIT_SUCCESS

[Previous](./1-10-modules.md) | [Next](./2-1-introspection.md)
