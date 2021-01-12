# Dale

[Previous](./1-1-introduction.md) | [Next](./1-3-types.md)

## 1.2 Setup

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

[Previous](./1-1-introduction.md) | [Next](./1-3-types.md)
