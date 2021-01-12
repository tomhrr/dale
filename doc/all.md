# Dale

## <a name="Introduction"></a> 1.1 Introduction

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

There are also a number of core differences in basic functionality, as
compared with C:

  * The only implicit casting that occurs is:
    * from non-const to const, generally;
    * for integral type arguments to `extern-c` functions; and
    * for values used to initialise explicitly-typed variable 
      declarations.

  * Arithmetical operators (`+`, `-`, etc.) are exposed as functions,
    rather than core forms. Functions like `+` are only defined for
    single types; i.e. `(+ int int)` is defined by default, but `(+
    int float)` is not. However, there are macros in the `stdlib`
    module corresponding to the core operators/functions (e.g. `+'`,
    `-'`) that cast the second argument to that of the first, to make
    this a bit easier.

  * Linkage is not implicit in the core language. This extends to
    local variables in functions and macros. (In practice, the `let`
    macro is generally used to introduce local variable bindings, so
    this isn't a significant problem.)

Throughout the documentation, the term 'procedure' is used to refer to
functions and macros as a single group.

Note that the language is still very much at the experimental/toy
stage, so there are likely to be bugs, documentation inconsistencies
and so on.

### Motivation

The aim with Dale is to see how a language with a minimal C-like core
plus syntactic macros works in practice, and whether things usually
implemented within languages can be moved out to libraries if they
have the appropriate level of support from the core.  As an ongoing
project, it will retain the following properties:

  * relatively simple core, with new features implemented as libraries
    wherever possible;
  * easy interoperation with C code;
  * no features that impose mandatory performance penalties (e.g. GC);
    and
  * one language (no separate macro language).



## <a name="Setup"></a> 1.2 Setup

### External dependencies

  * LLVM (3.2-11.0)
  * libffi

### Supported systems

This should be usable on most Linux/macOS/BSD systems where LLVM is
able to be built.

### Building

From the 'dale' directory, for an out-of-tree (recommended) build:

        mkdir ../build
        cd ../build
        cmake ../dale
        make
        make tests
        make install
    
By default, the installation prefix is `/usr/local/`. The compiler
executable (`dalec`) is installed to `/usr/local/bin`, its manpage is
installed to `/usr/local/share/man`, and all libraries are installed
to `/usr/local/lib/dale`.

The tests are written using Perl. `IPC::Run` is the only non-core
dependency required for testing: it can be installed by running `cpan
IPC::Run`, or by way of the system package manager.

### Common build problems

#### FFI

cmake needs to know the location of the ffi.h header. If this isn't
found in a default location, then it has to be provided during the
third step: e.g.

        cmake ../dale -DFFI_INCLUDE_DIR=/usr/include/i386-linux-gnu/

#### C compiler

Internally, the compiler uses the system's C compiler (`cc`) for
assembly and linking. If the version of LLVM being used produces
assembly that can't be processed by the system's compiler, then errors
about 'unknown directives', problems with relocation and similar may
be seen.  The easiest way to fix this is to get the version of `clang`
that corresponds to the version of LLVM used for the build (per
`-DLLVM_CONFIG`) and set it to be used as part of the build via the
`-DCC` option.  For example, on current Debian (Buster):

    apt-get install llvm-7-dev clang-7
    cmake ../dale -DLLVM_CONFIG=/usr/bin/llvm-config-7 \
                  -DCC=/usr/bin/clang-7

### Hello world

Copy the following into a file called `hello-world.dt`:

        (import cstdio)
    
        (def main (fn extern-c int (void)
          (printf "Hello, world\n")))
    
Compile it and run it:

        $ dalec hello-world.dt
        $ ./a.out
        Hello, world

The compiler manpage has more detail on supported flags, but most
things are as per other languages' compilers (`-c`, `-o`, etc.).



## <a name="Types"></a> 1.3 Types

### Primitive types

#### Integrals

`int` is the principal integral type. It has the same size as the
`int` provided by the C compiler used to build Dale, so as to
facilitate interoperability with the system's C libraries. `uint` is
the corresponding unsigned integral type.

The explicitly-sized signed and unsigned integral types are as
follows:

  * `int8`/`uint8`
  * `int16`/`uint16`
  * `int32`/`uint32`
  * `int64`/`uint64`

Integral literals can be plain numbers (e.g. `123`) or hexadecimal
literals (e.g. `0xFF`). Negative numbers are prefixed with `-`.

#### Floating-point types

  * `float`
  * `double`
  * `long-double`

Floating-point literals must contain a decimal point (e.g. 1.0). As
per integral literals, negative numbers are prefixed with `-`.

#### Other types

The following types operate in the same way as the corresponding C
type:

  * `bool` (`bool`)
  * `char` (`char`)
  * `void` (`void`)
  * `size` (`size_t`)
  * `ptrdiff` (`ptrdiff_t`)
  * `intptr` (`uintptr_t`)

The `bool` literals are `true` and `false`.

`char` literals comprise a hash character, a backslash and the actual
character. The literal for the letter `e`, for example, is `#\e`. The
specially-handled character literals are `#\SPACE`, `#\TAB`,
`#\NEWLINE`, `#\CARRIAGE`, `#\NULL` and `#\EOF`.

### Pointers

The syntax for pointer types is `(p {type})`.

### Const

The syntax for const types is `(const {type})`. Const semantics are
the same as in C.

### Arrays

The type of an array of values is `(array-of {n} {type})`, where `{n}`
is a literal unsigned integer or an expression that evaluates to one.
A zero-sized array may be declared when the array is populated during
declaration: see [Variables](#Variables).

The instances where arrays decay into pointers on use are the same as
in C.

The `{n}` expression is evaluated in a new anonymous scope: if the
type is specified within a macro or a function, it will not have
access to the bindings defined in that macro or function.
Variable-length arrays are not supported, either.

### Structures

A structure is declared like so:

        (def {name}
          (struct {linkage} [({member1} {member2} ... {memberN})]))

where `{linkage}` is one of `intern`, `extern` and `opaque`, and each
`{member}` contains a field name and type. A simple `pair` structure
would look like so:

        (def int-pair
          (struct intern ((first int) (second int))))

`{linkage}` controls the visibility of a struct defined within a
module. `extern` structs are exportable and usable outside the
module, while `intern` structs are not. `opaque` structs are those
whose members are defined elsewhere:

        (def some-other-struct
          (struct opaque))

Struct literals may be used wherever a reference to a struct may be
used. They take the following form:

        ({name} ({value1} {value2} ... {valueN}))

where each `{value}` is a name-value pair, corresponding to a struct
member. In situations where the value will be cast implicitly, e.g. in
an explicitly-typed variable declaration, it is not necessary to
include the enclosing ({name} ...) form.

### Other types

Function pointers and anonymous functions are described in the
[Functions](#Functions) section.



## <a name="Variables"></a> 1.4 Variables

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

`{linkage}` is one of `intern`, `extern`, `extern-c` and `auto`.  An
`intern` variable is visible only within the scope of the file where
it appears.  An `extern` or `extern-c` variable is either a
definition, in which case it's available globally, or a declaration,
in which case there must be a definition of the variable somewhere
else.  An `auto` variable is only valid for procedure-scoped
variables, and is visible within its lexical scope.  The other
placeholders are self-explanatory.

`setf` is the core form for setting a variable:

        (setf {pointer-to-variable} {new-value})

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

Variables defined outside of procedures may be initialised using
arbitrary expressions, including expressions that cause side effects.
These expressions are evaluated at compile time, rather than runtime.
If such a variable has a value that must be determined at runtime,
then that has to be handled by way of a function call at runtime.

### Local type deduction

If a variable definition includes a `{value}`, the `{type}` may be set
to `\` (backslash), in which case the type of the `{value}` will be
used as the type of the variable. Literals are handled specially:
integers always take the type `int`, and floating-point literals
always take the type `float`.



## <a name="Functions"></a> 1.5 Functions

Functions are declared like so:

        (def {name} (fn {linkage}
                        [(attr {attr1} {attr2} ... {attrN})]
                        {return-type}
                        [({param1} {param2} ... {paramN})]
          {body}))
    
`{linkage}` is one of `intern`, `extern` and `extern-c`. As per C++,
symbols for `extern` and `intern` functions are 'mangled' based on the
function's parameter types, so as to support overloading. Symbols for
`extern-c` functions are not mangled.

Each `{attr}` is a function attribute type. There are currently two
attributes that can be set:

  * `inline` causes the function to be inlined in all call sites; and
  * `cto` (compile-time only) indicates that the function should be
    removed after compilation.

Each `{param}` is a name-type pair. The last `{param}` may also be the
string '...', which denotes a `varargs` function. A function that
takes no parameters should have the atom `void` as the sole element
in its parameter list.

A function that adds three integers looks like so:

        (def add-3-ints (fn intern int ((a int) (b int) (c int))
          (return (+ (+ a b) c))))

If the last expression in a function is being returned by the
function, it is not necessary to add an explicit `return`.

### Overloading

Functions may be overloaded on their parameter types:

        (def + (fn intern int ((a int) (b int) (c int))
          (+ (+ a b) c)))

        (def + (fn intern int ((a int) (b int) (c int) (d int))
          (+ (+ a b c) d)))

Return types are not taken into account for the purposes of
overloading.

### Varargs

Varargs functions are written in nearly the same way as in C. The
`va-list` type and associated functions are provided by the compiler.
    
        (def va-test (fn intern int ((args int) ...)
          (def arglist (var auto va-list))
          (va-start (cast (# arglist) (p void)))

          ; within a loop:

            (va-arg (# arglist) {va-arg-type})

          ; at the end of the function:

          (va-end (cast (# arglist) (p void)))
          0))

### Overriding core forms

Certain core forms may be overridden by user-level functions, namely
`@`, `#` and `$`. `setf` may also be 'overridden', in effect, by
defining functions named `setf-copy-init`, `setf-copy-assign`,
`setf-move-init`, and `setf-move-assign`.  These are discussed in more
detail in [Initialisers and destructors](#Initialisers and destructors).

The `core` core form may, in turn, be used to ignore overridden core
forms. For example:

        (core @ my-pointer-type)

will return the raw dereferenced value of `my-pointer-type`.

### Function pointers

A function pointer is declared like so:

        (def fp (var auto (p {function-type}) {address-of-function}))

For example:

        (def fp (var auto (p (fn int ((a int) (b int)))) (# + int int)))

When using the address-of (`#`) operator on a function, it is
necessary to include the function argument types so as to distinguish
between overloaded functions. However, if the function only has one
definition, and that definition has `extern-c` linkage, the argument
types may be omitted.

Local type deduction makes function pointer definitions much simpler:

        (def fp (var auto \ (# + int int)))

Function pointers may be used in place of normal functions, or by
using the `funcall` core form:

        (printf "%d\n" (fp 1 2))         ; prints "3\n"
        (printf "%d\n" (funcall fp 1 2)) ; also prints "3\n"

### Anonymous functions

Anonymous functions are declared like so:

        (fn {return-type} ({param1} {param2} ... {paramN}) {body})

The type of that expression is 'pointer to underlying function type':
it is not necessary to use the address-of (`#`) operator to take the
address of the newly-'constructed' function.

Note that these functions are not closures, and do not have access to
the surrounding environment.

### Reference parameters

Function parameters may be passed by reference. Within the body of the
function, the arguments for those parameters have a type of 'pointer
to referenced type'; a reference is not a separate type as such.  For
example:

        (def add-2-ints (fn intern int ((a (ref int)) (b (ref int)))
          (return (+ (@ a) (@ b)))))

Values passed by reference must be addressable, unless the reference
is to a constant type.

### Rvalue reference parameters

These operate in the same way as reference parameters, except that the
arguments must be rvalues, and the parameter is marked with `rv-ref`
instead of `ref`. An lvalue may be converted into an rvalue by using
the `move` core form.

### Retvals

A function's return type may be marked as `retval`. Such a function,
rather than returning a value using `return`, should write the result
to the binding `retval`, which is provided implicitly by the compiler.
That binding has the type 'pointer to actual return type', and its
value is used as the return value of the function. `retval` allows for
avoiding unnecessary allocations/copies. For example:

        (def add-2-ints (fn intern (retval int) ((a int) (b int))
          (setf retval (+ a b))
          (return)))

        ...

        (def n (var auto int (add-2-ints 5 10)))
        (printf "%d\n" n) ; prints "15\n"

(As with reference parameters, `int` is not a type for which this
would be used in practice.)

### invoke

A form that is not a procedure call can be treated as though it were
one by defining a function or macro named `invoke`, which is able to
accept the element (or elements) of that form as its arguments.  For
example:

        (def invoke (fn intern int ((a int) (b int))
          (+ a b)))

        ...

        (def n (var auto int 1))
        (printf "%d\n" (n 2)) ; prints "3\n"



## <a name="Control flow"></a> 1.6 Control flow

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



## <a name="Macros"></a> 1.7 Macros

Macros are declared like so:

        (def {name} 
          (macro {linkage} 
                 ({param1} {param2} ... {paramN})
            {body}))

`{linkage}` is one of `intern` and `extern`.

Each `{param}` is either a parameter name alone, or a name-type pair.
Macros may be overloaded in the same way as functions. However, the
overloading only affects dispatch: all of the macro arguments within
the body of the macro are pointers to `DNode`s, which are described
below.

To see the input form and output expansion of the macro calls on
compilation, pass the `--print-expansions` flag to `dalec`.

### `DNode`

The `DNode` struct definition, provided by the compiler, is as
follows:

        (def DNode
          (struct extern ((is-list   int)
                          (token-str (p char))
                          (list-node (p DNode))
                          (next-node (p DNode)))))

(There are actually many other members in this struct, but they should
generally only be used by standard libraries, and may be
removed/changed without notice.)

`is-list` will be set to a non-zero value for nodes that represent
lists. For such nodes, `list-node` will point to the first node of the
list, unless the list is empty, in which case `list-node` will be
null. If `is-list` is zero, then the node is a non-list node, and
`token-str` will contain the node's content. `next-node` is applicable
to both list and non-list nodes, and points to the next node after the
current node.

The return type of all macros is a pointer to a `DNode`. This value
will replace the call to the macro. If the value is a null pointer,
then the call to the macro will be removed (i.e. the call will be a
no-op).

### Macro context

The body of a macro has access to a macro context argument named 'mc'.
This argument is of type `(p MContext)`, where `MContext` is an opaque
struct type. Many internal macro functions and introspection functions
take the macro context as their initial argument.

Heap allocations within macros should be performed using the
`pool-malloc` function:

        (def pool-malloc
          (fn extern (p void) ((mc (p MContext)) (n size))))

Memory allocated by way of this function will be freed by the compiler
after it has finished evaluating the macro.

The number of arguments passed to the macro can be retrieved by way of
the `arg-count` function, which takes the context as its single
argument. This function is present so that varargs macros can be
supported without requiring macro users to also provide the number of
arguments/forms being passed to the macro. Varargs macros are defined
different from varargs functions: to mark a macro as varargs, add a
final parameter with the name `rest`.  Within the macro, `rest` will
be bound to a pointer to a DNode pointer, containing the rest of the
arguments that were passed to the macro.

### Examples

The identity macro is like so:

        (def identity (macro extern (N) N))

        ; ...

        (identity (+ 1 1)) ; becomes (+ 1 1)

As per functions, the last value evaluated within the body of the
macro is returned implicitly, in the absence of an explicit `return`
instruction.

The quote (`q`) core form allows for converting an arbitrary form into
a pointer to a `DNode` (static). For example:

        (def const-string (macro extern (void) (q (p (const char)))))

The standard library for dealing with macros is (unsurprisingly)
[`macros`](#macros).  It provides many functions that ease
macro authorship, as well as the quasiquotation (`qq`) macro, which is
one of the more useful macro-writing macros. For example, an `unless`
(opposite of `if`) macro would look like so, when using `qq`:

        (def unless (macro extern (expr tc fc)
          (qq if (not (uq expr)) (uq tc) (uq fc))))

`uq` is a form that 'unquotes' the specified `DNode`, when used within
a `qq` form. There is a corresponding `uql` form ('unquote-list'),
that takes the list from the specified `DNode` and 'splices' it into
place.

Note that `qq` 'wraps' its arguments, whereas `q` does not:

        (qq p (const char))  -> (p const char)
        (q (p (const char))) -> (p const char)

### Typed macros

A macro may specify one or more typed parameters, starting from the
beginning of its parameter list. Typed and untyped parameters may not
be interleaved. So, a macro definition like:

        (def unless (macro extern ((expr bool) tc fc)
          (qq if (not (uq expr)) (uq tc) (uq fc))))

will work properly, while:

        (def unless (macro extern (expr (tc int) (fc int))
          (qq if (not (uq expr)) (uq tc) (uq fc))))

will not work properly.

A macro may not have the same set of parameter types as a function of
the same name, and vice-versa, because it would make dispatch
ambiguous. However, if a macro's set of parameters only differs from
that of a function insofar as one or more of the macro's parameters is
untyped, then both may be defined.

For dispatch, the procedure to call is selected from the set of
procedures that can be called using the following logic:

   * non-varargs functions are preferred to varargs functions;
   * varargs functions are preferred to macros;
   * macros with fewer `(p DNode)` parameters are preferred to other
     macros;
   * if there are multiple macros with the same number of `(p DNode)`
     parameters, the one with the earliest typed parameter is
     preferred.

### Evaluation and side effects

For procedure dispatch, the type of each argument needs to be known,
so each argument needs to be fully evaluated.  Since an argument can
itself be a macro call, there may be instances where a macro call only
occurs because of the evaluation necessary for dispatch:

        (def mprint (macro intern (void)
          (printf "test\n")
          (nullptr DNode)))

        (def identity (macro intern (ignored returned)
          returned))

        (def main (fn extern-c int (void)
          (identity (mprint) (printf "hello\n"))))

During compilation of the above, the string "test" will be printed,
even though `(mprint)` isn't evaluated directly.  This is because
`(mprint)` is evaluated as part of determining dispatch for
`identity`.  (The `ignored` argument in the call to `identity` will
still be `(mprint)`, though: the evaluation is purely for dispatch,
and macros always receive unevaluated arguments.)

In addition to the previous consideration, macros in general may be
evaluated more than once per call site by the compiler.  As a result,
it is typically safest for macros to avoid side effects, though there
may be some contexts where that is acceptable.  One exception to this
is that any errors reported by the macro to the compiler, by way of
the `report-error` function (see
[`introspection`](#introspection)), will be cleared by the
compiler in the event that it is unable to evaluate or otherwise use
the macro.

### Functions used by macros

By default, macros will not be included in a compiled program, so the
fact that they reference symbols that are only available at
compile-time is not a problem. However, functions that are used
exclusively by macros, and similarly rely on compile-time-only
symbols, are not removed by the compiler before the final compilation
steps. It is necessary to mark such functions with the `cto`
attribute, so that the compiler removes them before any linking errors
occur.



## <a name="Initialisers and destructors"></a> 1.8 Initialisers and destructors

### `init`

An `init` function is defined like so:

        (def init (fn {linkage} bool ((val (ref {type})))
          {body}))

If a lexically-scoped variable of the specified type is defined within
a procedure without being initialised, this function will be run with
that variable as its argument.

If `init` is not defined over a type, then a variable of that type
that is not explicitly initialised will have indeterminate state.
This is also the case for variables defined outside of procedures,
even if `init` is defined over the type: in those instances, `init`
must be called explicitly for the variable at runtime.

### `setf-copy-init`

A `setf-copy-init` function is defined like so:

        (def setf-copy-init (fn {linkage} bool ((dst (p {type}))
                                                (src (p (const {type}))))
          {body}))

Such functions will be used in lieu of the default `setf` behaviour
whenever `dst` is uninitialised.  The source type may be any type:
this allows for a variable to be initialised from a value of a
different type.  Multiple bindings may be defined for the same
destination type.  On defining a variable, if the initial value has a
type for which there is a `setf-copy-init` binding with that type for
the source, then that binding will be used to initialise the variable.

`setf-copy-init` is also used when a value needs to be copied
implicitly, such as when a value is returned from a function.  If it
is not defined over a type, then implicit copy operations are
equivalent to a `memcpy` from source to destination.  Only the binding
which has `(p (const {type}))` as its source type will be used for
implicit copying.

### `setf-copy-assign`

`setf-copy-assign` functions are defined in the same manner as
`setf-copy-init` functions.  They are used whenever `dst` has already
been initialised.  As with `setf-copy-init`, the source type may be
any type, and multiple bindings may be defined for the same
destination type.

### `setf-move-init`

`setf-move-init` functions are defined in the same manner as
`setf-copy-init` functions, save that the `src` parameter must be an
rvalue reference parameter.  This constraint means that there is only
ever one `setf-move-init` binding for a given destination type.

Where possible, `setf-move-init` is used instead of `setf-copy-init`
for implicit operations that would otherwise have to be copies.

### `setf-move-assign`

`setf-move-assign` functions are defined in the same manner as
`setf-move-init` functions.  They are used whenever `dst` has already
been initialised.

If a type should only be moved, and not copied, declare a function
named `setf-copy-disabled` over the type. If that declaration exists,
any attempt to implicitly or explicitly copy a value of that type will
fail with the error message 'copying is disabled for this type'.

Multiple `setf-` functions, supporting different source types, may be
defined over a single destination type.

### `requires-explicit-init`

If a function named `requires-explicit-init` is defined over a type,
then any lexically-scoped variable of that type must be explicitly
initialised, regardless of whether an `init` function is also defined
over the type.

### `destroy`

Destructors are defined like so:

        (def destroy (fn {linkage} void ((val (ref {source-type})))
          {body}))

They are run when a value of the relevant type goes out of scope.

If `destroy` is not defined over a particular type, then any attempt
to call `destroy` on a value of that type will become a no-op.

### Variable declaration and initialisation

There are some additional aspects to variable initialisation that only
make sense once overridden initialise/copy/move functions have been
covered:

 - The initialising `{value}` form in a variable declaration has
   access to the (unintialised) variable by its name.
 - The result of evaluating `{value}` will be copied/moved into the
   variable, except when `{value}` is a procedure call named `init`,
   in which case it is assumed that `{value}` will handle initialising
   the variable.  An `init` procedure call used in this way may have
   an arbitrary number of arguments, unlike the `init` that is called
   implicitly when no `{value}` is provided.
 - An alternative to using an `init` procedure call is to define a
   `retval` function instead.  If such a call is used as the `{value}`
   of a variable declaration, then its `retval` will be the address of
   the variable, which also allows for avoiding unnecessary
   copies/moves.



## <a name="Namespaces"></a> 1.9 Namespaces

Namespaces are defined like so:

        (namespace {name}
          {form1}
          {form2}
          ...
          {formN})

A binding within a namespace may be referred to by its name alone
from within the namespace, or by its qualified name from outside the
namespace declaration. Qualified namespace references comprise the
names of the namespaces, from least to most-specific, separated by
periods:

        (namespace ns1
          (def value (var intern int 123))
          (def function (fn intern int (void) value)))

        (def function (fn intern int (void) 
          ns1.value)) ; returns 123

To qualify a reference to a binding that is in the root namespace,
where e.g. the current namespace also defines a binding with that
name, prefix the name with a single period.

Namespaces may be nested:

        (namespace ns1
          (namespace ns2
            (def value (var intern int 123))
            (def function (fn intern int (void) value))))

        (def function (fn intern int (void)
          ns1.ns2.value)) ; returns 123

A namespace will be used for lookup when it is one of the
currently-active namespaces (i.e. introduced by way of a `namespace`
declaration), or when it is explicitly introduced for lookup by way of
`using-namespace`:

        (namespace ns1
          (namespace ns2
            (def value (var intern int 123))
            (def function (fn intern int (void) value))))

        (using-namespace ns1.ns2
          (def function (fn intern int (void)
            value))) ; returns 123

Note that the `new-scope` core form, discussed previously in
[Variables](#Variables), introduces a new anonymous namespace
when used. This operates in the same way as an explicitly-named
namespace for the purposes of lookup. Furthermore, each procedure body
has an implicit `new-scope` as its first instruction, and each of the
parameter variables are defined implicitly within that anonymous
namespace.

The lookup rules for bindings generally are as follows:

  * check namespaces that are active by way of `using-namespace`,
    `namespace` and `new-scope`, in reverse order of
    introduction/definition; and
  * check the implicit top-level namespace.



## <a name="Modules"></a> 1.10 Modules

### Declaration

Modules allow for related functions, variables, structs and so forth
(bindings) to be grouped into single logical units.

Module declarations are like so:

        (module {name} [({attr1} {attr2} ... {attrN})])

Each `{attr}` is a module attribute type. The only valid module
attribute is `cto`, which is short for 'compile-time only'. If a
module is declared with this attribute, none of its bindings will be
available at runtime.

A given file may only define one module, and modules may not be spread
across multiple files (cf. namespaces).

Module files are compiled by using the compiler's `-c` flag. The
following files are created on successful compilation, where `{name}`
is the module name:

  * `lib{name}.dtm`
    * Contains metadata about the module's bindings (functions,
      variables, structs, etc.).
  
  * `lib{name}.bc`
    * The LLVM bitcode for the compiled module.
  
  * `lib{name}.so`
    * The dynamic library for the compiled module.
  
  * `lib{name}-nomacros.bc`, `lib{name}-nomacros.so`
    * As per `lib{name}.bc` and `lib{name}.so`, respectively, except
      that all of the macros have been removed.

Bindings within a module that have `intern` linkage will not be
available outside of the module.

### Use

To use a module:

        (import {module-name} [({binding1} {binding2} ... {bindingN})])

By default, all `extern`-scope bindings within a module, including
bindings imported by the module itself, will be made available within
the importing file. If `{binding}` instructions are included, only
those bindings will be imported. A `{binding}` instruction may
reference an entire namespace, in which case all bindings within that
namespace will be imported.

Linking is handled automatically when a module is imported. Whether a
particular module is linked statically or dynamically depends on the
arguments passed to the compiler at the command line. However, if the
module depends on a non-Dale external library, that library must be
linked against manually.

### Includes

The `include` core form is supported for completeness' sake. It
operates in the same way as an `#include` directive in C's
preprocessor.  It may be used both at the top level and within a
procedure.

To prevent a file from being included more than once, the core form
`once` can be used as the first form within the file. It takes an
identifier as its single argument. If a file is subsequently loaded by
way of `include`, and that file's first form is a call to `once` with
the same identifier, the file's remaining forms will be ignored.

### Lazy loading

Module compilation and use depends on the system permitting lazy
loading of dynamic libraries.  If the system doesn't permit this, then
compilation will fail when a module M1 depends on a module M2, and M2
depends on a non-Dale library that isn't passed in via the `-a` (make
library available at compile-time) option.

NixOS is an example of a system that does not permit lazy loading by
default, and is affected by this problem.  In the NixOS case, this can
be worked around by way of the `hardeningDisable=bindnow` option.  See
[mawled/issues/12](#mawled/issues/12) for an
example case.



## <a name="Form reference"></a> 1.11 Form reference

In addition to the core forms, there are a number of core macros,
functions, structs and variables provided by the compiler to each
program by default. This reference is limited to forms usable from
within procedures.

### Core forms

#### (`setf` {`destination`} {`source`})

Copies `source` to `destination`.

May be overridden by way of `setf-copy-init`, `setf-copy-assign`,
`setf-move-init`, and `setf-move-assign`.

#### (`@` {`pointer`})

Returns the value pointed to by `pointer`.

May be overridden.

#### (`#` {`value`})

Returns the address of `value`.

May be overridden.

#### (`$` {`array-value`} {`array-index`})

Returns a pointer to the specified array index in `array-value`.

May be overridden.

#### (`:` {`struct-value`} {`struct-member-name`})

Returns a pointer to the specified struct member in `struct-value`.

#### (`null` {`expression`})

Returns a boolean indicating whether `expression` evaluates to a null
pointer.

#### (`nullptr` {`type`})

Returns a null pointer to `type`.

#### (`cast` {`from-value`} {`to-type`})

Casts `from-value` to the specified type and returns it.

Depending on the types involved, casting may change the underlying
representation of the data.  The instances where casting may occur are
listed below, with instances that may change representation marked with an
asterisk:

 - where the source and target types are integer types (includes
   `bool`) ( * );
 - where the source and target types are floating-point types ( * );
 - where the source type is a floating-point type and the targer type
   is an integer type, or vice-versa ( * );
 - where the source and target types are pointer types;
 - where the source type is a non-const struct type and the target
   type is a const struct type; and
 - where the source type is a pointer type and the target type is an
   integer type, or vice-versa ( * );

Casts may also occur implicitly during the compilation process.  The
instances where this occurs are:

 - during struct literal parsing, but only when both the source and
   target types are integer types or floating-point types; and
 - during `extern-c` function processing, for each argument.

#### (`funcall` {`function-pointer`} {`arg1`} {`arg2`} ... {`argN`})

Executes `function-pointer` using the provided arguments, and returns
its result.

#### (`do` {`form1`} {`form2`} ... {`formN`})

Evaluates each form in order. Returns the result of evaluating the
final form.

#### (`new-scope` {`form1`} {`form2`} ... {`formN`})

Introduces a new lexical scope, and evaluates each form within that
scope. As per `do`, the result is that of the final form.

#### (`label` {`name`})

Marks a point in the code, for later use by `goto`.

#### (`goto` {`label-name`})

Branches unconditionally to the specified label and continues
execution.

#### (`if` {`condition`} {`true-case`} {`false-case`})

Executes `true-case` if `condition` is true; otherwise, executes
`false-case`.

#### (`return` {`value`})

Returns `value` from the current procedure to the caller.

#### (`move` {`value`})

Converts an lvalue into an rvalue.

#### (`va-start` {`pointer-to-va-list`})

Initialises the `va-list`, for later use with `va-arg`.

#### (`va-arg` {`pointer-to-va-list`} {`argument-type`})

Returns the next argument from the `va-list`, cast to the specified
type.

#### (`va-end` {`pointer-to-va-list`})

Performs any necessary cleanup required by way of the use of the
`va-list`. 

#### (`array-of` {`count`} {`type`} (`array` {`a1`} {`a2`} ... {`aN`}))

Constructs and returns a new array literal of the specified type.

#### (`sizeof` {`type`})

Returns the size (in bytes) of the specified type.

#### (`offsetof` {`struct-type-name`} {`struct-member-name`})

Returns the offset (in bytes) of the specified struct member within
the struct.

#### (`alignmentof` {`type`})

Returns the required alignment (in bytes) of the specified type.

#### (`p=` {`pointer`} {`pointer`})
#### (`p<` {`pointer`} {`pointer`})
#### (`p>` {`pointer`} {`pointer`})
#### (`p+` {`pointer`} {`pointer-or-integer`})
#### (`p-` {`pointer`} {`pointer-or-integer`})

Pointer comparison and modification forms. The first three return
`bool` values, and the remaining two return pointers that have the
type of the first argument.

#### (`using-namespace` {`name`} {`form1`} {`form2`} ... {`formN`})

Adds the specified namespace to the list of namespaces used for
lookup, and evaluates each of the forms in that context. As per `do`,
the result is that of the final form.

#### (`q` {`form`})

Converts the form into a pointer to `DNode` (static), and returns that
pointer.

#### Other

Variables and structs may be defined within procedures in the same way
as they are defined at the top-level.

### Core macros

These are indistinguishable from core forms from a user's perspective,
but are implemented internally as macros.

#### (`setv` {`destination`} {`source`})

Expands to (`setf` (`#` {`destination`}) {`source`}).

#### (`@$` {`array-value`} {`array-index`})

Expands to (`@` (`$` {`array-value`} {`array-index`})).

#### (`@:` {`struct-value`} {`struct-member-name`})

Expands to (`@` (`:` {`struct-value`} {`struct-member-name`})).

#### (`:@` {`struct-value`} {`struct-member-name`})

Expands to (`:` (`@` {`struct-value`}) {`struct-member-name`}).

#### (`@:@` {`struct-value-pointer`} {`struct-member-name`})

Expands to (`@` (`:` (`@` {`struct-value-pointer`}) {`struct-member-name`})).

### Functions

The following functions are provided for the numeric types:

  * `+`, `-`, `*`, `/`, `%`, `=`, `!=`, `<`, `<=`, `>`, `>=` (all)
  * `<<`, `>>`, `&`, `|`, `^` (integral types only)

They operate in the same way as normal functions, i.e. they can be
addressed, cf. the core forms and macros. The last five operations are
'left shift', 'right shift', 'bitwise and', 'bitwise or' and 'bitwise
xor', respectively.  `-` is defined for both one argument (unary
negation) and two (standard subtraction).

Each shift function takes an `int` value as its second argument, and
returns a value of the type of its first argument.

#### (`pool-malloc` {`pointer-to-mcontext`} {`size`})

Allocates and returns uninitialised memory, as per `malloc`. For use
within macros. The memory will be freed by the compiler after the
macro has been evaluated.

#### (`arg-count` {`pointer-to-mcontext`})

Returns the number of arguments passed to the macro.

#### (`not` {`bool-expression`})

If the argument evaluates to false, returns true, and vice-versa.

### Structs

#### `DNode`

        (def DNode
          (struct extern ((is-list   int)
                          (token-str (p char))
                          (list-node (p DNode))
                          (next-node (p DNode)))))

See [Macros](#Macros).

#### `va-list` (opaque)

See [Functions](#Functions) for more information about this
struct.

### Variables

The following variables, defined by default, have the same meaning as
their analogues in C.

  * JMP_BUF_SIZE
  * FPOS_T
  * TIME_T
  * CLOCK_T
  * SIZEOF_LONG
  * SIZEOF_LONG_LONG
  * FLT_RADIX
  * FLT_MANT_DIG
  * FLT_DIG
  * FLT_EPSILON
  * FLT_ROUNDS
  * FLT_MIN_EXP
  * FLT_MAX_EXP
  * FLT_MIN
  * FLT_MAX
  * DBL_MANT_DIG
  * DBL_DIG
  * DBL_EPSILON
  * DBL_MIN_EXP
  * DBL_MAX_EXP
  * DBL_MIN
  * DBL_MAX
  * LDBL_MANT_DIG
  * LDBL_DIG
  * LDBL_EPSILON
  * LDBL_MIN_EXP
  * LDBL_MAX_EXP
  * LDBL_MIN
  * LDBL_MAX
  * HUGE_VAL
  * HUGE_VALF
  * HUGE_VALL
  * L_tmpnam
  * TMP_MAX
  * FILENAME_MAX
  * FOPEN_MAX
  * RAND_MAX
  * EXIT_FAILURE
  * EXIT_SUCCESS



## <a name="REPL"></a> 1.12 REPL

`daleci` is the REPL executable.  It works like so:

 - Top-level expressions, like variable and procedure definitions and
   module import statements, are evaluated in the same way as for a
   compiled program.
 - Each other expression is evaluated as if it were run within a new
   anonymous function.  The result of the expression is bound to the
   variable '\_', unless the expression has a type of `void`, in which
   case no binding occurs.
 - On submitting a non-top-level expression, the type of the
   expression is printed to standard output, followed by the output
   (if any) of executing the expression.
 - No modules are imported by default.  The only bindings that are
   available are those provided implicitly to all programs, such as
   the base set of arithmetical and relational operators.

An example session:

    > (+ 1 2)
    int
    > (+ 3 _)
    int
    > (import cstdio)
    > (printf "%d\n" _)
    int
    6
    > (+ 1.0 _)
    /dev/stdin:7:1: error: overloaded function/macro not in scope: '+'
    (parameters are float int, closest candidate expects (const float)
    (const float))
    > (+ 1.0 2.0)
    float

The REPL does not provide any readline/editline-like functionality, so
using something like `rlwrap` is recommended.



## <a name="Tools"></a> 1.13 Tools

### [dale-autowrap](#dale-autowrap)

Converts [c2ffi](#c2ffi) output files into Dale
program files, allowing for automatically generating bindings for C
libraries.

## <a name="introspection"></a> 2.1 introspection

### Details

Module: introspection

### Description

Exports declarations for various functions provided by the compiler.
These are only available at compile time, i.e. within macros. Almost
all of them are interrogative in nature, the two exceptions being
`register-type` and `report-error`. For the most part, the functions
that take `(p DNode)` arguments will expand macros in those arguments
before performing their core task.



### Functions

#### `exists-fn`

Linkage: `extern-c`
Returns: `bool`
Parameters:

  * `(mc (p MContext))`: An MContext.
  * `(frm (p DNode))`: The form describing the function.


Determine whether the specified function exists. The `frm` argument
should be a list node comprising the return type, the function name
and the argument types: e.g. `(bool < int int)`.


#### `exists-variable`

Linkage: `extern-c`
Returns: `bool`
Parameters:

  * `(mc (p MContext))`: An MContext.
  * `(frm (p DNode))`: The token node containing the variable name.


Determine whether the specified variable exists. The `frm` argument
should be a token node containing the name of the variable.


#### `exists-type`

Linkage: `extern-c`
Returns: `bool`
Parameters:

  * `(mc (p MContext))`: An MContext.
  * `(frm (p DNode))`: The form describing the type.


Determine whether the specified type exists.


#### `exists-macro`

Linkage: `extern-c`
Returns: `bool`
Parameters:

  * `(mc (p MContext))`: An MContext.
  * `(frm (p DNode))`: The form describing the macro.


Determine whether the specified macro exists. The `frm` argument
should be a list node comprising the macro name and the argument
types. It is not necessary to provide the exact types specified by the
macro for its arguments; for example, if a macro `testmacro` takes
three untyped arguments, then a call to `exists-macro` with a form
like `(testmacro int bool int)` will return `true`.


#### `type-of`

Linkage: `extern-c`
Returns: `(p DNode)`
Parameters:

  * `(mc (p MContext))`: An MContext.
  * `(frm (p DNode))`: The form for which the type should be determined.
  * `(report-errors bool)`: Whether to report or suppress errors.


Evaluates the form and returns its type. For example, if `frm` is `(+
1 1)`, the result node will be `int` (single token node).


#### `codomain`

Linkage: `extern-c`
Returns: `(p DNode)`
Parameters:

  * `(mc (p MContext))`: An MContext.
  * `(frm (p DNode))`: The form describing the function.


Returns the codomain (the return type) of the specified function. The
argument form has the same structure as for `exists-fn`, except that
there is no return type at the beginning. Returns a null pointer if
the function doesn't exist.


#### `arity`

Linkage: `extern-c`
Returns: `int`
Parameters:

  * `(mc (p MContext))`: An MContext.
  * `(frm (p DNode))`: The token node containing the function name.


Returns the arity of the specified function (the number of parameters
that it requires). Only works for functions with `extern-c` linkage.
The `frm` argument should contain only the name of the function.


#### `input-type`

Linkage: `extern-c`
Returns: `(p DNode)`
Parameters:

  * `(mc (p MContext))`: An MContext.
  * `(frm (p DNode))`: The token node containing the function name.
  * `(n int)`: The parameter index (begins from 0).


Returns the type of the specified function's `n`th parameter. Only
works for functions with `extern-c` linkage. As per `arity`, the `frm`
argument should contain only the name of the function.


#### `struct-member-count`

Linkage: `extern-c`
Returns: `int`
Parameters:

  * `(mc (p MContext))`: An MContext.
  * `(st (p DNode))`: The token node containing the struct name.


Returns the number of members present in the specified struct.


#### `struct-member-type`

Linkage: `extern-c`
Returns: `(p DNode)`
Parameters:

  * `(mc (p MContext))`: An MContext.
  * `(st (p DNode))`: The token node containing the struct name.
  * `(n int)`: The member index (begins from 0).


Returns the type of the specified struct's `n`th member.


#### `struct-member-name`

Linkage: `extern-c`
Returns: `(p char)`
Parameters:

  * `(mc (p MContext))`: An MContext.
  * `(st (p DNode))`: The token node containing the struct name.
  * `(n int)`: The member index (begins from 0).


Returns the name of the specified struct's `n`th member.


#### `report-error`

Linkage: `extern-c`
Returns: `void`
Parameters:

  * `(mc (p MContext))`: An MContext.
  * `(frm (p DNode))`: The node that 'caused' the error.
  * `(err (p (const char)))`: The error string.


Adds an error message to the compiler's internal error buffer. The
node provided to this function will be used to set the line/column
numbers and the filename in the error message.


#### `register-type`

Linkage: `extern-c`
Returns: `bool`
Parameters:

  * `(mc (p MContext))`: An MContext.
  * `(from (p char))`: The concrete type, as a string.
  * `(to (p char))`: The display name for the type, as a string.


Registers a name (the 'display name') against a concrete type. Such
registrations are used by the compiler for things like error messages,
so this function is primarily useful for macros that create generic
types. In the case of `(Pair int int)`, for example, the concrete type
is a struct with a name like `pairii`, but that type is registered
against the form `(Pair int int)`, so error messages concerning that
type use the latter form.


#### `type-to-display-string`

Linkage: `extern-c`
Returns: `bool`
Parameters:

  * `(mc (p MContext))`: An MContext.
  * `(frm (p DNode))`: The type form.
  * `(buf (p char))`: A buffer into which the display string will be written.


Writes the specified type's display string to `buf`. This string is
either the display name provided by way of `register-type`, or the
stringification of the provided form.


#### `type-to-string`

Linkage: `extern-c`
Returns: `bool`
Parameters:

  * `(mc (p MContext))`: An MContext.
  * `(frm (p DNode))`: The concrete type, as a form.
  * `(buf (p char))`: A buffer into which the encoded type name will be written.


Writes the specified type's concrete type string to `buf`. For
example, `(Pair int int)` has a concrete type of `pairii`, so passing
the former as the `frm` will cause the latter to be written to `buf`.


#### `is-char-type`

Linkage: `extern-c`
Returns: `bool`
Parameters:

  * `(mc (p MContext))`: An MContext.
  * `(frm (p DNode))`: The type form.


Returns a boolean indicating whether the specified type is a character
type.


#### `is-integer-type`

Linkage: `extern-c`
Returns: `bool`
Parameters:

  * `(mc (p MContext))`: An MContext.
  * `(frm (p DNode))`: The type form.


Returns a boolean indicating whether the specified type is an integer
type (either signed or unsigned).


#### `is-signed-integer-type`

Linkage: `extern-c`
Returns: `bool`
Parameters:

  * `(mc (p MContext))`: An MContext.
  * `(frm (p DNode))`: The type form.


Returns a boolean indicating whether the specified type is a signed
integer type.


#### `is-unsigned-integer-type`

Linkage: `extern-c`
Returns: `bool`
Parameters:

  * `(mc (p MContext))`: An MContext.
  * `(frm (p DNode))`: The type form.


Returns a boolean indicating whether the specified type is an unsigned
integer type.


#### `is-floating-point-type`

Linkage: `extern-c`
Returns: `bool`
Parameters:

  * `(mc (p MContext))`: An MContext.
  * `(frm (p DNode))`: The type form.


Returns a boolean indicating whether the specified type is a
floating-point type.


#### `is-pointer-type`

Linkage: `extern-c`
Returns: `bool`
Parameters:

  * `(mc (p MContext))`: An MContext.
  * `(frm (p DNode))`: The type form.


Returns a boolean indicating whether the specified type is a
pointer type.


#### `is-pointer-to-type`

Linkage: `extern-c`
Returns: `bool`
Parameters:

  * `(mc (p MContext))`: An MContext.
  * `(ptr-type (p DNode))`: The pointer type form.
  * `(pte-type (p DNode))`: The pointee type form.


Returns a boolean indicating whether the first type form is a pointer
to the type represented by the second form.


#### `pointee-type`

Linkage: `extern-c`
Returns: `(p DNode)`
Parameters:

  * `(mc (p MContext))`: An MContext.
  * `(ptr-type (p DNode))`: The pointer type form.


Returns the type to which the type form points.


#### `is-const`

Linkage: `extern-c`
Returns: `bool`
Parameters:

  * `(mc (p MContext))`: An MContext.
  * `(frm (p DNode))`: The type form.


Returns a boolean indicating whether the type is a `const` type.


#### `fn-by-args-count`

Linkage: `extern-c`
Returns: `int`
Parameters:

  * `(mc (p MContext))`: An MContext.
  * `(frm (p DNode))`: A list of parameter types.
  * `(prefix (p (const char)))`: An optional function name prefix.


Takes a form (list node) of parameter types and a function prefix
string (may be null). Returns the number of functions that have those
parameter types as their own. If the function prefix string is
provided, then only functions that begin with that prefix will be
taken into account. Calling this function initialises an internal list
containing the names of the functions with the specified parameter
types, which list is used by `fn-by-args-name`.


#### `fn-by-args-name`

Linkage: `extern-c`
Returns: `(p (const char))`
Parameters:

  * `(mc (p MContext))`: An MContext.
  * `(frm (p DNode))`: A list of parameter types.
  * `(n int)`: The function list index.


Takes a form (list node) of parameter types and an index. Returns the
name of the `n`th function that has those parameters types as its own.
The call to this function must be preceded, at some point, by a call
to `fn-by-args-count` that uses the same set of parameter types.


#### `fn-by-name-count`

Linkage: `extern-c`
Returns: `int`
Parameters:

  * `(mc (p MContext))`: An MContext.
  * `(name (p (const char)))`: A function name.


Takes a function name as its single argument.  Returns the number of
functions with that name that are currently in scope.  Calling this
function initialises an internal list containing the instances of the
specified function, which list is used by `fn-by-name-types-count` and
`fn-by-name-types-type`.


#### `fn-by-name-types-count`

Linkage: `extern-c`
Returns: `int`
Parameters:

  * `(mc (p MContext))`: An MContext.
  * `(name (p (const char)))`: A function name.
  * `(n int)`: The function list index.


Takes a function name and an index as its arguments.  Returns the
parameter count of the function at this index of the generated
list.


#### `fn-by-name-types-type`

Linkage: `extern-c`
Returns: `(p DNode)`
Parameters:

  * `(mc (p MContext))`: An MContext.
  * `(name (p (const char)))`: A function name.
  * `(n int)`: The function list index.
  * `(m int)`: The parameter list index.


Takes a function name, a function index, and a parameter index as its
arguments.  Returns the type of the parameter at this index of the
relevant function.


#### `has-errors`

Linkage: `extern-c`
Returns: `bool`
Parameters:

  * `(mc (p MContext))`: An MContext.
  * `(frm (p DNode))`: The form to be evaluated.


Evaluates the provided form and returns a boolean indicating whether
any errors were encountered.


#### `types-equal`

Linkage: `extern-c`
Returns: `bool`
Parameters:

  * `(mc (p MContext))`: An MContext.
  * `(type-1 (p DNode))`: The first type form.
  * `(type-2 (p DNode))`: The second type form.


Returns a boolean indicating whether the two type forms are equal.


#### `printf-length`

Linkage: `extern-c`
Returns: `(const (p char))`
Parameters:

  * `(mc (p MContext))`: An MContext.
  * `(frm (p DNode))`: The type of the printf argument.


Returns a 'length' string that can be put between the '%' and type
specifier in a printf formatter string, based on the size of the
provided type. For example, if the argument form is a token node
containing the string "size", then then returned string will be "z".


#### `is-copy-permitted`

Linkage: `extern-c`
Returns: `bool`
Parameters:

  * `(mc (p MContext))`: An MContext.
  * `(frm (p DNode))`: The type form.
  * `(report bool)`: Whether to report an error on copy not being permitted.


Returns a boolean indicating whether values of this type can be
copied.


#### `eval-expression`

Linkage: `extern-c`
Returns: `bool`
Parameters:

  * `(mc (p MContext))`: An MContext.
  * `(type (p DNode))`: The type form.
  * `(frm (p DNode))`: The literal form.
  * `(buf (p void))`: The buffer for the literal's memory.


Attempts to evaluate the provided form as a value of the specified
type, and writes the memory of the literal into the buffer.  If the
form does not evaluate successfully, returns false.  If the type is
not supported, reports an error and returns false.

Currently, the only supported type is `int`.


#### `eval-macro-call`

Linkage: `extern-c`
Returns: `(p DNode)`
Parameters:

  * `(mc (p MContext))`: An MContext.
  * `(frm (p DNode))`: The form.
  * `(mandatory bool)`
  * `(once bool)`


Attempts to evaluate the provided form as a macro call, and returns
the resulting form.  This function corresponds to Common Lisp's
`macroexpand` and `macroexpand-1` functions.

If `mandatory` is true, then any errors related to the attempted
expansion will be retained, and null will be returned.  If it is
false, then any errors will be suppressed, and the original node will
be returned.

If `once` is true, then only a single macro expansion will occur, if
applicable.  If it is false, then macro expansion will continue until
a form that is not a macro call is returned.


#### `is-lvalue`

Linkage: `extern-c`
Returns: `bool`
Parameters:

  * `(mc (p MContext))`: An MContext.
  * `(frm (p DNode))`: The form.


Determine whether a form is an lvalue.


## <a name="ctype"></a> 2.2 ctype

### Details

Module: ctype

### Description

Wrappers for the `cctype` functions. These differ insofar as they
accept `char` arguments, and return either `bool` or `char` results,
as appropriate. They are also located within the `std` namespace, as
opposed to the root namespace.



### Functions

#### `std.isalnum`

Linkage: `extern`
Returns: `bool`
Parameters:

  * `(c char)`: A character.


Determine whether a character is alphanumeric.


#### `std.isalpha`

Linkage: `extern`
Returns: `bool`
Parameters:

  * `(c char)`: A character.


Determine whether a character is alphabetical.


#### `std.iscntrl`

Linkage: `extern`
Returns: `bool`
Parameters:

  * `(c char)`: A character.


Determine whether a character is a control character.


#### `std.isdigit`

Linkage: `extern`
Returns: `bool`
Parameters:

  * `(c char)`: A character.


Determine whether a character is a digit.


#### `std.isgraph`

Linkage: `extern`
Returns: `bool`
Parameters:

  * `(c char)`: A character.


Determine whether a character is printable (excludes space).


#### `std.islower`

Linkage: `extern`
Returns: `bool`
Parameters:

  * `(c char)`: A character.


Determine whether a character is lowercase.


#### `std.isprint`

Linkage: `extern`
Returns: `bool`
Parameters:

  * `(c char)`: A character.


Determine whether a character is printable (includes space).


#### `std.ispunct`

Linkage: `extern`
Returns: `bool`
Parameters:

  * `(c char)`: A character.


Determine whether a character is a punctuation mark.


#### `std.isspace`

Linkage: `extern`
Returns: `bool`
Parameters:

  * `(c char)`: A character.


Determine whether a character is a whitespace character.


#### `std.isupper`

Linkage: `extern`
Returns: `bool`
Parameters:

  * `(c char)`: A character.


Determine whether a character is an uppercase character.


#### `std.isxdigit`

Linkage: `extern`
Returns: `bool`
Parameters:

  * `(c char)`: A character.


Determine whether a character is a hexadecimal digit.


#### `std.tolower`

Linkage: `extern`
Returns: `char`
Parameters:

  * `(c char)`: A character.


Convert a character into a lowercase character, if possible.


#### `std.toupper`

Linkage: `extern`
Returns: `char`
Parameters:

  * `(c char)`: A character.


Convert a character into an uppercase character, if possible.


## <a name="math"></a> 2.3 math

### Details

Module: math

### Description

Defines overloaded functions corresponding generally with those from
`cmath`. Each is in the `std` namespace.

  * Over signed and unsigned integers:
    * `mod` (same function as `%`).

  * Over signed integers only:
    * `abs`; and
    * `div`.

  * Over floating-point types:
    * `cos`, `sin`, `tan`;
    * `cosh`, `sinh`, `tanh`;
    * `acos`, `asin`, `atan`, `atan2`;
    * `sqrt`;
    * `exp`;
    * `log`, `log10`;
    * `ceil`, `floor`;
    * `fabs`;
    * `frexp`; and
    * `modf`, `fmod`.

A macro constant for `e` is also provided.

## <a name="macros"></a> 2.4 macros

### Details

Module: macros

### Description

Provides the core macro development functions: making, copying and
printing nodes, linking nodes together, gensym functions for
variables and labels, and the quasiquotation form.



### Functions

#### `std.macros.make-node`

Linkage: `extern`
Returns: `(p DNode)`
Parameters:

  * `(mc (p MContext))`: An MContext.


Returns a newly-allocated node.


#### `std.macros.is-empty-node`

Linkage: `extern`
Returns: `bool`
Parameters:

  * `(form (p DNode))`: The node.


Returns a boolean indicating whether the node is an empty node (no
list and no token).


#### `std.macros.make-empty-list-node`

Linkage: `extern`
Returns: `(p DNode)`
Parameters:

  * `(mc (p MContext))`: An MContext.


Returns a newly-allocated empty list node.


#### `std.macros.is-non-empty-list`

Linkage: `extern`
Returns: `bool`
Parameters:

  * `(form (p DNode))`: The node.



#### `std.macros.copy`

Linkage: `extern`
Returns: `(p DNode)`
Parameters:

  * `(mc (p MContext))`: An MContext.
  * `(follow bool)`: Whether to include the nodes that follow `form`.
  * `(form (p DNode))`: The node to copy.



#### `std.macros.copy-qstr`

Linkage: `extern`
Returns: `(p DNode)`
Parameters:

  * `(mc (p MContext))`: An MContext.
  * `(form (p DNode))`: The node to copy.
  * `(follow bool)`: Whether to include the nodes that follow `form`.


As per `copy`, except that if the first node is a token, it will have
double-quotations marks added at the start and end of it in the copied
node.


#### `std.macros.copy-to`

Linkage: `extern`
Returns: `void`
Parameters:

  * `(dst (p DNode))`: The destination node.
  * `(src (p DNode))`: The source node.


Copy (shallow) the details of the source node to the destination node.


#### `std.macros.print`

Linkage: `extern`
Returns: `bool`
Parameters:

  * `(form (p DNode))`: The node to print.


Prints the node to `stdout`.


#### `std.macros.sprint`

Linkage: `extern`
Returns: `bool`
Parameters:

  * `(buf (p char))`: The buffer to which the node will be printed.
  * `(form (p DNode))`: The node to print.


Prints the node to the provided buffer.


#### `std.macros.get-last-node`

Linkage: `extern`
Returns: `(p DNode)`
Parameters:

  * `(form (p DNode))`: The node for which the last node should be found.


Returns the last node in the list, by iterating over `next-node`
(i.e. this does not descend into the `list-node` of the argument
node).


#### `std.macros.mnfv`

Linkage: `extern`
Returns: `(p DNode)`
Parameters:

  * `(mc (p MContext))`: An MContext.
  * `(token-string (p (const char)))`: The token string for the new node.


Short for 'make-node-from-value'. There are several implementations of
this function: each is similar to `make-node`, except that each takes
an additional argument, which is used to populate the `token-str` of
the newly-allocated node. This implementation takes a `(p char)`,
copies it, and sets it in the new node.


#### `std.macros.mnfv`

Linkage: `extern`
Returns: `(p DNode)`
Parameters:

  * `(mc (p MContext))`: An MContext.
  * `(n int)`: An integer.



#### `std.macros.mnfv`

Linkage: `extern`
Returns: `(p DNode)`
Parameters:

  * `(mc (p MContext))`: An MContext.
  * `(n uint64)`: A uint64 integer.



#### `std.macros.mnfv`

Linkage: `extern`
Returns: `(p DNode)`
Parameters:

  * `(mc (p MContext))`: An MContext.
  * `(n int64)`: A int64 integer.



#### `std.macros.mnfv`

Linkage: `extern`
Returns: `(p DNode)`
Parameters:

  * `(mc (p MContext))`: An MContext.
  * `(f float)`: A float.



#### `std.macros.mnfv`

Linkage: `extern`
Returns: `(p DNode)`
Parameters:

  * `(mc (p MContext))`: An MContext.
  * `(d double)`: A double.



#### `std.macros.mnfv`

Linkage: `extern`
Returns: `(p DNode)`
Parameters:

  * `(mc (p MContext))`: An MContext.
  * `(ld long-double)`: A long double.



#### `std.macros.mnfv-wp`

Linkage: `extern`
Returns: `(p DNode)`
Parameters:

  * `(mc (p MContext))`: An MContext.
  * `(token-string (p (const char)))`: A string.
  * `(begin-line int)`: The beginning line number for the node.
  * `(begin-column int)`: The beginning column number for the node.
  * `(end-line int)`: The ending line number for the node.
  * `(end-column int)`: The ending column number for the node.
  * `(macro-begin-line int)`: The beginning macro line number.
  * `(macro-begin-column int)`: The beginning macro column number.
  * `(macro-end-line int)`: The ending macro line number.
  * `(macro-end-column int)`: The ending macro column number.


Short for 'make-node-from-value-with-position'. Takes additional node
position arguments, and sets them accordingly on the new node. Only
implemented for `(p char)` values.


#### `std.macros.link-nodes`

Linkage: `extern`
Returns: `(p DNode)`
Parameters:

  * `(argcount int)`: The number of nodes being provided.
  * `...`


Links the provided nodes together, without copying them, and returns
the first provided node. This is a varargs function. Note that the
`next-node` member of the last node is not nulled.


#### `std.macros.link-nodes-list`

Linkage: `extern`
Returns: `(p DNode)`
Parameters:

  * `(mc (p MContext))`: An MContext.
  * `(argcount int)`: The number of varargs being provided.
  * `...`


As per `link-nodes`, except that an additional list node is allocated
and returned. This list node points to the first provided node as its
`list-node`.


#### `std.macros.link-nodes-list-wp`

Linkage: `extern`
Returns: `(p DNode)`
Parameters:

  * `(mc (p MContext))`: An MContext.
  * `(argcount int)`: The number of varargs being provided.
  * `(begin-line int)`: The beginning line number for the node.
  * `(begin-column int)`: The beginning column number for the node.
  * `(end-line int)`: The ending line number for the node.
  * `(end-column int)`: The ending column number for the node.
  * `(macro-begin-line int)`: The beginning macro line number.
  * `(macro-begin-column int)`: The beginning macro column number.
  * `(macro-end-line int)`: The ending macro line number.
  * `(macro-end-column int)`: The ending macro column number.
  * `...`


As per `link-nodes-list`, except it also accepts additional node
position arguments, and sets them accordingly on the newly-allocated
list node.


#### `std.macros.link-nodes-array`

Linkage: `extern`
Returns: `(p DNode)`
Parameters:

  * `(argcount int)`: The number of arguments in the array.
  * `(arg-array (p (p DNode)))`: The argument array.


As per `link-nodes`, except that instead of being a varargs function,
it takes an array of nodes as its second argument.


#### `std.macros.gensym-var`

Linkage: `extern`
Returns: `bool`
Parameters:

  * `(buf (p char))`: The buffer for the variable name.


Prints a new, unused variable name to the provided buffer.


#### `std.macros.gensym-label`

Linkage: `extern`
Returns: `bool`
Parameters:

  * `(buf (p char))`: The buffer for the label name.
  * `(prefix (p (const char)))`: The prefix for the label name.


Prints a new, unused label name to the provided buffer. The prefix is
included in the new label name, so that it is a little easier to
determine what's happening when errors occur in the generated code.


#### `std.macros.make-gensym-label-node`

Linkage: `extern`
Returns: `(p DNode)`
Parameters:

  * `(mc (p MContext))`: An MContext.
  * `(prefix (p (const char)))`: The prefix for the label name.


Generates a new label name, constructs a token node to suit and
returns that node.


#### `std.macros.make-gensym-label-node`

Linkage: `extern`
Returns: `(p DNode)`
Parameters:

  * `(mc (p MContext))`: An MContext.


As per the previous implementation, except that no prefix is required
(it is set to the empty string).


#### `std.macros.is-gensym-label`

Linkage: `extern`
Returns: `bool`
Parameters:

  * `(label-node (p DNode))`: The label node.
  * `(prefix (p (const char)))`: The prefix for which to check in the label node.


Determines whether the provided label node is a gensym label with the
specified prefix.


#### `std.macros.make-gensym-var-node`

Linkage: `extern`
Returns: `(p DNode)`
Parameters:

  * `(mc (p MContext))`: An MContext.


Generates a new variable name, constructs a token node to suit and
returns that node.


#### `std.macros.walk-nodes`

Linkage: `extern`
Returns: `bool`
Parameters:

  * `(form (p DNode))`: The node to walk.
  * `(mc (p MContext))`: An MContext.
  * `(data (p void))`: Arbitrary data.
  * `(fn (p (fn int ((form (p DNode)) (mc (p MContext)) (data (p void))))))`: The function pointer to call on each node.


'Walks' through a node, recursively, calling the provided function
pointer on each node. The provided `data` argument is passed to the
function pointer on each call.


#### `std.macros.list-count`

Linkage: `extern`
Returns: `int`
Parameters:

  * `(form (p DNode))`: A list node.


Takes a list node, and counts the number of nodes that are within that
list. This is not recursive: it just counts the top-level nodes from
the list.


#### `std.macros.make-node-maker`

Linkage: `extern`
Returns: `(p DNode)`
Parameters:

  * `(mc (p MContext))`: An MContext.
  * `(form (p DNode))`: A node.


Constructs a node that, when evaluated, constructs the provided node.
For example, if the node is a simple token, then the returned node
will be `(mnfv mc token-str)`, where `token-str` is the token
from the provided node.


#### `std.macros.bqq-helper`

Linkage: `intern`
Returns: `(p DNode)`
Parameters:

  * `(frm (p DNode))`: A node.
  * `(mc (p MContext))`: An MContext.
  * `(arg-count int)`: The number of elements in the node.


A helper function for `bqq` (bootstrap-qq).


#### `std.macros.map-nodes`

Linkage: `(attr cto)`
Returns: `extern`
Parameters:

  * `p`: An MContext.
  * `DNode`: The node to map.


Similar to `walk-nodes`, except that it does not descend into list
nodes, and the provided function returns a node (possibly different
from the argument node), rather than mutating the argument node in
place.




### Macros

#### `std.macros.bqq`

Linkage: `intern`
Parameters:

  * `frm`: A node.
  * `rest`


The bootstrap quasiquotation macro. The general-use quasiquotation
macro, `qq`, is in the `macros` module. The forms handled specially
are `uq` (unquote), `uq-nc` (unquote-no-copy), `uql` (unquote-list)
and `uql-nc` (unquote-list-no-copy). The no-copy versions of these
forms should only be used when the nodes being unquoted will not be
used again.


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


#### `qq`

Linkage: `extern`
Parameters:

  * `frm`
  * `rest`


The 'general-use' quasiquotation macro. The forms handled specially
are:

  * `uq` (unquote): expands to the argument node, excluding any
    following nodes (i.e. excluding `next-node`);
  * `uql` (unquote-list): expands to the argument node, including all
    following nodes (i.e. including `next-node` and any subsequent
    `next-node`s of that node);
  * `uq-nc` (unquote no-copy): as per `uq`, except that the argument
    node is not copied on substitution; and
  * `uql-nc` (unquote-list no-copy): as per `uql`, except that the nodes
    are not copied on substitution.

Quasiquotation forms may be nested: each specially-handled form in a
nested `qq` must be wrapped with an additional `uq` for each level of
nesting.

The `-nc` versions should only be used when the argument node will not
be used again.

Previously, argument nodes for the various special forms had to be
DNode pointer variable names.  Arbitrary forms as argument nodes are
now supported.  For example, `(qq identity (uq (mnfv mc 1)))`
previously expanded to `(identity (mnfv mc 1))`, and now expands to
`(identity 1)`.


#### `std.macros.get-varargs-list`

Linkage: `extern`
Parameters:

  * `p`
  * `DNode`


Takes a macro context, an argument count, and a pointer to a DNode
pointer as its arguments.  Returns the first node of a linked node
list containing each of the nodes from the DNode pointer list,
suitable for using as the argument to the `uql` and `uql-nc` forms.
(The third argument here is typically the macro's 'rest' argument.
The difference between 'rest' and the return value of this function is
that the nodes here will be linked together.)


#### `std.macros.get-varargs-list`

Linkage: `extern`
Parameters:

  * `(count int)`


Simple version of `get-varargs-list`, only takes the count as parameter.


## <a name="stdlib"></a> 2.5 stdlib

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


## <a name="assert"></a> 2.6 assert

### Details

Module: assert

### Description

Macros for runtime assertions. User-defined compile-time errors can be
reported to the compiler by way of `report-error`, which is defined in
the `introspection` module.



### Macros

#### `std.disable-assertions`

Linkage: `extern`
Parameters: N/A

If called, subsequent calls to `assert` will expand to a no-op. Note
that assertions are enabled by default.


#### `std.enable-assertions`

Linkage: `extern`
Parameters: N/A

If called, subsequent calls to `assert` will expand to actual
assertions.


#### `std.assert`

Linkage: `extern`
Parameters:

  * `condition`: The condition expression.


Expands to a form that tests the condition; if the condition is false,
then an error message is printed to `stderr` and execution is aborted
(via `abort`). The error message will include the filename, line
number and column number of the condition node, as per a normal
compiler error message.

If assertions have been disabled, by way of `disable-assertions`, then
subsequent calls to this macro will expand to no-ops.


## <a name="concepts-core"></a> 2.7 concepts-core

### Details

Module: concepts-core

### Description

Provides the core functions and macros required for defining and
implementing concepts. The basic concepts are defined in
`concept-defs`, and they are implemented for the basic types in
`concepts`. Each binding in this module is in the `std.concepts`
namespace.

Concepts are a way in which the requirements of a macro can be
formalised, such that macro authors can depend on certain arguments
satisfying certain conditions. This means that problems are found
before a macro is instantiated, as opposed to after. They also allow
for multiple implementations of a given macro to operate concurrently,
with the specific macro to use being selected based on the concepts
implemented by the macro's arguments.

Concepts as implemented by this library are a little different from
those proposed from time to time for C++. The concept definitions
themselves are procedures, as opposed to lists of required
methods/attributes, and there is no support for concept maps or
axioms. There is also no facility for a given parameter in a concept
macro to satisfy multiple concepts at the same time, except by way of
a check in the concept definition body proper. (Allowing multiple
concepts per parameter complicates dispatch tremendously when concept
refinements are present.)

There are many `extern`-scoped functions in this module, but the only
ones that should be used directly are:

  * `exists-concept`;
  * `def-concept`;
  * `implement`;
  * `implements`; and
  * `def-concept-macro`.



### Functions

#### `std.concepts.exists-concept-fn`

Linkage: `extern`
Returns: `bool`
Parameters:

  * `(mc (p MContext))`: An MContext.
  * `(frm (p DNode))`: The node containing the concept name.


Determines whether the given concept, as described by the node, has
been defined.


#### `std.concepts.implements-fn`

Linkage: `extern`
Returns: `bool`
Parameters:

  * `(mc (p MContext))`: An MContext.
  * `(T (p DNode))`: A type node.
  * `(C (p DNode))`: A concept node.



#### `std.concepts.get-node-type-concept-list`

Linkage: `extern`
Returns: `(p (p char))`
Parameters:

  * `(mc (p MContext))`: An MContext.
  * `(T (p DNode))`: A type node.


Takes a type DNode as its single argument.  Returns a list of char
pointers, each being the name of a concept implemented by this type.
This is recursive, so if the type implements a concept A, which is in
turn a refinement of concept B, the list will includes entries for
both A and B.


#### `std.concepts.refines`

Linkage: `extern`
Returns: `bool`
Parameters:

  * `(mc (p MContext))`: An MContext.
  * `(Tupper (p (const char)))`: The possibly-refined type.
  * `(Tlower (p (const char)))`: The refining type.



#### `std.concepts.refines-multiple`

Linkage: `extern`
Returns: `bool`
Parameters:

  * `(mc (p MContext))`: An MContext.
  * `(Tupper (p (const char)))`: The possibly-refined type.
  * `(Tlower (p (const char)))`: The refining type.



#### `std.concepts.add-refinement`

Linkage: `extern`
Returns: `void`
Parameters:

  * `(current-cn (p concept-node))`: The current concept node.
  * `(refine-cn (p concept-node))`: The refinement concept node.



#### `std.concepts.make-simple-node`

Linkage: `extern`
Returns: `(p concept-node)`
Parameters:

  * `(mc (p MContext))`: An MContext.
  * `(name (p DNode))`: The name of the concept.



#### `std.concepts.dump-concept-map`

Linkage: `extern`
Returns: `void`
Parameters:

  * `(mc (p MContext))`
  * `(mapp (p concept-node))`
  * `(mlist bool)`
  * `(n int)`



#### `std.concepts.concept-map-to-string`

Linkage: `extern`
Returns: `void`
Parameters:

  * `(mc (p MContext))`
  * `(mapp (p concept-node))`
  * `(buf (p char))`



#### `std.concepts.concepts-to-string`

Linkage: `extern`
Returns: `void`
Parameters:

  * `(mc (p MContext))`
  * `(name (p DNode))`
  * `(buf (p char))`



#### `std.concepts.get-type-concept-map`

Linkage: `extern`
Returns: `(p concept-node)`
Parameters:

  * `(mc (p MContext))`
  * `(T (p DNode))`



#### `std.concepts.get-concept-macro-name`

Linkage: `intern`
Returns: `bool`
Parameters:

  * `(mc (p MContext))`: An MContext.
  * `(macro-name (p DNode))`: The macro name node.
  * `(mt-vl (p DNode))`: The concept macro type.
  * `(tnames bool)`: Whether the type is a list or an atom.
  * `(buf (p char))`: The buffer for the name.



#### `std.concepts.exists-concept-macro`

Linkage: `extern`
Returns: `bool`
Parameters:

  * `(mc (p MContext))`: An MContext.
  * `(macro-name (p DNode))`: The name of the concept macro.
  * `(macro-types (p DNode))`: The types of the concept macro.



#### `std.concepts.all-applicable`

Linkage: `intern`
Returns: `bool`
Parameters:

  * `(tcl-list (p (p concept-node)))`
  * `(tcl-count int)`


Returns true if the list of concept nodes does not contain any
instances of "not-applicable" (i.e. of the root, unused concept node).


#### `std.concepts.get-candidate-toplevel`

Linkage: `intern`
Returns: `bool`
Parameters:

  * `(mc (p MContext))`
  * `(tcl-list (p (p concept-node)))`
  * `(tcl-count int)`
  * `(macname (p char))`
  * `(macbuf (p char))`
  * `(hascnamebuf bool)`
  * `(cname (p char))`


Checks at the top-level of the concept list for a candidate that
matches the requested macro name.  If one exists, writes it to the
macro buffer and returns true.  Otherwise, returns false.


#### `std.concepts.get-candidate`

Linkage: `intern`
Returns: `int`
Parameters:

  * `(mc (p MContext))`
  * `(errn (p DNode))`
  * `(tcl-list (p (p concept-node)))`
  * `(tcl-count int)`
  * `(arg-cycle int)`
  * `(macname (p char))`
  * `(macbuf (p char))`
  * `(is-error (p int))`
  * `(hascnamebuf bool)`
  * `(cname (p char))`



#### `std.concepts.is-forced-concept`

Linkage: `intern`
Returns: `bool`
Parameters:

  * `(node (p DNode))`



#### `std.concepts.validate-forced-concept`

Linkage: `intern`
Returns: `bool`
Parameters:

  * `(mc (p MContext))`
  * `(node (p DNode))`



#### `std.concepts.get-type-concept-list`

Linkage: `intern`
Returns: `bool`
Parameters:

  * `(mc (p MContext))`
  * `(varargs-list (p DNode))`
  * `(varargs-count int)`
  * `(type-concept-list (p (p concept-node)))`
  * `(type-list (p (p DNode)))`


Takes a list of concept instantiation arguments, except for the
initial concept name.  Populates the type concept list, being the
mapping between the parameter and the concept map for the parameter.

Most parameters are plain types, for which see get-type-concept-map.
The special processing here is for disambiguation (forcing the use of
a specific concept) and non-types, the latter of which are treated as
being of the Value concept.




### Macros

#### `std.concepts.exists-concept`

Linkage: `extern`
Parameters:

  * `concept-name`: The node containing the concept name.


Expands to `true` or `false`, depending on whether the concept, as
described by the argument node, has been defined.


#### `std.concepts.def-concept`

Linkage: `extern`
Parameters:

  * `concept-name-node`
  * `refinement-list`
  * `type-arguments`
  * `rest`


Define a new concept. Takes a name, a list of refinements, a list of
parameters and an arbitrary number of forms representing the body of
the concept.

The list of refinements has the following form:

        (refines {concept-1} {concept-2} ... {concept-N})

A concept P 'refines' another concept Q when it must meet the criteria
of Q, as well as certain other criteria. If the concept being defined
does not refine any other concept, then the second argument should be
`(refines)`.  Apart from allowing a concept to import the criteria of
another concept, refinements are also used during `instantiate`: the
instantiation that is most-specific with respect to the arguments'
concepts, taking into account their refinements, is the one that is
used.

The parameter list is as per a macro's parameter list, except that all
of the parameters must be untyped. Currently, a concept may only
accept one parameter.

The body of the concept must evaluate to a boolean value indicating
whether the specified parameters satisfy the concept. Its environment
is as per a normal macro, so an `mc` is present.

Concepts, once defined, can be used to 'tag' the parameters to a macro
constructed by way of `def-concept-macro`.


#### `std.concepts.implement`

Linkage: `extern`
Parameters:

  * `concept-name-node`
  * `rest`


Takes a concept name and a list of type arguments. Attempts to
'implement' the concept for those type arguments, which involves
checking whether the type satisfies the concept's constraints and
marking the type as such if it does. If the type does not satisfy the
constraints, the expansion is a no-op, though note that the concept
body may (generally, will) add errors if its requirements are not met.


#### `std.concepts.implements`

Linkage: `extern`
Parameters:

  * `T`: A type node.
  * `C`: A concept node.


If the type implements the concept, this is a no-op. Otherwise, it
reports an error about the concept not being implemented.


#### `std.concepts.def-concept-macro`

Linkage: `extern`
Parameters:

  * `macro-name`: The name of the concept macro.
  * `linkage`: The linkage of the concept macro.
  * `macro-types`: The parameters (typed) for the macro.
  * `rest`


Define a new concept macro. Takes a name, a linkage type, a list of
macro parameters and an arbitrary number of forms (the body of the
macro) as its parameters. The list of macro parameters is as per a
normally-defined macro, except that each parameter must be 'typed'
with a concept.

Macros defined by way of this macro are executed by running
`instantiate`. That macro takes care of determining which concept
macro to actually run, based on the concepts implemented by the
arguments.


#### `std.concepts.assert-return`

Linkage: `intern`
Parameters:

  * `error-cond`
  * `report-error-node`
  * `report-error-str`



#### `std.concepts.assert-return-b`

Linkage: `intern`
Parameters:

  * `error-cond`
  * `report-error-node`
  * `report-error-str`



#### `std.concepts.instantiate`

Linkage: `extern`
Parameters:

  * `macro-name`: The name of the macro to be instantiated.
  * `rest`


Takes a concept macro name and a series of arguments for that concept
macro as its arguments. Determines the most appropriate concrete
concept macro, based on the provided arguments, and expands into a
call to that macro. The term `instantiate` is used because in nearly
all cases, the concept macro being run is something that in turn
expands into a series of bindings; for example, `Vector`.

When multiple equally-preferred instantiations are available for a
given call, this will report an error and expand to nothing.
Disambiguation is achieved by way of the `force` form:

        (instantiate MacroName (force ConceptName Type) ...)

Disambiguation is not required when an argument implements a concept
that refines another concept, and there are instantiations available
for both concepts.  In that case, the instantiation for the former
concept will be preferred.

Each argument to `instantiate` must implement one or more concepts.
If any argument does not, the expansion will fail with the error
message "type does not implement any concepts".  See `implement` in
this module for details on how types can be made to implement
concepts.

If no concept macros that have this name exist, the expansion will
fail with the error "concept not found".  This is usually caused by a
missing import statement.


## <a name="concept-defs"></a> 2.8 concept-defs

### Details

Module: concept-defs

### Description

Basic concept definitions. Copied from C++'s STL, for the most part.



### Concepts

#### `Value`

Returns true, regardless of the argument. This is mainly useful when a
concept macro needs to accept a parameter that isn't a type; there is
corresponding logic in `concepts-core` for that case that relies on
this concept being present.


#### `Type`

Returns true if the argument is a type.


#### `PreferRefs`

Returns true if the argument is a type for which references should be
used where possible.


#### `Struct`

Returns true if the argument is a struct type.


#### `DefaultConstructible`

Returns true if variables of the specified type may be declared and
left uninitialised. (This is a really unfortunate name, but not sure
what would be better.)


#### `MoveConstructible`

Returns true if variables of the specified type may be initialised by
way of an rvalue.


#### `MoveAssignable`

Returns true if variables of the specified type may be assigned by way
of an rvalue.


#### `CopyConstructible`

Returns true if variables of the specified type may be initialised by
way of an lvalue.


#### `CopyAssignable`

Returns true if variables of the specified type may be assigned by way
of an lvalue.


#### `Swappable`

Returns true if the type is `MoveConstructible` and `MoveAssignable`,
and a `swap` function exists over the type.


#### `EqualityComparable`

Returns true if `=` and `!=` are implemented over the type.


#### `LessThanComparable`

Returns true if `<`, `<=`, `>` and `>=` are implemented over the type.


#### `Container`

Returns true if the type is `Swappable`, and the following other
conditions hold:

  * `value-type`, `difference-type` and `size-type` macros exist over
    pointers to the type;
  * `size`, `max-size`, `empty`, `swap` and `init` are defined over
    pointers to the type;
  * the container has an iterator type; and
  * `begin` and `end` are defined over the container, and return
    iterators.


#### `ForwardContainer`

Refines `Container`.

Additional requirements:

  * the iterator type must be an `InputIterator`.


#### `ReversibleContainer`

Refines `ForwardContainer`.

Additional requirements:

  * the container has a reverse iterator type;
  * that type is an `InputIterator`; and
  * `rbegin` and `rend` are defined over pointers to the container
    type, and they both return reverse iterators.


#### `RandomAccessContainer`

Refines `ReversibleContainer`. Additionally, the iterator for the
container must be a `RandomAccessIterator`, and `$` must be defined
over the container.


#### `Sequence`

Refines `ForwardContainer`.

Additional requirements:

  * `front` is defined over pointers to the type;
  * `insert` is defined, and it takes a pointer to the type, an
    iterator, and an instance of value-type;
  * `erase` is defined, and it takes a pointer to the type and an
    iterator; and
  * `clear` is defined over pointers to the type.


#### `FrontInsertionSequence`

Refines `Sequence`.

Additional requirements:

  * `push-front` and `pop-front` are defined over pointers to the
    type.


#### `BackInsertionSequence`

Refines `Sequence`.

Additional requirements:

  * `push-back`, `pop-back` and `back` are defined over pointers to
    the type.


#### `AssociativeContainer`

Refines `ForwardContainer` and `DefaultConstructible`.

Additional requirements:

  * `key-type`, `erase` (key), `find` (key) and `count` (key) are
    defined over pointers to the type; and
  * `erase` is defined over the type's iterator.


#### `SimpleAssociativeContainer`

Refines `AssociativeContainer`. The only additional requirement is that
the key type and value type of the container are the same type.


#### `SortedAssociativeContainer`

Refines `AssociativeContainer` and `ReversibleContainer`.
Additionally, `lower-bound` and `upper-bound`, each taking a container
pointer type and a key value (and returning an iterator) must be
defined.


#### `UniqueAssociativeContainer`

Refines `AssociativeContainer`. Additionally, `insert` must be
defined, accepting a pointer to the type and an instance of
value-type.


#### `PairAssociativeContainer`

Refines `AssociativeContainer`. The value-type for the container must
be a struct, and it must contain two members named `first` and
`second`.


#### `TrivialIterator`

Refines `Swappable`, `EqualityComparable` and `DefaultConstructible`.
Additionally, `value-type`, and `source` (returning a pointer to a
value of type `value-type`) must be defined over the iterator (or a
pointer to the iterator, in the case of `value-type`).


#### `InputIterator`

Refines `TrivialIterator`. `successor`, returning an iterator of the
same type as the argument iterator, must be defined over the type.


#### `OutputIterator`

Refines `Swappable` and `DefaultConstructible`. Additionally,
`value-type`, `sink` (for setting the iterator's value) and
`successor` (for getting the next iterator) must be defined over the
iterator (or a pointer to the iterator, in the case of `value-type`).


#### `ForwardIterator`

Refines `InputIterator`. Additionally, calls to `successor` must not
invalidate previous iterators. (This can't be determined
automatically, so this concept exists for documentation purposes
only.)


#### `BidirectionalIterator`

Refines `ForwardIterator`. Additionally, `predecessor` must be
implemented over the iterator, and it must return an instance of the
same type of iterator.


#### `RandomAccessIterator`

Refines `BidirectionalIterator` and `LessThanComparable`.
Additionally, `distance-type` must be defined over pointers to the
type, and `+` and `-` must also be defined, each taking an iterator
and a value of type `distance-type`, and returning a new iterator.


## <a name="concepts"></a> 2.9 concepts

### Details

Module: concepts

### Description

Imports the other concept-related modules, and implements the
following concepts:

  * `Swappable`;
  * `EqualityComparable`; and
  * `LessThanComparable`

over the built-in integral types, as well as `char`, `size` and
`ptrdiff`. Provides `swap` functions to suit, too.



### Macros

#### `refconst`

Linkage: `extern`
Parameters:

  * `frm`


Expands a form `frm` into `(ref (const frm))`.


#### `move@`

Linkage: `extern`
Parameters:

  * `frm`


Expands a form `frm` into `(move (@ frm))`.


#### `prefer-ref-bindings`

Linkage: `extern`
Parameters:

  * `T2`
  * `S`


Takes a type and a suffix as its arguments.  Expands into a series of
`def` forms: the first is `prefer-refs`, being a boolean indicating
whether this type implements the `PreferRefs` concept; the second is
`tpw` (type parameter wrapper), which should be used for parameters of
the specified type (e.g. `((uq tpw) parameter)`), and the third is
`tvw` (type value wrapper), which should be used when accessing the
value of the relevant parameter.  There are additionally `tpw-ro` and
`tvw-ro`, for when types are used in a read-only fashion.  The form
names described above each take a hyphen and the string of the suffix
node, so as to allow disambiguation when multiple calls are required.

There is an additional overload of this macro, which takes two forms
that should evaluate to either `true` or `false`, representing whether
copy is disabled for the type and whether the type prefers references,
as well as a suffix node.  It uses those forms to determine the
appropriate type parameter and value wrappers, but otherwise operates
as per the other version.


## <a name="utility"></a> 2.10 utility

### Details

Module: utility

### Description

Provides the common functions and macros used by the container and
algorithm modules.



### Functions

#### `make-type-string`

Linkage: `extern`
Returns: `bool`
Parameters:

  * `(mc (p MContext))`: An MContext.
  * `(prefix (p (const char)))`: The type string prefix.
  * `(T (p DNode))`: The type node.
  * `(buf (p char))`: The buffer for the type string.


Writes the prefix, and the internal string representation of the type
node, to the provided buffer. If the type node is a token that begins
with a digit, then the token's contents are written to the buffer
instead.


#### `make-type-string`

Linkage: `extern`
Returns: `bool`
Parameters:

  * `(mc (p MContext))`: An MContext.
  * `(prefix (p (const char)))`: The type string prefix.
  * `(T1 (p DNode))`: The first type node.
  * `(T2 (p DNode))`: The second type node.
  * `(buf (p char))`: The buffer for the type string.


As per the earlier implementation, except that it takes two type
nodes.


#### `make-type-string`

Linkage: `extern`
Returns: `bool`
Parameters:

  * `(mc (p MContext))`: An MContext.
  * `(prefix (p (const char)))`: The type string prefix.
  * `(T1 (p DNode))`: The first type node.
  * `(T2 (p DNode))`: The second type node.
  * `(T3 (p DNode))`: The third type node.
  * `(buf (p char))`: The buffer for the type string.


As per the earlier implementation, except that it takes three type
nodes.


#### `make-type-display-string`

Linkage: `extern`
Returns: `bool`
Parameters:

  * `(mc (p MContext))`: An MContext.
  * `(prefix (p (const char)))`: The type display string prefix.
  * `(T (p DNode))`: The type node.
  * `(buf (p char))`: The buffer for the type display string.


Similar to `make-type-string`, except that it adds the display
representation (i.e. the one set by way of `register-type`, if
applicable) to the buffer, rather than the internal representation.


#### `make-type-display-string`

Linkage: `extern`
Returns: `bool`
Parameters:

  * `(mc (p MContext))`: An MContext.
  * `(prefix (p (const char)))`: The type display string prefix.
  * `(T1 (p DNode))`: The first type node.
  * `(T2 (p DNode))`: The second type node.
  * `(buf (p char))`: The buffer for the type display string.


As per the earlier implementation, except that it takes two type
nodes.


#### `make-type-display-string`

Linkage: `extern`
Returns: `bool`
Parameters:

  * `(mc (p MContext))`: An MContext.
  * `(prefix (p (const char)))`: The type display string prefix.
  * `(T1 (p DNode))`: The first type node.
  * `(T2 (p DNode))`: The second type node.
  * `(T3 (p DNode))`: The third type node.
  * `(buf (p char))`: The buffer for the type display string.


As per the earlier implementation, except that it takes two type
nodes.




### Concept macros

#### `Pair`

Linkage: `extern`
Parameters:

  * `(T1 Type)`: The first type node.
  * `(T2 Type)`: The second type node.


Expands to a struct definition with two members, named `first` and
`second`.


#### `Triple`

Linkage: `extern`
Parameters:

  * `(T1 Type)`: The first type node.
  * `(T2 Type)`: The second type node.
  * `(T3 Type)`: The third type node.


Expands to a struct definition with three members, named `first`,
`second` and `third`.




### Macros

#### `def-type-macro`

Linkage: `extern`
Parameters:

  * `name`: The type macro name.


Takes a node as its single argument.  Constructs a macro with that
name that takes one arbitrary type and expands to the concatenation of
that name and the stringification of the type.


#### `Iterator`

Linkage: `extern`
Parameters:

  * `T`: The type node.


Expands to a string that can be used as the `Iterator` type name for
the argument type.


#### `ReverseIterator`

Linkage: `extern`
Parameters:

  * `T`: The type node.


Expands to a string that can be used as the `Iterator` type name for
the argument type.


#### `Pair`

Linkage: `extern`
Parameters:

  * `T1`: The first type node.
  * `T2`: The second type node.


Expands to the concrete type name of the `Pair` generated by way of
the concept macro.


#### `Triple`

Linkage: `extern`
Parameters:

  * `T1`: The first type node.
  * `T2`: The second type node.
  * `T3`: The third type node.


Expands to the concrete type name of the `Triple` generated by way of
the concept macro.


## <a name="derivations"></a> 2.11 derivations

### Details

Module: derivations

### Description

Provides concept macros for generating common functions over types.
The difference between this module and `algorithms` is that this one
is limited to very simple functions, namely the basic relations and
the `swap` function.



### Concept macros

#### `swap`

Linkage: `extern`
Parameters:

  * `(T Type)`: The type node.


Expands to a `swap` function over references to the provided type.


#### `!=`

Linkage: `extern`
Parameters:

  * `(T Type)`: The type node.


Expands to a `!=` function over the provided type. `=` must be defined
over the type before calling this macro.


#### `<=`

Linkage: `extern`
Parameters:

  * `(T Type)`: The type node.


Expands to a `<=` function over the provided type. `<` must be defined
over the type before calling this macro.


#### `>`

Linkage: `extern`
Parameters:

  * `(T Type)`: The type node.


Expands to a `>` function over the provided type. `<` must be defined
over the type before calling this macro.


#### `>=`

Linkage: `extern`
Parameters:

  * `(T Type)`: The type node.


Expands to a `>=` function over the provided type. `<` must be defined
over the type before calling this macro.


#### `=`

Linkage: `extern`
Parameters:

  * `(T Struct)`: The struct type node.


Expands to a `=` function over the provided struct type. `=` must be
defined over each of the struct's member types before calling this
macro. 


#### `<`

Linkage: `extern`
Parameters:

  * `(T Struct)`: The struct type node.


Expands to a `<` function over the provided struct type. `<` must be
defined over each of the struct's member types before calling this
macro. 


#### `relations`

Linkage: `extern`
Parameters:

  * `(T Type)`: The struct type node.


Expands to `=`, `!=`, `<`, `<=`, `>` and `>=` functions over the
provided struct type. Any that have already been defined are skipped.


## <a name="algorithms"></a> 2.12 algorithms

### Details

Module: algorithms

### Description

Provides concept macros for generating algorithm functions.



### Concept macros

#### `find`

Linkage: `extern`
Parameters:

  * `(Ti InputIterator)`: The iterator type.


Expands to a function that accepts two of the provided iterator types
and an instance of the iterator type's `value-type`. On calling that
function, if the value is found in the range, the iterator containing
the value is returned. Otherwise, the second argument iterator is
returned.


#### `find-if`

Linkage: `extern`
Parameters:

  * `(Ti InputIterator)`: The iterator type.


Expands to a function that accepts two of the provided iterator types
and a `Predicate`. On calling that function, if a value satisfying the
predicate is found in the range, the iterator containing that value is
returned. Otherwise, the second argument iterator is returned.


#### `find-if-not`

Linkage: `extern`
Parameters:

  * `(Ti InputIterator)`: The iterator type.


Expands to a function that accepts two of the provided iterator types
and a `Predicate`. On calling that function, if a value that does not
satisfy the predicate is found in the range, the iterator containing
that value is returned. Otherwise, the second argument iterator is
returned.


#### `sort`

Linkage: `extern`
Parameters:

  * `(Ti RandomAccessIterator)`: The iterator type.


Expands to a function that accepts two of the provided iterator types.
On calling that function, the corresponding range is sorted. (The
implementation here needs some work, because the resulting function is
pretty slow.)


#### `for-each`

Linkage: `extern`
Parameters:

  * `(Ti InputIterator)`: The iterator type.


Expands to a function that accepts two of the provided iterator types
and an `Action`. On calling that function, the action is run against
each value in the range.


#### `lower-bound`

Linkage: `extern`
Parameters:

  * `(Ti RandomAccessIterator)`: The iterator type.


Expands to a function that accepts two of the provided iterator types
and a `value-type`. On calling that function, returns the position of
the first value in the range that is not less than the provided value.
The function assumes that the range is sorted.


#### `upper-bound`

Linkage: `extern`
Parameters:

  * `(Ti RandomAccessIterator)`: The iterator type.


Expands to a function that accepts two of the provided iterator types
and a `value-type`. On calling that function, returns the position of
the first value in the range that is greater than the provided value.
The function assumes that the range is sorted.


#### `binary-search`

Linkage: `extern`
Parameters:

  * `(Ti RandomAccessIterator)`: The iterator type.


Expands to a function that accepts two of the provided iterator types
and a `value-type`. On calling that function, returns a boolean
indicating whether the value is present in the range. The function
assumes that the range is sorted.


#### `equal-range`

Linkage: `extern`
Parameters:

  * `(Ti RandomAccessIterator)`: The iterator type.


Expands to a function that accepts two of the provided iterator types
and a `value-type`. It also instantiates a `Pair` type over the
iterator type, if required. On calling that function, it returns a
`Pair` containing the `lower-bound` and `upper-bound` iterators for
the value.


#### `max`

Linkage: `extern`
Parameters:

  * `(T Type)`: The type node.


Expands to a function that takes two values of the specified type. If
the first value is greater than the second value, then the first is
returned. Otherwise, the second is returned. Does not support types
that do not permit copying.


#### `min`

Linkage: `extern`
Parameters:

  * `(T Type)`: The type node.


Expands to a function that takes two values of the specified type. If
the first value is less than than the second value, then the first is
returned. Otherwise, the second is returned. Does not support types
that do not permit copying.


#### `copy`

Linkage: `extern`
Parameters:

  * `(Ti InputIterator)`: The input iterator type.
  * `(To OutputIterator)`: The output iterator type.


Takes input and output iterator types as its arguments. Expands to a
function that takes two of the input iterators and an output iterator.
That function iterates over the provided range, sinking values into
the output iterator at each step.


#### `assign`

Linkage: `extern`
Parameters:

  * `(Tc BackInsertionSequence)`: The container type.
  * `(Ti InputIterator)`: The input iterator type.


Takes a container type and an input iterator type as its arguments.
Expands to a function that takes a container value and two input
iterators as its arguments. That function clears the container, and
then copies each element from the range formed by the input iterators
into the cleared container.


#### `foldl`

Linkage: `extern`
Parameters:

  * `(Ti InputIterator)`: The input iterator type.


Takes an input iterator type as its arguments.  Expands to a fold-left
function that takes a binary operation function pointer, an initial
value, and a pair of input iterators as its arguments.  Does not
support types that do not permit copying.


#### `=`

Linkage: `extern`
Parameters:

  * `(Tc Container)`: The container type.


Takes a container type as its argument. Expands to a function that
takes two container pointers and returns a boolean indicating whether
the elements of those containers match.


#### `<`

Linkage: `extern`
Parameters:

  * `(Tc Container)`: The container type.


Takes a container type as its argument. Expands to a function that
takes two container pointers and returns a boolean indicating whether
the first container is 'less than' the second. This will be so when:

  * the first element of the first container that is not equal to the
    element at the same position in the second container compares as
    less than that element; or
  * there are fewer elements in the first container than in the
    second.

(`<` is defined over pointers to containers, rather than container
values, for the same reasons as `=`).




### Macros

#### `Predicate`

Linkage: `extern`
Parameters:

  * `T`: The type node.


Expands to a function pointer type that takes the `value-type` of the
corresponding type argument and returns a `bool`. This name is used as
shorthand in this module's documentation.


#### `RefPredicate`

Linkage: `extern`
Parameters:

  * `T`: The type node.


Like `Predicate`, except the function pointer type takes a const reference.


#### `Action`

Linkage: `extern`
Parameters:

  * `T`: The type node.


Expands to a function pointer type that takes the `value-type` of the
corresponding type argument and returns `void`. This name is used as
shorthand in this module's documentation.


#### `RefAction`

Linkage: `extern`
Parameters:

  * `T`: The type node.


Like `Action`, except the function pointer takes a reference.


## <a name="list"></a> 2.13 list

### Details

Module: list

### Description

Concept macro for a doubly-linked list. Apart from the `List` macro
and concept macro, the documentation in this module is for a generated
list instance of type `T`.

All of the functions that take `Iterator` arguments are defined for
`ReverseIterator`s as well, notwithstanding that there is no
documentation for those instances.

The `List` type implements the following concepts:

  * `FrontInsertionSequence`;
  * `BackInsertionSequence`; and
  * `ReversibleContainer`.

Its iterators implement `OutputIterator` and `BidirectionalIterator`.



### Structs

#### `(List T)`

Linkage: `extern`
Members: N/A

The core list structure type.


#### `(Iterator (List T))`

Linkage: `extern`
Members:

  * `(node (p nodetypenode))`



#### `(ReverseIterator (List T))`

Linkage: `extern`
Members:

  * `(node (p nodetypenode))`





### Functions

#### `init`

Linkage: `extern`
Returns: `bool`
Parameters:

  * `(lst (ref (List T)))`: A list reference.


Initialise a list.


#### `empty`

Linkage: `extern`
Returns: `bool`
Parameters:

  * `(lst (ref (const (List T))))`: A list reference.


Determine whether the list is empty.


#### `size`

Linkage: `extern`
Returns: `size`
Parameters:

  * `(lst (ref (const (List T))))`: A list reference.


Returns the number of elements in the list.


#### `max-size`

Linkage: `extern`
Returns: `size`
Parameters:

  * `(lst (ref (const (List T))))`: A list reference.


Returns the number of elements that can be accommodated by the
list.


#### `front`

Linkage: `extern`
Returns: `(p T)`
Parameters:

  * `(lst (ref (const (List T))))`: A list reference.


Returns a pointer to the value of the first element in the list.


#### `back`

Linkage: `extern`
Returns: `(p T)`
Parameters:

  * `(lst (ref (const (List T))))`: A list reference.


Returns a pointer to the value of the last element in the list.


#### `push-back`

Linkage: `extern`
Returns: `bool`
Parameters:

  * `(lst (ref (List T)))`: A list reference.
  * `(value T)`: The value to add to the list.


Adds an element to the end of the list.


#### `pop-back`

Linkage: `extern`
Returns: `void`
Parameters:

  * `(lst (ref (List T)))`: A list reference.


Removes an element from the end of the list.


#### `push-front`

Linkage: `extern`
Returns: `bool`
Parameters:

  * `(lst (ref (List T)))`: A list reference.
  * `(value T)`: The value to add to the list.


Adds an element to the beginning of the list.


#### `pop-front`

Linkage: `extern`
Returns: `void`
Parameters:

  * `(lst (ref (List T)))`: A list reference.


Removes an element from the beginning of the list.


#### `begin`

Linkage: `extern`
Returns: `(Iterator (List T))`
Parameters:

  * `(lst (ref (List T)))`: A list reference.


Returns the iterator for the first list element.


#### `end`

Linkage: `extern`
Returns: `(Iterator (List T))`
Parameters:

  * `(lst (ref (List T)))`: A list reference.


Returns the iterator representing the end of the list (sentinel).


#### `source`

Linkage: `extern`
Returns: `(p T)`
Parameters:

  * `(iter (Iterator (List T)))`: An iterator.


Returns a pointer to the iterator's value.


#### `sink`

Linkage: `extern`
Returns: `bool`
Parameters:

  * `(iter (Iterator (List T)))`: An iterator.
  * `(v T)`: The new value.


Set the given value at the specified position in the list.


#### `successor`

Linkage: `extern`
Returns: `(Iterator (List T))`
Parameters:

  * `(iter (Iterator (List T)))`: An iterator.


Returns the iterator for the position that follows the argument
iterator.


#### `predecessor`

Linkage: `extern`
Returns: `(Iterator (List T))`
Parameters:

  * `(iter (Iterator (List T)))`: An iterator.


Returns the iterator for the position just before the argument
iterator.


#### `=`

Linkage: `extern`
Returns: `bool`
Parameters:

  * `(iter1 (Iterator (List T)))`: The first iterator.
  * `(iter2 (Iterator (List T)))`: The second iterator.



#### `rbegin`

Linkage: `extern`
Returns: `(ReverseIterator (List T))`
Parameters:

  * `(lst (ref (List T)))`: A list reference.


Returns the iterator for the last list element.


#### `rend`

Linkage: `extern`
Returns: `(ReverseIterator (List T))`
Parameters:

  * `(lst (ref (List T)))`: A list reference.


Returns the iterator representing the beginning of the list (sentinel).


#### `insert`

Linkage: `extern`
Returns: `(Iterator (List T))`
Parameters:

  * `(lst (ref (List T)))`: The list reference.
  * `(iter (Iterator (List T)))`: The iterator for the position.
  * `(value T)`: The new value.


Inserts a new element into the list before the specified position,
and returns the iterator for the new element.


#### `erase`

Linkage: `extern`
Returns: `(Iterator (List T))`
Parameters:

  * `(lst (ref (List T)))`: The list reference.
  * `(iter (Iterator (List T)))`: The iterator for the position.


Remove the element at the specified position from the list.


#### `clear`

Linkage: `extern`
Returns: `bool`
Parameters:

  * `(lst (ref (List T)))`: The list reference.


Remove all of the elements from the list.


#### `swap`

Linkage: `extern`
Returns: `void`
Parameters:

  * `(lst1 (ref (List T)))`: The first list reference.
  * `(lst2 (ref (List T)))`: The second list reference.





### Concept macros

#### `List`

Linkage: `extern`
Parameters:

  * `(T Type)`: The type node.


Expands to a `List` definition over the relevant type.




### Macros

#### `List`

Linkage: `extern`
Parameters:

  * `T`: The type node.


Expands to the concrete type name of the `List` generated by way of
the concept macro.


#### `value-type`

Linkage: `extern`
Parameters:

  * `(lst (p (List T)))`: A type node.


Expands to the underlying value type (i.e. `T`) of the list. This
only uses the type node for dispatch purposes, so it's safe to
call this with e.g. `(nullptr (List T))` as the argument.


#### `size-type`

Linkage: `extern`
Parameters:

  * `(lst (p (List T)))`: A type node.


Expands to the underlying size type of the list.


#### `difference-type`

Linkage: `extern`
Parameters:

  * `(lst (p (List T)))`: A type node.


Expands to the underlying iterator difference type of the list.


#### `value-type`

Linkage: `extern`
Parameters:

  * `(iter (p (Iterator (List T))))`: A type node.


Expands to the underlying value type (i.e. `T`) of the iterator.


#### `distance-type`

Linkage: `extern`
Parameters:

  * `(iter (p (Iterator (List T))))`: A type node.


Expands to the underlying distance type of the iterator.


## <a name="vector"></a> 2.14 vector

### Details

Module: vector

### Description

Concept macro for a vector. Apart from the `Vector` macro and concept
macro, the documentation in this module is for a generated vector
instance of type `T`.

All of the functions that take `Iterator` arguments are defined for
`ReverseIterator`s as well, notwithstanding that there is no
documentation for those instances.

The `Vector` type implements `BackInsertionSequence` and
`RandomAccessContainer`. Its iterators implement `OutputIterator` and
`RandomAccessIterator`.



### Structs

#### `(Vector T)`

Linkage: `extern`
Members: N/A

The core vector structure type.


#### `(Iterator (Vector T))`

Linkage: `extern`
Members:

  * `(parent-vector (p (Vector T)))`
  * `(element (p T))`



#### `(ReverseIterator (Vector T))`

Linkage: `extern`
Members:

  * `(element (p T))`





### Functions

#### `init`

Linkage: `extern`
Returns: `bool`
Parameters:

  * `(vecp (ref (Vector T)))`: A vector reference.
  * `(cap int)`: The capacity of the new vector.


Initialise a vector by providing a specific capacity.


#### `init`

Linkage: `extern`
Returns: `bool`
Parameters:

  * `(vecp (ref (Vector T)))`: A vector reference.


Initialise a vector.


#### `empty`

Linkage: `extern`
Returns: `bool`
Parameters:

  * `(vecp (ref (const (Vector T))))`: A vector reference.


Determine whether the vector is empty.


#### `size`

Linkage: `extern`
Returns: `(const size)`
Parameters:

  * `(vecp (ref (const (Vector T))))`: A vector reference.


Returns the number of elements in the vector.


#### `max-size`

Linkage: `extern`
Returns: `size`
Parameters:

  * `(vecp (ref (const (Vector T))))`: A vector reference.


Returns the number of elements that can be accommodated by the
vector.


#### `front`

Linkage: `extern`
Returns: `(const (p T))`
Parameters:

  * `(vecp (ref (const (Vector T))))`: A vector reference.


Returns a pointer to the value of the first element in the vector.


#### `back`

Linkage: `extern`
Returns: `(const (p T))`
Parameters:

  * `(vecp (ref (const (Vector T))))`: A vector reference.


Returns a pointer to the value of the last element in the vector.


#### `pop-back`

Linkage: `extern`
Returns: `void`
Parameters:

  * `(vecp (ref (Vector T)))`: A vector reference.


Removes an element from the end of the vector.


#### `resize`

Linkage: `extern`
Returns: `bool`
Parameters:

  * `(vecp (ref (Vector T)))`: A vector reference.
  * `(new-capacity size)`: The new capacity for the vector.


Change the vector's capacity.


#### `reserve`

Linkage: `extern`
Returns: `bool`
Parameters:

  * `(vecp (ref (Vector T)))`: A vector reference.
  * `(extra size)`: The amount by which the capacity should be increased.


Increase the vector's capacity by the specified amount.


#### `push-back`

Linkage: `extern`
Returns: `bool`
Parameters:

  * `(vecp (ref (Vector T)))`: A vector reference.
  * `(value T)`: The value to add to the vector.


Adds an element to the end of the vector.


#### `$`

Linkage: `extern`
Returns: `(p T)`
Parameters:

  * `(vecp (ref (const (Vector T))))`: A vector reference.
  * `(index int)`: The index of the element.



#### `$`

Linkage: `extern`
Returns: `(p T)`
Parameters:

  * `(vecp (ref (const (Vector T))))`: A vector reference.
  * `(index size)`: The index of the element.



#### `begin`

Linkage: `extern`
Returns: `(Iterator (Vector T))`
Parameters:

  * `(vecp (ref (Vector T)))`: A vector reference.


Returns the iterator for the first vector element.


#### `end`

Linkage: `extern`
Returns: `(Iterator (Vector T))`
Parameters:

  * `(vecp (ref (Vector T)))`: A vector reference.


Returns the iterator representing the end of the vector (sentinel).


#### `source`

Linkage: `extern`
Returns: `(p T)`
Parameters:

  * `(iter (Iterator (Vector T)))`: An iterator.


Returns a pointer to the iterator's value.


#### `sink`

Linkage: `extern`
Returns: `bool`
Parameters:

  * `(iter (Iterator (Vector T)))`: An iterator.
  * `(v T)`: The new value.


Set the given value at the specified position in the vector.


#### `successor`

Linkage: `extern`
Returns: `(Iterator (Vector T))`
Parameters:

  * `(iter (Iterator (Vector T)))`: An iterator.


Returns the iterator for the position that follows the argument
iterator.


#### `predecessor`

Linkage: `extern`
Returns: `(Iterator (Vector T))`
Parameters:

  * `(iter (Iterator (Vector T)))`: An iterator.


Returns the iterator for the position just before the argument
iterator.


#### `=`

Linkage: `extern`
Returns: `bool`
Parameters:

  * `(iter1 (Iterator (Vector T)))`: The first iterator.
  * `(iter2 (Iterator (Vector T)))`: The second iterator.



#### `<`

Linkage: `extern`
Returns: `bool`
Parameters:

  * `(iter1 (Iterator (Vector T)))`: The first iterator.
  * `(iter2 (Iterator (Vector T)))`: The second iterator.



#### `+`

Linkage: `extern`
Returns: `(Iterator (Vector T))`
Parameters:

  * `(iter1 (Iterator (Vector T)))`: The first iterator.
  * `(n size)`: The number of steps to advance.



#### `-`

Linkage: `extern`
Returns: `(Iterator (Vector T))`
Parameters:

  * `(iter1 (Iterator (Vector T)))`: The first iterator.
  * `(n size)`: The number of steps by which to move backwards.



#### `distance`

Linkage: `extern`
Returns: `size`
Parameters:

  * `(iter1 (Iterator (Vector T)))`: The first iterator.
  * `(iter2 (Iterator (Vector T)))`: The second iterator.


Return the distance between the two iterators.


#### `rbegin`

Linkage: `extern`
Returns: `(ReverseIterator (Vector T))`
Parameters:

  * `(vec (ref (Vector T)))`: A vector reference.


Returns the iterator for the last vector element.


#### `rend`

Linkage: `extern`
Returns: `(ReverseIterator (Vector T))`
Parameters:

  * `(vec (ref (Vector T)))`: A vector reference.


Returns the iterator representing the beginning of the vector
(sentinel).


#### `insert`

Linkage: `extern`
Returns: `(Iterator (Vector T))`
Parameters:

  * `(unused (ref (const (Vector T))))`: The vector reference.
  * `(iter (Iterator (Vector T)))`: The iterator for the position.
  * `(value T)`: The new value.


Inserts a new element into the vector before the specified position,
and returns the iterator for the new element.


#### `erase`

Linkage: `extern`
Returns: `(Iterator (Vector T))`
Parameters:

  * `(unused (ref (const (Vector T))))`: The vector reference.
  * `(iter (Iterator (Vector T)))`: The iterator for the position.


Remove the element at the specified position from the vector.


#### `clear`

Linkage: `extern`
Returns: `bool`
Parameters:

  * `(vecp (ref (Vector T)))`: The vector reference.


Remove all of the elements from the vector.


#### `swap`

Linkage: `extern`
Returns: `void`
Parameters:

  * `(vec1 (ref (Vector T)))`: The first vector reference.
  * `(vec2 (ref (Vector T)))`: The second vector reference.





### Concept macros

#### `Vector`

Linkage: `extern`
Parameters:

  * `(T MoveConstructible)`: The type node.


Expands to a `Vector` definition over the relevant type.  If `T`
implements `EqualityComparable` and/or `LessThanComparable`, the
new vector type will implement them as well.




### Macros

#### `Vector`

Linkage: `extern`
Parameters:

  * `T`: The type node.


Expands to the concrete type name of the `Vector` generated by way of
the concept macro.


#### `value-type`

Linkage: `extern`
Parameters:

  * `(vecp (p (Vector T)))`: A type node.


Expands to the underlying value type (i.e. `T`) of the vector.
This only uses the type node for dispatch purposes, so it's safe
to call this with e.g. `(nullptr (Vector T))` as the argument.


#### `size-type`

Linkage: `extern`
Parameters:

  * `(vecp (p (Vector T)))`: A type node.


Expands to the underlying size type of the vector.


#### `difference-type`

Linkage: `extern`
Parameters:

  * `(vecp (p (Vector T)))`: A type node.


Expands to the underlying iterator difference type of the vector.


#### `value-type`

Linkage: `extern`
Parameters:

  * `(veciter (p (Iterator (Vector T))))`: A type node.


Expands to the underlying value type (i.e. `T`) of the iterator.


#### `distance-type`

Linkage: `extern`
Parameters:

  * `(veciter (p (Iterator (Vector T))))`: A type node.


Expands to the underlying distance type of the iterator.


## <a name="set"></a> 2.15 set

### Details

Module: set

### Description

Concept macro for a set. Apart from the `Set` macro and concept macro,
the documentation in this module is for a generated set instance of
type `T`.

All of the functions that take `Iterator` arguments are defined for
`ReverseIterator`s as well, notwithstanding that there is no
documentation for those instances.

The `Set` type implements the following concepts:

  * `SimpleAssociativeContainer`;
  * `SortedAssociativeContainer`; and
  * `UniqueAssociativeContainer`.

Its iterators implement `ForwardIterator`.



### Structs

#### `(Set T)`

Linkage: `extern`
Members: N/A

The core set structure type.


#### `(Iterator (Set T))`

Linkage: `extern`
Members:

  * `(setp (p (Set T)))`
  * `(element (p nodetypenode))`



#### `(ReverseIterator (Set T))`

Linkage: `extern`
Members:

  * `(element (p nodetypenode))`
  * `(setp (p (Set T)))`





### Functions

#### `init`

Linkage: `extern`
Returns: `bool`
Parameters:

  * `(setp (ref typenode))`: A set reference.


Initialise a set.


#### `empty`

Linkage: `extern`
Returns: `bool`
Parameters:

  * `(setp (ref (const (Set T))))`: A set reference.


Determine whether the set is empty.


#### `max-size`

Linkage: `extern`
Returns: `size`
Parameters:

  * `(setp (ref (const (Set T))))`: A set reference.


Returns the number of elements that can be accommodated by the
set.


#### `end`

Linkage: `extern`
Returns: `(Iterator (Set T))`
Parameters:

  * `(setp (ref (Set T)))`: A set reference.


Returns the iterator representing the end of the set (sentinel).


#### `begin`

Linkage: `extern`
Returns: `(Iterator (Set T))`
Parameters:

  * `(setp (ref (Set T)))`: A set reference.


Returns the iterator for the first set element.


#### `source`

Linkage: `extern`
Returns: `(p T)`
Parameters:

  * `(iter (Iterator (Set T)))`: An iterator.


Returns a pointer to the iterator's value.


#### `successor`

Linkage: `extern`
Returns: `(Iterator (Set T))`
Parameters:

  * `(iter (Iterator (Set T)))`: An iterator.


Returns the iterator for the position that follows the argument
iterator.


#### `=`

Linkage: `extern`
Returns: `bool`
Parameters:

  * `(iter1 (Iterator (Set T)))`: The first iterator.
  * `(iter2 (Iterator (Set T)))`: The second iterator.



#### `<`

Linkage: `extern`
Returns: `bool`
Parameters:

  * `(iter1 (Iterator (Set T)))`: The first iterator.
  * `(iter2 (Iterator (Set T)))`: The second iterator.



#### `rend`

Linkage: `extern`
Returns: `(ReverseIterator (Set T))`
Parameters:

  * `(setp (ref (Set T)))`: A set reference.


Returns the iterator representing the beginning of the set (sentinel).


#### `rbegin`

Linkage: `extern`
Returns: `(ReverseIterator (Set T))`
Parameters:

  * `(setp (ref (Set T)))`: A set reference.


Returns the iterator for the last set element.


#### `insert`

Linkage: `extern`
Returns: `bool`
Parameters:

  * `(setp (ref (Set T)))`: A set reference.
  * `(newval T)`: The value to insert into the set.


Insert a new element into the set.


#### `find`

Linkage: `extern`
Returns: `(Iterator (Set T))`
Parameters:

  * `(setp (ref (Set T)))`: A set reference.
  * `(value T)`: The value to find in the set.


Find an element within the set, and return the iterator for its
position.


#### `erase`

Linkage: `extern`
Returns: `bool`
Parameters:

  * `(iter (Iterator (Set T)))`: The iterator.


Erase an element from the set, by specifying the iterator for its
position.


#### `erase`

Linkage: `extern`
Returns: `bool`
Parameters:

  * `(setp (ref (Set T)))`: A set reference.
  * `(v T)`: The value to remove from the set.


Erase an element from the set, by value.


#### `lower-bound`

Linkage: `extern`
Returns: `(Iterator (Set T))`
Parameters:

  * `(setp (ref (Set T)))`: A set reference.
  * `(value T)`: The value for which the lower bound should be found.


Find the lower bound for a given value.


#### `upper-bound`

Linkage: `extern`
Returns: `(Iterator (Set T))`
Parameters:

  * `(setp (ref (Set T)))`: A set reference.
  * `(value T)`: The value for which the upper bound should be found.


Find the upper bound for a given value.


#### `count`

Linkage: `extern`
Returns: `size`
Parameters:

  * `(setp (ref (Set T)))`: A set reference.
  * `(v T)`: The value for which the count should be determined.


Return the number of times that the value appears in the set. For
a `UniqueAssociativeContainer` such as `Set`, this can only return
1 or 0, depending on whether the element is present in the set.


#### `size`

Linkage: `extern`
Returns: `size`
Parameters:

  * `(setp (ref (Set T)))`: A set reference.


Return the number of elements in the set.


#### `clear`

Linkage: `extern`
Returns: `void`
Parameters:

  * `(setp (ref (Set T)))`: The set reference.


Remove all of the elements from the set.


#### `swap`

Linkage: `extern`
Returns: `void`
Parameters:

  * `(set1 (ref (Set T)))`: The first set reference.
  * `(set2 (ref (Set T)))`: The second set reference.





### Concept macros

#### `Set`

Linkage: `N/A`
Parameters:

  * `T`: The type node.


Expands to a `Set` definition over the relevant type. Note that `T`
must also implement `LessThanComparable`.




### Macros

#### `Set`

Linkage: `extern`
Parameters:

  * `T`: The type node.


Expands to the concrete type name of the `Set` generated by way of
the concept macro.


#### `value-type`

Linkage: `extern`
Parameters:

  * `(setp (p (Set T)))`: A type node.


Expands to the underlying value type (i.e. `T`) of the set. This
only uses the type node for dispatch purposes, so it's safe to
call this with e.g. `(nullptr (Set T))` as the argument.


#### `key-type`

Linkage: `extern`
Parameters:

  * `(setp (p (Set T)))`: A type node.


Expands to the underlying key type of the set. Since `Set` is a
`SimpleAssociativeContainer`, this returns the same type as
`value-type`.


#### `size-type`

Linkage: `extern`
Parameters:

  * `(setp (p (Set T)))`: A type node.


Expands to the underlying size type of the set.


#### `difference-type`

Linkage: `extern`
Parameters:

  * `(setp (p (Set T)))`: A type node.


Expands to the underlying iterator difference type of the set.


#### `value-type`

Linkage: `extern`
Parameters:

  * `(setiter (p (Iterator (Set T))))`: A type node.


Expands to the underlying value type (i.e. `T`) of the iterator.


## <a name="map"></a> 2.16 map

### Details

Module: map

### Description

Concept macro for a map. Apart from the `Map` macro and concept macro,
the documentation in this module is for a generated set instance
mapping from type `Tk` to type `Tv`.

All of the functions that take `Iterator` arguments are defined for
`ReverseIterator`s as well, notwithstanding that there is no
documentation for those instances.

The `Map` type implements the following concepts:

  * `PairAssociativeContainer`;
  * `SortedAssociativeContainer`; and
  * `UniqueAssociativeContainer`.

Its iterators implement `ForwardIterator`.



### Structs

#### `(Map Tk Tv)`

Linkage: `extern`
Members: N/A

The core map structure type.


#### `(Iterator (Map Tk) (Map Tv))`

Linkage: `extern`
Members: N/A


#### `(ReverseIterator (Map Tk Tv))`

Linkage: `extern`
Members:

  * `(mapp (p (Map Tk Tv)))`
  * `(setiter (ReverseIterator (Set (value-type (nullptr (Map Tk Tv))))))`





### Functions

#### `init`

Linkage: `extern`
Returns: `bool`
Parameters:

  * `(mapp (ref (Map Tk Tv)))`: A map reference.


Initialise a map.


#### `size`

Linkage: `extern`
Returns: `size`
Parameters:

  * `(mapp (ref (Map Tk Tv)))`: A map reference.


Return the number of elements in the map.


#### `max-size`

Linkage: `extern`
Returns: `size`
Parameters:

  * `(mapp (ref (const (Map Tk Tv))))`: A map reference.


Returns the number of elements that can be accommodated by the
map.


#### `empty`

Linkage: `extern`
Returns: `bool`
Parameters:

  * `(mapp (ref (const (Map Tk Tv))))`: A map reference.


Determine whether the map is empty.


#### `end`

Linkage: `extern`
Returns: `(Iterator (Map Tk Tv))`
Parameters:

  * `(mapp (ref (Map Tk Tv)))`: A map reference.


Returns the iterator representing the end of the map (sentinel).


#### `begin`

Linkage: `extern`
Returns: `(Iterator (Map Tk Tv))`
Parameters:

  * `(mapp (ref (Map Tk Tv)))`: A map reference.


Returns the iterator for the first map element.


#### `source`

Linkage: `extern`
Returns: `(p (value-type (nullptr (Map Tk Tv))))`
Parameters:

  * `(iter (Iterator (Map Tk Tv)))`: An iterator.


Returns a pointer to the iterator's value.


#### `successor`

Linkage: `extern`
Returns: `(Iterator (Map Tk Tv))`
Parameters:

  * `(iter (Iterator (Map Tk Tv)))`: An iterator.


Returns the iterator for the position that follows the argument
iterator.


#### `=`

Linkage: `extern`
Returns: `bool`
Parameters:

  * `(a (Iterator (Map Tk Tv)))`: The first iterator.
  * `(b (Iterator (Map Tk Tv)))`: The second iterator.



#### `<`

Linkage: `extern`
Returns: `bool`
Parameters:

  * `(a (Iterator (Map Tk Tv)))`: The first iterator.
  * `(b (Iterator (Map Tk Tv)))`: The second iterator.



#### `rend`

Linkage: `extern`
Returns: `(ReverseIterator (Map Tk Tv))`
Parameters:

  * `(mapp (ref (Map Tk Tv)))`: A map reference.


Returns the iterator representing the beginning of the map (sentinel).


#### `rbegin`

Linkage: `extern`
Returns: `(ReverseIterator (Map Tk Tv))`
Parameters:

  * `(mapp (ref (Map Tk Tv)))`: A map reference.


Returns the iterator for the last map element.


#### `clear`

Linkage: `extern`
Returns: `void`
Parameters:

  * `(mapp (ref (Map Tk Tv)))`: The map reference.


Remove all of the elements from the map.


#### `insert`

Linkage: `extern`
Returns: `bool`
Parameters:

  * `(mapp (ref (Map Tk Tv)))`: A map reference.
  * `(valt (value-type (nullptr (Map Tk Tv))))`: The value to insert into the map.


Insert a new element into the map.


#### `insert`

Linkage: `extern`
Returns: `bool`
Parameters:

  * `(mapp (ref (Map Tk Tv)))`: A map reference.
  * `(key Tk)`: The key for the new map element.
  * `(value Tv)`: The value for the new map element.


Insert a new element into the map.


#### `find`

Linkage: `extern`
Returns: `(Iterator (Map Tk Tv))`
Parameters:

  * `(mapp (ref (Map Tk Tv)))`: A map reference.
  * `(key Tk)`: The value to find in the set.


Find an element within the map, and return the iterator for its
position.


#### `erase`

Linkage: `extern`
Returns: `bool`
Parameters:

  * `(iter (Iterator (Map Tk Tv)))`: The iterator.


Erase an element from the map, by specifying the iterator for its
position.


#### `erase`

Linkage: `extern`
Returns: `bool`
Parameters:

  * `(mapp (ref (Map Tk Tv)))`: A map reference.
  * `(key Tk)`: The value to remove from the set.


Erase an element from the map by value.


#### `count`

Linkage: `extern`
Returns: `size`
Parameters:

  * `(mapp (ref (Map Tk Tv)))`: A map reference.
  * `(key Tk)`: The value for which the count should be determined.


Return the number of times that the value appears in the map. For
a `UniqueAssociativeContainer` such as `Map`, this can only return
1 or 0, depending on whether the element is present in the set.


#### `lower-bound`

Linkage: `extern`
Returns: `(Iterator (Map Tk Tv))`
Parameters:

  * `(mapp (ref (Map Tk Tv)))`: A map reference.
  * `(key Tk)`: The value for which the lower bound should be found.


Find the lower bound for a given value.


#### `upper-bound`

Linkage: `extern`
Returns: `(Iterator (Map Tk Tv))`
Parameters:

  * `(mapp (ref (Map Tk Tv)))`: A map reference.
  * `(key Tk)`: The value for which the upper bound should be found.


Find the upper bound for a given value.




### Concept macros

#### `Map`

Linkage: `extern`
Parameters:

  * `(Tk EqualityComparable)`: The key type node.
  * `(Tv Type)`: The value type node.


Expands to a `Set` definition over the relevant type. Note that `Tk`
must also implement `LessThanComparable`.




### Macros

#### `Map`

Linkage: `extern`
Parameters:

  * `Tk`: The key type node.
  * `Tv`: The value type node.


Expands to the concrete type name of the `Map` generated by way of
the concept macro.


#### `key-type`

Linkage: `extern`
Parameters:

  * `(mapp (p (Map Tk Tv)))`: A type node.


Expands to the underlying key type of the map (i.e. `Tk`). This
only uses the type node for dispatch purposes, so it's safe to
call this with e.g. `(nullptr (Map Tk Tv))` as the argument.


#### `data-type`

Linkage: `extern`
Parameters:

  * `(mapp (p (Map Tk Tv)))`: A type node.


Expands to the underlying data type (i.e. `Tv`) of the map.


#### `value-type`

Linkage: `extern`
Parameters:

  * `(mapp (p (Map Tk Tv)))`: A type node.


Expands to the underlying value type of the set. Note that for
maps, the value type is a specially-constructed pair type that is
not otherwise accessible. However, it is guaranteed to be a struct
that has the members `first` and `second`, with those members
mapping to `Tk` and `Tv` respectively.


#### `size-type`

Linkage: `extern`
Parameters:

  * `(mapp (p (Map Tk Tv)))`: A type node.


Expands to the underlying size type of the map.


#### `difference-type`

Linkage: `extern`
Parameters:

  * `(mapp (p (Map Tk Tv)))`: A type node.


Expands to the underlying iterator difference type of the map.


#### `value-type`

Linkage: `extern`
Parameters:

  * `(mapiter (p (Iterator (Map Tk Tv))))`: A type node.


Expands to the underlying value type of the iterator. This is the
same as that of `value-type` for the container.


## <a name="array"></a> 2.17 array

### Details

Module: array

### Description

Concept macro for a fixed-size container. Apart from the `Array` macro
and concept macro, the documentation in this module is for a generated
array instance of type `T`, with size `N`.

All of the functions that take `Iterator` arguments are defined for
`ReverseIterator`s as well, notwithstanding that there is no
documentation for those instances.

The `Array` type implements `RandomAccessContainer`. Its iterators
implement `OutputIterator` and `RandomAccessIterator`.


### Structs

#### `(Array T N)`

Linkage: `extern`
Members:

  * `(elements (array-of N T))`


The core array structure type.


#### `(Iterator (Array T N))`

Linkage: `extern`
Members:

  * `(element (p T))`
  * `(arrp (p (Array T N)))`



#### `(ReverseIterator (Array T N))`

Linkage: `extern`
Members:

  * `(element (p T))`
  * `(arrp (p (Array T N)))`





### Functions

#### `init`

Linkage: `extern`
Returns: `bool`
Parameters:

  * `(arrp (ref (Array T N)))`: An array reference.


Initialise an array. (This is a no-op for this container: to
actually zero the array, it is necessary to iterate over it and
set values accordingly.)


#### `empty`

Linkage: `extern`
Returns: `bool`
Parameters:

  * `(arrp (ref (const (Array T N))))`: An array reference.


Determine whether the array is empty. (Since arrays have a fixed
size, this will always return true, unless the array actually has
a size of zero.)


#### `size`

Linkage: `extern`
Returns: `size`
Parameters:

  * `(arrp (ref (const (Array T N))))`: An array reference.


Returns the number of elements in the array (This is `N`, for all
instances of the array.)


#### `max-size`

Linkage: `extern`
Returns: `size`
Parameters:

  * `(arrp (ref (const (Array T N))))`: An array reference.


Returns the number of elements that can be accommodated by the
array. (This is also `N`, for all instances of the array.)


#### `begin`

Linkage: `extern`
Returns: `(Iterator (Array T N))`
Parameters:

  * `(arrp (ref (Array T N)))`: An array reference.


Returns the iterator for the first array element.


#### `end`

Linkage: `extern`
Returns: `(Iterator (Array T N))`
Parameters:

  * `(arrp (ref (Array T N)))`: An array reference.


Returns the iterator representing the end of the list (sentinel).


#### `source`

Linkage: `extern`
Returns: `(p T)`
Parameters:

  * `(iter (Iterator (Array T N)))`: An iterator.


Returns a pointer to the iterator's value.


#### `sink`

Linkage: `extern`
Returns: `bool`
Parameters:

  * `(iter (Iterator (Array T N)))`: An iterator.
  * `(v T)`: The new value.


Set the given value at the specified position in the list.


#### `successor`

Linkage: `extern`
Returns: `(Iterator (Array T N))`
Parameters:

  * `(iter (Iterator (Array T N)))`: An iterator.


Returns the iterator for the position that follows the argument
iterator.


#### `predecessor`

Linkage: `extern`
Returns: `(Iterator (Array T N))`
Parameters:

  * `(iter (Iterator (Array T N)))`: An iterator.


Returns the iterator for the position just before the argument
iterator.


#### `=`

Linkage: `extern`
Returns: `bool`
Parameters:

  * `(iter1 (Iterator (Array T N)))`: The first iterator.
  * `(iter2 (Iterator (Array T N)))`: The second iterator.



#### `<`

Linkage: `extern`
Returns: `bool`
Parameters:

  * `(iter1 (Iterator (Array T N)))`: The first iterator.
  * `(iter2 (Iterator (Array T N)))`: The second iterator.



#### `+`

Linkage: `extern`
Returns: `(Iterator (Array T N))`
Parameters:

  * `(iter (Iterator (Array T N)))`: An array iterator.
  * `(n size)`: The number of steps forward.



#### `-`

Linkage: `extern`
Returns: `(Iterator (Array T N))`
Parameters:

  * `(iter (Iterator (Array T N)))`: An array iterator.
  * `(n size)`: The number of steps backward.



#### `rbegin`

Linkage: `extern`
Returns: `(ReverseIterator (Array T N))`
Parameters:

  * `(arrp (ref (Array T N)))`: An array reference.


Returns the iterator for the last array element.


#### `rend`

Linkage: `extern`
Returns: `(ReverseIterator (Array T N))`
Parameters:

  * `(arrp (ref (Array T N)))`: An array reference.


Returns the iterator representing the beginning of the array (sentinel).


#### `front`

Linkage: `extern`
Returns: `(p T)`
Parameters:

  * `(arrp (ref (Array T N)))`: An array reference.


Returns a pointer to the value of the first element in the array.


#### `back`

Linkage: `extern`
Returns: `(p T)`
Parameters:

  * `(arrp (ref (Array T N)))`: An array reference.


Returns a pointer to the value of the last element in the array.


#### `$`

Linkage: `extern`
Returns: `(p T)`
Parameters:

  * `(arrp (ref (Array T N)))`: An array reference.
  * `(index int)`: The index of the element.



#### `$`

Linkage: `extern`
Returns: `(p T)`
Parameters:

  * `(arrp (ref (Array T N)))`: An array reference.
  * `(index size)`: The index of the element.





### Concept macros

#### `Array`

Linkage: `extern`
Parameters:

  * `(T Type)`: The type for the elements of the array.
  * `(N Value)`: The length of the array.


Expands to an `Array` definition over the relevant type.




### Macros

#### `Array`

Linkage: `extern`
Parameters:

  * `T`: The type node.
  * `N`


Expands to the concrete type name of the `Array` generated by way of
the concept macro.


#### `value-type`

Linkage: `extern`
Parameters:

  * `(arrp (p (Array T N)))`: An array pointer.


Expands to the underlying value type (i.e. `T`) of the array. This
only uses the type node for dispatch purposes, so it's safe to
call this with e.g. `(nullptr (Array T N))` as the argument.


#### `size-type`

Linkage: `extern`
Parameters:

  * `(arrp (p (Array T N)))`: A type node.


Expands to the underlying size type of the array.


#### `difference-type`

Linkage: `extern`
Parameters:

  * `(arrp (p (Array T N)))`: A type node.


Expands to the underlying iterator difference type of the array.


#### `value-type`

Linkage: `extern`
Parameters:

  * `(iterp (p (Iterator (Array T N))))`: An iterator pointer.


Expands to the underlying value type (i.e. `T`) of the iterator.


#### `distance-type`

Linkage: `extern`
Parameters:

  * `(iterp (p (Iterator (Array T N))))`: An iterator pointer.


Expands to the underlying distance type of the iterator.


## <a name="shared-ptr"></a> 2.18 shared-ptr

### Details

Module: shared-ptr

### Description

A simple shared pointer module. Apart from the `SharedPtr` macro and
concept macro, the documentation in this module is for a generated
shared pointer instance of type `T`.



### Structs

#### `(SharedPtr T)`

Linkage: `N/A`
Members: N/A

The core shared pointer structure type.




### Functions

#### `init`

Linkage: `extern`
Returns: `bool`
Parameters:

  * `(loc (ref (SharedPtr T)))`: The shared pointer.
  * `(value (p T))`: The value to assign to the shared pointer.


Initialise the shared pointer structure with a pointer. Once the
structure has been initialised with the pointer, it takes
ownership of it. The structure assumes that the pointer was
created by way of malloc.


#### `init`

Linkage: `extern`
Returns: `bool`
Parameters:

  * `(loc (ref (SharedPtr T)))`: The shared pointer.


Initialise an empty/null shared pointer structure.


#### `get`

Linkage: `extern`
Returns: `(p T)`
Parameters:

  * `(mloc (SharedPtr T))`: The shared pointer.


Returns the underlying pointer.


#### `@`

Linkage: `extern`
Returns: `T`
Parameters:

  * `(mloc (SharedPtr T))`: The shared pointer.


Dereferencing the shared pointer returns the value from the
underlying pointer.




### Concept macros

#### `SharedPtr`

Linkage: `extern`
Parameters:

  * `(T Type)`: The type node.


Expands to a `SharedPtr` definition over the relevant type.




### Macros

#### `SharedPtr`

Linkage: `extern`
Parameters:

  * `T`: The type node.


Expands to the concrete type name of the `SharedPtr` generated by way
of the concept macro.


## <a name="unique-ptr"></a> 2.19 unique-ptr

### Details

Module: unique-ptr

### Description

A unique pointer module. Apart from the `UniquePtr` macro and concept
macro, the documentation in this module is for a generated unique
pointer instance of type `T`.



### Structs

#### `(UniquePtr T)`

Linkage: `N/A`
Members: N/A

The core unique pointer structure type.




### Functions

#### `init`

Linkage: `extern`
Returns: `bool`
Parameters:

  * `(loc (ref (UniquePtr T)))`: The unique pointer.
  * `(value (p T))`: The value to assign to the unique pointer.


Initialise the unique pointer structure with a pointer. Once the
structure has been initialised with the pointer, it takes
ownership of it. The structure assumes that the pointer was
created by way of malloc.


#### `init`

Linkage: `extern`
Returns: `bool`
Parameters:

  * `(loc (ref (UniquePtr T)))`: The unique pointer.


Initialise an empty/null unique pointer structure.


#### `get`

Linkage: `extern`
Returns: `(p T)`
Parameters:

  * `(mloc (ref (UniquePtr T)))`: The unique pointer.


Returns the underlying pointer.


#### `@`

Linkage: `extern`
Returns: `T`
Parameters:

  * `(mloc (ref (UniquePtr T)))`: The unique pointer.


Dereferencing the unique pointer returns the value from the
underlying pointer.




### Concept macros

#### `UniquePtr`

Linkage: `extern`
Parameters:

  * `(T Type)`: The type node.


Expands to a `UniquePtr` definition over the relevant type.




### Macros

#### `UniquePtr`

Linkage: `extern`
Parameters:

  * `T`: The type node.


Expands to the concrete type name of the `UniquePtr` generated by way
of the concept macro.


## <a name="operator-macros"></a> 2.20 operator-macros

### Details

Module: operator-macros

### Description

Provides utilities to generate operators that accept an arbitrary
number of arguments.  Such operators are already defined for the
following operators:

 - arithmetic operators: `+ - * / +' -' *' /'`
 - bitwise and logical or: `& | and or`
 - arithmetic relations: `< <= = => > <' <=' =' =>' >'


### Macros

#### `std.macros.def-identity-macro`

Linkage: `extern`
Parameters:

  * `name`: Name of the macro.
  * `linkage`: Linkage.


Defines a macro that takes a single argument and returns it.


#### `std.macros.def-right-associative-macro`

Linkage: `extern`
Parameters:

  * `name`: Name of the macro.
  * `linkage`: Linkage.
  * `opname`: Optional, defaults to `name`


Defines a macro:
`(name a b rest) will be expanded to `(opname a (name b rest))`


#### `std.macros.def-left-associative-macro`

Linkage: `extern`
Parameters:

  * `name`: Name of the macro.
  * `linkage`: Linkage.
  * `opname`: Optional, defaults to `name`


Defines a macro:
`(name a b rest)` will be expanded to `(name (opname a b) rest)`


#### `def-transitive-macro`

Linkage: `extern`
Parameters:

  * `name`: The macro name.
  * `linkage`: Linkage.
  * `fn`: The name of the binary function. Defaults to name.


Defines a macro for transitive binary functions (that normally should
return bool and transitive), so that you don't have to use variables
twice when you want to test some function.

For example:

        (def-transitive-macro < intern)
        (< 0.0 x 1.0)

expands to:

        (and (< 0.0 x) (< x 1.0))



## <a name="enum"></a> 2.21 enum

### Details

Module: enum

### Description

Enumerations ('enums') are declared like so:

        (def-enum {name} {linkage} {type}
          ({member1} {member2} ... {memberN}))

where `{linkage}` is one of `intern` and `extern`, and `{type}` is the
underlying integral type for the enum.

Each `{member}` is either a name atom, or a name-value pair. By
default, enum member values begin from zero. If a member does not have
an explicitly-defined value (i.e. it is a name atom), it takes the
value of the previous member, plus one. For example:

        (def-enum my-enum extern int
          (a b c (d 1) e (f 0) (g -1) h i j k))

The member-value assignments for this enum are as follows:

        a: 0; b:  1; c: 2; d: 1; e: 2; f: 0;
        g: -1; h: 0; i: 1; j: 2; k: 3

When an enum is defined, const variable bindings are introduced for
each enum element, mapping to the value for that element.

Enums are strongly-typed. The type name for an enum is `{name}`. When
an enum is defined, a series of related functions are defined at the
same time: `+`, `-`, `*`, `/`, `%`, `=`, `!=`, `<`, `<=`, `>`, `>=`,
`<<`, `>>`, `&`, `|` and `^`. These functions are in turn defined only
over that enum's type, so a function like `+`, for example, takes two
instances of the enum as its arguments and returns a new enum value as
its result. The exceptions to this are `<<` and `>>` (left and
right-shift), which take `int`s as their second arguments. Note that
enums can be cast to and from their underlying types, too.



### Macros

#### `def-enum`

Linkage: `extern`
Parameters:

  * `name`: The name of the new enum.
  * `linkage`: The linkage for the new enum.
  * `type`: The type for the new enum.
  * `forms`: The elements for the new enum.



## <a name="bitset-enum"></a> 2.22 bitset-enum

### Details

Module: bitset-enum

### Description

Provides `def-bitset-enum`, which allows for defining bitset enums.
These operate in the same way as normal enums, except that the
sequence goes 0, 1, 2, 4, ..., with each successive value being twice
that of the previous one.  For example,

        (def-bitset-enum my-enum intern int (a b c d))

expands to:

        (def-enum my-enum intern int ((a 0) (b 1) (c 2) (d 4)))

and

        (def-bitset-enum my-enum intern int ((a 0x2) b c d))

expands to:

        (def-enum my-enum intern int ((a 0x2) (b 0x4) (c 0x8) (d 0x10)))



### Macros

#### `def-bitset-enum`

Linkage: `extern`
Parameters:

  * `name`: The name of the new enum.
  * `linkage`: The linkage for the new enum.
  * `type`: The type for the new enum.
  * `forms`: The elements for the new enum.



## <a name="variant"></a> 2.23 variant

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



## <a name="cerrno"></a> 3.1 cerrno

### Details

Module: cerrno

### Description

Provides the `errno` macro, and the standard error constant macros:

  * ENOENT
  * ESRCH
  * EINTR
  * EIO
  * ENXIO
  * E2BIG
  * ENOEXEC
  * EBADF
  * ECHILD
  * EAGAIN
  * ENOMEM
  * EACCES
  * EFAULT
  * ENOTBLK
  * EBUSY
  * EEXIST
  * EXDEV
  * ENODEV
  * ENOTDIR
  * EISDIR
  * EINVAL
  * ENFILE
  * EMFILE
  * ENOTTY
  * ETXTBSY
  * EFBIG
  * ENOSPC
  * ESPIPE
  * EROFS
  * EMLINK
  * EPIPE
  * EDOM
  * ERANGE



### Macros

#### `errno`

Linkage: `extern`
Parameters: N/A

Expands to the current error number.


## <a name="cfloat"></a> 3.2 cfloat

### Details

Module: cfloat

### Description

Provides the following floating-point macro constants:

  * FLT-RADIX
  * FLT-ROUNDS
  * FLT-DIG
  * FLT-EPSILON
  * FLT-MANT-DIG
  * FLT-MAX
  * FLT-MAX-EXP
  * FLT-MIN
  * FLT-MIN-EXP
  * DBL-DIG
  * DBL-EPSILON
  * DBL-MANT-DIG
  * DBL-MAX
  * DBL-MAX-EXP
  * DBL-MIN
  * DBL-MIN-EXP
  * LDBL-DIG
  * LDBL-EPSILON
  * LDBL-MANT-DIG
  * LDBL-MAX
  * LDBL-MAX-EXP
  * LDBL-MIN
  * LDBL-MIN-EXP

## <a name="clocale"></a> 3.3 clocale

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



## <a name="cmath"></a> 3.4 cmath

### Details

Module: cmath

### Description

Bindings to `math.h`.



### Structs

#### `div-t`

Linkage: `extern`
Members:

  * `(quot int)`
  * `(rem int)`





### Functions

#### `log`

Linkage: `extern-c`
Returns: `double`
Parameters:

  * `(n double)`



#### `log10`

Linkage: `extern-c`
Returns: `double`
Parameters:

  * `(n double)`



#### `ceil`

Linkage: `extern-c`
Returns: `double`
Parameters:

  * `(n double)`



#### `floor`

Linkage: `extern-c`
Returns: `double`
Parameters:

  * `(n double)`



#### `fabs`

Linkage: `extern-c`
Returns: `double`
Parameters:

  * `(n double)`



#### `frexp`

Linkage: `extern-c`
Returns: `double`
Parameters:

  * `(n double)`
  * `(exp (p int))`



#### `modf`

Linkage: `extern-c`
Returns: `double`
Parameters:

  * `(n double)`
  * `(exp (p double))`



#### `fmod`

Linkage: `extern-c`
Returns: `double`
Parameters:

  * `(n double)`
  * `(m double)`



#### `asin`

Linkage: `extern-c`
Returns: `double`
Parameters:

  * `(n double)`



#### `acos`

Linkage: `extern-c`
Returns: `double`
Parameters:

  * `(n double)`



#### `atan`

Linkage: `extern-c`
Returns: `double`
Parameters:

  * `(n double)`



#### `atan2`

Linkage: `extern-c`
Returns: `double`
Parameters:

  * `(n double)`



#### `abs`

Linkage: `extern-c`
Returns: `int`
Parameters:

  * `(a int)`



#### `div`

Linkage: `extern-c`
Returns: `div-t`
Parameters:

  * `(num int)`
  * `(denom int)`



#### `cos`

Linkage: `extern-c`
Returns: `double`
Parameters:

  * `(n double)`



#### `cosf`

Linkage: `extern-c`
Returns: `float`
Parameters:

  * `(n float)`



#### `cosl`

Linkage: `extern-c`
Returns: `long-double`
Parameters:

  * `(n long-double)`



#### `sin`

Linkage: `extern-c`
Returns: `double`
Parameters:

  * `(n double)`



#### `sinf`

Linkage: `extern-c`
Returns: `float`
Parameters:

  * `(n float)`



#### `sinl`

Linkage: `extern-c`
Returns: `long-double`
Parameters:

  * `(n long-double)`



#### `sqrt`

Linkage: `extern-c`
Returns: `double`
Parameters:

  * `(n double)`



#### `sqrtf`

Linkage: `extern-c`
Returns: `float`
Parameters:

  * `(n float)`



#### `sqrtl`

Linkage: `extern-c`
Returns: `long-double`
Parameters:

  * `(n long-double)`



#### `pow`

Linkage: `extern-c`
Returns: `double`
Parameters:

  * `(n double)`
  * `(m double)`



#### `powf`

Linkage: `extern-c`
Returns: `float`
Parameters:

  * `(n float)`
  * `(m float)`



#### `powl`

Linkage: `extern-c`
Returns: `long-double`
Parameters:

  * `(n long-double)`
  * `(m long-double)`



## <a name="csignal"></a> 3.5 csignal

### Details

Module: csignal

### Description

Bindings to `signal.h`. Provides the following signal macro constants:

  * SIGHUP
  * SIGINT
  * SIGQUIT
  * SIGILL
  * SIGTRAP
  * SIGABRT
  * SIGIOT
  * SIGBUS
  * SIGFPE
  * SIGKILL
  * SIGUSR1
  * SIGSEGV
  * SIGUSR2
  * SIGPIPE
  * SIGALRM
  * SIGTERM
  * SIGSTKFLT
  * SIGCHLD
  * SIGCONT
  * SIGSTOP
  * SIGTSTP
  * SIGTTIN
  * SIGTTOU
  * SIGURG
  * SIGXCPU
  * SIGXFSZ
  * SIGVTALRM
  * SIGPROF
  * SIGWINCH
  * SIGIO
  * SIGPOLL
  * SIGPWR
  * SIGSYS
  * SIGUNUSED


### Functions

#### `signal`

Linkage: `extern-c`
Returns: `(p (fn (p void) ((n int))))`
Parameters:

  * `(sig int)`
  * `(handler (p (fn (p void) ((n int)))))`



#### `raise`

Linkage: `extern-c`
Returns: `int`
Parameters:

  * `(sig int)`



## <a name="cstdio-core"></a> 3.6 cstdio-core

### Details

Module: cstdio-core

### Description

Provides bindings for the parts of `stdio.h` on which the `macros`
module depends. This is separate from `cstdio`, so that `cstdio` can
depend in turn on `macros`.



### Structs

#### `file`

Linkage: `extern`
Members: N/A




### Functions

#### `strlen`

Linkage: `extern-c`
Returns: `size`
Parameters:

  * `(str (p (const char)))`



#### `strncpy`

Linkage: `extern-c`
Returns: `(p char)`
Parameters:

  * `(dest (p char))`
  * `(source (p (const char)))`
  * `(size size)`



#### `fprintf`

Linkage: `extern-c`
Returns: `int`
Parameters:

  * `(stream (p file))`
  * `(str (p (const char)))`
  * `...`



#### `printf`

Linkage: `extern-c`
Returns: `int`
Parameters:

  * `(str (p (const char)))`
  * `...`



#### `sprintf`

Linkage: `extern-c`
Returns: `int`
Parameters:

  * `(str (p char))`
  * `(fmt (p (const char)))`
  * `...`



#### `strchr`

Linkage: `extern-c`
Returns: `(p char)`
Parameters:

  * `(str (p char))`
  * `(c int)`



#### `memset`

Linkage: `extern-c`
Returns: `(p void)`
Parameters:

  * `(a (p void))`
  * `(b int)`
  * `(c size)`



#### `strcmp`

Linkage: `extern-c`
Returns: `int`
Parameters:

  * `(str (p (const char)))`
  * `(str2 (p (const char)))`



#### `strncmp`

Linkage: `extern-c`
Returns: `int`
Parameters:

  * `(str (p (const char)))`
  * `(str2 (p (const char)))`
  * `(n size)`



## <a name="cstdio"></a> 3.7 cstdio

### Details

Module: cstdio

### Description

Bindings to `stdio.h`. Provides the following macro constants:

  * L-tmpnam
  * TMP-MAX
  * FILENAME-MAX
  * FOPEN-MAX
  * RAND-MAX
  * EXIT-FAILURE
  * EXIT-SUCCESS



### Variables

#### `stdin`

Linkage: `extern-c`
Type: `(p file)`

The standard input handle.


#### `stdout`

Linkage: `extern-c`
Type: `(p file)`

The standard output handle.


#### `stderr`

Linkage: `extern-c`
Type: `(p file)`

The standard error handle.




### Structs

#### `file`

Linkage: `extern`
Members: N/A


#### `buffer-mode`

Linkage: `extern`
Members:

  * `(value int)`



#### `fseekorigin`

Linkage: `extern`
Members:

  * `(value int)`



#### `fpos`

Linkage: `extern`
Members:

  * `(n (array-of (MFPOS_T) char))`





### Functions

#### `fdopen`

Linkage: `extern-c`
Returns: `(p file)`
Parameters:

  * `(fd int)`
  * `(mode (p (const char)))`



#### `fopen`

Linkage: `extern-c`
Returns: `(p file)`
Parameters:

  * `(filename (p (const char)))`
  * `(mode (p (const char)))`



#### `freopen`

Linkage: `extern-c`
Returns: `(p file)`
Parameters:

  * `(filename (p (const char)))`
  * `(mode (p (const char)))`
  * `(stream (p file))`



#### `fflush`

Linkage: `extern-c`
Returns: `int`
Parameters:

  * `(stream (p file))`



#### `fclose`

Linkage: `extern-c`
Returns: `int`
Parameters:

  * `(stream (p file))`



#### `remove`

Linkage: `extern-c`
Returns: `int`
Parameters:

  * `(filename (p (const char)))`



#### `rename`

Linkage: `extern-c`
Returns: `int`
Parameters:

  * `(old-name (p (const char)))`
  * `(new-name (p (const char)))`



#### `tmpfile`

Linkage: `extern-c`
Returns: `(p file)`
Parameters: N/A


#### `tmpnam`

Linkage: `extern-c`
Returns: `(p char)`
Parameters:

  * `(buffer (p char))`



#### `setvbuf`

Linkage: `extern-c`
Returns: `int`
Parameters:

  * `(stream (p file))`
  * `(buf (p char))`
  * `(mode buffer-mode)`
  * `(size size)`



#### `setbuf`

Linkage: `extern-c`
Returns: `void`
Parameters:

  * `(stream (p file))`
  * `(buf (p char))`



#### `fprintf`

Linkage: `extern-c`
Returns: `int`
Parameters:

  * `(stream (p file))`
  * `(str (p (const char)))`
  * `...`



#### `printf`

Linkage: `extern-c`
Returns: `int`
Parameters:

  * `(str (p (const char)))`
  * `...`



#### `sprintf`

Linkage: `extern-c`
Returns: `int`
Parameters:

  * `(str (p char))`
  * `(fmt (p (const char)))`
  * `...`



#### `vprintf`

Linkage: `extern-c`
Returns: `int`
Parameters:

  * `(str (p (const char)))`
  * `(arg va-list)`



#### `vfprintf`

Linkage: `extern-c`
Returns: `int`
Parameters:

  * `(stream (p file))`
  * `(str (p (const char)))`
  * `(arg va-list)`



#### `vsprintf`

Linkage: `extern-c`
Returns: `int`
Parameters:

  * `(str (p char))`
  * `(fmt (p (const char)))`
  * `(arg va-list)`



#### `fgetc`

Linkage: `extern-c`
Returns: `int`
Parameters:

  * `(stream (p file))`



#### `fgets`

Linkage: `extern-c`
Returns: `(p char)`
Parameters:

  * `(str (p char))`
  * `(n int)`
  * `(stream (p file))`



#### `fputc`

Linkage: `extern-c`
Returns: `int`
Parameters:

  * `(c int)`
  * `(stream (p file))`



#### `fputs`

Linkage: `extern-c`
Returns: `int`
Parameters:

  * `(str (p (const char)))`
  * `(stream (p file))`



#### `getc`

Linkage: `extern-c`
Returns: `int`
Parameters:

  * `(stream (p file))`



#### `getchar`

Linkage: `extern-c`
Returns: `int`
Parameters:

  * `(stream (p file))`



#### `gets`

Linkage: `extern-c`
Returns: `(p char)`
Parameters:

  * `(str (p char))`



#### `putc`

Linkage: `extern-c`
Returns: `int`
Parameters:

  * `(c int)`
  * `(stream (p file))`



#### `putchar`

Linkage: `extern-c`
Returns: `int`
Parameters:

  * `(c int)`



#### `puts`

Linkage: `extern-c`
Returns: `int`
Parameters:

  * `(str (p (const char)))`



#### `ungetc`

Linkage: `extern-c`
Returns: `int`
Parameters:

  * `(c int)`
  * `(stream (p file))`



#### `fread`

Linkage: `extern-c`
Returns: `size`
Parameters:

  * `(ptr (p void))`
  * `(size size)`
  * `(count size)`
  * `(stream (p file))`



#### `fwrite`

Linkage: `extern-c`
Returns: `size`
Parameters:

  * `(ptr (p void))`
  * `(size size)`
  * `(count size)`
  * `(stream (p file))`



#### `fseek`

Linkage: `extern-c`
Returns: `int`
Parameters:

  * `(stream (p file))`
  * `(offset int)`
  * `(origin fseekorigin)`



#### `ftell`

Linkage: `extern-c`
Returns: `int`
Parameters:

  * `(stream (p file))`



#### `rewind`

Linkage: `extern-c`
Returns: `void`
Parameters:

  * `(stream (p file))`



#### `fgetpos`

Linkage: `extern-c`
Returns: `int`
Parameters:

  * `(stream (p file))`
  * `(ptr (p fpos))`



#### `fsetpos`

Linkage: `extern-c`
Returns: `int`
Parameters:

  * `(stream (p file))`
  * `(ptr (p fpos))`



#### `clearerr`

Linkage: `extern-c`
Returns: `void`
Parameters:

  * `(stream (p file))`



#### `feof`

Linkage: `extern-c`
Returns: `int`
Parameters:

  * `(stream (p file))`



#### `ferror`

Linkage: `extern-c`
Returns: `int`
Parameters:

  * `(stream (p file))`



#### `perror`

Linkage: `extern-c`
Returns: `void`
Parameters:

  * `(str (p (const char)))`





### Macros

#### `MFPOS_T`

Linkage: `extern`
Parameters: N/A


## <a name="ctime"></a> 3.8 ctime

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


## <a name="cstdlib"></a> 3.9 cstdlib

### Details

Module: cstdlib

### Description

Bindings to `stdlib.h`.

The `strto*` functions, with the exception of `strtod`, should not be used
directly. Instead, use the functions with the explicit type names: e.g.
`strtoi32`.



### Functions

#### `atexit`

Linkage: `extern-c`
Returns: `int`
Parameters:

  * `(fcn (p (fn void (void))))`



#### `system`

Linkage: `extern-c`
Returns: `int`
Parameters:

  * `(s (p (const char)))`



#### `getenv`

Linkage: `extern-c`
Returns: `(p (const char))`
Parameters:

  * `(s (p (const char)))`



#### `bsearch`

Linkage: `extern-c`
Returns: `(p void)`
Parameters:

  * `(key (p void))`
  * `(base (p void))`
  * `(n size)`
  * `(size size)`
  * `(cmp (p (fn int ((keyval (p void)) (datum (p void))))))`



#### `qsort`

Linkage: `extern-c`
Returns: `void`
Parameters:

  * `(base (p void))`
  * `(n size)`
  * `(size size)`
  * `(cmp (p (fn int ((keyval (p void)) (datum (p void))))))`



#### `exit`

Linkage: `extern-c`
Returns: `int`
Parameters:

  * `(exit-code int)`



#### `abort`

Linkage: `extern-c`
Returns: `void`
Parameters: N/A


#### `malloc`

Linkage: `extern-c`
Returns: `(p void)`
Parameters:

  * `(num size)`



#### `free`

Linkage: `extern-c`
Returns: `void`
Parameters:

  * `(ptr (p void))`



#### `calloc`

Linkage: `extern-c`
Returns: `(p void)`
Parameters:

  * `(num size)`
  * `(size size)`



#### `realloc`

Linkage: `extern-c`
Returns: `(p void)`
Parameters:

  * `(ptr (p void))`
  * `(size size)`



#### `atoi`

Linkage: `extern-c`
Returns: `int`
Parameters:

  * `(str (p (const char)))`



#### `atof`

Linkage: `extern-c`
Returns: `double`
Parameters:

  * `(str (p (const char)))`



#### `strtod`

Linkage: `extern-c`
Returns: `double`
Parameters:

  * `(str (p (const char)))`
  * `(endp (p (const char)))`



#### `strtol`

Linkage: `extern-c`
Returns: `(long-type)`
Parameters:

  * `(str (p (const char)))`
  * `(endp (p (p (const char))))`
  * `(base int)`



#### `strtoul`

Linkage: `extern-c`
Returns: `(ulong-type)`
Parameters:

  * `(str (p (const char)))`
  * `(endp (p (p (const char))))`
  * `(base int)`



#### `strtoll`

Linkage: `extern-c`
Returns: `(long-long-type)`
Parameters:

  * `(str (p (const char)))`
  * `(endp (p (p (const char))))`
  * `(base int)`



#### `strtoull`

Linkage: `extern-c`
Returns: `(ulong-long-type)`
Parameters:

  * `(str (p (const char)))`
  * `(endp (p (p (const char))))`
  * `(base int)`



#### `strtoi32`

Linkage: `extern`
Returns: `int32`
Parameters:

  * `(str (p (const char)))`
  * `(endp (p (p (const char))))`
  * `(base int)`



#### `strtoi64`

Linkage: `extern`
Returns: `int64`
Parameters:

  * `(str (p (const char)))`
  * `(endp (p (p (const char))))`
  * `(base int)`



#### `strtoui32`

Linkage: `extern`
Returns: `uint32`
Parameters:

  * `(str (p (const char)))`
  * `(endp (p (p (const char))))`
  * `(base int)`



#### `strtoui64`

Linkage: `extern`
Returns: `uint64`
Parameters:

  * `(str (p (const char)))`
  * `(endp (p (p (const char))))`
  * `(base int)`



#### `rand`

Linkage: `extern-c`
Returns: `int`
Parameters: N/A


#### `srand`

Linkage: `extern-c`
Returns: `void`
Parameters:

  * `(seed uint)`



## <a name="cstring"></a> 3.10 cstring

### Details

Module: cstring

### Description

Bindings to `string.h`.



### Functions

#### `strcpy`

Linkage: `extern-c`
Returns: `int`
Parameters:

  * `(dest (p char))`
  * `(source (const (p (const char))))`



#### `strncpy`

Linkage: `extern-c`
Returns: `(p char)`
Parameters:

  * `(dest (p char))`
  * `(source (p (const char)))`
  * `(size size)`



#### `strcat`

Linkage: `extern-c`
Returns: `int`
Parameters:

  * `(str (p char))`
  * `(str2 (p (const char)))`



#### `strncat`

Linkage: `extern-c`
Returns: `int`
Parameters:

  * `(str (p char))`
  * `(str2 (p char))`
  * `(size size)`



#### `strcmp`

Linkage: `extern-c`
Returns: `int`
Parameters:

  * `(str (p char))`
  * `(str2 (p char))`



#### `strncmp`

Linkage: `extern-c`
Returns: `int`
Parameters:

  * `(str (p char))`
  * `(str2 (p char))`
  * `(n size)`



#### `strchr`

Linkage: `extern-c`
Returns: `(p char)`
Parameters:

  * `(str (p char))`
  * `(c int)`



#### `strrchr`

Linkage: `extern-c`
Returns: `(p (const char))`
Parameters:

  * `(str (p (const char)))`
  * `(c int)`



#### `strspn`

Linkage: `extern-c`
Returns: `size`
Parameters:

  * `(str (p char))`
  * `(str2 (p char))`



#### `strcspn`

Linkage: `extern-c`
Returns: `size`
Parameters:

  * `(str (p char))`
  * `(str2 (p char))`



#### `strpbrk`

Linkage: `extern-c`
Returns: `(p char)`
Parameters:

  * `(str (p char))`
  * `(str2 (p char))`



#### `strstr`

Linkage: `extern-c`
Returns: `(p (const char))`
Parameters:

  * `(str (p (const char)))`
  * `(str2 (p (const char)))`



#### `strlen`

Linkage: `extern-c`
Returns: `size`
Parameters:

  * `(str (p (const char)))`



#### `strerror`

Linkage: `extern-c`
Returns: `(p char)`
Parameters:

  * `(n size)`



#### `strtok`

Linkage: `extern-c`
Returns: `(p char)`
Parameters:

  * `(str (p char))`
  * `(str2 (p char))`



#### `memcpy`

Linkage: `extern-c`
Returns: `(p void)`
Parameters:

  * `(str (p void))`
  * `(str2 (p void))`
  * `(n size)`



#### `memmove`

Linkage: `extern-c`
Returns: `(p void)`
Parameters:

  * `(str (p void))`
  * `(str2 (p void))`
  * `(n size)`



#### `memcmp`

Linkage: `extern-c`
Returns: `int`
Parameters:

  * `(str (p void))`
  * `(str2 (p void))`
  * `(n size)`



#### `memchr`

Linkage: `extern-c`
Returns: `(p void)`
Parameters:

  * `(a (p void))`
  * `(b int)`
  * `(c size)`



#### `memset`

Linkage: `extern-c`
Returns: `(p void)`
Parameters:

  * `(a (p void))`
  * `(b int)`
  * `(c size)`



## <a name="cctype"></a> 3.11 cctype

### Details

Module: cctype

### Description

Bindings to `ctype.h`. These bindings are awkward to use, because each
takes an `int`, instead of a `char`. See the `ctype` module for a
neater interface.



### Functions

#### `isalnum`

Linkage: `extern-c`
Returns: `int`
Parameters:

  * `(c int)`: A character as an integer.


Determine whether a character is alphanumeric.


#### `isalpha`

Linkage: `extern-c`
Returns: `int`
Parameters:

  * `(c int)`: A character as an integer.


Determine whether a character is alphabetical.


#### `iscntrl`

Linkage: `extern-c`
Returns: `int`
Parameters:

  * `(c int)`: A character as an integer.


Determine whether a character is a control character.


#### `isdigit`

Linkage: `extern-c`
Returns: `int`
Parameters:

  * `(c int)`: A character as an integer.


Determine whether a character is a digit.


#### `isgraph`

Linkage: `extern-c`
Returns: `int`
Parameters:

  * `(c int)`: A character as an integer.


Determine whether a character is printable (excludes space).


#### `islower`

Linkage: `extern-c`
Returns: `int`
Parameters:

  * `(c int)`: A character as an integer.


Determine whether a character is lowercase.


#### `isprint`

Linkage: `extern-c`
Returns: `int`
Parameters:

  * `(c int)`: A character as an integer.


Determine whether a character is printable (includes space).


#### `ispunct`

Linkage: `extern-c`
Returns: `int`
Parameters:

  * `(c int)`: A character as an integer.


Determine whether a character is a punctuation mark.


#### `isspace`

Linkage: `extern-c`
Returns: `int`
Parameters:

  * `(c int)`: A character as an integer.


Determine whether a character is a whitespace character.


#### `isupper`

Linkage: `extern-c`
Returns: `int`
Parameters:

  * `(c int)`: A character as an integer.


Determine whether a character is uppercase.


#### `isxdigit`

Linkage: `extern-c`
Returns: `int`
Parameters:

  * `(c int)`: A character as an integer.


Determine whether a character is a hexadecimal digit.


#### `tolower`

Linkage: `extern-c`
Returns: `int`
Parameters:

  * `(c int)`: A character as an integer.


Convert a character into a lowercase character, if possible.


#### `toupper`

Linkage: `extern-c`
Returns: `int`
Parameters:

  * `(c int)`: A character as an integer.


Convert a character into an uppercase character, if possible.


## <a name="csetjmp"></a> 3.12 csetjmp

### Details

Module: csetjmp

### Description

Bindings to `setjmp.h`. 

Note that destructors will not work properly when `setjmp`/`longjmp`
are used.



### Structs

#### `jmpbuf`

Linkage: `extern`
Members:

  * `(a (array-of (MJMP_BUF_SIZE) char))`





### Functions

#### `setjmp`

Linkage: `extern-c`
Returns: `int`
Parameters:

  * `(a (p jmpbuf))`



#### `longjmp`

Linkage: `extern-c`
Returns: `int`
Parameters:

  * `(a (p jmpbuf))`
  * `(b int)`



