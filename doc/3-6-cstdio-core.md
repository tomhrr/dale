# Dale

[Previous](./3-5-csignal.md) | [Next](./3-7-cstdio.md)

## 3.6 cstdio-core

### Details

Module: cstdio-core

### Description

Provides bindings for the parts of `stdio.h` on which the `macros`
module depends. This is separate from `cstdio`, so that `cstdio` can
depend in turn on `macros`.



### Structs

#### `file`

Linkage: `extern`
Members: N/A




### Functions

#### `strlen`

Linkage: `extern-c`
Returns: `size`
Parameters:

  * `(str (p (const char)))`



#### `strncpy`

Linkage: `extern-c`
Returns: `(p char)`
Parameters:

  * `(dest (p char))`
  * `(source (p (const char)))`
  * `(size size)`



#### `fprintf`

Linkage: `extern-c`
Returns: `int`
Parameters:

  * `(stream (p file))`
  * `(str (p (const char)))`
  * `...`



#### `printf`

Linkage: `extern-c`
Returns: `int`
Parameters:

  * `(str (p (const char)))`
  * `...`



#### `sprintf`

Linkage: `extern-c`
Returns: `int`
Parameters:

  * `(str (p char))`
  * `(fmt (p (const char)))`
  * `...`



#### `strchr`

Linkage: `extern-c`
Returns: `(p char)`
Parameters:

  * `(str (p char))`
  * `(c int)`



#### `memset`

Linkage: `extern-c`
Returns: `(p void)`
Parameters:

  * `(a (p void))`
  * `(b int)`
  * `(c size)`



#### `strcmp`

Linkage: `extern-c`
Returns: `int`
Parameters:

  * `(str (p (const char)))`
  * `(str2 (p (const char)))`



#### `strncmp`

Linkage: `extern-c`
Returns: `int`
Parameters:

  * `(str (p (const char)))`
  * `(str2 (p (const char)))`
  * `(n size)`



[Previous](./3-5-csignal.md) | [Next](./3-7-cstdio.md)

