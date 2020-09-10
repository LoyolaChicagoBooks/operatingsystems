Introduction to Processes
===========================

-  An executing program or job

-  Components

   -  Memory address space

   -  Program image

   -  Handles to files

   -  Interprocess communication

-  Every process has a parent

   -  top most process in UNIX is "init"

   -  Windows top most processes are:

      -  system.exe - OS kernel

      -  csrss.exe - handles user mode win32 calls

      -  wininit.exe - manages system services

      -  explorer.exe - OS shell

      -  winlogin.exe - OS login service

Process Memory Layout
---------------------

-  Text - machine instructions of a program.

-  Data - initialized static data and constants

-  BSS - uninitialized static data

-  Heap - dynamic memory of a process

   -  one or more per process

-  Stack - local variables, call stack, return values

   -  one per thread for kernel mode threads

   -  one per n-threads for user mode threads

-  layout of and allocation of text, data, bss, and initial values for
   first thread’s stack and heap are the responsibility of the loader

-  management of stack and heap are through the process’s runtime
   library and program instructions

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

-  Code for examples in these notes can be found at https://github.com/gkthiruvathukal/systems-code-examples.

-  You can use ``git clone https://github.com/gkthiruvathukal/systems-code-examples`` to clone to a folder ``systems-code-examples``.

-  All subsequent examples that are full-working demonstrations will be referenced as ``systems-code-examples/<example-name>``.

-  To run examples, make sure you have `gcc`, `cmake`, and `make` on your computer.

-  We only have tested on Ubuntu Linux, MacOS, and Windows Subsystem for Linux 2 (with Ubuntu 20.04 LTS). Most others should work.



Examine Process Layout Example
---------------------------------

-  Most of our examples are written in C with some C++.

-  `cd systems-code-examples/c_intro`

-  Generate the `Makefile` using `cmake`::

     $ cd c_intro
     $ cmake .
     -- The C compiler identification is GNU 9.3.0
     -- The CXX compiler identification is GNU 9.3.0
     -- Check for working C compiler: /usr/bin/cc
     -- Check for working C compiler: /usr/bin/cc -- works
     -- Detecting C compiler ABI info
     -- Detecting C compiler ABI info - done
     -- Detecting C compile features
     -- Detecting C compile features - done
     -- Check for working CXX compiler: /usr/bin/c++
     -- Check for working CXX compiler: /usr/bin/c++ -- works
     -- Detecting CXX compiler ABI info
     -- Detecting CXX compiler ABI info - done
     -- Detecting CXX compile features
     -- Detecting CXX compile features - done
     >> Linux
     -- Configuring done
     -- Generating done
     -- Build files have been written to: /home/gkt/Work/systems-code-examples/c_intro


-  Run `make` to crate the executable::

     $ make
     Scanning dependencies of target c-intro-demo
     [ 20%] Building CXX object CMakeFiles/c-intro-demo.dir/main.cc.o
     [ 40%] Building CXX object CMakeFiles/c-intro-demo.dir/debug.cc.o
     [ 60%] Building CXX object CMakeFiles/c-intro-demo.dir/list.cc.o
     [ 80%] Building CXX object CMakeFiles/c-intro-demo.dir/tests.cc.o
     [100%] Linking CXX executable bin/c-intro-demo
     [100%] Built target c-intro-demo

-  Note that for all of our examples, the output executable appears in the `bin` subdirectory.

-  Note also that almost all of our examples use `cmake` and `make` as shown here.
     
-  Run the `layout` shell script, which shows the size of the text, data, and bss in *bytes*::

     $ ./layout

     section               size   addr
     .text                 3957    4352
     .data                   24   20480
     .bss                     8   20504
     
Loading Programs
----------------

-  loader allocates memory for executable's text, data, bss, heap, and
   stack. and loads program's image into memory

-  loader gets information from OS where shared libraries are already
   allocated in memory and loads the ones that are not already loaded.
   each shared library has its own text, data, and bss

-  loader goes through executable and adjusts the list of external
   symbols to point to the correct spots in memory (to shared libraries)

-  try the ``nm`` command to see the symbols in a compiled object/executable.

-  once program is ready, loader invokes ``_start()`` method

-  ``_start()`` calls ``_init()`` for each shared library

