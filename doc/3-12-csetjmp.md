# Dale

[Previous](./3-11-cctype.md)

## 3.12 csetjmp

### Details

Module: csetjmp

### Description

Bindings to `setjmp.h`. 

Note that destructors will not work properly when `setjmp`/`longjmp`
are used.



### Structs

#### `jmpbuf`

Linkage: `extern`
Members:

  * `(a (array-of (MJMP_BUF_SIZE) char))`





### Functions

#### `setjmp`

Linkage: `extern-c`
Returns: `int`
Parameters:

  * `(a (p jmpbuf))`



#### `longjmp`

Linkage: `extern-c`
Returns: `int`
Parameters:

  * `(a (p jmpbuf))`
  * `(b int)`



[Previous](./3-11-cctype.md)

