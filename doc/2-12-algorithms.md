# Dale

[Previous](./2-11-derivations.md) | [Next](./2-13-list.md)

## 2.12 algorithms

### Details

Module: algorithms

### Description

Provides concept macros for generating algorithm functions.



### Concept macros

#### `find`

Linkage: `extern`
Parameters:

  * `(Ti InputIterator)`: The iterator type.


Expands to a function that accepts two of the provided iterator types
and an instance of the iterator type's `value-type`. On calling that
function, if the value is found in the range, the iterator containing
the value is returned. Otherwise, the second argument iterator is
returned.


#### `find-if`

Linkage: `extern`
Parameters:

  * `(Ti InputIterator)`: The iterator type.


Expands to a function that accepts two of the provided iterator types
and a `Predicate`. On calling that function, if a value satisfying the
predicate is found in the range, the iterator containing that value is
returned. Otherwise, the second argument iterator is returned.


#### `find-if-not`

Linkage: `extern`
Parameters:

  * `(Ti InputIterator)`: The iterator type.


Expands to a function that accepts two of the provided iterator types
and a `Predicate`. On calling that function, if a value that does not
satisfy the predicate is found in the range, the iterator containing
that value is returned. Otherwise, the second argument iterator is
returned.


#### `sort`

Linkage: `extern`
Parameters:

  * `(Ti RandomAccessIterator)`: The iterator type.


Expands to a function that accepts two of the provided iterator types.
On calling that function, the corresponding range is sorted. (The
implementation here needs some work, because the resulting function is
pretty slow.)


#### `for-each`

Linkage: `extern`
Parameters:

  * `(Ti InputIterator)`: The iterator type.


Expands to a function that accepts two of the provided iterator types
and an `Action`. On calling that function, the action is run against
each value in the range.


#### `lower-bound`

Linkage: `extern`
Parameters:

  * `(Ti RandomAccessIterator)`: The iterator type.


Expands to a function that accepts two of the provided iterator types
and a `value-type`. On calling that function, returns the position of
the first value in the range that is not less than the provided value.
The function assumes that the range is sorted.


#### `upper-bound`

Linkage: `extern`
Parameters:

  * `(Ti RandomAccessIterator)`: The iterator type.


Expands to a function that accepts two of the provided iterator types
and a `value-type`. On calling that function, returns the position of
the first value in the range that is greater than the provided value.
The function assumes that the range is sorted.


#### `binary-search`

Linkage: `extern`
Parameters:

  * `(Ti RandomAccessIterator)`: The iterator type.


Expands to a function that accepts two of the provided iterator types
and a `value-type`. On calling that function, returns a boolean
indicating whether the value is present in the range. The function
assumes that the range is sorted.


#### `equal-range`

Linkage: `extern`
Parameters:

  * `(Ti RandomAccessIterator)`: The iterator type.


Expands to a function that accepts two of the provided iterator types
and a `value-type`. It also instantiates a `Pair` type over the
iterator type, if required. On calling that function, it returns a
`Pair` containing the `lower-bound` and `upper-bound` iterators for
the value.


#### `max`

Linkage: `extern`
Parameters:

  * `(T Type)`: The type node.


Expands to a function that takes two values of the specified type. If
the first value is greater than the second value, then the first is
returned. Otherwise, the second is returned. Does not support types
that do not permit copying.


#### `min`

Linkage: `extern`
Parameters:

  * `(T Type)`: The type node.


Expands to a function that takes two values of the specified type. If
the first value is less than than the second value, then the first is
returned. Otherwise, the second is returned. Does not support types
that do not permit copying.


#### `copy`

Linkage: `extern`
Parameters:

  * `(Ti InputIterator)`: The input iterator type.
  * `(To OutputIterator)`: The output iterator type.


Takes input and output iterator types as its arguments. Expands to a
function that takes two of the input iterators and an output iterator.
That function iterates over the provided range, sinking values into
the output iterator at each step.


#### `assign`

Linkage: `extern`
Parameters:

  * `(Tc BackInsertionSequence)`: The container type.
  * `(Ti InputIterator)`: The input iterator type.


Takes a container type and an input iterator type as its arguments.
Expands to a function that takes a container value and two input
iterators as its arguments. That function clears the container, and
then copies each element from the range formed by the input iterators
into the cleared container.


#### `foldl`

Linkage: `extern`
Parameters:

  * `(Ti InputIterator)`: The input iterator type.


Takes an input iterator type as its arguments.  Expands to a fold-left
function that takes a binary operation function pointer, an initial
value, and a pair of input iterators as its arguments.  Does not
support types that do not permit copying.


#### `=`

Linkage: `extern`
Parameters:

  * `(Tc Container)`: The container type.


Takes a container type as its argument. Expands to a function that
takes two container pointers and returns a boolean indicating whether
the elements of those containers match.


#### `<`

Linkage: `extern`
Parameters:

  * `(Tc Container)`: The container type.


Takes a container type as its argument. Expands to a function that
takes two container pointers and returns a boolean indicating whether
the first container is 'less than' the second. This will be so when:

  * the first element of the first container that is not equal to the
    element at the same position in the second container compares as
    less than that element; or
  * there are fewer elements in the first container than in the
    second.

(`<` is defined over pointers to containers, rather than container
values, for the same reasons as `=`).




### Macros

#### `Predicate`

Linkage: `extern`
Parameters:

  * `T`: The type node.


Expands to a function pointer type that takes the `value-type` of the
corresponding type argument and returns a `bool`. This name is used as
shorthand in this module's documentation.


#### `RefPredicate`

Linkage: `extern`
Parameters:

  * `T`: The type node.


Like `Predicate`, except the function pointer type takes a const reference.


#### `Action`

Linkage: `extern`
Parameters:

  * `T`: The type node.


Expands to a function pointer type that takes the `value-type` of the
corresponding type argument and returns `void`. This name is used as
shorthand in this module's documentation.


#### `RefAction`

Linkage: `extern`
Parameters:

  * `T`: The type node.


Like `Action`, except the function pointer takes a reference.


[Previous](./2-11-derivations.md) | [Next](./2-13-list.md)

