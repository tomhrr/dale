# Dale

[Previous](./2-21-operator-macros.md) | [Next](./3-1-cerrno.md)

## 2.22 bitset-enum

### Details

Module: bitset-enum

### Description

Provides `def-bitset-enum`, which allows for defining bitset enums.
These operate in the same way as normal enums, except that the initial
enum value is 1, and each successor enum value is twice that of the
previous one.



### Macros

#### `def-bitset-enum`

Linkage: `extern`
Parameters:

  * `name`: The name of the new enum.
  * `linkage`: The linkage for the new enum.
  * `type`: The type for the new enum.
  * `forms`: The elements for the new enum.



[Previous](./2-21-operator-macros.md) | [Next](./3-1-cerrno.md)

