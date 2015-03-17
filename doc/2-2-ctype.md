# Dale

[Previous](./2-1-introspection.md) | [Next](./2-3-math.md)

## 2.2 ctype

### Details

Module: ctype

### Description

Wrappers for the `cctype` functions. These differ insofar as they
accept `char` arguments, and return either `bool` or `char` results,
as appropriate. They are also located within the `std` namespace, as
opposed to the root namespace.



### Functions

#### `std.isalnum`

Linkage: `extern`
Returns: `bool`
Parameters:

  * `(c char)`: A character.


Determine whether a character is alphanumeric.


#### `std.isalpha`

Linkage: `extern`
Returns: `bool`
Parameters:

  * `(c char)`: A character.


Determine whether a character is alphabetical.


#### `std.iscntrl`

Linkage: `extern`
Returns: `bool`
Parameters:

  * `(c char)`: A character.


Determine whether a character is a control character.


#### `std.isdigit`

Linkage: `extern`
Returns: `bool`
Parameters:

  * `(c char)`: A character.


Determine whether a character is a digit.


#### `std.isgraph`

Linkage: `extern`
Returns: `bool`
Parameters:

  * `(c char)`: A character.


Determine whether a character is printable (excludes space).


#### `std.islower`

Linkage: `extern`
Returns: `bool`
Parameters:

  * `(c char)`: A character.


Determine whether a character is lowercase.


#### `std.isprint`

Linkage: `extern`
Returns: `bool`
Parameters:

  * `(c char)`: A character.


Determine whether a character is printable (includes space).


#### `std.ispunct`

Linkage: `extern`
Returns: `bool`
Parameters:

  * `(c char)`: A character.


Determine whether a character is a punctuation mark.


#### `std.isspace`

Linkage: `extern`
Returns: `bool`
Parameters:

  * `(c char)`: A character.


Determine whether a character is a whitespace character.


#### `std.isupper`

Linkage: `extern`
Returns: `bool`
Parameters:

  * `(c char)`: A character.


Determine whether a character is an uppercase character.


#### `std.isxdigit`

Linkage: `extern`
Returns: `bool`
Parameters:

  * `(c char)`: A character.


Determine whether a character is a hexadecimal digit.


#### `std.tolower`

Linkage: `extern`
Returns: `char`
Parameters:

  * `(c char)`: A character.


Convert a character into a lowercase character, if possible.


#### `std.toupper`

Linkage: `extern`
Returns: `char`
Parameters:

  * `(c char)`: A character.


Convert a character into an uppercase character, if possible.


[Previous](./2-1-introspection.md) | [Next](./2-3-math.md)

