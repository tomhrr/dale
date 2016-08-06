# Dale

[Previous](./1-5-functions.md) | [Next](./1-7-macros.md)

## 1.6 Control flow

There are two control flow constructs in the core language: `if` and
`goto`.

### `if`

`if` usage is as follows:

        (if {bool-expression} {true-case} {false-case})

`if` is an expression, rather than a statement.  If the true and false
branches evaluate to values of the same type, then the expression as a
whole evaluates to the value of whichever branch is selected:

        (def n (var auto \ (if (< 5 10) 1 0)))
        (printf "%d\n" n) ; prints "1\n"

If either branch terminates (i.e. concludes with a `goto` or a
`return`), while the other returns a usable value, then the expression
as a whole will evaluate to the result of the latter branch.  If both
branches terminate, or if the types of both branches are not the same,
then the expression as a whole will evaluate to `void`.

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

[Previous](./1-5-functions.md) | [Next](./1-7-macros.md)
