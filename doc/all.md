# Dale

## <a name="Introduction"></a> 1.1 Introduction

Dale is an unsafe (no GC), homoiconic programming language. It uses
S-expressions for its syntax, and is essentially an experiment in
seeing how a language at this level can work when it has 'proper'
macros. The basic language is similar to C; the notable additions are:

  * local type deduction;
  * overloaded functions;
  * anonymous functions;
  * function structs;
  * overridden copy/assign operations;
  * destructors;
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
    single types; i.e. `(+ int int)` is defined by default, but `(+ int
    float)` is not.

  * Linkage is not implicit in the core language. This extends to
    local variables in functions and macros. (In practice, the `let`
    macro is generally used to introduce local variable bindings, so
    this isn't a significant problem.)

Throughout the documentation, the term 'procedure' is used to refer to
functions and macros as a single group.

The language is still very much at the experimental/toy stage. There
are likely to be significant bugs and other problems, as well as
documentation inconsistencies. The compiler itself is very slow and
leaky, the generated code is not very good and the code (both for the
compiler and the libraries) is best avoided.



## <a name="Setup"></a> 1.2 Setup

The Dale compiler is written in C++. It does not depend on any C++11
features, so most versions of clang/gcc should suffice.

### External dependencies

  * LLVM (3.0-3.5)
  * libffi

### Tested platforms

  * Linux (Debian), x86 and x86-64

### Building

From the 'dale' directory, for an out-of-tree (recommended) build:

        mkdir ../build
        cd ../build
        cmake ../dale
        make
        make tests
        make install
    
cmake needs to know the location of the ffi.h header. If this isn't
found in a default location, then it has to be provided during the
third step: e.g.

        cmake ../dale -DFFI_INCLUDE_DIR=/usr/include/i386-linux-gnu/

By default, the installation prefix is `/usr/local/`. The compiler
executable (`dalec`) is installed to `/usr/local/bin`, its manpage is
installed to `/usr/local/share/man`, and all libraries are installed
to `/usr/local/lib/dale`.

The tests are written using Perl. `IPC::Run` is the only non-core
dependency required for testing: it can be installed by running `cpan
IPC::Run`, or by way of your system package manager.

### Hello world

Copy the following into a file called `hello-world.dt`:

        (import cstdio)
    
        (def main (fn extern-c int (void)
          (printf "Hello, world\n")))
    
Compile it and run it:

        user@main:~$ dalec hello-world.dt
        user@main:~$ ./a.out
        Hello, world
        user@main:~$

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
is a literal unsigned integer. A zero-sized array may be declared when
the array is populated during declaration; see
[Variables](#Variables).

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
include the encloding ({name} ...) form.

### Enumerations

Enumerations ('enums') are declared like so:

        (def {name}
          (enum {linkage} {type} ({member1} {member2} ... {memberN})))

where `{linkage}` is one of `intern` and `extern`, and `{type}` is the
underlying integral type for the enum.

Each `{member}` is either a name atom, or a name-value pair. By
default, enum member values begin from zero. If a member does not have
an explicitly-defined value (i.e. it is a name atom), it takes the
value of the previous member, plus one. For example:

        (def my-enum
          (enum extern int (a b c (d 1) e (f 0) (g -1) h i j k)))

The member-value assignments for this enum are as follows:

  * a: 0
  * b: 1
  * c: 2
  * d: 1
  * e: 2
  * f: 0
  * g: -1
  * h: 0
  * i: 1
  * j: 2
  * k: 3

Enum literals may be used wherever a reference to an enum may be used.
They take the form `({name} {member-name})`. Similarly to structs, the
`{member-name}` alone can be used where the value will be cast
implicitly.

Enums are strongly-typed. The type name for an enum is `{name}`. When
an enum is defined, a series of related functions are defined at the
same time: `+`, `-`, `*`, `/`, `=`, `!=`, `<`, `<=`, `>`, `>=`, `<<`,
`>>`, `&`, `|` and `^`. These functions are in turn defined only over
that enum's type, so a function like `+`, for example, takes two
instances of the enum as its arguments and returns a new enum value as
its result. The exceptions to this are `<<` and `>>` (left and
right-shift), which take `int`s as their second arguments. Note that
enums can be cast to and from their underlying types, too.

### Other types

Function pointers, anonymous functions and function structs are
described in the [Functions](#Functions) section.



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

`{linkage}` is one of `intern`, `extern`, `extern-c` and `auto`. The
other placeholders are self-explanatory. `auto` is only valid for
procedure-scoped variables.

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
defining functions named `setf-copy` and `setf-assign`; these are
discussed in more detail in [Copy/destroy](#Copy/destroy).

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

The above example is much saner when local type deduction is used:

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
to referenced type'; i.e., a reference is not a separate type as such.
For example:

        (def add-2-ints (fn intern int ((a (ref int)) (b (ref int)))
          (return (+ (@ a) (@ b)))))

Values passed by reference must be addressable, unless the reference
is to a constant type.

### Retvals

A function's return type may be marked as `retval`. Such a function,
rather than returning a value using `return`, writes the result to the
binding `retval`, which is provided implicitly by the compiler. That
binding has the type 'pointer to actual return type', and its value is
used as the return value of the function. `retval` allows for avoiding
unnecessary allocations/copies. For example:

        (def add-2-ints (fn intern (retval int) ((a int) (b int))
          (setf retval (+ a b))
          (return)))

        ...

        (def n (var auto int (add-2-ints 5 10)))
        (printf "%d\n" n) ; prints "15\n"

(As with reference parameters, `int` is not a type for which this
would be used in practice.)

### Function structs

A function struct is a struct that has a member named `apply`, which
member's type is a pointer to a function that takes a pointer to the
struct as its first argument, and arbitrary other arguments. For
example:

        (def adder
          (struct intern 
            ((apply (p (fn int ((self (p adder)) (m int)))))
             (n int))))

Function structs may be 'called' in the same fashion as a function:

        (def add
          (fn intern int ((self (p adder)) (m int))
            (setf (:@ self n) (+ m (@:@ self n)))
            (return (@:@ self n))))

        (def main
          (fn extern-c int (void)
            (def m (var auto adder ((apply (# add (p adder))) (m 0))))
            (m 1) (m 2) (m 3)
            (printf "%d\n" (m 0)) ; prints "6\n"
            0))

(The terminology 'function struct' is used to avoid any confusion with
C++'s function objects, which are quite different.)



## <a name="Control flow"></a> 1.6 Control flow

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
list. If `is-list` is zero, then the node is a non-list node, and
`token-str` will contain the node's content. `next-node` is applicable
to both list and non-list nodes, and points to the next node after the
current node.

The return type of all macros is a pointer to a `DNode`. This value
will replace the call to the macro. If the value is a null pointer,
then the call to the macro will be removed (i.e. the call will be a
no-op).

### Macro context

The body of a macro has access to a macro context argument named
'mc'. This argument is of type `(p MContext)`, where `MContext` is an
opaque struct type. The context can be used to allocate memory for use
within the macro by way of the `pool-malloc` function, which has the
following prototype:

        (def pool-malloc
          (fn extern (p void) ((pool-node (p PoolNode)) (n size))))

Memory allocated by way of this function will be freed by the compiler
after it has finished evaluating the macro.

The number of arguments passed to the macro can be retrieved by way of
the `arg-count` function, which takes the context as its single
argument. This function is present so that varargs macros can be
supported without requiring macro users to also provide the number of
arguments/forms being passed to the macro. Varargs macros otherwise
operate in the same way as varargs functions.

Aside from these, each of the [introspection](#introspection)
functions takes a macro context as its first argument.

### Examples

The identity macro is like so:

        (def identity (macro extern (N) N))

        ; ...

        (identity (+ 1 1)) ; becomes (+ 1 1)

As per functions, the last value evaluated within the body of the
macro is returned implicitly, in the absence of an explicit `return`
instruction.

There are two principal standard libraries that deal with macros:
[`macros-core`](#macros-core) and [`macros`](#macros).
The former provides many functions that ease macro authorship. The
latter provides the quasiquotation (`qq`) macro, which is one of the
more useful macro-writing macros. For example, an `unless` (opposite
of `if`) macro would look like so, when using `qq`:

        (def unless (macro extern (expr tc fc)
          (qq if (not (uq expr)) (uq tc) (uq fc))))

`uq` is a form that 'unquotes' the specified `DNode`, when used within
a `qq` form. There is a corresponding `uql` form ('unquote-list'),
that takes the list from the specified `DNode` and 'splices' it into
place.

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

### Idempotence

Macros may be evaluated more than once per call site by the compiler,
so it is important that they are idempotent. One exception to this is
that any errors reported by the macro to the compiler, by way of the
`report-error` function (see
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



## <a name="Copy/destroy"></a> 1.8 Copy/destroy

The `setf` core form may be overridden for a particular type, in
effect, by defining functions named `setf-copy` and `setf-assign`.  A
corresponding destructor for the type may be defined by implementing a
function named `destroy`, which function takes a pointer to the
relevant type as its single argument.

### `setf-copy`

A `setf-copy` function is defined like so:

        (def setf-copy (fn {linkage} bool ((dst (p {source-type}))
                                           (src (const {source-type})))
          {body}))

Such functions will be used in lieu of the default `setf` behaviour
whenever `dst` is uninitialised.

### `setf-assign`

`setf-assign` functions are defined in the same manner as `setf-copy`
functions, save for the name. They are used whenever `dst` has already
been initialised.

Since the compiler cannot determine whether a particular variable or
value has actually been initialised in all cases, it is important that
all declared variables are initialised properly, and as early as
possible.

Multiple `setf` functions, supporting different source types, may be
defined over a single destination type.

### `destroy`

Destructors are defined like so:

        (def destroy (fn {linkage} void ((val {pointer-to-type}))
          {body}))

They are run when a value of the relevant type goes out of scope.

If `destroy` is not defined over a particular type, then any attempt
to call `destroy` on a pointer to that type will become a no-op.



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
preprocessor. 

To prevent a file from being included more than once, the core form
`once` can be used as the first form within the file. It takes an
identifier as its single argument. If a file is subsequently loaded by
way of `include`, and that file's first form is a call to `once` with
the same identifier, the file's remaining forms will be ignored.



## <a name="Form reference"></a> 1.11 Form reference

In addition to the core forms, there are a number of core macros,
functions, structs and variables provided by the compiler to each
program by default. This reference is limited to forms usable from
within procedures.

### Core forms

#### (`setf` {`destination`} {`source`})

Copies `source` to `destination`.

May be overridden by way of `setf-copy` and `setf-assign`.

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

#### (`funcall` {`function-pointer`} {`arg1`} *`arg2`* ... *`argN`*)

Executes `function-pointer` using the provided arguments, and returns
its result.

#### (`do` {`form1`} {`form2`} ... *`formN`*)

Evaluates each form in order. Returns the result of evaluating the
final form.

#### (`new-scope` {`form1`} {`form2`} ... *`formN`*)

Introduces a new lexical scope, and evaluates each form within that
scope. As per `do`, the result is that of the final form.

#### (`label` {`name`})

Marks a point in the code, for later use by `goto`.

#### (`goto` {`label-name`})

Branches unconditionally to the specified label and continues
execution.

#### (`if` {`condition`} {`true-case`} *`false-case`*)

Executes `true-case` if `condition` is true; otherwise, executes
`false-case`.

#### (`return` {`value`})

Returns `value` from the current procedure to the caller.

#### (`va-start` {`pointer-to-va-list`})

Initialises the `va-list`, for later use with `va-arg`.

#### (`va-arg` {`pointer-to-va-list`} {`argument-type`})

Returns the next argument from the `va-list`, cast to the specified
type.

#### (`va-end` {`pointer-to-va-list`})

Performs any necessary cleanup required by way of the use of the
`va-list`. 

#### (`array-of` {`count`} {`type`} (`array` *`a1`* *`a2`* ... *`aN`*))

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

#### (`using-namespace` {`name`} {`form1`} *`form2`* ... *`formN`*)

Adds the specified namespace to the list of namespaces used for
lookup, and evaluates each of the forms in that context. As per `do`,
the result is that of the final form.

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

  * `+`, `-`, `*`, `/`, `=`, `!=`, `<`, `<=`, `>`, `>=` (all)
  * `<<`, `>>`, `&`, `|`, `^` (integral types only)

They operate in the same way as normal functions, i.e. they can be
addressed, cf. the core forms and macros. The last five operations are
'left shift', 'right shift', 'bitwise and', 'bitwise or' and 'bitwise
xor', respectively.

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

#### `PoolNode` (opaque)

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

## <a name="introspection"></a> 2.1 introspection

