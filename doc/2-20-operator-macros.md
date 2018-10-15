# Dale

[Previous](./2-19-unique-ptr.md) | [Next](./2-21-enum.md)

## 2.20 operator-macros

### Details

Module: operator-macros

### Description

Provides utilities to generate operators that accept an arbitrary
number of arguments.  Such operators are already defined for the
following operators:

 - arithmetic operators: `+ - * / +' -' *' /'`
 - bitwise and logical or: `& | and or`
 - arithmetic relations: `< <= = => > <' <=' =' =>' >'


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
`(name a b rest) will be expanded to `(opname a (name b rest))`


#### `std.macros.def-left-associative-macro`

Linkage: `extern`
Parameters:

  * `name`: Name of the macro.
  * `linkage`: Linkage.
  * `opname`: Optional, defaults to `name`


Defines a macro:
`(name a b rest)` will be expanded to `(name (opname a b) rest)`


#### `def-transitive-macro`

Linkage: `extern`
Parameters:

  * `name`: The macro name.
  * `linkage`: Linkage.
  * `fn`: The name of the binary function. Defaults to name.


Defines a macro for transitive binary functions (that normally should
return bool and transitive), so that you don't have to use variables
twice when you want to test some function.

For example:

        (def-transitive-macro < intern)
        (< 0.0 x 1.0)

expands to:

        (and (< 0.0 x) (< x 1.0))



[Previous](./2-19-unique-ptr.md) | [Next](./2-21-enum.md)

