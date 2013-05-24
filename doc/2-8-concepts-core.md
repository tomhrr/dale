# Dale

## 2.8 concepts-core

### Details

Module: concepts-core  
File: concepts-core  

### Description

Provides the core functions and macros required for defining and
implementing concepts. The basic concepts are defined in
`concept-defs`, and they are implemented for the basic types in
`concepts`. Each binding in this module is in the `std.concepts`
namespace.

Concepts are a way in which the requirements of a macro can be
formalised, such that macro authors can depend on certain arguments
satisfying certain conditions. This means that problems are found
before a macro is instantiated, as opposed to after. They also allow
for multiple implementations of a given macro to operate concurrently,
with the specific macro to use being selected based on the concepts
implemented by the macro's arguments.

Concepts as implemented by this library are a little different from
those proposed from time to time for C++. The concept definitions
themselves are procedures, as opposed to lists of required
methods/attributes, and there is no support for concept maps or
axioms. There is also no facility for a given parameter in a concept
macro to satisfy multiple concepts at the same time, except by way of
a check in the concept definition body proper. (Allowing multiple
concepts per parameter complicates dispatch tremendously when concept
refinements are present.)

There are many `extern`-scoped functions in this module, but the only
ones that should be used directly are:

  * `exists-concept`;
  * `def-concept`;
  * `implement`;
  * `implements`; and
  * `def-concept-macro`.











### Functions

#### `std.concepts.exists-concept-fn`

Linkage: `extern`  
Returns: `bool`  
Parameters:  

  * `(pool (p PoolNode))`: A PoolNode.  
  * `(frm (p DNode))`: The node containing the concept name.  

Determines whether the given concept, as described by the node, has
been defined.


#### `std.concepts.implements`

Linkage: `extern`  
Returns: `bool`  
Parameters:  

  * `(pool (p PoolNode))`  
  * `(T (p DNode))`  
  * `(C (p DNode))`  




#### `std.concepts.get-type-concept-list`

Linkage: `extern`  
Returns: `(p (p char))`  
Parameters:  

  * `(pool (p PoolNode))`  
  * `(T (p DNode))`  




#### `std.concepts.refines`

Linkage: `extern`  
Returns: `bool`  
Parameters:  

  * `(pool (p PoolNode))`  
  * `(Tupper (p char))`  
  * `(Tlower (p char))`  




#### `std.concepts.add-refinement`

Linkage: `extern`  
Returns: `void`  
Parameters:  

  * `(current-cn (p concept-node))`  
  * `(refine-cn (p concept-node))`  




#### `std.concepts.make-simple-node`

Linkage: `extern`  
Returns: `(p concept-node)`  
Parameters:  

  * `(pool (p PoolNode))`  
  * `(name (p DNode))`  




#### `std.concepts.get-type-concept-map`

Linkage: `extern`  
Returns: `(p concept-node)`  
Parameters:  

  * `(pool (p PoolNode))`  
  * `(T (p DNode))`  




#### `std.concepts.get-concept-macro-name`

Linkage: `intern`  
Returns: `bool`  
Parameters:  

  * `(pool (p PoolNode))`  
  * `(macro-name (p DNode))`  
  * `(mt-vl (p DNode))`  
  * `(tnames bool)`  
  * `(buf (p char))`  




#### `std.concepts.exists-concept-macro`

Linkage: `extern`  
Returns: `bool`  
Parameters:  

  * `(pool (p PoolNode))`  
  * `(macro-name (p DNode))`  
  * `(macro-types (p DNode))`  




#### `std.concepts.get-candidate`

Linkage: `intern`  
Returns: `int`  
Parameters:  

  * `(pool (p PoolNode))`  
  * `(errn (p DNode))`  
  * `(tcl-list (p (p concept-node)))`  
  * `(tcl-count int)`  
  * `(arg-cycle int)`  
  * `(macname (p char))`  
  * `(macbuf (p char))`  
  * `(is-error (p int))`  
  * `(hasbuf bool)`  
  * `(mybuf2 (p char))`  








### Macros

#### `std.concepts.exists-concept`

Linkage: `extern`  
Parameters:  

  * `concept-name`: The node containing the concept name.  

Expands to `true` or `false`, depending on whether the concept, as
described by the argument node, has been defined.


#### `std.concepts.def-concept`

Linkage: `extern`  
Parameters: `void`


Define a new concept. Takes a name, a list of refinements, a list of
parameters and an arbitrary number of forms representing the body of
the concept.

The list of refinements has the following form:

        (refines {concept-1} {concept-2} ... {concept-N})

A concept P 'refines' another concept Q when it must meet the criteria
of Q, as well as certain other criteria. If the concept being defined
does not refine any other concept, then the second argument should be
`(refines)`.

The parameter list is as per a macro's parameter list, except that all
of the parameters must be untyped. Currently, a concept may only
accept one parameter.

The body of the concept must evaluate to a boolean value indicating
whether the specified parameters satisfy the concept. Its environment
is as per a normal macro, so a `pool` is present.

Concepts, once defined, can be used to 'tag' the parameters to a macro
constructed by way of `def-concept-macro`.


#### `std.concepts.implement`

Linkage: `extern`  
Parameters: `void`


Takes a concept name and a list of type arguments. Attempts to
'implement' the concept for those type arguments, which involves
checking whether the type satisfies the concept's constraints and
marking the type as such if it does. If the type does not satisfy the
constraints, the expansion is a no-op, though note that the concept
body may (generally, will) add errors if its requirements are not met.


#### `std.concepts.implements`

Linkage: `extern`  
Parameters:  

  * `T`: A type node.  
  * `C`: A concept node.  

If the type implements the concept, this is a no-op. Otherwise, it
reports an error about the concept not being implemented.


#### `std.concepts.def-concept-macro`

Linkage: `extern`  
Parameters:  

  * `macro-name`: The name of the concept macro.  
  * `linkage`: The linkage of the concept macro.  
  * `macro-types`: The parameters (typed) for the macro.  

Define a new concept macro. Takes a name, a linkage type, a list of
macro parameters and an arbitrary number of forms (the body of the
macro) as its parameters. The list of macro parameters is as per a
normally-defined macro, except that each parameter must be 'typed'
with a concept.

Macros defined by way of this macro are executed by running
`instantiate`. That macro takes care of determining which concept
macro to actually run, based on the concepts implemented by the
arguments.


#### `assert-return`

Linkage: `N/A`  
Parameters: `void`





#### `instantiate`

Linkage: `N/A`  
Parameters:  

  * `macro-name`: The name of the macro to be instantiated.  

Takes a concept macro name and a series of arguments for that concept
macro as its arguments. Determines the most appropriate concrete
concept macro, based on the provided arguments, and expands into a
call to that macro. The term `instantiate` is used because in nearly
all cases, the concept macro being run is something that in turn
expands into a series of bindings; for example, `Vector`.