# Dale

## 2.10 concepts

[Previous](./2-9-concept-defs.md) | [Next](./2-11-utility.md)

### Details

Module: concepts

### Description

Imports the other concept-related modules, and implements the
following concepts:

  * `Assignable`;
  * `EqualityComparable`; and
  * `LessThanComparable`

over the built-in integral types, as well as `char`, `size` and
`ptrdiff`. Provides `swap` functions to suit, too.



### Macros

#### `refconst`

Linkage: `extern`
Parameters:

  * `frm`


Expands a form `frm` into `(ref (const frm))`.


#### `prefer-ref-bindings`

Linkage: `extern`
Parameters:

  * `T2`


Takes a type as its single argument.  Expands into a series of `def`
forms: the first is `prefer-refs`, being a boolean indicating whether
this type implements the `PreferRefs` concept; the second is `tpw`
(type parameter wrapper), which expands to `refconst` for types
preferring references and `identity` otherwise, and the third is `tvw'
(type value wrapper), which expands to `@` for types preferring
references and `identity` otherwise.


[Previous](./2-9-concept-defs.md) | [Next](./2-11-utility.md)

