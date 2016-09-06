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
                             (Float  ((a float)))
                             (Double ((a double)))))

        (def main (fn extern-c int (void)
          (let ((ni Number (Int 1))
                (nf Number (Float 2.0))
                (nd Number (Double (cast 3.0 double)))
                (na (array-of 3 Number) (array ni nf nd)))
            (for (i \ 0) (< i 3) (incv i)
              (let ((nc Number (@$ na i)))
                (case nc
                  (Int    (printf "Number is int (%d)\n"    (@:@ nc a)))
                  (Float  (printf "Number is float (%f)\n"  (@:@ nc a)))
                  (Double (printf "Number is double (%f)\n" (@:@ nc a)))))))
          0))

yields:

	Number is int (1)
	Number is float (2.000000)
	Number is double (3.000000)

[Previous](./2-22-bitset-enum.md) | [Next](./3-1-cerrno.md)

