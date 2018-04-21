# Dale

[Previous](./2-4-macros.md) | [Next](./2-6-assert.md)

## 2.5 stdlib

### Details

Module: stdlib

### Description

Provides commonly-used macros and functions, including the core
control structures (`for`, `while` and `let`). All of the bindings
provided by this library are in the root namespace.



### Macros

#### `is-valid`

Linkage: `extern`
Parameters:

  * `(arg bool)`: A boolean.


Returns its boolean argument.  Defined for use with `and` and `or`.


#### `and`

Linkage: `extern`
Parameters:

  * `a`: The condition expression.
  * `b`: The form to run when condition is true.


Returns the first argument, if it is not valid. Returns the last
argument otherwise.  A object `o` is valid if `(is-valid o)` returns
true.


#### `or`

Linkage: `extern`
Parameters:

  * `a`: The condition expression.
  * `b`: The form to run when condition is false.


Returns the first argument, if it is valid. Returns the last argument
otherwise.  A object `o` is valid if `(is-valid o)` returns true.


#### `while`

Linkage: `extern`
Parameters:

  * `condition`: The condition expression form.
  * `rest`


Takes a condition expression and an arbitrary number of other forms as
its arguments. Expands to a form that loops over the provided forms
for so long as the condition expression evaluates to `true`.


#### `incf`

Linkage: `extern`
Parameters:

  * `form`: The pointer variable to be incremented.


Takes a pointer form as its single argument. Expands to a form that
increments the underlying value and returns true.


#### `decf`

Linkage: `extern`
Parameters:

  * `form`: The pointer variable to be decremented.


Takes a pointer form as its single argument. Expands to a form that
decrements the underlying value and returns true.


#### `incv`

Linkage: `extern`
Parameters:

  * `form`: The variable to be incremented.


Takes a variable form as its single argument. Expands to a form that
increments the underlying value and returns true.


#### `decv`

Linkage: `extern`
Parameters:

  * `form`: The variable to be decremented.


Takes a variable form as its single argument. Expands to a form that
decrements the underlying value and returns true.


#### `for`

Linkage: `extern`
Parameters:

  * `init-form`: The initialisation form.
  * `condition`: The condition expression.
  * `loop-entry`: The loop entry form.
  * `rest`


