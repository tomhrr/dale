# Dale

## 2.14 list

### Details

Module: list  
File: list  

### Description

Concept macro for a doubly-linked list. Apart from the `List` macro
and concept macro, the documentation in this module is for a generated
list instance of type `T`.

All of the functions that take `Iterator` arguments are defined for
`ReverseIterator`s as well, notwithstanding that there is no
documentation for those instances.

The `List` type implements the following concepts:

  * `FrontInsertionSequence`;
  * `BackInsertionSequence`; and
  * `ReversibleContainer`.

Its iterators implement `OutputIterator` and `BidirectionalIterator`.







### Structs

#### `(List T)`

Linkage: `extern`  
Members: N/A


The core list structure type.


#### `(Iterator (List T))`

Linkage: `extern`  
Members: N/A





#### `(ReverseIterator (List T))`

Linkage: `extern`  
Members: N/A









### Functions

#### `init`

Linkage: `extern`  
Returns: `bool`  
Parameters:  

  * `(lst (p (List T)))`: A list pointer.  

Initialise a list.


#### `empty`

Linkage: `extern`  
Returns: `bool`  
Parameters:  

  * `(lst (p (List T)))`: A list pointer.  

Determine whether the list is empty.


#### `size`

Linkage: `extern`  
Returns: `size`  
Parameters:  

  * `(lst (p (List T)))`: A list pointer.  

Returns the number of elements in the list.


#### `max-size`

Linkage: `extern`  
Returns: `size`  
Parameters:  

  * `(lst (p (List T)))`: A list pointer.  

Returns the number of elements that can be accommodated by the
    list.


#### `front`

Linkage: `extern`  
Returns: `T`  
Parameters:  

  * `(lst (p (List T)))`: A list pointer.  

Returns the value of the first element in the list.


#### `back`

Linkage: `extern`  
Returns: `T`  
Parameters:  

  * `(lst (p (List T)))`: A list pointer.  

Returns the value of the last element in the list.


#### `push-back`

Linkage: `extern`  
Returns: `bool`  
Parameters:  

  * `(lst (p (List T)))`: A list pointer.  
  * `(value T)`: The value to add to the list.  

Adds an element to the end of the list.


#### `pop-back`

Linkage: `extern`  
Returns: `void`  
Parameters:  

  * `(lst (p (List T)))`: A list pointer.  

Removes an element from the end of the list.


#### `push-front`

Linkage: `extern`  
Returns: `bool`  
Parameters:  

  * `(lst (p (List T)))`: A list pointer.  
  * `(value T)`: The value to add to the list.  

Adds an element to the beginning of the list.


#### `pop-front`

Linkage: `extern`  
Returns: `void`  
Parameters:  

  * `(lst (p (List T)))`: A list pointer.  

Removes an element from the beginning of the list.


#### `begin`

Linkage: `extern`  
Returns: `(Iterator (List T))`  
Parameters:  

  * `(lst (p (List T)))`: A list pointer.  

Returns the iterator for the first list element.


#### `end`

Linkage: `extern`  
Returns: `(Iterator (List T))`  
Parameters:  

  * `(lst (p (List T)))`: A list pointer.  

Returns the iterator representing the end of the list (sentinel).


#### `@source`

Linkage: `extern`  
Returns: `T`  
Parameters:  

  * `(iter (Iterator (List T)))`: An iterator.  

Returns the iterator's value.


#### `source`

Linkage: `extern`  
Returns: `(p T)`  
Parameters:  

  * `(iter (Iterator (List T)))`: An iterator.  

Returns a pointer to the iterator's value.


#### `sink`

Linkage: `N/A`  
Returns: `bool`  
Parameters:  

  * `(iter (Iterator (List T)))`: An iterator.  
  * `(v T)`: The new value.  

Set the given value at the specified position in the list.


#### `successor`

Linkage: `extern`  
Returns: `(Iterator (List T))`  
Parameters:  

  * `(iter (Iterator (List T)))`: An iterator.  

Returns the iterator for the position that follows the argument
    iterator.


#### `predecessor`

Linkage: `extern`  
Returns: `(Iterator (List T))`  
Parameters:  

  * `(iter (Iterator (List T)))`: An iterator.  

Returns the iterator for the position just before the argument
    iterator.


#### `=`

Linkage: `extern`  
Returns: `bool`  
Parameters:  

  * `(iter1 (Iterator (List T)))`: The first iterator.  
  * `(iter2 (Iterator (List T)))`: The second iterator.  




#### `rbegin`

Linkage: `extern`  
Returns: `(ReverseIterator (List T))`  
Parameters:  

  * `(lst (p (List T)))`: A list pointer.  

Returns the iterator for the last list element.


#### `rend`

Linkage: `extern`  
Returns: `(ReverseIterator (List T))`  
Parameters:  

  * `(lst (p (List T)))`: A list pointer.  

Returns the iterator representing the beginning of the list (sentinel).


#### `insert`

Linkage: `extern`  
Returns: `(Iterator (List T))`  
Parameters:  

  * `(lst (p (List T)))`: The list pointer.  
  * `(iter (Iterator (List T)))`: The iterator for the position.  
  * `(value T)`: The new value.  

Inserts a new element into the list before the specified position,
    and returns the iterator for the new element.


#### `erase`

Linkage: `extern`  
Returns: `(Iterator (List T))`  
Parameters:  

  * `(lst (p (List T)))`: The list pointer.  
  * `(iter (Iterator (List T)))`: The iterator for the position.  

Remove the element at the specified position from the list.


#### `clear`

Linkage: `extern`  
Returns: `bool`  
Parameters:  

  * `(lst (p (List T)))`: The list pointer.  

Remove all of the elements from the list.


#### `swap`

Linkage: `extern`  
Returns: `void`  
Parameters:  

  * `(lst1 (p (List T)))`: The first list pointer.  
  * `(lst2 (p (List T)))`: The second list pointer.  






### Concept macros

#### `List`

Linkage: `extern`  
Parameters:  

  * `(T Type)`: The type node.  

Expands to a `List` definition over the relevant type.




### Macros

#### `List`

Linkage: `extern`  
Parameters:  

  * `T`: The type node.  

Expands to the concrete type name of the `List` generated by way of
the concept macro.


#### `value-type`

Linkage: `extern`  
Parameters:  

  * `lst`: A type node.  

Expands to the underlying value type (i.e. `T`) of the list. This
    only uses the type node for dispatch purposes, so it's safe to
    call this with e.g. `(nullptr (List T))` as the argument.


#### `size-type`

Linkage: `extern`  
Parameters:  

  * `lst`: A type node.  

Expands to the underlying size type of the list.


#### `difference-type`

Linkage: `extern`  
Parameters:  

  * `lst`: A type node.  

Expands to the underlying iterator difference type of the list.


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