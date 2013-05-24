# Dale

## 2.6 macros

### Details

Module: macros  
File: macros  

### Description

Provides the quasiquotation macro.















### Macros

#### `std.macros.qq`

Linkage: `extern`  
Parameters: `void`


The 'general-use' quasiquotation macro. The forms handled specially
are:

  * `uq` (unquote): expands to the argument node (token or list);
  * `uql` (unquote-list): expands to the list node of the argument
    node, including all following nodes;
  * `uq-nc` (unquote no-copy): as per `uq`, except that the argument
    node is not copied on substitution; and
  * `uql-nc` (unquote-list no-copy): as per `uql`, except that the list
    nodes are not copied on substitution.

Quasiquotation forms may be nested: each specially-handled form in a
nested `qq` must be wrapped with an additional `uq` for each level of
nesting.

The `-nc` versions should only be used when the argument node will not
be used again.