Takes an initialisation form, a condition expression, a loop entry
form and an arbitrary number of other forms as its arguments. Expands
into a 'for' loop: the initialisation form is run at the beginning,
the loop entry form is run on loop entry (except on the first
iteration), and the loop is only run for so long as the condition
expression evaluates to true.  The initialisation form must be either
a `let` variable binding (e.g. `(i \ 0)`), or a `setv` call (e.g.
`(setv i 0)`, or the atom `true`.


#### `let`

Linkage: `extern`
Parameters:

  * `new-vars`
  * `rest`


A form for introducing local (automatic storage) variables. It's
easier to explain by example.

        (let (({name1} {type1} [{value1}])
              ({name2} {type2} [{value2}]))
          ; ...
          )

expands to:

        (new-scope
          (def {name1} (var auto {type1} {value1}))
          (def {name2} (var auto {type2} {value2}))
          ; ...
          )


#### `cond`

Linkage: `extern`
Parameters:

  * `rest`


As per Common Lisp's `cond`, except that the first element of each
argument must be a condition, and each argument must have at least two
elements.

    (cond ((= x 1) 2)
          ((= x 2) 3)
          (true    4))

expands to:

    (if (= x 1) 2
    (if (= x 2) 3
                4))

The last condition test must be the literal `true`.

#### `malloc'`

Linkage: `extern`
Parameters:

  * `n`: The number of objects being allocated.
  * `T`: The type of the objects being allocated.


Expands to a `malloc` that allocates memory sufficient for `n` `T`s.


#### `free'`

Linkage: `extern`
Parameters:

  * `form`: The pointer form.


Expands to a `free` to which the pointer form argument, after being
cast to a void pointer, is passed.


#### `p<=`

Linkage: `extern`
Parameters:

  * `ptr1`
  * `ptr2`



#### `p>=`

Linkage: `extern`
Parameters:

  * `ptr1`
  * `ptr2`



#### `make-macro-constant`

Linkage: `extern`
Parameters:

  * `name`: The name of the macro constant.
  * `value`: The value for the new macro constant.


Expands to a macro definition, with the specified name, that in turn
expands to the value.


#### `short-type`

Linkage: `extern`
Parameters: N/A

Expands to an integer type that has the same size as the system C
compiler's `short` type. Only intended for use when writing C bindings.


#### `ushort-type`

Linkage: `extern`
Parameters: N/A

Expands to an unsigned integer type that has the same size as the
system C compiler's `unsigned short` type. Only intended for use when
writing C bindings.


#### `long-type`

Linkage: `extern`
Parameters: N/A

Expands to an integer type that has the same size as the system C
compiler's `long` type. Only intended for use when writing C bindings.


#### `ulong-type`

Linkage: `extern`
Parameters: N/A

Expands to an unsigned integer type that has the same size as the
system C compiler's `unsigned long` type. Only intended for use when
writing C bindings.


#### `long-long-type`

Linkage: `extern`
Parameters: N/A

Expands to an integer type that has the same size as the system C
compiler's `long long` type. Only intended for use when writing C
bindings.


#### `ulong-long-type`

Linkage: `extern`
Parameters: N/A

Expands to an unsigned integer type that has the same size as the
system C compiler's `unsigned long long` type. Only intended for use
when writing C bindings.


#### `mfor`

Linkage: `extern`
Parameters:

  * `value-name`
  * `value-list`
  * `rest`


Short for 'macro for', but more akin to a 'foreach'. Takes a
value-name form, a value-list form and an arbitrary number of other
forms. For each entry in value-list, bind value-name to that value,
and expand the other forms, substituting any instance of value-name in
those forms with the value from value-list. For example:

        (mfor str ("asdf" "zxcv" "qwer")
          (printf "%s\n" str))

expands to:

        (printf "%s\n" "asdf")
        (printf "%s\n" "zxcv")
        (printf "%s\n" "qwer")


#### `mfor'`

Linkage: `extern`
Parameters:

  * `value-name`
  * `value-list`
  * `rest`


Like `mfor`, except that it evaluates the value-list form as a macro
call to determine the actual list values that should be used.  This
allows for calls like:

        (mfor' N (range 0 2)
          (printf "%d\n" N))

to expand to:

        (printf "0\n")
        (printf "1\n")
        (printf "2\n")


#### `mif`

Linkage: `extern`
Parameters:

  * `condition`
  * `then`
  * `else`


Short for 'macro if'.  Operates in the same way as the core `if` form,
except that the condition is evaluated at compile-time and the form as
a whole expands to one of the provided branches.


#### `no-op`

Linkage: `extern`
Parameters: N/A

Expands to an empty `do` form, which is a no-op both at the top-level
and within a procedure.


#### `ignore`

Linkage: `extern`
Parameters:

  * `rest`


Takes any number of arguments, and expands to `(no-op)`.  Useful for
ignoring parts of s-expressions without having to search, where they
end.


#### `identity`

Linkage: `extern`
Parameters:

  * `frm`: A node.


Expands to the argument form.


#### `range`

Linkage: `extern`
Parameters:

  * `a`: An integer DNode.
  * `b`: An integer DNode.


Expands to a list of integers.


#### `+'`

Linkage: `N/A`
Parameters: N/A

For each of the primitive numeric types, macros that correspond to the
core arithmetical and relational operators are defined which cast the
second argument so that it has the same type as the first.  Each of
these macros has the same name as the original function, except with a
' symbol appended.


[Previous](./2-4-macros.md) | [Next](./2-6-assert.md)

