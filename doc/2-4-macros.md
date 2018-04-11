# Dale

[Previous](./2-3-math.md) | [Next](./2-5-stdlib.md)

## 2.4 macros

### Details

Module: macros

### Description

Provides the core macro development functions: making, copying and
printing nodes, linking nodes together, gensym functions for
variables and labels, and the quasiquotation form.



### Functions

#### `std.macros.make-node`

Linkage: `extern`
Returns: `(p DNode)`
Parameters:

  * `(mc (p MContext))`: An MContext.


Returns a newly-allocated node.


#### `std.macros.is-empty-node`

Linkage: `extern`
Returns: `bool`
Parameters:

  * `(form (p DNode))`: The node.


Returns a boolean indicating whether the node is an empty node (no
list and no token).


#### `std.macros.make-empty-list-node`

Linkage: `extern`
Returns: `(p DNode)`
Parameters:

  * `(mc (p MContext))`: An MContext.


Returns a newly-allocated empty list node.


#### `std.macros.is-non-empty-list`

Linkage: `extern`
Returns: `bool`
Parameters:

  * `(form (p DNode))`: The node.



#### `std.macros.copy`

Linkage: `extern`
Returns: `(p DNode)`
Parameters:

  * `(mc (p MContext))`: An MContext.
  * `(follow bool)`: Whether to include the nodes that follow `form`.
  * `(form (p DNode))`: The node to copy.



#### `std.macros.copy-qstr`

Linkage: `extern`
Returns: `(p DNode)`
Parameters:

  * `(mc (p MContext))`: An MContext.
  * `(form (p DNode))`: The node to copy.
  * `(follow bool)`: Whether to include the nodes that follow `form`.


As per `copy`, except that if the first node is a token, it will have
double-quotations marks added at the start and end of it in the copied
node.


#### `std.macros.copy-to`

Linkage: `extern`
Returns: `void`
Parameters:

  * `(dst (p DNode))`: The destination node.
  * `(src (p DNode))`: The source node.


Copy (shallow) the details of the source node to the destination node.


#### `std.macros.print`

Linkage: `extern`
Returns: `bool`
Parameters:

  * `(form (p DNode))`: The node to print.


Prints the node to `stdout`.


#### `std.macros.sprint`

Linkage: `extern`
Returns: `bool`
Parameters:

  * `(buf (p char))`: The buffer to which the node will be printed.
  * `(form (p DNode))`: The node to print.


Prints the node to the provided buffer.


#### `std.macros.get-last-node`

Linkage: `extern`
Returns: `(p DNode)`
Parameters:

  * `(form (p DNode))`: The node for which the last node should be found.


Returns the last node in the list, by iterating over `next-node`
(i.e. this does not descend into the `list-node` of the argument
node).


#### `std.macros.mnfv`

Linkage: `extern`
Returns: `(p DNode)`
Parameters:

  * `(mc (p MContext))`: An MContext.
  * `(token-string (p (const char)))`: The token string for the new node.


Short for 'make-node-from-value'. There are several implementations of
this function: each is similar to `make-node`, except that each takes
an additional argument, which is used to populate the `token-str` of
the newly-allocated node. This implementation takes a `(p char)`,
copies it, and sets it in the new node.


#### `std.macros.mnfv`

Linkage: `extern`
Returns: `(p DNode)`
Parameters:

  * `(mc (p MContext))`: An MContext.
  * `(n int)`: An integer.



#### `std.macros.mnfv`

Linkage: `extern`
Returns: `(p DNode)`
Parameters:

  * `(mc (p MContext))`: An MContext.
  * `(n uint64)`: A uint64 integer.



#### `std.macros.mnfv`

Linkage: `extern`
Returns: `(p DNode)`
Parameters:

  * `(mc (p MContext))`: An MContext.
  * `(n int64)`: A int64 integer.



#### `std.macros.mnfv`

Linkage: `extern`
Returns: `(p DNode)`
Parameters:

  * `(mc (p MContext))`: An MContext.
  * `(f float)`: A float.



#### `std.macros.mnfv`

Linkage: `extern`
Returns: `(p DNode)`
Parameters:

  * `(mc (p MContext))`: An MContext.
  * `(d double)`: A double.



