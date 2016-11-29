# Dale

[Previous](./2-5-stdlib.md) | [Next](./2-7-assert.md)

## 2.6 macros

### Details

Module: macros

### Description

Provides the quasiquotation macro.



### Macros

#### `qq`

Linkage: `extern`
Parameters:

  * `frm`
  * `rest`


The 'general-use' quasiquotation macro. The forms handled specially
are:

  * `uq` (unquote): expands to the argument node, excluding any
    following nodes (i.e. excluding `next-node`);
  * `uql` (unquote-list): expands to the argument node, including all
    following nodes (i.e. including `next-node` and any subsequent
    `next-node`s of that node);
  * `uq-nc` (unquote no-copy): as per `uq`, except that the argument
    node is not copied on substitution; and
  * `uql-nc` (unquote-list no-copy): as per `uql`, except that the nodes
    are not copied on substitution.

Quasiquotation forms may be nested: each specially-handled form in a
nested `qq` must be wrapped with an additional `uq` for each level of
nesting.

The `-nc` versions should only be used when the argument node will not
be used again.

Argument nodes for the various unquote forms must be DNode pointer
variable names.  If any other type of argument is provided, the
unquote form will expand to that argument.  For example, `(qq do (uq
(mnfv mc 1)))` will expand to `(do (mnfv mc 1))`.  Issue #140 is
tracking this problem, and will be resolved when arbitrary argument
nodes are supported.


[Previous](./2-5-stdlib.md) | [Next](./2-7-assert.md)

