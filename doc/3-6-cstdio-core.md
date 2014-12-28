# Dale

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

  * `(str (p char))`



#### `strncpy`

Linkage: `extern-c`
Returns: `(p char)`
Parameters:

  * `(dest (p char))`
  * `(source (p char))`
  * `(size size)`



#### `fprintf`

Linkage: `extern-c`
Returns: `int`
Parameters:

  * `(stream (p file))`
  * `(str (p char))`
  * `...`



#### `printf`

Linkage: `extern-c`
Returns: `int`
Parameters:

  * `(str (p char))`
  * `...`



#### `sprintf`

Linkage: `extern-c`
Returns: `int`
Parameters:

  * `(str (p char))`
  * `(fmt (p char))`
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

  * `(str (p char))`
  * `(str2 (p char))`



#### `strncmp`

Linkage: `extern-c`
Returns: `int`
Parameters:

  * `(str (p char))`
  * `(str2 (p char))`
  * `(n size)`



