# Dale

## 2.15 vector

### Details

Module: vector  
File: vector  

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
Members: N/A





#### `(ReverseIterator (Vector T))`

Linkage: `extern`  
Members: N/A









### Functions

#### `init`

Linkage: `extern`  
Returns: `bool`  
Parameters:  

  * `(vecp (p (Vector T)))`: A vector pointer.  
  * `(cap int)`: The capacity of the new vector.  

Initialise a vector by providing a specific capacity.


#### `init`

Linkage: `extern`  
Returns: `bool`  
Parameters:  

  * `(vecp (p (Vector T)))`: A vector pointer.  

Initialise a vector.


#### `empty`

Linkage: `extern`  
Returns: `bool`  
Parameters:  

  * `(vecp (p (Vector T)))`: A vector pointer.  

Determine whether the vector is empty.


#### `size`

Linkage: `extern`  
Returns: `size`  
Parameters:  

  * `(vecp (p (Vector T)))`: A vectorp ointer.  

Returns the number of elements in the vector.


#### `max-size`

Linkage: `extern`  
Returns: `size`  
Parameters:  

  * `(vecp (p (Vector T)))`: A vector pointer.  

Returns the number of elements that can be accommodated by the
    vector.


#### `front`

Linkage: `extern`  
Returns: `T`  
Parameters:  

  * `(vecp (p (List T)))`: A vector pointer.  

Returns the value of the first element in the vector.


#### `back`

Linkage: `extern`  
Returns: `T`  
Parameters:  

  * `(vecp (p (Vector T)))`: A vector pointer.  

Returns the value of the last element in the vector.


#### `pop-back`

Linkage: `extern`  
Returns: `void`  
Parameters:  

  * `(vecp (p (Vector T)))`: A vector pointer.  

Removes an element from the end of the vector.


#### `resize`

Linkage: `extern`  
Returns: `bool`  
Parameters:  

  * `(vecp (p (Vector T)))`: A vector pointer.  
  * `(cap size)`: The new capacity for the vector.  

Change the vector's capacity.


#### `reserve`

Linkage: `extern`  
Returns: `bool`  
Parameters:  

  * `(vecp (p (Vector T)))`: A vector pointer.  
  * `(extra size)`  

Increase the vector's capacity by the specified amount.


#### `push-back`

Linkage: `extern`  
Returns: `bool`  
Parameters:  

  * `(vecp (p (Vector T)))`: A vector pointer.  
  * `(value T)`: The value to add to the vector.  

Adds an element to the end of the vector.


#### `$`

Linkage: `extern`  
Returns: `(p T)`  
Parameters:  

  * `(vecp (p (Vector T)))`: A vector pointer.  
  * `(value int)`: The index of the element.  




#### `$`

Linkage: `extern`  
Returns: `(p T)`  
Parameters:  

  * `(vecp (p (Vector T)))`: A vector pointer.  
  * `(value size)`: The index of the element.  




#### `begin`

Linkage: `extern`  
Returns: `(Iterator (Vector T))`  
Parameters:  

  * `(vecp (p (Vector T)))`: A vector pointer.  

Returns the iterator for the first vector element.


#### `end`

Linkage: `extern`  
Returns: `(Iterator (Vector T))`  
Parameters:  

  * `(vecp (p (Vector T)))`: A vector pointer.  

Returns the iterator representing the end of the vector (sentinel).


#### `@source`

Linkage: `extern`  
Returns: `T`  
Parameters:  

  * `(iter (Iterator (Vector T)))`: An iterator.  

Returns the iterator's value.


#### `source`

Linkage: `extern`  
Returns: `(p T)`  
Parameters:  

  * `(iter (Iterator (Vector T)))`: An iterator.  

Returns a pointer to the iterator's value.


#### `sink`

Linkage: `N/A`  
Returns: `bool`  
Parameters:  

  * `(iter (Iterator (List T)))`: An iterator.  
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

  * `(vecp (p (Vector T)))`: A vector pointer.  

Returns the iterator for the last vector element.


#### `rend`

Linkage: `extern`  
Returns: `(ReverseIterator (Vector T))`  
Parameters:  

  * `(vecp (p (Vector T)))`: A vector pointer.  

Returns the iterator representing the beginning of the vector
    (sentinel).


#### `insert`

Linkage: `extern`  
Returns: `(Iterator (Vector T))`  
Parameters:  

  * `(vecp (p (Vector T)))`: The vector pointer.  
  * `(iter (Iterator (Vector T)))`: The iterator for the position.  
  * `(value T)`: The new value.  

Inserts a new element into the vector before the specified position,
    and returns the iterator for the new element.


#### `erase`

Linkage: `extern`  
Returns: `(Iterator (Vector T))`  
Parameters:  

  * `(vecp (p (Vector T)))`: The vector pointer.  
  * `(iter (Iterator (Vector T)))`: The iterator for the position.  

Remove the element at the specified position from the vector.


#### `clear`

Linkage: `extern`  
Returns: `bool`  
Parameters:  

  * `(vecp (p (Vector T)))`: The vector pointer.  

Remove all of the elements from the vector.


#### `swap`

Linkage: `extern`  
Returns: `void`  
Parameters:  

  * `(vec1 (p (Vector T)))`: The first vector pointer.  
  * `(vec2 (p (Vector T)))`: The second pointer pointer.  






### Concept macros

#### `Vector`

Linkage: `extern`  
Parameters:  

  * `(T EqualityComparable)`: The type node.  

Expands to a `Vector` definition over the relevant type. Note that `T`
must also implement `LessThanComparable`.




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

  * `vecp`: A type node.  

Expands to the underlying value type (i.e. `T`) of the vector.
    This only uses the type node for dispatch purposes, so it's safe
    to call this with e.g. `(nullptr (Vector T))` as the argument.


#### `size-type`

Linkage: `extern`  
Parameters:  

  * `vecp`: A type node.  

Expands to the underlying size type of the vector.


#### `difference-type`

Linkage: `extern`  
Parameters:  

  * `vecp`: A type node.  

Expands to the underlying iterator difference type of the vector.


#### `value-type`

Linkage: `extern`  
Parameters:  

  * `iter`: A type node.  

Expands to the underlying value type (i.e. `T`) of the iterator.


#### `distance-type`

Linkage: `extern`  
Parameters:  

  * `iter`: A type node.  

Expands to the underlying distance type of the iterator.