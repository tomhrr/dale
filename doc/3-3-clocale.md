# Dale

[Previous](./3-2-cfloat.md) | [Next](./3-4-cmath.md)

## 3.3 clocale

### Details

Module: clocale

### Description

Bindings from `locale.h`.



### Structs

#### `lconv`

Linkage: `extern`
Members:

  * `(decimal-point (p char))`
  * `(grouping (p char))`
  * `(thousands-sep (p char))`
  * `(currency-symbol (p char))`
  * `(int-curr-symbol (p char))`
  * `(mon-decimal-point (p char))`
  * `(mon-grouping (p char))`
  * `(mon-thousands-sep (p char))`
  * `(negative-sign (p char))`
  * `(positive-sign (p char))`
  * `(frac-digits char)`
  * `(int-frac-digits char)`
  * `(n-cs-precedes char)`
  * `(n-sep-by-space char)`
  * `(n-sign-posn char)`
  * `(p-cs-precedes char)`
  * `(p-sep-by-space char)`
  * `(p-sign-posn char)`





### Functions

#### `localeconv`

Linkage: `extern-c`
Returns: `(p lconv)`
Parameters: N/A


#### `setlocale`

Linkage: `extern-c`
Returns: `(p char)`
Parameters:

  * `(category int)`
  * `(locale (p char))`



[Previous](./3-2-cfloat.md) | [Next](./3-4-cmath.md)

