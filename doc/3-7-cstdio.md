# Dale

[Previous](./3-6-cstdio-core.md) | [Next](./3-8-ctime.md)

## 3.7 cstdio

### Details

Module: cstdio

### Description

Bindings to `stdio.h`. Provides the following macro constants:

  * L-tmpnam
  * TMP-MAX
  * FILENAME-MAX
  * FOPEN-MAX
  * RAND-MAX
  * EXIT-FAILURE
  * EXIT-SUCCESS



### Variables

#### `stdin`

Linkage: `extern-c`
Type: `(p file)`

The standard input handle.


#### `stdout`

Linkage: `extern-c`
Type: `(p file)`

The standard output handle.


#### `stderr`

Linkage: `extern-c`
Type: `(p file)`

The standard error handle.




### Structs

#### `file`

Linkage: `extern`
Members: N/A


#### `buffer-mode`

Linkage: `extern`
Members:

  * `(value int)`



#### `fseekorigin`

Linkage: `extern`
Members:

  * `(value int)`



#### `fpos`

Linkage: `extern`
Members:

  * `(n (array-of (MFPOS_T) char))`





### Functions

#### `fdopen`

Linkage: `extern-c`
Returns: `(p file)`
Parameters:

  * `(fd int)`
  * `(mode (p (const char)))`



#### `fopen`

Linkage: `extern-c`
Returns: `(p file)`
Parameters:

  * `(filename (p (const char)))`
  * `(mode (p (const char)))`



#### `freopen`

Linkage: `extern-c`
Returns: `(p file)`
Parameters:

  * `(filename (p (const char)))`
  * `(mode (p (const char)))`
  * `(stream (p file))`



#### `fflush`

Linkage: `extern-c`
Returns: `int`
Parameters:

  * `(stream (p file))`



#### `fclose`

Linkage: `extern-c`
Returns: `int`
Parameters:

  * `(stream (p file))`



#### `remove`

Linkage: `extern-c`
Returns: `int`
Parameters:

  * `(filename (p (const char)))`



#### `rename`

Linkage: `extern-c`
Returns: `int`
Parameters:

  * `(old-name (p (const char)))`
  * `(new-name (p (const char)))`



#### `tmpfile`

Linkage: `extern-c`
Returns: `(p file)`
Parameters: N/A


#### `tmpnam`

Linkage: `extern-c`
Returns: `(p char)`
Parameters:

  * `(buffer (p char))`



#### `setvbuf`

Linkage: `extern-c`
Returns: `int`
Parameters:

  * `(stream (p file))`
  * `(buf (p char))`
  * `(mode buffer-mode)`
  * `(size size)`



#### `setbuf`

Linkage: `extern-c`
Returns: `void`
Parameters:

  * `(stream (p file))`
  * `(buf (p char))`



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



#### `vprintf`

Linkage: `extern-c`
Returns: `int`
Parameters:

  * `(str (p (const char)))`
  * `(arg va-list)`



#### `vfprintf`

Linkage: `extern-c`
Returns: `int`
Parameters:

  * `(stream (p file))`
  * `(str (p (const char)))`
  * `(arg va-list)`



#### `vsprintf`

Linkage: `extern-c`
Returns: `int`
Parameters:

  * `(str (p char))`
  * `(fmt (p (const char)))`
  * `(arg va-list)`



#### `fgetc`

Linkage: `extern-c`
Returns: `int`
Parameters:

  * `(stream (p file))`



#### `fgets`

Linkage: `extern-c`
Returns: `(p char)`
Parameters:

  * `(str (p char))`
  * `(n int)`
  * `(stream (p file))`



#### `fputc`

Linkage: `extern-c`
Returns: `int`
Parameters:

  * `(c int)`
  * `(stream (p file))`



#### `fputs`

Linkage: `extern-c`
Returns: `int`
Parameters:

  * `(str (p (const char)))`
  * `(stream (p file))`



#### `getc`

Linkage: `extern-c`
Returns: `int`
Parameters:

  * `(stream (p file))`



#### `getchar`

Linkage: `extern-c`
Returns: `int`
Parameters:

  * `(stream (p file))`



#### `gets`

Linkage: `extern-c`
Returns: `(p char)`
Parameters:

  * `(str (p char))`



#### `putc`

Linkage: `extern-c`
Returns: `int`
Parameters:

  * `(c int)`
  * `(stream (p file))`



#### `putchar`

Linkage: `extern-c`
Returns: `int`
Parameters:

  * `(c int)`



#### `puts`

Linkage: `extern-c`
Returns: `int`
Parameters:

  * `(str (p (const char)))`



#### `ungetc`

Linkage: `extern-c`
Returns: `int`
Parameters:

  * `(c int)`
  * `(stream (p file))`



#### `fread`

Linkage: `extern-c`
Returns: `size`
Parameters:

  * `(ptr (p void))`
  * `(size size)`
  * `(count size)`
  * `(stream (p file))`



#### `fwrite`

Linkage: `extern-c`
Returns: `size`
Parameters:

  * `(ptr (p void))`
  * `(size size)`
  * `(count size)`
  * `(stream (p file))`



#### `fseek`

Linkage: `extern-c`
Returns: `int`
Parameters:

  * `(stream (p file))`
  * `(offset int)`
  * `(origin fseekorigin)`



#### `ftell`

Linkage: `extern-c`
Returns: `int`
Parameters:

  * `(stream (p file))`



#### `rewind`

Linkage: `extern-c`
Returns: `void`
Parameters:

  * `(stream (p file))`



#### `fgetpos`

Linkage: `extern-c`
Returns: `int`
Parameters:

  * `(stream (p file))`
  * `(ptr (p fpos))`



#### `fsetpos`

Linkage: `extern-c`
Returns: `int`
Parameters:

  * `(stream (p file))`
  * `(ptr (p fpos))`



#### `clearerr`

Linkage: `extern-c`
Returns: `void`
Parameters:

  * `(stream (p file))`



#### `feof`

Linkage: `extern-c`
Returns: `int`
Parameters:

  * `(stream (p file))`



#### `ferror`

Linkage: `extern-c`
Returns: `int`
Parameters:

  * `(stream (p file))`



#### `perror`

Linkage: `extern-c`
Returns: `void`
Parameters:

  * `(str (p (const char)))`





### Macros

#### `MFPOS_T`

Linkage: `extern`
Parameters: N/A


[Previous](./3-6-cstdio-core.md) | [Next](./3-8-ctime.md)

