# Dale

[Previous](./2-21-enum.md) | [Next](./2-23-variant.md)

## 2.22 bitset-enum

### Details

Module: bitset-enum

### Description

Provides `def-bitset-enum`, which allows for defining bitset enums.
These operate in the same way as normal enums, except that the
sequence goes 0, 1, 2, 4, ..., with each successive value being twice
that of the previous one.  For example,

        (def-bitset-enum my-enum intern int (a b c d))

expands to:

        (def-enum my-enum intern int ((a 0) (b 1) (c 2) (d 4)))

and

        (def-bitset-enum my-enum intern int ((a 0x2) b c d))

expands to:

        (def-enum my-enum intern int ((a 0x2) (b 0x4) (c 0x8) (d 0x10)))



### Macros

#### `def-bitset-enum`

Linkage: `extern`
Parameters:

  * `name`: The name of the new enum.
  * `linkage`: The linkage for the new enum.
  * `type`: The type for the new enum.
  * `forms`: The elements for the new enum.



[Previous](./2-21-enum.md) | [Next](./2-23-variant.md)

