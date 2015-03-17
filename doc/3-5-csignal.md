# Dale

[Previous](./3-4-cmath.md) | [Next](./3-6-cstdio-core.md)

## 3.5 csignal

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



[Previous](./3-4-cmath.md) | [Next](./3-6-cstdio-core.md)