#### `std.macros.mnfv`

Linkage: `extern`
Returns: `(p DNode)`
Parameters:

  * `(mc (p MContext))`: An MContext.
  * `(ld long-double)`: A long double.



#### `std.macros.mnfv-wp`

Linkage: `extern`
Returns: `(p DNode)`
Parameters:

  * `(mc (p MContext))`: An MContext.
  * `(token-string (p (const char)))`: A string.
  * `(begin-line int)`: The beginning line number for the node.
  * `(begin-column int)`: The beginning column number for the node.
  * `(end-line int)`: The ending line number for the node.
  * `(end-column int)`: The ending column number for the node.
  * `(macro-begin-line int)`: The beginning macro line number.
  * `(macro-begin-column int)`: The beginning macro column number.
  * `(macro-end-line int)`: The ending macro line number.
  * `(macro-end-column int)`: The ending macro column number.


Short for 'make-node-from-value-with-position'. Takes additional node
position arguments, and sets them accordingly on the new node. Only
implemented for `(p char)` values.


#### `std.macros.link-nodes`

Linkage: `extern`
Returns: `(p DNode)`
Parameters:

  * `(argcount int)`: The number of nodes being provided.
  * `...`


Links the provided nodes together, without copying them, and returns
the first provided node. This is a varargs function. Note that the
`next-node` member of the last node is not nulled.


#### `std.macros.link-nodes-list`

Linkage: `extern`
Returns: `(p DNode)`
Parameters:

  * `(mc (p MContext))`: An MContext.
  * `(argcount int)`: The number of varargs being provided.
  * `...`


As per `link-nodes`, except that an additional list node is allocated
and returned. This list node points to the first provided node as its
`list-node`.


#### `std.macros.link-nodes-list-wp`

Linkage: `extern`
Returns: `(p DNode)`
Parameters:

  * `(mc (p MContext))`: An MContext.
  * `(argcount int)`: The number of varargs being provided.
  * `(begin-line int)`: The beginning line number for the node.
  * `(begin-column int)`: The beginning column number for the node.
  * `(end-line int)`: The ending line number for the node.
  * `(end-column int)`: The ending column number for the node.
  * `(macro-begin-line int)`: The beginning macro line number.
  * `(macro-begin-column int)`: The beginning macro column number.
  * `(macro-end-line int)`: The ending macro line number.
  * `(macro-end-column int)`: The ending macro column number.
  * `...`


As per `link-nodes-list`, except it also accepts additional node
position arguments, and sets them accordingly on the newly-allocated
list node.


#### `std.macros.link-nodes-array`

Linkage: `extern`
Returns: `(p DNode)`
Parameters:

  * `(argcount int)`: The number of arguments in the array.
  * `(arg-array (p (p DNode)))`: The argument array.


As per `link-nodes`, except that instead of being a varargs function,
it takes an array of nodes as its second argument.


#### `std.macros.gensym-var`

Linkage: `extern`
Returns: `bool`
Parameters:

  * `(buf (p char))`: The buffer for the variable name.


Prints a new, unused variable name to the provided buffer.


#### `std.macros.gensym-label`

Linkage: `extern`
Returns: `bool`
Parameters:

  * `(buf (p char))`: The buffer for the label name.
  * `(prefix (p (const char)))`: The prefix for the label name.


Prints a new, unused label name to the provided buffer. The prefix is
included in the new label name, so that it is a little easier to
determine what's happening when errors occur in the generated code.


#### `std.macros.make-gensym-label-node`

Linkage: `extern`
Returns: `(p DNode)`
Parameters:

  * `(mc (p MContext))`: An MContext.
  * `(prefix (p (const char)))`: The prefix for the label name.


Generates a new label name, constructs a token node to suit and
returns that node.


#### `std.macros.make-gensym-label-node`

Linkage: `extern`
Returns: `(p DNode)`
Parameters:

  * `(mc (p MContext))`: An MContext.


As per the previous implementation, except that no prefix is required
(it is set to the empty string).


#### `std.macros.is-gensym-label`

