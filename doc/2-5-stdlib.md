# Dale

[Previous](./2-4-macros-core.md) | [Next](./2-6-macros.md)

## 2.5 stdlib

### Details

Module: stdlib

### Description

Provides commonly-used macros and functions, including the core
control structures (`for`, `while` and `let`). All of the bindings
provided by this library are in the root namespace.



### Macros

#### `@:'`

Linkage: `extern`
Parameters:

  * `structp`: The struct pointer argument.
  * `member`: The first member's name.
  * `rest`


Takes a struct pointer and one or more struct member names as its
arguments. Expands to repeated calls to `@:@` over the struct and the
member names. For example, if two members are provided, the expansion
is `(@:@ (@:@ structp member1) member2)`.


#### `and`

Linkage: `extern`
Parameters:

  * `(condition bool)`: The condition expression.
  * `true-case`: The form to run when condition is true.


Takes a condition expression and a form to execute when the condition
is true. If the condition is true, returns the result of evaluating
the form. If the condition is false, returns `false`.


#### `or`

Linkage: `extern`
Parameters:

  * `(condition bool)`: The condition expression.
  * `false-case`: The form to run when condition is false.


Takes a condition expression and a form to execute when the condition
is false. If the condition is false, returns the result of evaluating
the form. If the condition is true, returns `true`.


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


#### `long-type`

Linkage: `extern`
Parameters: N/A

Expands to `sizeof(long)`. Only intended for use when writing C
bindings.


#### `ulong-type`

Linkage: `extern`
Parameters: N/A

Expands to `sizeof(unsigned long)`. Only intended for use when writing
C bindings.


#### `long-long-type`

Linkage: `extern`
Parameters: N/A

Expands to `sizeof(long long)`. Only intended for use when writing
C bindings.


#### `ulong-long-type`

Linkage: `extern`
Parameters: N/A

Expands to `sizeof(unsigned long long)`. Only intended for use when
writing C bindings.


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


#### `+'`

Linkage: `N/A`
Parameters: N/A

For each of the primitive numeric types, macros that correspond to the
core arithmetical and relational operators are defined which cast the
second argument so that it has the same type as the first.  Each of
these macros has the same name as the original function, except with a
' symbol appended.


[Previous](./2-4-macros-core.md) | [Next](./2-6-macros.md)

