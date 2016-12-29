# Dale

[Previous](./2-6-assert.md) | [Next](./2-8-concept-defs.md)

## 2.7 concepts-core

### Details

Module: concepts-core

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

  * `(mc (p MContext))`: An MContext.
  * `(frm (p DNode))`: The node containing the concept name.


Determines whether the given concept, as described by the node, has
been defined.


#### `std.concepts.implements-fn`

Linkage: `extern`
Returns: `bool`
Parameters:

  * `(mc (p MContext))`: An MContext.
  * `(T (p DNode))`: A type node.
  * `(C (p DNode))`: A concept node.



#### `std.concepts.get-node-type-concept-list`

Linkage: `extern`
Returns: `(p (p char))`
Parameters:

  * `(mc (p MContext))`: An MContext.
  * `(T (p DNode))`: A type node.


Takes a type DNode as its single argument.  Returns a list of char
pointers, each being the name of a concept implemented by this type.
This is recursive, so if the type implements a concept A, which is in
turn a refinement of concept B, the list will includes entries for
both A and B.


#### `std.concepts.refines`

Linkage: `extern`
Returns: `bool`
Parameters:

  * `(mc (p MContext))`: An MContext.
  * `(Tupper (p (const char)))`: The possibly-refined type.
  * `(Tlower (p (const char)))`: The refining type.



#### `std.concepts.refines-multiple`

Linkage: `extern`
Returns: `bool`
Parameters:

  * `(mc (p MContext))`: An MContext.
  * `(Tupper (p (const char)))`: The possibly-refined type.
  * `(Tlower (p (const char)))`: The refining type.



#### `std.concepts.add-refinement`

Linkage: `extern`
Returns: `void`
Parameters:

  * `(current-cn (p concept-node))`: The current concept node.
  * `(refine-cn (p concept-node))`: The refinement concept node.



#### `std.concepts.make-simple-node`

Linkage: `extern`
Returns: `(p concept-node)`
Parameters:

  * `(mc (p MContext))`: An MContext.
  * `(name (p DNode))`: The name of the concept.



#### `std.concepts.dump-concept-map`

Linkage: `extern`
Returns: `void`
Parameters:

  * `(mc (p MContext))`
  * `(mapp (p concept-node))`
  * `(mlist bool)`
  * `(n int)`



#### `std.concepts.concept-map-to-string`

Linkage: `extern`
Returns: `void`
Parameters:

  * `(mc (p MContext))`
  * `(mapp (p concept-node))`
  * `(buf (p char))`



#### `std.concepts.concepts-to-string`

Linkage: `extern`
Returns: `void`
Parameters:

  * `(mc (p MContext))`
  * `(name (p DNode))`
  * `(buf (p char))`



#### `std.concepts.get-type-concept-map`

Linkage: `extern`
Returns: `(p concept-node)`
Parameters:

  * `(mc (p MContext))`
  * `(T (p DNode))`



#### `std.concepts.get-concept-macro-name`

Linkage: `intern`
Returns: `bool`
Parameters:

  * `(mc (p MContext))`: An MContext.
  * `(macro-name (p DNode))`: The macro name node.
  * `(mt-vl (p DNode))`: The concept macro type.
  * `(tnames bool)`: Whether the type is a list or an atom.
  * `(buf (p char))`: The buffer for the name.



#### `std.concepts.exists-concept-macro`

Linkage: `extern`
Returns: `bool`
Parameters:

  * `(mc (p MContext))`: An MContext.
  * `(macro-name (p DNode))`: The name of the concept macro.
  * `(macro-types (p DNode))`: The types of the concept macro.



#### `std.concepts.all-applicable`

Linkage: `intern`
Returns: `bool`
Parameters:

  * `(tcl-list (p (p concept-node)))`
  * `(tcl-count int)`


Returns true if the list of concept nodes does not contain any
instances of "not-applicable" (i.e. of the root, unused concept node).


#### `std.concepts.get-candidate-toplevel`

Linkage: `intern`
Returns: `bool`
Parameters:

  * `(mc (p MContext))`
  * `(tcl-list (p (p concept-node)))`
  * `(tcl-count int)`
  * `(macname (p char))`
  * `(macbuf (p char))`
  * `(hascnamebuf bool)`
  * `(cname (p char))`


Checks at the top-level of the concept list for a candidate that
matches the requested macro name.  If one exists, writes it to the
macro buffer and returns true.  Otherwise, returns false.


#### `std.concepts.get-candidate`

