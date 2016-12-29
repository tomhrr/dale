# Dale

[Previous](./2-13-list.md) | [Next](./2-15-set.md)

## 2.14 vector

### Details

Module: vector

### Description

Concept macro for a vector. Apart from the `Vector` macro and concept
macro, the documentation in this module is for a generated vector
instance of type `T`.

All of the functions that take `Iterator` arguments are defined for
`ReverseIterator`s as well, notwithstanding that there is no
documentation for those instances.

The `Vector` type implements `BackInsertionSequence` and
`RandomAccessContainer`. Its iterators implement `OutputIterator` and
`RandomAccessIterator`.



### Structs

#### `(Vector T)`

Linkage: `extern`
Members: N/A

The core vector structure type.


#### `(Iterator (Vector T))`

Linkage: `extern`
Members:

  * `(parent-vector (p (Vector T)))`
  * `(element (p T))`



#### `(ReverseIterator (Vector T))`

Linkage: `extern`
Members:

  * `(element (p T))`





### Functions

#### `init`

Linkage: `extern`
Returns: `bool`
Parameters:

  * `(vecp (ref (Vector T)))`: A vector reference.
  * `(cap int)`: The capacity of the new vector.


Initialise a vector by providing a specific capacity.


#### `init`

Linkage: `extern`
Returns: `bool`
Parameters:

  * `(vecp (ref (Vector T)))`: A vector reference.


Initialise a vector.


#### `empty`

Linkage: `extern`
Returns: `bool`
Parameters:

  * `(vecp (ref (const (Vector T))))`: A vector reference.


Determine whether the vector is empty.


#### `size`

Linkage: `extern`
Returns: `(const size)`
Parameters:

  * `(vecp (ref (const (Vector T))))`: A vector reference.


Returns the number of elements in the vector.


#### `max-size`

Linkage: `extern`
Returns: `size`
Parameters:

  * `(vecp (ref (const (Vector T))))`: A vector reference.


Returns the number of elements that can be accommodated by the
vector.


#### `front`

Linkage: `extern`
Returns: `(const (p T))`
Parameters:

  * `(vecp (ref (const (Vector T))))`: A vector reference.


Returns a pointer to the value of the first element in the vector.


#### `back`

Linkage: `extern`
Returns: `(const (p T))`
Parameters:

  * `(vecp (ref (const (Vector T))))`: A vector reference.


Returns a pointer to the value of the last element in the vector.


#### `pop-back`

Linkage: `extern`
Returns: `void`
Parameters:

  * `(vecp (ref (Vector T)))`: A vector reference.


Removes an element from the end of the vector.


#### `resize`

Linkage: `extern`
Returns: `bool`
Parameters:

  * `(vecp (ref (Vector T)))`: A vector reference.
  * `(new-capacity size)`: The new capacity for the vector.


Change the vector's capacity.


#### `reserve`

Linkage: `extern`
Returns: `bool`
Parameters:

  * `(vecp (ref (Vector T)))`: A vector reference.
  * `(extra size)`: The amount by which the capacity should be increased.


Increase the vector's capacity by the specified amount.


#### `push-back`

Linkage: `extern`
Returns: `bool`
Parameters:

  * `(vecp (ref (Vector T)))`: A vector reference.
  * `(value T)`: The value to add to the vector.


Adds an element to the end of the vector.


#### `$`

Linkage: `extern`
Returns: `(p T)`
Parameters:

  * `(vecp (ref (const (Vector T))))`: A vector reference.
  * `(index int)`: The index of the element.



#### `$`

Linkage: `extern`
Returns: `(p T)`
Parameters:

  * `(vecp (ref (const (Vector T))))`: A vector reference.
  * `(index size)`: The index of the element.



#### `begin`

Linkage: `extern`
Returns: `(Iterator (Vector T))`
Parameters:

  * `(vecp (ref (Vector T)))`: A vector reference.


Returns the iterator for the first vector element.


#### `end`

Linkage: `extern`
Returns: `(Iterator (Vector T))`
Parameters:

  * `(vecp (ref (Vector T)))`: A vector reference.


Returns the iterator representing the end of the vector (sentinel).


#### `source`

Linkage: `extern`
Returns: `(p T)`
Parameters:

  * `(iter (Iterator (Vector T)))`: An iterator.


Returns a pointer to the iterator's value.


#### `sink`

Linkage: `extern`
Returns: `bool`
Parameters:

  * `(iter (Iterator (Vector T)))`: An iterator.
  * `(v T)`: The new value.


Set the given value at the specified position in the vector.


#### `successor`

