# Dale

[Previous](./2-5-stdlib.md) | [Next](./2-7-concepts-core.md)

## 2.6 assert

### Details

Module: assert

### Description

Macros for runtime assertions. User-defined compile-time errors can be
reported to the compiler by way of `report-error`, which is defined in
the `introspection` module.



### Macros

#### `std.disable-assertions`

Linkage: `extern`
Parameters: N/A

If called, subsequent calls to `assert` will expand to a no-op. Note
that assertions are enabled by default.


#### `std.enable-assertions`

Linkage: `extern`
Parameters: N/A

If called, subsequent calls to `assert` will expand to actual
assertions.


#### `std.assert`

Linkage: `extern`
Parameters:

  * `condition`: The condition expression.


Expands to a form that tests the condition; if the condition is false,
then an error message is printed to `stderr` and execution is aborted
(via `abort`). The error message will include the filename, line
number and column number of the condition node, as per a normal
compiler error message.

If assertions have been disabled, by way of `disable-assertions`, then
subsequent calls to this macro will expand to no-ops.


[Previous](./2-5-stdlib.md) | [Next](./2-7-concepts-core.md)

