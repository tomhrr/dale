# Dale

## 2.18 array

### Details

Module: array

### Description

Concept macro for a fixed-size container. Apart from the `Array` macro
and concept macro, the documentation in this module is for a generated
array instance of type `T`, with size `N`.

All of the functions that take `Iterator` arguments are defined for
`ReverseIterator`s as well, notwithstanding that there is no
documentation for those instances.

The `Array` type implements `RandomAccessContainer`. Its iterators
implement `OutputIterator` and `RandomAccessIterator`.


### Structs

#### `(Array T N)`

Linkage: `extern`
Members: N/A

The core array structure type.


#### `(Iterator (Array T N))`

Linkage: `extern`
Members: N/A


#### `(ReverseIterator (Array T N))`

Linkage: `extern`
Members: N/A




### Functions

#### `init`

Linkage: `extern`
Returns: `bool`
Parameters:

  * `(arrp (p (Array T N)))`: An array pointer.


Initialise an array. (This is a no-op for this container: to
actually zero the array, it is necessary to iterate over it and
set values accordingly.)


#### `empty`

Linkage: `extern`
Returns: `bool`
Parameters:

  * `(arrp (p (Array T N)))`: An array pointer.


Determine whether the array is empty. (Since arrays have a fixed
size, this will always return true, unless the array actually has
a size of zero.)


#### `size`

Linkage: `extern`
Returns: `size`
Parameters:

  * `(arrp (p (Array T N)))`: An array pointer.


Returns the number of elements in the array (This is `N`, for all
instances of the array.)


#### `max-size`

Linkage: `extern`
Returns: `size`
Parameters:

  * `(arrp (p (Array T N)))`: An array pointer.


Returns the number of elements that can be accommodated by the
array. (This is also `N`, for all instances of the array.)


#### `begin`

Linkage: `extern`
Returns: `(Iterator (Array T N))`
Parameters:

  * `(arrp (p (Array T N)))`: An array pointer.


Returns the iterator for the first array element.


#### `end`

Linkage: `extern`
Returns: `(Iterator (Array T N))`
Parameters:

  * `(arrp (p (Array T N)))`: An array pointer.


Returns the iterator representing the end of the list (sentinel).


#### `@source`

Linkage: `extern`
Returns: `T`
Parameters:

  * `(iter (Iterator (Array T N)))`: An iterator.


Returns the iterator's value.


#### `source`

Linkage: `extern`
Returns: `(p T)`
Parameters:

  * `(iter (Iterator (Array T N)))`: An iterator.


Returns a pointer to the iterator's value.


#### `sink`

Linkage: `extern`
Returns: `bool`
Parameters:

  * `(iter (Iterator (Array T N)))`: An iterator.
  * `(v T)`: The new value.


Set the given value at the specified position in the list.


#### `successor`

Linkage: `extern`
Returns: `(Iterator (Array T N))`
Parameters:

  * `(iter (Iterator (Array T N)))`: An iterator.


Returns the iterator for the position that follows the argument
iterator.


#### `predecessor`

Linkage: `extern`
Returns: `(Iterator (Array T N))`
Parameters:

  * `(iter (Iterator (Array T N)))`: An iterator.


Returns the iterator for the position just before the argument
iterator.


#### `=`

Linkage: `extern`
Returns: `bool`
Parameters:

  * `(iter1 (Iterator (Array T N)))`: The first iterator.
  * `(iter2 (Iterator (Array T N)))`: The second iterator.



#### `<`

Linkage: `extern`
Returns: `bool`
Parameters:

  * `(iter1 (Iterator (Array T N)))`: The first iterator.
  * `(iter2 (Iterator (Array T N)))`: The second iterator.



#### `+`

Linkage: `extern`
Returns: `(Iterator (Array T N))`
Parameters:

  * `(iter1 (Iterator (Array T N)))`: An array iterator.
  * `(n size)`: The number of steps forward.



#### `-`

Linkage: `extern`
Returns: `(Iterator (Array T N))`
Parameters:

  * `(iter1 (Iterator (Array T N)))`: An array iterator.
  * `(n size)`: The number of steps backward.



#### `rbegin`

Linkage: `extern`
Returns: `(ReverseIterator (Array T N))`
Parameters:

  * `(arrp (p (Array T N)))`: An array pointer.


Returns the iterator for the last array element.


#### `rend`

Linkage: `extern`
Returns: `(ReverseIterator (Array T N))`
Parameters:

  * `(arrp (p (Array T N)))`: An array pointer.


Returns the iterator representing the beginning of the array (sentinel).


#### `front`

Linkage: `extern`
Returns: `T`
Parameters:

  * `(arrp (p (Array T N)))`: An array pointer.


Returns the value of the first element in the array.


#### `back`

Linkage: `extern`
Returns: `T`
Parameters:

  * `(arrp (p (Array T N)))`: An array pointer.


Returns the value of the last element in the array.


#### `$`

Linkage: `extern`
Returns: `(p T)`
Parameters:

  * `(arrp (p (Array T N)))`: An array pointer.
  * `(index int)`: The index of the element.



#### `$`

Linkage: `extern`
Returns: `(p T)`
Parameters:

  * `(arrp (p (Array T N)))`: An array pointer.
  * `(index size)`: The index of the element.





### Concept macros

#### `Array`

Linkage: `N/A`
Parameters:

  * `(T EqualityComparable)`: The type for the elements of the array.
  * `(N Value)`: The length of the array.


Expands to an `Array` definition over the relevant type. Note that `T`
must also implement `LessThanComparable`.




### Macros

#### `Array`

Linkage: `N/A`
Parameters:

  * `T`: The type node.


Expands to the concrete type name of the `Array` generated by way of
the concept macro.


#### `value-type`

Linkage: `extern`
Parameters:

  * `(lst (p (Array T N)))`: A type node.


Expands to the underlying value type (i.e. `T`) of the array. This
only uses the type node for dispatch purposes, so it's safe to
call this with e.g. `(nullptr (Array T N))` as the argument.


#### `size-type`

Linkage: `extern`
Parameters:

  * `(lst (p (Array T N)))`: A type node.


Expands to the underlying size type of the array.


#### `difference-type`

Linkage: `extern`
Parameters:

  * `(lst (p (Array T N)))`: A type node.


Expands to the underlying iterator difference type of the array.


#### `value-type`

Linkage: `extern`
Parameters:

  * `(iter (p (Iterator (Array T N))))`: A type node.


Expands to the underlying value type (i.e. `T`) of the iterator.


#### `distance-type`

Linkage: `extern`
Parameters:

  * `(iter (p (Iterator (Array T))))`: A type node.


Expands to the underlying distance type of the iterator.


