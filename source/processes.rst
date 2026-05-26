Introduction to Processes
===========================

.. index::
   single: process
   single: address space
   pair: process; definition

A process is an executing program. It has a memory address space, a
program image, handles to files, and the operating system state needed
for interprocess communication.

.. index::
   single: process tree
   single: init
   pair: UNIX; init
   single: system.exe
   single: csrss.exe
   single: wininit.exe
   single: explorer.exe
   pair: Windows; process hierarchy

Every process has a parent. In traditional UNIX systems, the top of the
process tree is ``init``. On Windows, several system processes fill
similar roles, including ``system.exe`` for the kernel, ``csrss.exe`` for
user-mode Win32 support, ``wininit.exe`` for system services,
``explorer.exe`` for the shell, and ``winlogin.exe`` for login services.

Process Memory Layout
---------------------

.. index::
   single: process memory layout
   single: text segment
   single: data segment
   single: BSS segment
   pair: memory; process layout

A process address space is normally divided into a few major regions.
The text segment contains the machine instructions of the program. The
data segment contains initialized static data and constants. The BSS
segment contains uninitialized static data.

.. index::
   single: heap
   single: stack
   pair: memory; heap
   pair: memory; stack
   single: user-mode threads
   single: kernel-mode threads

The heap is used for dynamic memory allocation. A process may have one
or more heaps. The stack contains local variables, call frames, and
return values. Kernel-mode threads normally have one stack per thread.
User-mode threading libraries may multiplex many user threads over a
smaller number of kernel-thread stacks.

.. index::
   single: loader
   pair: memory; loader

The loader is responsible for laying out the text, data, BSS, initial
heap, and initial stack. After the process starts, stack and heap
management are handled by the program, its runtime library, and the
operating system.

Process Memory Layout
---------------------

    .. figure:: processes/diagrams/memory_layout.png
       :align: center
       :alt: Memory Layout

       Memory Layout

Multithreaded Memory Layout
---------------------------

    .. figure:: processes/diagrams/memory_layout_multithreaded.png
       :align: center
       :alt: Memory Layout for Multithreaded Programs

       Memory Layout for Multithreaded Programs

Preliminaries
-----------------

The code examples are maintained at |systems-code-examples-url|. You can
use |systems-code-examples-clone| to clone them into a folder named
``systems-code-examples``.

Full working demonstrations are referenced by directory name, such as
``systems-code-examples/fork``. To build most examples, install ``gcc``,
``cmake``, and ``make``. The examples have been tested on Ubuntu Linux,
macOS, and Windows Subsystem for Linux 2 with Ubuntu.

Most examples use the same pattern::

   $ cd systems-code-examples/<example-name>
   $ cmake .
   $ make

The output executable is normally written to the example's ``bin``
subdirectory.

Examine Process Layout Example
---------------------------------

The ``systems-code-examples/c_intro`` example is a small C++ program used
to examine process layout and the basic build process. It is also a
useful first example because it has separate source files and a CMake
build.

Get and build the code::

   $ git clone https://github.com/SoftwareSystemsLaboratory/systems-code-examples
   $ cd systems-code-examples/c_intro
   $ cmake .
   $ make

Run the ``layout`` shell script to show the size of the text, data, and
BSS sections in bytes::

   $ ./layout

   section               size   addr
   .text                 3957    4352
   .data                   24   20480
   .bss                     8   20504

The main program for this example is:

.. literalinclude:: ../examples/systems-code-examples/c_intro/main.cc
   :language: c++
   :linenos:

Key points:

- The program is split across multiple source files, so the build has to
  compile each file and link the resulting object files.
- The example gives us an executable we can inspect with tools such as
  ``size`` and ``nm``.
- The ``layout`` script connects the compiled program back to the text,
  data, and BSS sections discussed above.

Loading Programs
----------------

.. index::
   single: program loading
   pair: loader; shared libraries
   single: nm command
   single: _start()
   single: _init()
   single: main()

When a program is loaded, the loader allocates memory for the executable
text, data, BSS, heap, and stack. It then loads the program image into
memory.