Linkage: `intern`
Returns: `int`
Parameters:

  * `(mc (p MContext))`
  * `(errn (p DNode))`
  * `(tcl-list (p (p concept-node)))`
  * `(tcl-count int)`
  * `(arg-cycle int)`
  * `(macname (p char))`
  * `(macbuf (p char))`
  * `(is-error (p int))`
  * `(hascnamebuf bool)`
  * `(cname (p char))`



#### `std.concepts.is-forced-concept`

Linkage: `intern`
Returns: `bool`
Parameters:

  * `(node (p DNode))`



#### `std.concepts.validate-forced-concept`

Linkage: `intern`
Returns: `bool`
Parameters:

  * `(mc (p MContext))`
  * `(node (p DNode))`



#### `std.concepts.get-type-concept-list`

Linkage: `intern`
Returns: `bool`
Parameters:

  * `(mc (p MContext))`
  * `(varargs-list (p DNode))`
  * `(varargs-count int)`
  * `(type-concept-list (p (p concept-node)))`
  * `(type-list (p (p DNode)))`


Takes a list of concept instantiation arguments, except for the
initial concept name.  Populates the type concept list, being the
mapping between the parameter and the concept map for the parameter.

Most parameters are plain types, for which see get-type-concept-map.
The special processing here is for disambiguation (forcing the use of
a specific concept) and non-types, the latter of which are treated as
being of the Value concept.




### Macros

#### `std.concepts.exists-concept`

Linkage: `extern`
Parameters:

  * `concept-name`: The node containing the concept name.


Expands to `true` or `false`, depending on whether the concept, as
described by the argument node, has been defined.


#### `std.concepts.def-concept`

Linkage: `extern`
Parameters:

  * `concept-name-node`
  * `refinement-list`
  * `type-arguments`
  * `rest`


Define a new concept. Takes a name, a list of refinements, a list of
parameters and an arbitrary number of forms representing the body of
the concept.

The list of refinements has the following form:

        (refines {concept-1} {concept-2} ... {concept-N})

A concept P 'refines' another concept Q when it must meet the criteria
of Q, as well as certain other criteria. If the concept being defined
does not refine any other concept, then the second argument should be
`(refines)`.  Apart from allowing a concept to import the criteria of
another concept, refinements are also used during `instantiate`: the
instantiation that is most-specific with respect to the arguments'
concepts, taking into account their refinements, is the one that is
used.

The parameter list is as per a macro's parameter list, except that all
of the parameters must be untyped. Currently, a concept may only
accept one parameter.

The body of the concept must evaluate to a boolean value indicating
whether the specified parameters satisfy the concept. Its environment
is as per a normal macro, so an `mc` is present.

Concepts, once defined, can be used to 'tag' the parameters to a macro
constructed by way of `def-concept-macro`.


#### `std.concepts.implement`

Linkage: `extern`
Parameters:

  * `concept-name-node`
  * `rest`


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
  * `rest`


Define a new concept macro. Takes a name, a linkage type, a list of
macro parameters and an arbitrary number of forms (the body of the
macro) as its parameters. The list of macro parameters is as per a
normally-defined macro, except that each parameter must be 'typed'
with a concept.

Macros defined by way of this macro are executed by running
`instantiate`. That macro takes care of determining which concept
macro to actually run, based on the concepts implemented by the
arguments.


#### `std.concepts.assert-return`

Linkage: `intern`
Parameters:

  * `error-cond`
  * `report-error-node`
  * `report-error-str`



#### `std.concepts.assert-return-b`

Linkage: `intern`
Parameters:

  * `error-cond`
  * `report-error-node`
  * `report-error-str`



#### `std.concepts.instantiate`

Linkage: `extern`
Parameters:

  * `macro-name`: The name of the macro to be instantiated.
  * `rest`


Takes a concept macro name and a series of arguments for that concept
macro as its arguments. Determines the most appropriate concrete
concept macro, based on the provided arguments, and expands into a
call to that macro. The term `instantiate` is used because in nearly
all cases, the concept macro being run is something that in turn
expands into a series of bindings; for example, `Vector`.

When multiple equally-preferred instantiations are available for a
given call, this will report an error and expand to nothing.
Disambiguation is achieved by way of the `force` form:

        (instantiate MacroName (force ConceptName Type) ...)

Disambiguation is not required when an argument implements a concept
that refines another concept, and there are instantiations available
for both concepts.  In that case, the instantiation for the former
concept will be preferred.

Each argument to `instantiate` must implement one or more concepts.
If any argument does not, the expansion will fail with the error
message "type does not implement any concepts".  See `implement` in
this module for details on how types can be made to implement
concepts.

If no concept macros that have this name exist, the expansion will
fail with the error "concept not found".  This is usually caused by a
missing import statement.


[Previous](./2-6-assert.md) | [Next](./2-8-concept-defs.md)

