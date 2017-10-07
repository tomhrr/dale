## Dale

[![Build Status](https://travis-ci.org/tomhrr/dale.png)](https://travis-ci.org/tomhrr/dale)

Dale is a system (no GC) programming language that uses S-expressions
for syntax and supports syntactic macros. The basic language is
similar to C, with the following additional features:

  * local type deduction;
  * overloaded functions;
  * anonymous functions;
  * reference parameters;
  * initialisers and destructors;
  * variants;
  * namespaces;
  * modules;
  * concepts; and
  * compiler introspection.

### Supported systems

This should be usable on most Linux/OS X/BSD systems where LLVM is
able to be built.  It has been tested on the following:

  * Debian 7.8, 8.0 (x86 and x86-64)
  * OS X 10.9.2
  * FreeBSD 10.0, 11.0 (x86-64)

### Documentation

[Index](./doc/index.md)  
[As single page](./doc/all.md)

### Install

#### Dependencies

  * LLVM (3.2-3.5)
  * libffi

#### Out-of-tree (recommended)

    mkdir ../build
    cd ../build
    cmake ../dale
    make
    make tests
    make install

#### In-tree

    cmake .
    make
    make tests
    make install

### Examples

**hello-world**

```lisp
(import cstdio)

(def main (fn extern-c int (void)
  (printf "hello, world\n")))
```
```
> hello, world
```

**hello-name**

```lisp
(import cstdio)

(def main (fn extern-c int (void)
  (def name (var auto (p (const char)) "name"))
  (printf "hello, %s\n" name)))
```
```
> hello, name
```

**type-deduction**

```lisp
(import cstdio)
(import stdlib)

(def main (fn extern-c int (void)
  (let ((a \ 1)
        (b \ 2))
    (printf "%d\n" (+ a b)))))
```
```
> 3
```

**overloading**

```lisp
(import cstdio)
(import cstdlib)

(def Point (struct intern ((x int) (y int))))

(def + (fn intern Point ((a Point) (b Point))
  (let ((r Point ((x (+ (@: a x) (@: b x)))
                  (y (+ (@: a y) (@: b y))))))
    (return r))))

(def main (fn extern-c int (void)
  (let ((a Point ((x 1) (y 2)))
        (b Point ((x 3) (y 4)))
        (c Point (+ a b)))
    (printf "%d %d\n" (@: c x) (@: c y)))))
```
```
> 4 6
```

**anonymous-functions**

```lisp
(import cstdio)
(import stdlib)

(def main (fn extern-c int (void)
  (let ((anon-fn \ (fn int ((n int)) (* 2 n))))
    (printf "%d\n" (anon-fn 5)))))
```
```
> 10
```

**macros**

```lisp
(import cstdio)
(import stdlib)
(import macros)

(using-namespace std.macros
  (def unless (macro intern (condition statement)
    (qq do
      (and (not (uq condition))
           (do (uq statement) true)))))

  (def main (fn extern-c int (void)
    (unless (= 1 2)
      (printf "1 does not equal 2\n"))
    (return 0))))
```
```
> 1 does not equal 2
```

**typed-macros**

```lisp
(import cstdio)
(import macros)

(using-namespace std.macros
  (def + (macro intern ((a int) (b int) (c int))
    (qq do (+ (uq a) (+ (uq b) (uq c))))))

  (def main (fn extern-c int (void)
    (printf "%d\n" (+ 1 2 3)))))
```
```
> 6
```

**introspection**

```lisp
(import introspection)
(import stdlib)

(def Point (struct intern ((x int) (y int))))

(using-namespace std.macros
  (def show-struct-details (macro intern (st)
     (let ((name  \ (@:@ st token-str))
           (count \ (struct-member-count mc st)))
       (printf "Struct: %s\n"       name)
       (printf "Member count: %d\n" count)
       (let ((i \ 0))
         (for true (< i count) (incv i)
           (printf "Member %d: %s\n" (+ i 1) (struct-member-name mc st i))))
       (nullptr DNode))))

  (def main (fn extern-c int (void)
    (show-struct-details Point)
    0)))
```
```
> Struct: Point
> Member count: 2
> Member 1: x
> Member 2: y
```

**error-reporting**

```lisp
(import introspection)
(import stdlib)

(using-namespace std.macros
  (def assert-is-struct (macro intern (st)
    (let ((count \ (struct-member-count mc st)))
      (and (= -1 count)
           (do (report-error mc st "struct type does not exist")
               true))
      (return (nullptr DNode)))))

  (def main (fn extern-c int (void)
    (assert-is-struct Point)
    0)))
```
```
> ./error-reporting.dt:13:23: error: struct type does not exist (see macro at 13:5)
> ./error-reporting.dt:13:5: error: macro expansion error (see previous)
```

**derivations**

```lisp
(import derivations)

(def Point (struct intern ((x int) (y int))))

(std.concepts.implement Struct Point)
(std.concepts.instantiate relations Point)

(def main (fn extern-c int (void)
  (let ((p1 Point ((x 1) (y 2)))
        (p2 Point ((x 3) (y 4))))
    (and (!= p1 p2)
         (do (printf "p1 and p2 are not equal\n") true))
    (and (<= p1 p2)
         (do (printf "p1 is less than or equal to p2\n") true))
    (and (> p2 p1)
         (do (printf "p2 is more than p1\n") true))
    0)))
```
```
> p1 and p2 are not equal
> p1 is less than or equal to p2
> p2 is more than p1
```

**containers**

```lisp
(import vector)
(import array)
(import algorithms)
(import cstdio)

(using-namespace std.concepts
  (instantiate Vector int)
  (instantiate Array int 10)
  (instantiate copy (Iterator (Vector int))
                    (Iterator (Array int 10))))

(def main (fn extern-c int (void)
  (let ((vec (Vector int))
        (arr (Array int 10))
        (i int))
    (for (setv i 0) (< i 10) (incv i)
      (push-back vec i))
    (copy (begin vec) (end vec) (begin arr))
    (let ((b \ (begin arr))
          (e \ (end arr)))
      (for true (!= b e) (setv b (successor b))
        (printf "%d " (@ (source b)))))
    (printf "\n")
    0)))
```
```
> 0 1 2 3 4 5 6 7 8 9 
```

**variants**

```lisp
(import variant)
(import cstdio)

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

```
```
> Number is int (1)
> Number is float (2.000000)
```

### Bugs/problems/suggestions

Please report to the [GitHub issue tracker](https://github.com/tomhrr/dale/issues).

### Discussion

The mailing list address is
[dale-lang@googlegroups.com](mailto:dale-lang@googlegroups.com).  See
also
[https://groups.google.com/forum/#!forum/dale-lang](https://groups.google.com/forum/#!forum/dale-lang).

### Licence

See LICENCE.

