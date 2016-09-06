# Dale

[Previous](./2-20-unique-ptr.md) | [Next](./2-22-bitset-enum.md)

## 2.21 operator-macros

### Details

Module: operator-macros

### Description

Provides utilities to generate operators that accept an arbitrary
number of arguments.  Such operators are already defined for the
following operators:

 - arithmetic operators: `+ - * / +' -' *' /'`; and
 - bitwise and logical or: `& | and or`.



### Macros

#### `std.macros.def-identity-macro`

Linkage: `extern`
Parameters:

  * `name`: Name of the macro.
  * `linkage`: Linkage.


Defines a macro that takes a single argument and returns it.


#### `std.macros.def-right-associative-macro`

Linkage: `extern`
Parameters:

  * `name`: Name of the macro.
  * `linkage`: Linkage.
  * `opname`: Optional, defaults to `name`


Defines a macro:
`(name a b ...)` will be expanded to `(opname a (name b ...))`


#### `std.macros.def-left-associative-macro`

Linkage: `extern`
Parameters:

  * `name`: Name of the macro.
  * `linkage`: Linkage.
  * `opname`: Optional, defaults to `name`


Defines a macro:
`(name a b ...)` will be expanded to `(name (opname a b) ...)`


[Previous](./2-20-unique-ptr.md) | [Next](./2-22-bitset-enum.md)

