# Dale

[Previous](./3-9-cstdlib.md) | [Next](./3-11-cctype.md)

## 3.10 cstring

### Details

Module: cstring

### Description

Bindings to `string.h`.



### Functions

#### `strcpy`

Linkage: `extern-c`
Returns: `int`
Parameters:

  * `(dest (p char))`
  * `(source (const (p (const char))))`



#### `strncpy`

Linkage: `extern-c`
Returns: `(p char)`
Parameters:

  * `(dest (p char))`
  * `(source (p (const char)))`
  * `(size size)`



#### `strcat`

Linkage: `extern-c`
Returns: `int`
Parameters:

  * `(str (p char))`
  * `(str2 (p (const char)))`



#### `strncat`

Linkage: `extern-c`
Returns: `int`
Parameters:

  * `(str (p char))`
  * `(str2 (p char))`
  * `(size size)`



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



#### `strchr`

Linkage: `extern-c`
Returns: `(p char)`
Parameters:

  * `(str (p char))`
  * `(c int)`



#### `strrchr`

Linkage: `extern-c`
Returns: `(p (const char))`
Parameters:

  * `(str (p (const char)))`
  * `(c int)`



#### `strspn`

Linkage: `extern-c`
Returns: `size`
Parameters:

  * `(str (p char))`
  * `(str2 (p char))`



#### `strcspn`

Linkage: `extern-c`
Returns: `size`
Parameters:

  * `(str (p char))`
  * `(str2 (p char))`



#### `strpbrk`

Linkage: `extern-c`
Returns: `(p char)`
Parameters:

  * `(str (p char))`
  * `(str2 (p char))`



#### `strstr`

Linkage: `extern-c`
Returns: `(p (const char))`
Parameters:

  * `(str (p (const char)))`
  * `(str2 (p (const char)))`



#### `strlen`

Linkage: `extern-c`
Returns: `size`
Parameters:

  * `(str (p (const char)))`



#### `strerror`

Linkage: `extern-c`
Returns: `(p char)`
Parameters:

  * `(n size)`



#### `strtok`

Linkage: `extern-c`
Returns: `(p char)`
Parameters:

  * `(str (p char))`
  * `(str2 (p char))`



#### `memcpy`

Linkage: `extern-c`
Returns: `(p void)`
Parameters:

  * `(str (p void))`
  * `(str2 (p void))`
  * `(n size)`



#### `memmove`

Linkage: `extern-c`
Returns: `(p void)`
Parameters:

  * `(str (p void))`
  * `(str2 (p void))`
  * `(n size)`



#### `memcmp`

Linkage: `extern-c`
Returns: `int`
Parameters:

  * `(str (p void))`
  * `(str2 (p void))`
  * `(n size)`



#### `memchr`

Linkage: `extern-c`
Returns: `(p void)`
Parameters:

  * `(a (p void))`
  * `(b int)`
  * `(c size)`



#### `memset`

Linkage: `extern-c`
Returns: `(p void)`
Parameters:

  * `(a (p void))`
  * `(b int)`
  * `(c size)`



[Previous](./3-9-cstdlib.md) | [Next](./3-11-cctype.md)

