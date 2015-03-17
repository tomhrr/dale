# Dale

[Previous](./1-5-functions.md) | [Next](./1-7-macros.md)

## 1.6 Control flow

There are two control flow constructs in the core language: `if` and
`goto`.

### `if`

`if` usage is as follows:

        (if {bool-expression} {true-case} {false-case})

`if` is an expression, rather than a statement: it returns the value
of evaluating whichever case is selected:

        (def n (var auto \ (if (< 5 10) 1 0)))
        (printf "%d\n" n) ; prints "1\n"

Because it is an expression, both branches must return the same type
of value. However, if either branch terminates (i.e. concludes with a
`goto` or a `return`), it doesn't have a 'type' as such, and the other
branch may then evaluate to any type.

The lack of implicit casting means that many expressions which would
evaluate to true/false in C do not work in the same way in Dale:

        (if (+ n 1) 1 0)        ; will not compile
        (if (!= 0 (+ n 1)) 1 0) ; will compile

### `goto`

`goto` depends on `label`, which is another core form. Usage is like
so:

        (def i (var auto \ 0))
        (label start)
          (if (< i 10)
              (do (printf "In loop: %d\n" i)
                  (setv i (+ i 1))
                  (goto start))
              (goto end))
        (label end)

(`do` is a core form that takes a series of forms as its 'arguments',
evaluates each in turn, and returns the value yielded by the last
form.)

Labels are scoped to the containing procedure, surprisingly enough:
`new-scope` does not allow for label names to be shadowed.

[Previous](./1-5-functions.md) | [Next](./1-7-macros.md)
