# Dale

[Previous](./1-9-namespaces.md) | [Next](./1-11-form-reference.md)

## 1.10 Modules

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
[mawled/issues/12](https://github.com/tomhrr/mawled/issues/12) for an
example case.

[Previous](./1-9-namespaces.md) | [Next](./1-11-form-reference.md)
