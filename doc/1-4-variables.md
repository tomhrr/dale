# Dale

## 1.4 Variables

### Basic core forms and syntax

`#` (hash) is the 'address-of' core form, analogous to `&` in C.

`@` (at) is the 'dereference' core form, analogous to `*` in C.

`;` (semicolon) is used to introduce single-line comments. `#|` and
`|#` are used to delimit multi-line comments.

Identifiers for variables and procedures may include any printable
non-whitespace ASCII character, but must start with a non-numeric
character.

### Variable definitions

Variables are declared like so:

        (def {name} (var {linkage} {type} [{value}]))

`{linkage}` is one of `intern`, `extern`, `extern-c` and `auto`. The
other placeholders are self-explanatory. `auto` is only valid for
procedure-scoped variables.

`setf` is the core form for setting a variable:

        (setf {pointer-to-variable} {new-value})

`setf` takes a pointer as its first argument so that it can be
overridden for specific types by user-level functions. (References, in
the C++ sense of the word, are not supported.)

`setv` is an additional core form that can be used for setting
variables. It is provided by the compiler, but operates as a macro:

        (setv {variable} {new-value})
            => (setf (# {variable}) {new-value})

### Arrays

Array variables can be populated at definition time:

        (def myarray (var auto (array-of 3 int) (array 1 2 3)))

If the array type is zero-sized, then it will be set based on the
number of elements in the array being used to populate the variable.

`$` (dollar sign) is the array accessor core form. It returns a
pointer to the specified array index:

        (setf ($ myarray 1) 5)
        (printf "%d\n" (@ ($ myarray 1))) ; prints "5\n"

### Structs

As per array variables, struct variables can be populated at
definition time. `{value}`, in that case, comprises a list of
name-value pairs, as per the following example:

        (def Point (struct intern ((x int) (y int))))

        ; within a procedure:

        (def mypoint (var auto Point ((x 1) (y 2))))

`:` (colon) is the struct accessor core form. It returns a pointer to
the specified struct member value:

        (setf (: mypoint x) 2)
        (printf "%d\n" (@ (: mypoint x))) ; prints "2\n"

### Enums

Enum variables can be set at definition time:

        (def Lang (enum intern int (en fr de)))

        ; within a procedure:

        (def mypoint (var auto Lang en))

There are no special enum accessor forms. Setting an enum variable
is done in the same way as for other scalar variables.

### Variable scope

Variables within procedures are lexically-scoped. A new lexical scope
can be introduced by way of the `new-scope` core form:

        (def x (var auto int 1))
        (printf "%d\n" x) ; prints "1\n"
        (new-scope
          (def x (var auto int 2))
          (printf "%d\n" x)) ; prints "2\n"
        (printf "%d\n" x) ; prints "1\n"
    
It should not generally be necessary to use the `new-scope` form in
user-level code. It is primarily intended for use in control flow
macros and similar, such as `for`, `while` and `let`.

### Local type deduction

If a variable definition includes a `{value}`, the `{type}` may be set
to `\` (backslash), in which case the type of the `{value}` will be
used as the type of the variable. Literals are handled specially:
integers always take the type `int`, and floating-point literals
always take the type `float`.
