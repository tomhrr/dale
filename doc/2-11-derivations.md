# Dale

[Previous](./2-10-utility.md) | [Next](./2-12-algorithms.md)

## 2.11 derivations

### Details

Module: derivations

### Description

Provides concept macros for generating common functions over types.
The difference between this module and `algorithms` is that this one
is limited to very simple functions, namely the basic relations and
the `swap` function.



### Concept macros

#### `swap`

Linkage: `extern`
Parameters:

  * `(T Type)`: The type node.


Expands to a `swap` function over references to the provided type.


#### `!=`

Linkage: `extern`
Parameters:

  * `(T Type)`: The type node.


Expands to a `!=` function over the provided type. `=` must be defined
over the type before calling this macro.


#### `<=`

Linkage: `extern`
Parameters:

  * `(T Type)`: The type node.


Expands to a `<=` function over the provided type. `<` must be defined
over the type before calling this macro.


#### `>`

Linkage: `extern`
Parameters:

  * `(T Type)`: The type node.


Expands to a `>` function over the provided type. `<` must be defined
over the type before calling this macro.


#### `>=`

Linkage: `extern`
Parameters:

  * `(T Type)`: The type node.


Expands to a `>=` function over the provided type. `<` must be defined
over the type before calling this macro.


#### `=`

Linkage: `extern`
Parameters:

  * `(T Struct)`: The struct type node.


Expands to a `=` function over the provided struct type. `=` must be
defined over each of the struct's member types before calling this
macro. 


#### `<`

Linkage: `extern`
Parameters:

  * `(T Struct)`: The struct type node.


Expands to a `<` function over the provided struct type. `<` must be
defined over each of the struct's member types before calling this
macro. 


#### `relations`

Linkage: `extern`
Parameters:

  * `(T Type)`: The struct type node.


Expands to `=`, `!=`, `<`, `<=`, `>` and `>=` functions over the
provided struct type. Any that have already been defined are skipped.


[Previous](./2-10-utility.md) | [Next](./2-12-algorithms.md)

