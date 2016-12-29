# Dale

[Previous](./2-8-concept-defs.md) | [Next](./2-10-utility.md)

## 2.9 concepts

### Details

Module: concepts

### Description

Imports the other concept-related modules, and implements the
following concepts:

  * `Swappable`;
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


#### `move@`

Linkage: `extern`
Parameters:

  * `frm`


Expands a form `frm` into `(move (@ frm))`.


#### `prefer-ref-bindings`

Linkage: `extern`
Parameters:

  * `T2`
  * `S`


Takes a type and a suffix as its arguments.  Expands into a series of
`def` forms: the first is `prefer-refs`, being a boolean indicating
whether this type implements the `PreferRefs` concept; the second is
`tpw` (type parameter wrapper), which should be used for parameters of
the specified type (e.g. `((uq tpw) parameter)`), and the third is
`tvw` (type value wrapper), which should be used when accessing the
value of the relevant parameter.  There are additionally `tpw-ro` and
`tvw-ro`, for when types are used in a read-only fashion.  The form
names described above each take a hyphen and the string of the suffix
node, so as to allow disambiguation when multiple calls are required.

There is an additional overload of this macro, which takes two forms
that should evaluate to either `true` or `false`, representing whether
copy is disabled for the type and whether the type prefers references,
as well as a suffix node.  It uses those forms to determine the
appropriate type parameter and value wrappers, but otherwise operates
as per the other version.


[Previous](./2-8-concept-defs.md) | [Next](./2-10-utility.md)