Linkage: `extern`
Returns: `bool`
Parameters:

  * `(label-node (p DNode))`: The label node.
  * `(prefix (p (const char)))`: The prefix for which to check in the label node.


Determines whether the provided label node is a gensym label with the
specified prefix.


#### `std.macros.make-gensym-var-node`

Linkage: `extern`
Returns: `(p DNode)`
Parameters:

  * `(mc (p MContext))`: An MContext.


Generates a new variable name, constructs a token node to suit and
returns that node.


#### `std.macros.walk-nodes`

Linkage: `extern`
Returns: `bool`
Parameters:

  * `(form (p DNode))`: The node to walk.
  * `(mc (p MContext))`: An MContext.
  * `(data (p void))`: Arbitrary data.
  * `(fn (p (fn int ((form (p DNode)) (mc (p MContext)) (data (p void))))))`: The function pointer to call on each node.


'Walks' through a node, recursively, calling the provided function
pointer on each node. The provided `data` argument is passed to the
function pointer on each call.


#### `std.macros.list-count`

Linkage: `extern`
Returns: `int`
Parameters:

  * `(form (p DNode))`: A list node.


Takes a list node, and counts the number of nodes that are within that
list. This is not recursive: it just counts the top-level nodes from
the list.


#### `std.macros.make-node-maker`

Linkage: `extern`
Returns: `(p DNode)`
Parameters:

  * `(mc (p MContext))`: An MContext.
  * `(form (p DNode))`: A node.


Constructs a node that, when evaluated, constructs the provided node.
For example, if the node is a simple token, then the returned node
will be `(mnfv mc token-str)`, where `token-str` is the token
from the provided node.


#### `std.macros.bqq-helper`

Linkage: `intern`
Returns: `(p DNode)`
Parameters:

  * `(frm (p DNode))`: A node.
  * `(mc (p MContext))`: An MContext.
  * `(arg-count int)`: The number of elements in the node.


A helper function for `bqq` (bootstrap-qq).


#### `std.macros.map-nodes`

Linkage: `(attr cto)`
Returns: `extern`
Parameters:

  * `p`: An MContext.
  * `DNode`: The node to map.


Similar to `walk-nodes`, except that it does not descend into list
nodes, and the provided function returns a node (possibly different
from the argument node), rather than mutating the argument node in
place.




### Macros

#### `std.macros.bqq`

Linkage: `intern`
Parameters:

  * `frm`: A node.
  * `rest`


The bootstrap quasiquotation macro. The general-use quasiquotation
macro, `qq`, is in the `macros` module. The forms handled specially
are `uq` (unquote), `uq-nc` (unquote-no-copy), `uql` (unquote-list)
and `uql-nc` (unquote-list-no-copy). The no-copy versions of these
forms should only be used when the nodes being unquoted will not be
used again.


#### `@:'`

Linkage: `extern`
Parameters:

  * `structp`: The struct pointer argument.
  * `member`: The first member's name.
  * `rest`


Takes a struct pointer and one or more struct member names as its
arguments. Expands to repeated calls to `@:@` over the struct and the
member names. For example, if two members are provided, the expansion
is `(@:@ (@:@ structp member1) member2)`.


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

Previously, argument nodes for the various special forms had to be
DNode pointer variable names.  Arbitrary forms as argument nodes are
now supported.  For example, `(qq identity (uq (mnfv mc 1)))`
previously expanded to `(identity (mnfv mc 1))`, and now expands to
`(identity 1)`.


#### `std.macros.get-varargs-list`

Linkage: `extern`
Parameters:

  * `p`
  * `DNode`


Takes a macro context, an argument count, and a pointer to a DNode
pointer as its arguments.  Returns the first node of a linked node
list containing each of the nodes from the DNode pointer list,
suitable for using as the argument to the `uql` and `uql-nc` forms.
(The third argument here is typically the macro's 'rest' argument.
The difference between 'rest' and the return value of this function is
that the nodes here will be linked together.)


#### `std.macros.get-varargs-list`

Linkage: `extern`
Parameters:

  * `(count int)`


Simple version of `get-varargs-list`, only takes the count as parameter.


[Previous](./2-3-math.md) | [Next](./2-5-stdlib.md)