The loader also handles shared libraries. It asks the operating system
which shared libraries are already loaded and loads the ones that are
not. Each shared library has its own text, data, and BSS sections. The
loader then resolves external symbols so references in the executable
point to the correct locations in memory.

The ``nm`` command shows symbols in a compiled object file or executable.
After the executable is ready, the loader invokes ``_start()``.
``_start()`` calls ``_init()`` for each shared library, initializes static
constructors for global objects, and then calls ``main()``.

Simple Executable Example
-------------------------

The ``systems-code-examples/hello-exe`` example is the smallest complete
executable example in the repository. It is useful for confirming the
build process before looking at larger programs.

.. literalinclude:: ../examples/systems-code-examples/hello-exe/demo.c
   :language: c
   :linenos:

Key points:

- ``main()`` is the entry point that application programmers normally
  write, even though the loader starts execution before ``main()``.
- The program calls a function from a header-defined interface rather
  than doing all work inline.
- This is a minimal executable for checking compiler, linker, and loader
  behavior.

Loading shared libraries (.so)
------------------------------

.. index::
   single: shared libraries
   single: position-independent code (PIC)
   pair: GCC; -fpic flag
   pair: GCC; -fPIC flag
   single: global offset table (GOT)
   single: dynamic linker
   pair: linking; dynamic

Libraries also have text, data, and BSS sections. Shared libraries must
use position-independent memory references so the same library can be
mapped at different addresses in different processes. GCC uses the
``-fpic`` or ``-fPIC`` flags for this. Newer GCC versions often make PIC
the default. Use ``-fno-pic`` when you need to compare against non-PIC
output.

The dynamic linker resolves position-independent memory accesses by
writing entries in the global offset table, or GOT, for each process.
This is necessary because a shared library may be loaded at different
offsets in different executions, while still allowing the operating
system to share the library's text segment among processes.

Shared Library Example
----------------------

The ``systems-code-examples/hello-lib`` example separates a function into
a small library and calls it from an executable. This is the simplest
repository example for discussing the boundary between application code
and library code.

.. literalinclude:: ../examples/systems-code-examples/hello-lib/hello.c
   :language: c
   :linenos:

.. literalinclude:: ../examples/systems-code-examples/hello-lib/hello.h
   :language: c
   :linenos:

Key points:

- ``hello.h`` declares the interface that other source files can include.
- ``hello.c`` provides the implementation that can be compiled into a
  library.
- The executable depends on the library code being linked or loaded
  before the call to ``say_hi()`` can run.

more about GCC pic option
---------------------------

.. index::
   pair: GCC; position-independent code
   pair: position-independent code; global offset table

Based on the *man page* for ``gcc``:

Generate position-independent code (PIC) suitable for use in a shared
library, if supported for the target machine. Such code accesses all
constant addresses through a global offset table (GOT). The dynamic
loader resolves the GOT entries when the program starts. If the GOT size
for the linked executable exceeds a machine-specific maximum size, the
linker reports that ``-fpic`` does not work. In that case, recompile with
``-fPIC`` instead.

Loading static libraries
------------------------

.. index::
   single: static libraries
   pair: linking; static
   single: object files (.o)

Static libraries do not contain position-independent code. A static
library is a collection of unlinked ``.o`` object files. When the program
is linked, the linker copies the needed object code into the final
executable.

Position Independent Code Example
------------------------------------------

The ``systems-code-examples/pic`` example shows the difference between
generating PIC and non-PIC assembly.

Get the code and build it with the example makefile::

  $ git clone https://github.com/SoftwareSystemsLaboratory/systems-code-examples
  $ cd systems-code-examples/pic
  $ make -f Makefile.pic

main.c
---------

.. literalinclude:: ../examples/systems-code-examples/pic/main.c
   :language: c
   :linenos:

main.nopic.s - non-position independent code (gcc -fno-pic)
--------------------------------------------------------------

.. literalinclude:: ../examples/systems-code-examples/pic/results-gcc-9.3.0/main.nopic.s
   :linenos:
   :end-before: cfi_endproc

main.nopic.s - position independent code (gcc -fpic, default option)
---------------------------------------------------------------------------