-  ``_start()`` initializes static constructors of objects defined as
   global variables

-  ``_start()`` calls main() and program begins

Loading shared libraries (.so)
------------------------------

-  Libraries also have a data, bss, and text segment

-  Memory references in shared libraries are position independent (GCC
   ``-fpic`` or ``-fPIC`` flagsA. Newer GCC makes PIC defualt. Use ``-fno-pic``
   if you notice this.

-  Linker must resolve all of these position independent memory accesses
   to local accesses. This is accomplished by writing the GOT for each
   linked process.

-  The need for the memory addresses to be position independent is
   because the offset that a shared library will be loaded will differ
   between executions of the same program and among other programs.

-  Also, the same loaded shared library will be shared with other
   processes without reloading. So, the same library may have different
   offsets for different programs

more about GCC pic option
---------------------------

Based on the *man page* for ``gcc``

Generate position-independent code (PIC) suitable for use in a shared
library, if supported for the target machine. Such code accesses all
constant addresses through a global offset table (GOT). The dynamic
loader resolves the GOT entries when the program starts (the dynamic
loader is not part of GCC; it is part of the operating system). If the
GOT size for the linked executable exceeds a machine-specific maximum
size, you get an error message from the linker indicating that -fpic
does not work; in that case, recompile with -fPIC instead. (These
maximums are 8k on the SPARC and 32k on the m68k and RS/6000. The 386
has no such limit.)

Loading shared libraries (.so)
------------------------------

-  Static libraries do not contain position independent code

-  Static libraries are simply a collection of unlinked ``.o`` (object)
   files

-  The dynamic linker simply loads the text, data, and bss sections of
   each object file into the program's address space

Position Independent Code Example
------------------------------------------

-  The source can be found in ``systems-code-examples/pic``.

-  For this example, you can build it using ``make -f Makefile.pic``. 

-  The main purpose of this example is to show the difference between generating PIC vs. non-PIC. 

main.cc
---------

.. literalinclude:: ../examples/systems-code-examples/pic/main.c
   :language: c
   :linenos:


main.nopic.s - non-position independent code (gcc -fno-pic)
--------------------------------------------------------------

.. literalinclude:: ../examples/systems-code-examples/pic/results-gcc-9.3.0/main.nopic.s
   :linenos:
   :end-before: cfi_endproc


main.nopic.s - non-position independent code (gcc -fpic, default option)
---------------------------------------------------------------------------

.. literalinclude:: ../examples/systems-code-examples/pic/results-gcc-9.3.0/main.pic.s
   :linenos:
   :end-before: cfi_endproc

What's the difference?
-------------------------

.. literalinclude:: ../examples/systems-code-examples/pic/results-gcc-9.3.0/diff-pic-vs-nopic.diff
   :linenos:

Shared Libraries - Evaluation
---------------------------------

Strengths

-  Reduced memory footprint. If two programs load the same shared
   library, the .text segment is reused across processes thanks to the
   GOT

Weaknesses

-  Requires a more advanced virtual memory implementation in the
   operating system. Sometimes not practical for simple or embedded
   systems

-  Requires more advanced compiler code generators. Different processors
   have special features regarding memory offset registers or function
   table size limitations.

Static Libraries - Evaluation
--------------------------------

Strengths

-  Makes sense when re-use is not desired. A good example would be
   installer executables with very large .data segments.

-  Faster first load loading time than shared libraries.

-  Fewer instructions generated for GOT lookups (minor issue)

Weaknesses

-  Much larger memory footprint. Little reuse of common code between
   applications.


Libraries vs. Statically-Linked Programs
----------------------------------------

Dynamic Linking advantages:

-  Memory footprint

-  Code reuse

-  Improvement with new versions of shared libraries

-  Smaller executables

Static linking advantages:

-  When deploying software, dependencies are less of a concern (e.g.
   missing dependencies, incorrectly upgraded dependencies, custom
   patches and alterations to shared code)

-  Versioning and path problems are less of a concern

-  Code obfuscation can obfuscate across object files

-  Compiler optimizers can optimize across object files

Process Protection
------------------

In modern operating systems with virtual memory and privileged
separation the following protections are afforded:

-  One process cannot read the memory of another process (except when
   explicitly permitted)

-  A process can fully manage the memory that it can access - garbage
   collection, explicit allocation/deallocation, method call and
   parameter passing standards, stack management, etc.

-  A crash, exception, resource starvation, deadlock, or other fault in
   one process does not directly affect other processes

-  While mapped to the same address space, the process cannot modify
   kernel memory or memory otherwise protected by the operating system
   (such as text pages).

Process Creation with fork()
----------------------------

From the ``fork()`` man page:

-  ``fork()`` creates a new process by duplicating the calling process. 
   The new process, referred to as the child, is an exact duplicate of the calling process,
   referred to as the parent, except for the following points:

-  the child has its own unique process id (PID)

-  the child's parent PID is the same as the parent's PID

-  the parent's threads are not recreated on the child

interesting point: in Linux, ``fork() != fork()``; ``fork()`` calls ``clone()``

From the man page:

-  ``fork()`` returns the child PID to the parent

-  ``fork()`` returns 0 to the child

-  ``fork()`` returns -1 if the child cannot be created

fork() example
--------------

See ``systems-code-examples/fork`` if you want to run this program.

::
   
   $ cd systems-code-examples/fork
   $ cmake
   $ make


.. literalinclude:: ../examples/systems-code-examples/fork/main.c
   :language: c
   :linenos:

Process Creation with clone()
-----------------------------

-  similar to ``fork()`` in that a child process is created.

-  ``clone()`` allows different parts of the parent process to be shared
   with the child process

-  flags for creating a light weight process (kernel thread):

   -  ``CLONE_FS`` - share FS information (chroot, chdir, umask)

   -  ``CLONE_FILES`` - share file descriptor table

   -  ``CLONE_SIGHAND`` - share signal handlers

   -  ``CLONE_VM`` - share page table

-  many more flags exist - don't forget this little known capability!

-  glibc's version of ``fork()``, calls ``clone()`` without any of these flags

-  ``clone()`` not present in every UNIX OS (available in Linux but not
   Minix)

Windows CreateProcess() and CreateThread()
------------------------------------------

-  Different from UNIX ``fork()``/``clone()`` - parts of processes are not
   shared

-  Windows has two flavors:

   -  ``CreateProcess()`` - creates a new process, equivalent of calling
      ``fork()`` then ``execve()`` in UNIX

   -  ``CreateThread()`` - equivalent of creating ``clone()`` with thread flags

-  Is this a disadvantage?

   -  For most use cases and most programs, no.

   -  The vast majority of calls to ``clone()`` in UNIX are equivalent to
      ``CreateThread()``

   -  The vast majority of calls to ``fork()`` in UNIX are equivalent to
      ``CreateProcess()``

Emulating fork() on Windows
---------------------------

A well known system, Cygwin, implements ``fork()`` on Windows as follows:

1. cygwin.dll calls ``CreateProcess()`` to create a suspended child process

2. parent process calls ``setjmp()`` to save registers

3. parent process copies its BSS and DATA sections to the child's
   address space.

4. parent wakes child up and waits on a named mutex (mutual exclusion mechanism).

5. child wakes up, realizes it was a forked process, then longjumps to
   the saved jump buffer. child unlock's

6. parent's named mutex and waits on a second mutex

7. parent wakes up, copies its stack and heap to the child process.
   release's child's named mutex

8. Child wakes up and copies any memory mapped regions the parent
   signals to the child through shared memory

9. ``fork()`` system call in Cygwin does not use copy on write, but "copy on
   fork". this is similar to ``fork()`` implementations in early UNIX
   operating systems

Causes of process termination
-----------------------------

-  Normal exit–return from ``main(...)``

-  Error exit–return from ``main(...)`` with an error code

-  Fatal error

   -  segfault/bus error–process tries to read/write inaccessible memory
      or write to read-only memory.

   -  stack overflow–stack pointer grows to larger than stack area

   -  protection fault–trying to run privileged instructions such as
      enabling/disabling interrupts

   -  instruction faults–divide by zero

-  External termination by another process either through signals or
   system calls

wait() and waitpid() examples
-----------------------------

See ``systems-code-examples/wait`` if you want to run this program.

.. literalinclude:: ../examples/systems-code-examples/wait/main.c
   :language: c
   :linenos:

