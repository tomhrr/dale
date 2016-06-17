# Dale

[Previous](./3-8-ctime.md) | [Next](./3-10-cstring.md)

## 3.9 cstdlib

### Details

Module: cstdlib

### Description

Bindings to `stdlib.h`.

The `strto*` functions, with the exception of `strtod`, should not be used
directly. Instead, use the functions with the explicit type names: e.g.
`strtoi32`.



### Functions

#### `atexit`

Linkage: `extern-c`
Returns: `int`
Parameters:

  * `(fcn (p (fn void (void))))`



#### `system`

Linkage: `extern-c`
Returns: `int`
Parameters:

  * `(s (p (const char)))`



#### `getenv`

Linkage: `extern-c`
Returns: `(p (const char))`
Parameters:

  * `(s (p (const char)))`



#### `bsearch`

Linkage: `extern-c`
Returns: `(p void)`
Parameters:

  * `(key (p void))`
  * `(base (p void))`
  * `(n size)`
  * `(size size)`
  * `(cmp (p (fn int ((keyval (p void)) (datum (p void))))))`



#### `qsort`

Linkage: `extern-c`
Returns: `void`
Parameters:

  * `(base (p void))`
  * `(n size)`
  * `(size size)`
  * `(cmp (p (fn int ((keyval (p void)) (datum (p void))))))`



#### `exit`

Linkage: `extern-c`
Returns: `int`
Parameters:

  * `(exit-code int)`



#### `abort`

Linkage: `extern-c`
Returns: `void`
Parameters: N/A


#### `malloc`

Linkage: `extern-c`
Returns: `(p void)`
Parameters:

  * `(num size)`



#### `free`

Linkage: `extern-c`
Returns: `void`
Parameters:

  * `(ptr (p void))`



#### `calloc`

Linkage: `extern-c`
Returns: `(p void)`
Parameters:

  * `(num size)`
  * `(size size)`



#### `realloc`

Linkage: `extern-c`
Returns: `(p void)`
Parameters:

  * `(ptr (p void))`
  * `(size size)`



#### `atoi`

Linkage: `extern-c`
Returns: `int`
Parameters:

  * `(str (p (const char)))`



#### `atof`

Linkage: `extern-c`
Returns: `double`
Parameters:

  * `(str (p (const char)))`



#### `strtod`

Linkage: `extern-c`
Returns: `double`
Parameters:

  * `(str (p (const char)))`
  * `(endp (p (const char)))`



#### `strtol`

Linkage: `extern-c`
Returns: `(long-type)`
Parameters:

  * `(str (p (const char)))`
  * `(endp (p (p (const char))))`
  * `(base int)`



#### `strtoul`

Linkage: `extern-c`
Returns: `(ulong-type)`
Parameters:

  * `(str (p (const char)))`
  * `(endp (p (p (const char))))`
  * `(base int)`



#### `strtoll`

Linkage: `extern-c`
Returns: `(long-long-type)`
Parameters:

  * `(str (p (const char)))`
  * `(endp (p (p (const char))))`
  * `(base int)`



#### `strtoull`

Linkage: `extern-c`
Returns: `(ulong-long-type)`
Parameters:

  * `(str (p (const char)))`
  * `(endp (p (p (const char))))`
  * `(base int)`



#### `strtoi32`

Linkage: `extern`
Returns: `int32`
Parameters:

  * `(str (p (const char)))`
  * `(endp (p (p (const char))))`
  * `(base int)`



#### `strtoi64`

Linkage: `extern`
Returns: `int64`
Parameters:

  * `(str (p (const char)))`
  * `(endp (p (p (const char))))`
  * `(base int)`



#### `strtoui32`

Linkage: `extern`
Returns: `uint32`
Parameters:

  * `(str (p (const char)))`
  * `(endp (p (p (const char))))`
  * `(base int)`



#### `strtoui64`

Linkage: `extern`
Returns: `uint64`
Parameters:

  * `(str (p (const char)))`
  * `(endp (p (p (const char))))`
  * `(base int)`



#### `rand`

Linkage: `extern-c`
Returns: `int`
Parameters: N/A


#### `srand`

Linkage: `extern-c`
Returns: `void`
Parameters:

  * `(seed uint)`



[Previous](./3-8-ctime.md) | [Next](./3-10-cstring.md)