.. literalinclude:: ../examples/systems-code-examples/pic/results-gcc-9.3.0/main.pic.s
   :linenos:
   :end-before: cfi_endproc

What's the difference?
-------------------------

.. literalinclude:: ../examples/systems-code-examples/pic/results-gcc-9.3.0/diff-pic-vs-nopic.diff
   :linenos:

Key points:

- The C source is intentionally small so the assembly differences are
  easier to see.
- The non-PIC version can use more direct addressing because it assumes a
  fixed relationship between code and data.
- The PIC version uses relative addressing through the global offset
  table so the code can be loaded at different virtual addresses.

Shared Libraries - Evaluation
---------------------------------

.. index::
   single: shared libraries; evaluation
   pair: memory; text segment sharing

Shared libraries reduce memory use when multiple processes load the same
library because the text segment can be reused. They also allow programs
to benefit from library updates without rebuilding the executable.

The cost is complexity. Shared libraries require virtual memory support,
dynamic linking, position-independent code, and careful handling of
version and path compatibility.

Static Libraries - Evaluation
--------------------------------

.. index::
   single: static libraries; evaluation

Static libraries make sense when reuse is not important, when deployment
must avoid runtime dependencies, or when the program must be self
contained. Static linking can also avoid version and path problems.

The main cost is memory use. Common code is copied into each executable,
so there is less sharing across processes.

Libraries vs. Statically-Linked Programs
----------------------------------------

.. index::
   pair: dynamic linking; static linking
   single: whole-program optimization

Dynamic linking is usually chosen for memory footprint, code reuse,
smaller executables, and easier library updates. Static linking is useful
when deployment simplicity matters more than sharing. It can also make
whole-program optimization and obfuscation easier because more code is
available to the linker at once.

Process Protection
------------------

.. index::
   single: process protection
   single: virtual memory
   pair: memory; protection
   single: privilege separation

Modern operating systems use virtual memory and privilege separation to
protect processes from each other and from the kernel.

One process cannot read another process's memory unless the operating
system explicitly permits it. A process can manage the memory it owns,
including allocation, deallocation, garbage collection, calling
conventions, and stack management. A crash, exception, resource
starvation, or deadlock in one process does not directly modify another
process. Even though kernel memory may be mapped into the process address
space, user code cannot modify kernel memory or other protected pages.

Process Creation with fork()
----------------------------

.. index::
   single: fork()
   pair: system call; fork
   single: process creation
   pair: process; creation
   single: parent process
   single: child process
   single: process ID (PID)

``fork()`` creates a new process by duplicating the calling process. The
new process is the child. The calling process is the parent. The child has
its own process ID, and its parent process ID is the parent's process ID.
The parent's threads are not recreated in the child.

.. index::
   pair: Linux; fork() implementation
   pair: glibc; fork()

In Linux, the C library implementation of ``fork()`` uses ``clone()``.
From the programmer's point of view, ``fork()`` returns the child PID to
the parent, returns ``0`` to the child, and returns ``-1`` if the child
cannot be created.

fork() example
--------------

The ``systems-code-examples/fork`` example shows the basic return-value
pattern used to distinguish parent and child execution.

::

   $ cd systems-code-examples/fork
   $ cmake .
   $ make

.. literalinclude:: ../examples/systems-code-examples/fork/main.c
   :language: c
   :linenos:

Key points:

- The call to ``fork()`` creates two processes that continue from the
  same point in the program.
- The parent sees the child's process ID as the return value.
- The child sees ``0`` as the return value, which is how the code chooses
  the child branch.

fork() and exec() example
-------------------------

.. index::
   single: exec()
   single: execvp()
   pair: system call; exec
   single: pipe()
   pair: system call; pipe
   single: dup2()
   pair: system call; dup2
   single: standard input
   single: standard output

The ``systems-code-examples/fork_exec`` example creates a pipe, forks,
redirects standard input and standard output with ``dup2()``, and then
uses ``execvp()`` to replace each process image. The parent runs ``wc``.
The child runs ``echo``.

