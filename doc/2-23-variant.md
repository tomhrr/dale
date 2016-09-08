# Dale

[Previous](./2-22-bitset-enum.md) | [Next](./3-1-cerrno.md)

## 2.23 variant

### Details

Module: variant

### Description

Provides `def-variant`, which allows for defining variants, also known
as sum types or tagged unions.  Variant definition introduces the
following bindings:

   * a struct type for the variant;
   * a struct type for each of the variant's potential types;
   * constructors for each of the potential types, which take as their
     arguments the members for that potential type in definition
     order (if any);
   * a `case` form, which allows for executing a statement based on
     the underlying type of a particular variant.

For example:

        (def-variant Direction (North South East West))

        (def main (fn extern-c int (void)
          (let ((direction Direction (North)))
            (case direction
              (North (printf "Direction is north\n"))
              (true  (printf "Direction is not north\n"))))
          0))

yields:

        Direction is north

With parameters for the potential types:

        (def-variant Number ((Int    ((a int)))
                             (Float  ((a float)))))

        (def main (fn extern-c int (void)
          (let ((ni Number (Int 1))
                (nf Number (Float 2.0))
                (na (array-of 2 Number) (array ni nf)))
            (for (i \ 0) (< i 2) (incv i)
              (let ((nc Number (@$ na i)))
                (case nc
                  (Int    (printf "Number is int (%d)\n"    (@:@ nc a)))
                  (Float  (printf "Number is float (%f)\n"  (@:@ nc a)))))))
          0))

yields:

	Number is int (1)
	Number is float (2.000000)

The first element of each defined struct type is an `int` named
'type'.  This value will be 1 for the first potential type, 2 for the
second potential type, and so on.  This is a documented part of this
interface so as to support interoperating with C libraries that use
unions to achieve the same outcome.  (At the moment, there's no need
for customising the type of this first field, but that may be added
later.)



### Macros

#### `def-variant`

Linkage: `extern`
Parameters:

  * `name`: The name of the new variant.
  * `instances`: The type definitions for the variant.



[Previous](./2-22-bitset-enum.md) | [Next](./3-1-cerrno.md)

