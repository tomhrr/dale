# Dale

[Previous](./2-20-operator-macros.md) | [Next](./2-22-bitset-enum.md)

## 2.21 enum

### Details

Module: enum

### Description

Enumerations ('enums') are declared like so:

        (def-enum {name} {linkage} {type}
          ({member1} {member2} ... {memberN}))

where `{linkage}` is one of `intern` and `extern`, and `{type}` is the
underlying integral type for the enum.

Each `{member}` is either a name atom, or a name-value pair. By
default, enum member values begin from zero. If a member does not have
an explicitly-defined value (i.e. it is a name atom), it takes the
value of the previous member, plus one. For example:

        (def-enum my-enum extern int
          (a b c (d 1) e (f 0) (g -1) h i j k))

The member-value assignments for this enum are as follows:

        a: 0; b:  1; c: 2; d: 1; e: 2; f: 0;
        g: -1; h: 0; i: 1; j: 2; k: 3

When an enum is defined, const variable bindings are introduced for
each enum element, mapping to the value for that element.

Enums are strongly-typed. The type name for an enum is `{name}`. When
an enum is defined, a series of related functions are defined at the
same time: `+`, `-`, `*`, `/`, `%`, `=`, `!=`, `<`, `<=`, `>`, `>=`,
`<<`, `>>`, `&`, `|` and `^`. These functions are in turn defined only
over that enum's type, so a function like `+`, for example, takes two
instances of the enum as its arguments and returns a new enum value as
its result. The exceptions to this are `<<` and `>>` (left and
right-shift), which take `int`s as their second arguments. Note that
enums can be cast to and from their underlying types, too.



### Macros

#### `def-enum`

Linkage: `extern`
Parameters:

  * `name`: The name of the new enum.
  * `linkage`: The linkage for the new enum.
  * `type`: The type for the new enum.
  * `forms`: The elements for the new enum.



[Previous](./2-20-operator-macros.md) | [Next](./2-22-bitset-enum.md)