.. literalinclude:: ../examples/systems-code-examples/fork_exec/main.c
   :language: c
   :linenos:

Key points:

- ``pipe()`` creates a kernel buffer with one file descriptor for reading
  and one for writing.
- ``dup2()`` redirects standard input or standard output before the
  process image is replaced.
- ``execvp()`` replaces the current program with another executable; code
  after a successful ``execvp()`` call does not run.

Process Creation with clone()
-----------------------------

.. index::
   single: clone()
   pair: system call; clone
   single: CLONE_FS
   single: CLONE_FILES
   single: CLONE_SIGHAND
   single: CLONE_VM
   pair: Linux; clone() flags
   single: lightweight processes

``clone()`` is similar to ``fork()`` because it creates a child execution
context. Unlike ``fork()``, it allows selected parts of the parent process
to be shared with the child.

Linux uses flags to control what is shared. ``CLONE_FS`` shares file
system information such as ``chroot``, ``chdir``, and ``umask``.
``CLONE_FILES`` shares the file descriptor table. ``CLONE_SIGHAND``
shares signal handlers. ``CLONE_VM`` shares the page table. These flags
are part of how Linux creates lightweight processes and kernel threads.

glibc's ``fork()`` calls ``clone()`` without these sharing flags.
``clone()`` is Linux-specific and is not present in every UNIX-like
operating system.

Windows CreateProcess() and CreateThread()
------------------------------------------

.. index::
   single: CreateProcess()
   single: CreateThread()
   pair: Windows; process creation
   pair: Windows; thread creation

Windows does not expose the same process-sharing model as UNIX
``fork()`` and Linux ``clone()``. ``CreateProcess()`` creates a new
process and is closest to calling ``fork()`` followed by ``execve()`` on
UNIX. ``CreateThread()`` creates a new thread and is closest to using
``clone()`` with thread-sharing flags.

For most programs, this is not a practical disadvantage. Most UNIX uses
of ``fork()`` are followed by ``exec()``, and most uses of ``clone()`` are
for thread creation.

Emulating fork() on Windows
---------------------------

.. index::
   single: Cygwin
   pair: Windows; fork() emulation
   single: setjmp()
   single: copy-on-write

Cygwin implements ``fork()`` on Windows by combining ``CreateProcess()``,
saved register state, copied memory regions, and synchronization. The
parent creates a suspended child process, saves register state with
``setjmp()``, copies the BSS and data sections to the child, wakes the
child, and then coordinates stack, heap, and memory-mapped region copies.

This approach is closer to "copy on fork" than modern UNIX copy-on-write.
It is useful for compatibility, but it is more expensive than a native
UNIX ``fork()`` implementation.

System Call Boundary Example
----------------------------

.. index::
   single: system calls
   single: printf()
   single: write()
   single: syscall()
   pair: system call; boundary
   pair: C library; printf
   pair: POSIX; write()

The ``systems-code-examples/syscall`` example compares three ways to
write output: the C library ``printf()``, the POSIX ``write()`` wrapper,
and a direct ``syscall()`` invocation.

.. literalinclude:: ../examples/systems-code-examples/syscall/main.c
   :language: c
   :linenos:

Key points:

- ``printf()`` is a C library call, not a direct system call.
- ``write()`` is the usual POSIX wrapper for the kernel's write system
  call.
- ``syscall(SYS_write, ...)`` bypasses the wrapper and invokes the system
  call interface directly.

Command-Line Option Example
---------------------------

.. index::
   single: argc
   single: argv
   single: getopt()
   pair: process; command-line arguments
   pair: C library; getopt

The ``systems-code-examples/getopt`` example shows how a process receives
command-line arguments through ``argc`` and ``argv`` and turns them into
structured options.

.. literalinclude:: ../examples/systems-code-examples/getopt/main.c
   :language: c
   :linenos:

.. literalinclude:: ../examples/systems-code-examples/getopt/cl.h
   :language: c
   :linenos:

Key points:

- ``argc`` and ``argv`` are the process's command-line interface.
- The parsing code converts raw strings into a structured options object.
- The program exits with success or failure depending on whether the
  command line was valid.

