# Dale

[Previous](./1-8-init-destroy.md) | [Next](./1-10-modules.md)

## 1.9 Namespaces

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
[Variables](./1-4-variables.md), introduces a new anonymous namespace
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

[Previous](./1-8-init-destroy.md) | [Next](./1-10-modules.md)
