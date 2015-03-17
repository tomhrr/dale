# Dale

[Previous](./3-10-cstring.md) | [Next](./3-12-csetjmp.md)

## 3.11 cctype

### Details

Module: cctype

### Description

Bindings to `ctype.h`. These bindings are awkward to use, because each
takes an `int`, instead of a `char`. See the `ctype` module for a
neater interface.



### Functions

#### `isalnum`

Linkage: `extern-c`
Returns: `int`
Parameters:

  * `(c int)`: A character as an integer.


Determine whether a character is alphanumeric.


#### `isalpha`

Linkage: `extern-c`
Returns: `int`
Parameters:

  * `(c int)`: A character as an integer.


Determine whether a character is alphabetical.


#### `iscntrl`

Linkage: `extern-c`
Returns: `int`
Parameters:

  * `(c int)`: A character as an integer.


Determine whether a character is a control character.


#### `isdigit`

Linkage: `extern-c`
Returns: `int`
Parameters:

  * `(c int)`: A character as an integer.


Determine whether a character is a digit.


#### `isgraph`

Linkage: `extern-c`
Returns: `int`
Parameters:

  * `(c int)`: A character as an integer.


Determine whether a character is printable (excludes space).


#### `islower`

Linkage: `extern-c`
Returns: `int`
Parameters:

  * `(c int)`: A character as an integer.


Determine whether a character is lowercase.


#### `isprint`

Linkage: `extern-c`
Returns: `int`
Parameters:

  * `(c int)`: A character as an integer.


Determine whether a character is printable (includes space).


#### `ispunct`

Linkage: `extern-c`
Returns: `int`
Parameters:

  * `(c int)`: A character as an integer.


Determine whether a character is a punctuation mark.


#### `isspace`

Linkage: `extern-c`
Returns: `int`
Parameters:

  * `(c int)`: A character as an integer.


Determine whether a character is a whitespace character.


#### `isupper`

Linkage: `extern-c`
Returns: `int`
Parameters:

  * `(c int)`: A character as an integer.


Determine whether a character is uppercase.


#### `isxdigit`

Linkage: `extern-c`
Returns: `int`
Parameters:

  * `(c int)`: A character as an integer.


Determine whether a character is a hexadecimal digit.


#### `tolower`

Linkage: `extern-c`
Returns: `int`
Parameters:

  * `(c int)`: A character as an integer.


Convert a character into a lowercase character, if possible.


#### `toupper`

Linkage: `extern-c`
Returns: `int`
Parameters:

  * `(c int)`: A character as an integer.


Convert a character into an uppercase character, if possible.


[Previous](./3-10-cstring.md) | [Next](./3-12-csetjmp.md)