Process Termination
-------------------

.. index::
   single: process termination
   single: segmentation fault
   single: bus error
   single: stack overflow
   single: exit code
   pair: process; termination

A process can terminate normally by returning from ``main()``. It can
also terminate with an error code, by encountering a fatal error, or by
being terminated externally by another process.

Fatal errors include segmentation faults or bus errors from invalid
memory access, stack overflow, protection faults from privileged
instructions, and instruction faults such as divide by zero.

wait() and waitpid() examples
-----------------------------

.. index::
   single: wait()
   single: waitpid()
   pair: system call; wait
   pair: system call; waitpid
   single: zombie process
   single: exit status

The ``systems-code-examples/wait`` example shows how a parent process can
wait for a child process to terminate.

.. literalinclude:: ../examples/systems-code-examples/wait/main.c
   :language: c
   :linenos:

Key points:

- The parent process uses ``wait()`` or ``waitpid()`` to collect child
  exit status.
- Waiting also prevents terminated children from remaining as zombie
  processes.
- The status value encodes how the child terminated.

SIGCHLD examples
----------------

.. index::
   single: SIGCHLD
   single: sigaction()
   pair: signal; SIGCHLD
   single: WNOHANG
   pair: signal; handler

The ``systems-code-examples/fork_sigchld`` example handles ``SIGCHLD`` to
notice when a child process exits. It uses ``wait()`` in the signal
handler to collect the child's exit status.

.. literalinclude:: ../examples/systems-code-examples/fork_sigchld/main.c
   :language: c
   :linenos:

Key points:

- ``sigaction()`` installs a handler for ``SIGCHLD`` before the fork.
- The kernel sends ``SIGCHLD`` to the parent when the child exits.
- The handler calls ``wait()`` to collect the child's exit status.

The ``systems-code-examples/fork_sigchld2`` example extends this pattern
to multiple child processes. It uses ``waitpid()`` with ``WNOHANG`` so the
handler can collect every child that has exited without blocking.

.. literalinclude:: ../examples/systems-code-examples/fork_sigchld2/main.c
   :language: c
   :linenos:

Key points:

- The parent tracks child process IDs in a small table.
- The signal handler loops with ``waitpid(-1, ..., WNOHANG)`` to collect
  all children that have exited.
- ``WNOHANG`` matters because a signal handler should not block waiting
  for children that are still running.

Process-Oriented C Case Studies
-------------------------------

.. index::
   pair: C programming; object-oriented style
   single: struct (C)

Some examples in the repository are best treated as short systems C case
studies. They support the process chapter because they make memory,
calling conventions, and low-level representation concrete.

The ``systems-code-examples/objects`` example shows object-style
programming in C using ``struct`` values and functions.

.. literalinclude:: ../examples/systems-code-examples/objects/main.c
   :language: c
   :linenos:

Key points:

- The ``Employee`` structure groups related fields in one allocation.
- Functions such as ``Employee_New`` and ``Employee_Print`` act like
  methods by taking an ``Employee*`` argument.
- The example uses dynamic allocation, so it also previews memory
  management issues handled later.

.. index::
   single: strlen()
   pair: C library; strlen
   single: C strings
   pair: C; string handling

The ``systems-code-examples/strlen`` example is a small C string and
memory-layout demonstration.

.. literalinclude:: ../examples/systems-code-examples/strlen/main.c
   :language: c
   :linenos:

Key points:

- C strings are byte arrays terminated by ``'\0'``.
- ``strlen()`` counts characters before the terminator, not the storage
  used by the array.
- The example is useful for connecting string behavior to memory layout.

.. index::
   single: bit manipulation
   pair: C programming; bit operations
   single: bitwise operators

The ``systems-code-examples/parity`` example is a low-level bit
manipulation example.

.. literalinclude:: ../examples/systems-code-examples/parity/parity.c
   :language: c
   :linenos:

Key points:

- The code works directly with integer bits rather than higher-level data
  structures.
- Bit operations are common in operating systems code for flags, masks,
  and packed state.
- Small examples like this help make machine representation visible.
