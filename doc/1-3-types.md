# Dale

[Previous](./1-2-setup.md) | [Next](./1-4-variables.md)

## 1.3 Types

### Primitive types

#### Integrals

`int` is the principal integral type. It has the same size as the
`int` provided by the C compiler used to build Dale, so as to
facilitate interoperability with the system's C libraries. `uint` is
the corresponding unsigned integral type.

The explicitly-sized signed and unsigned integral types are as
follows:

  * `int8`/`uint8`
  * `int16`/`uint16`
  * `int32`/`uint32`
  * `int64`/`uint64`

Integral literals can be plain numbers (e.g. `123`) or hexadecimal
literals (e.g. `0xFF`). Negative numbers are prefixed with `-`.

#### Floating-point types

  * `float`
  * `double`
  * `long-double`

Floating-point literals must contain a decimal point (e.g. 1.0). As
per integral literals, negative numbers are prefixed with `-`.

#### Other types

The following types operate in the same way as the corresponding C
type:

  * `bool` (`bool`)
  * `char` (`char`)
  * `void` (`void`)
  * `size` (`size_t`)
  * `ptrdiff` (`ptrdiff_t`)
  * `intptr` (`uintptr_t`)

The `bool` literals are `true` and `false`.

`char` literals comprise a hash character, a backslash and the actual
character. The literal for the letter `e`, for example, is `#\e`. The
specially-handled character literals are `#\SPACE`, `#\TAB`,
`#\NEWLINE`, `#\CARRIAGE`, `#\NULL` and `#\EOF`.

### Pointers

The syntax for pointer types is `(p {type})`.

### Const

The syntax for const types is `(const {type})`. Const semantics are
the same as in C.

### Arrays

The type of an array of values is `(array-of {n} {type})`, where `{n}`
is a literal unsigned integer or an expression that evaluates to one.
A zero-sized array may be declared when the array is populated during
declaration: see [Variables](./1-4-variables.md).

The instances where arrays decay into pointers on use are the same as
in C.

The `{n}` expression is evaluated in a new anonymous scope: if the
type is specified within a macro or a function, it will not have
access to the bindings defined in that macro or function.
Variable-length arrays are not supported, either.

### Structures

A structure is declared like so:

        (def {name}
          (struct {linkage} [({member1} {member2} ... {memberN})]))

where `{linkage}` is one of `intern`, `extern` and `opaque`, and each
`{member}` contains a field name and type. A simple `pair` structure
would look like so:

        (def int-pair
          (struct intern ((first int) (second int))))

`{linkage}` controls the visibility of a struct defined within a
module. `extern` structs are exportable and usable outside the
module, while `intern` structs are not. `opaque` structs are those
whose members are defined elsewhere:

        (def some-other-struct
          (struct opaque))

Struct literals may be used wherever a reference to a struct may be
used. They take the following form:

        ({name} ({value1} {value2} ... {valueN}))

where each `{value}` is a name-value pair, corresponding to a struct
member. In situations where the value will be cast implicitly, e.g. in
an explicitly-typed variable declaration, it is not necessary to
include the enclosing ({name} ...) form.

### Other types

Function pointers and anonymous functions are described in the
[Functions](./1-5-functions.md) section.

[Previous](./1-2-setup.md) | [Next](./1-4-variables.md)