Linkage: `extern`
Returns: `(Iterator (Vector T))`
Parameters:

  * `(iter (Iterator (Vector T)))`: An iterator.


Returns the iterator for the position that follows the argument
iterator.


#### `predecessor`

Linkage: `extern`
Returns: `(Iterator (Vector T))`
Parameters:

  * `(iter (Iterator (Vector T)))`: An iterator.


Returns the iterator for the position just before the argument
iterator.


#### `=`

Linkage: `extern`
Returns: `bool`
Parameters:

  * `(iter1 (Iterator (Vector T)))`: The first iterator.
  * `(iter2 (Iterator (Vector T)))`: The second iterator.



#### `<`

Linkage: `extern`
Returns: `bool`
Parameters:

  * `(iter1 (Iterator (Vector T)))`: The first iterator.
  * `(iter2 (Iterator (Vector T)))`: The second iterator.



#### `+`

Linkage: `extern`
Returns: `(Iterator (Vector T))`
Parameters:

  * `(iter1 (Iterator (Vector T)))`: The first iterator.
  * `(n size)`: The number of steps to advance.



#### `-`

Linkage: `extern`
Returns: `(Iterator (Vector T))`
Parameters:

  * `(iter1 (Iterator (Vector T)))`: The first iterator.
  * `(n size)`: The number of steps by which to move backwards.



#### `distance`

Linkage: `extern`
Returns: `size`
Parameters:

  * `(iter1 (Iterator (Vector T)))`: The first iterator.
  * `(iter2 (Iterator (Vector T)))`: The second iterator.


Return the distance between the two iterators.


#### `rbegin`

Linkage: `extern`
Returns: `(ReverseIterator (Vector T))`
Parameters:

  * `(vec (ref (Vector T)))`: A vector reference.


Returns the iterator for the last vector element.


#### `rend`

Linkage: `extern`
Returns: `(ReverseIterator (Vector T))`
Parameters:

  * `(vec (ref (Vector T)))`: A vector reference.


Returns the iterator representing the beginning of the vector
(sentinel).


#### `insert`

Linkage: `extern`
Returns: `(Iterator (Vector T))`
Parameters:

  * `(unused (ref (const (Vector T))))`: The vector reference.
  * `(iter (Iterator (Vector T)))`: The iterator for the position.
  * `(value T)`: The new value.


Inserts a new element into the vector before the specified position,
and returns the iterator for the new element.


#### `erase`

Linkage: `extern`
Returns: `(Iterator (Vector T))`
Parameters:

  * `(unused (ref (const (Vector T))))`: The vector reference.
  * `(iter (Iterator (Vector T)))`: The iterator for the position.


Remove the element at the specified position from the vector.


#### `clear`

Linkage: `extern`
Returns: `bool`
Parameters:

  * `(vecp (ref (Vector T)))`: The vector reference.


Remove all of the elements from the vector.


#### `swap`

Linkage: `extern`
Returns: `void`
Parameters:

  * `(vec1 (ref (Vector T)))`: The first vector reference.
  * `(vec2 (ref (Vector T)))`: The second vector reference.





### Concept macros

#### `Vector`

Linkage: `extern`
Parameters:

  * `(T MoveConstructible)`: The type node.


Expands to a `Vector` definition over the relevant type.  If `T`
implements `EqualityComparable` and/or `LessThanComparable`, the
new vector type will implement them as well.




### Macros

#### `Vector`

Linkage: `extern`
Parameters:

  * `T`: The type node.


Expands to the concrete type name of the `Vector` generated by way of
the concept macro.


#### `value-type`

Linkage: `extern`
Parameters:

  * `(vecp (p (Vector T)))`: A type node.


Expands to the underlying value type (i.e. `T`) of the vector.
This only uses the type node for dispatch purposes, so it's safe
to call this with e.g. `(nullptr (Vector T))` as the argument.


#### `size-type`

Linkage: `extern`
Parameters:

  * `(vecp (p (Vector T)))`: A type node.


Expands to the underlying size type of the vector.


#### `difference-type`

Linkage: `extern`
Parameters:

  * `(vecp (p (Vector T)))`: A type node.


Expands to the underlying iterator difference type of the vector.


#### `value-type`

Linkage: `extern`
Parameters:

  * `(veciter (p (Iterator (Vector T))))`: A type node.


Expands to the underlying value type (i.e. `T`) of the iterator.


#### `distance-type`

Linkage: `extern`
Parameters:

  * `(veciter (p (Iterator (Vector T))))`: A type node.


Expands to the underlying distance type of the iterator.


[Previous](./2-13-list.md) | [Next](./2-15-set.md)

