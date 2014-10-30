## Dale

Dale is a system (no GC) programming language with a homoiconic
syntax.

### Supported systems

Tested on Linux (Debian) x86 and x86-64.

### Documentation

See the /doc directory ([index](./doc/index.md)).

### Install

#### Dependencies

  * LLVM (3.0-3.5)
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

### Bugs/problems/suggestions

Please report to the [GitHub issue tracker](https://github.com/tomhrr/dale/issues).

### Licence

See LICENCE.
