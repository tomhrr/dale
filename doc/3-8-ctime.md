# Dale

[Previous](./3-7-cstdio.md) | [Next](./3-9-cstdlib.md)

## 3.8 ctime

### Details

Module: ctime

### Description

Bindings to `time.h`.



### Structs

#### `time`

Linkage: `extern`
Members:

  * `(n (array-of (MTIME_T) char))`



#### `clock`

Linkage: `extern`
Members:

  * `(n (array-of (MCLOCK_T) char))`



#### `tm`

Linkage: `extern`
Members:

  * `(sec int)`
  * `(min int)`
  * `(hour int)`
  * `(mday int)`
  * `(mon int)`
  * `(year int)`
  * `(wday int)`
  * `(yday int)`
  * `(isdst int)`





### Functions

#### `clock`

Linkage: `extern-c`
Returns: `clock`
Parameters: N/A


#### `time`

Linkage: `extern-c`
Returns: `(p time)`
Parameters:

  * `(t (p time))`



#### `difftime`

Linkage: `extern-c`
Returns: `double`
Parameters:

  * `(time2 time)`
  * `(time1 time)`



#### `mktime`

Linkage: `extern-c`
Returns: `time`
Parameters:

  * `(tp (p tm))`



#### `asctime`

Linkage: `extern-c`
Returns: `(p char)`
Parameters:

  * `(tp (p tm))`



#### `ctime`

Linkage: `extern-c`
Returns: `(p char)`
Parameters:

  * `(tp (p time))`



#### `localtime`

Linkage: `extern-c`
Returns: `(p tm)`
Parameters:

  * `(t (p time))`



#### `gmtime`

Linkage: `extern-c`
Returns: `(p tm)`
Parameters:

  * `(t (p time))`



#### `strftime`

Linkage: `extern-c`
Returns: `int`
Parameters:

  * `(b (p (const char)))`
  * `(s size)`
  * `(f (p (const char)))`
  * `(t (p tm))`





### Macros

#### `MTIME_T`

Linkage: `extern`
Parameters: N/A

Macro constant for TIME_T.


#### `MCLOCK_T`

Linkage: `extern`
Parameters: N/A

Macro constant for CLOCK_T.


[Previous](./3-7-cstdio.md) | [Next](./3-9-cstdlib.md)

