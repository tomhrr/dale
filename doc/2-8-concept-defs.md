# Dale

[Previous](./2-7-concepts-core.md) | [Next](./2-9-concepts.md)

## 2.8 concept-defs

### Details

Module: concept-defs

### Description

Basic concept definitions. Copied from C++'s STL, for the most part.



### Concepts

#### `Value`

Returns true, regardless of the argument. This is mainly useful when a
concept macro needs to accept a parameter that isn't a type; there is
corresponding logic in `concepts-core` for that case that relies on
this concept being present.


#### `Type`

Returns true if the argument is a type.


#### `PreferRefs`

Returns true if the argument is a type for which references should be
used where possible.


#### `Struct`

Returns true if the argument is a struct type.


#### `DefaultConstructible`

Returns true if variables of the specified type may be declared and
left uninitialised. (This is a really unfortunate name, but not sure
what would be better.)


#### `MoveConstructible`

Returns true if variables of the specified type may be initialised by
way of an rvalue.


#### `MoveAssignable`

Returns true if variables of the specified type may be assigned by way
of an rvalue.


#### `CopyConstructible`

Returns true if variables of the specified type may be initialised by
way of an lvalue.


#### `CopyAssignable`

Returns true if variables of the specified type may be assigned by way
of an lvalue.


#### `Swappable`

Returns true if the type is `MoveConstructible` and `MoveAssignable`,
and a `swap` function exists over the type.


#### `EqualityComparable`

Returns true if `=` and `!=` are implemented over the type.


#### `LessThanComparable`

Returns true if `<`, `<=`, `>` and `>=` are implemented over the type.


#### `Container`

Returns true if the type is `Swappable`, and the following other
conditions hold:

  * `value-type`, `difference-type` and `size-type` macros exist over
    pointers to the type;
  * `size`, `max-size`, `empty`, `swap` and `init` are defined over
    pointers to the type;
  * the container has an iterator type; and
  * `begin` and `end` are defined over the container, and return
    iterators.


#### `ForwardContainer`

Refines `Container`.

Additional requirements:

  * the iterator type must be an `InputIterator`.


#### `ReversibleContainer`

Refines `ForwardContainer`.

Additional requirements:

  * the container has a reverse iterator type;
  * that type is an `InputIterator`; and
  * `rbegin` and `rend` are defined over pointers to the container
    type, and they both return reverse iterators.


#### `RandomAccessContainer`

Refines `ReversibleContainer`. Additionally, the iterator for the
container must be a `RandomAccessIterator`, and `$` must be defined
over the container.


#### `Sequence`

Refines `ForwardContainer`.

Additional requirements:

  * `front` is defined over pointers to the type;
  * `insert` is defined, and it takes a pointer to the type, an
    iterator, and an instance of value-type;
  * `erase` is defined, and it takes a pointer to the type and an
    iterator; and
  * `clear` is defined over pointers to the type.


#### `FrontInsertionSequence`

Refines `Sequence`.

Additional requirements:

  * `push-front` and `pop-front` are defined over pointers to the
    type.


#### `BackInsertionSequence`

Refines `Sequence`.

Additional requirements:

  * `push-back`, `pop-back` and `back` are defined over pointers to
    the type.


#### `AssociativeContainer`

Refines `ForwardContainer` and `DefaultConstructible`.

Additional requirements:

  * `key-type`, `erase` (key), `find` (key) and `count` (key) are
    defined over pointers to the type; and
  * `erase` is defined over the type's iterator.


#### `SimpleAssociativeContainer`

Refines `AssociativeContainer`. The only additional requirement is that
the key type and value type of the container are the same type.


#### `SortedAssociativeContainer`

Refines `AssociativeContainer` and `ReversibleContainer`.
Additionally, `lower-bound` and `upper-bound`, each taking a container
pointer type and a key value (and returning an iterator) must be
defined.


#### `UniqueAssociativeContainer`

Refines `AssociativeContainer`. Additionally, `insert` must be
defined, accepting a pointer to the type and an instance of
value-type.


#### `PairAssociativeContainer`

Refines `AssociativeContainer`. The value-type for the container must
be a struct, and it must contain two members named `first` and
`second`.


#### `TrivialIterator`

Refines `Swappable`, `EqualityComparable` and `DefaultConstructible`.
Additionally, `value-type`, and `source` (returning a pointer to a
value of type `value-type`) must be defined over the iterator (or a
pointer to the iterator, in the case of `value-type`).


#### `InputIterator`

Refines `TrivialIterator`. `successor`, returning an iterator of the
same type as the argument iterator, must be defined over the type.


#### `OutputIterator`

Refines `Swappable` and `DefaultConstructible`. Additionally,
`value-type`, `sink` (for setting the iterator's value) and
`successor` (for getting the next iterator) must be defined over the
iterator (or a pointer to the iterator, in the case of `value-type`).


#### `ForwardIterator`

Refines `InputIterator`. Additionally, calls to `successor` must not
invalidate previous iterators. (This can't be determined
automatically, so this concept exists for documentation purposes
only.)


#### `BidirectionalIterator`

Refines `ForwardIterator`. Additionally, `predecessor` must be
implemented over the iterator, and it must return an instance of the
same type of iterator.


#### `RandomAccessIterator`

Refines `BidirectionalIterator` and `LessThanComparable`.
Additionally, `distance-type` must be defined over pointers to the
type, and `+` and `-` must also be defined, each taking an iterator
and a value of type `distance-type`, and returning a new iterator.


[Previous](./2-7-concepts-core.md) | [Next](./2-9-concepts.md)

