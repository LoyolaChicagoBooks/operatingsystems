Introduction
============

Crash Course on the UNIX Shell
------------------------------

/bin/bash
---------

print the contents of a file to standard out::

   $ cat file

print the contents of more than one file (use '{' and '}' to create lists, a form known as bracket expansion)::

   $ cat file1 file2 file3
   $ cat {file1,file2,file3}

creating folders (-p creates parent folders if they don't already exist)::

   $ mkdir foobar
   $ mkdir -p foo/bar/baz

creating files (or update time stamp of existing files)::

   $ touch file1
   $ touch file1 file2 file3
   $ touch {file1,file2,file3}

You can also create an empty file as follows::

   > filename.txt

Be careful. Many operations can be *destructive* if you are not careful.


more useful commands
--------------------

print text to the console::

   $ echo "hello world"
   $ echo -e "hello\t\t world\n"

read the contents of a file, one screen at a time::

   $ less file1.txt

search a file for some text::

   $ grep "hello world" file1.txt
   $ grep -e "hello.* d" file1.txt

Files and Folders
-----------------

creating files::

   > file1


deleting files::

   $ rm file1
   $ rm file2

creating folders::

   $ mkdir foo

deleting folders::

   $ rm -rf foo

Moving a file::

   $ mv file1 folder/file1

Renaming a file::

   $ mv file1 file2

UNIX file permissions
---------------------

- UNIX files have a concept of access permissions and ownership

- ownership is to a user and a user's group

- access permissions are read, write, and execute as applied to the
  owning user, the group, and all others

- if a folder is marked as executable, its contents can be listed and
  it can be entered into. if it is not, files in it can be accessed
  but not listed.

UNIX file permissions
---------------------

changing the user / group ownership of a file::

   $ chown username:users file1

changing the user/group ownership of an entire folder and its children::

   $ chown -R username:users folder1

setting the owning user's permission to be read,write, and execute::

   $ chmod u+rwx file1

setting the group's permissions to be read, and execute::

   $ chmod g+rx file1

removing permissions from "group" and "other" to write to the file::

   $ chmod go-w file1


Combining commands
------------------

searching two files for a pattern, then writing the results to a report::

   $ cat {file1, file2} | grep -e "Hello.*" » report.txt

counting the number of lines in two files and printing result to console::

   $ cat {file1, file2} | wc -l

executing the second command only if the first succeeds::

   $ cat file1 | grep -e "Hello.*" && echo "found a match!"

taking the output of one command as an argument for another::

   $ grep "Hello" `ls *.txt`
   $ echo "result: " `cat file1 | wc -l`

Backround Processes and Jobs
----------------------------

- if you're running a long running process and want to send suspended
  it to the background, just press `Ctrl+z`
- to unsuspend it and keep it in the background, use the 'bg' command
- to bring the process back to the foreground, use the 'fg' command
- to bring the process back, just 

::

   $ tail -f logfile.log
   $ ^Z
   [1] + Stopped tail -f logfile.log
   $ jobs -l
   [1] + 5123 Stopped tail -f logfile.log
   $ bg 1
   [1] + tail -f logfile.log &
   $ jobs -l
   [1]+ 5123 Running tail -f logfile.log
   $ fg 1
   tail -f logfile.log


Background Processes
--------------------

- Background processes will allow you to make the most of the machine
  you have available to you.
- If you have a long running process or task, or if you don't want the
  task you've launched to take over the terminal, just append '&' to
  the command to launch it as a background task
- Beware: multiple processes will print to the same console which may
  become garbled.

Crash Course on UNIX Editors
----------------------------


Choosing an Editor
------------------

- In Minix, your choices are limited. More ideal choices like
  MonoDevelop, gedit, Eclipse, or Intellij-Idea are not present.
- Your choices are basically: Emacs, vim, or nano.
- We recommend starting with nano. It takes the least time to learn.
- Emacs and vim have many more features that will increase your
  productivity if you learn them, but both have a learning curve. It
  is my opinion that vim has a steeper learning curve than Emacs, but
  each his/her own.

Crash Course in C Programming
-----------------------------


C - Hello World
---------------

::

  #include «stdio.h»

  int main(int argc, char* argv[]) {
     printf("Hello world\n");
     getchar();
     return 0;
  }


C - header files
----------------

.. csv-table:: List of Important Header Files
   :header: "Filename", "Description"
   :widths: 15,65

   "assert.h", "runtime assertion macro library"
   "complex.h", "arithmetic library - trig functions, complex numbers"
   "ctype.h", "character type library (isdigit, islower, isupper, etc...)"
   "errno.h", "error definition file for system error numbers"
   "limits.h", "system limit constants defined, data type limits defined"
   "math.h", "arithmetic library"
   "setjmp.h", "defines setjmp/longjmp - good for implementing exceptions, cooperative multi-tasking"
   "signal.h", "system singals - more when we talk about IPC"
   "stdarg.h", "var_args helpers, important for printf("", ....) like functions"
   "stddef.h", "defines NULL, wchar_t, size_t, ptrdiff_t"
   "stdio.h", "defines printf, putchar, getchar, other file I/O methods"
   "stdlib.h", "defines heap management, environment variable management, random number generators, and integer to string converters"
   "string.h", "string manipulation methods"
   "time.h", "time / date functions and types"

C - operator precedence (highest to lowest)
-------------------------------------------

#. ``()``, ``[]``, ``.``, ``->``, ``++`` (postfix) , ``--`` (postfix)
#. ``++`` (prefix), ``--`` (prefix), ``+`` (unary), ``-`` (unary), ``!``, ``~``, (type), ``*`` (dereference), ``&`` (address-of), sizeof
#. ``*``, ``/``, ``%``
#. ``+``, ``-``
#. ``<<``, ``>>`` (bitwise shift left/right)
#. ``<<``, ``<<=``, ``>>``, ``>>=``
#. ``==``, ``!=``
#. ``&`` (bitwise and)
#. ``^`` (bitwise xor)
#. ``|`` (bitwise or)
#. ``&&`` (logical and)
#. ``||`` (logical or)
#. ``?`` : (ternary operator)
#. ``=``, ``+=``, ``-=``, ``*=``, ``/=``, ``%=``, ``&=``, ``^=``, ``|=``, ``<<=``, ``>>=``, (assignment operators)
#. ``,`` (expression separator)

C - type declaration
--------------------


simple types::

   typedef unsigned int uint;
   typedef unsigned long file_size;
   typedef int* intptr;

   uint myVal = 55;

structured types::

   typedef struct {
      int foo;
      char bar;
   } foobar;

   foobar x;

C - type casting
----------------

::

   float x = 2.2;
   int y = (int)x;         // y now has the value 2. loss of precision
   int z = 2.5 * 4.2;      // 2.5 and 4.2 are floats, they multiply as float
                           // first, you get 10.5, then cast to int and you get 10
   int m = 4;
   int n = 3;
   float o = m / n;        // in this case, 4 / 3 is int type and the value is 1. 
                           // it is assinged to float and becomes 1.0
   float p = (float)m / n  // in this case, one variable is of type float, so
                           // the value of p is 1.333333




C - chacater strings (arrays)
--------------------------------

- Strings in C are implemented as *arrays* of type char.
- Strings are all null terminated.

  - ``"Hello World"`` would be the following array:
  - ``{ 'H', 'e', 'l', 'l', 'o', ' ', 'W', 'o', 'r', 'l', 'd', 0 }``
  - ``strlen("Hello World")`` returns 11, and uses 12 bytes of memory
  - When allocating space for a string, always allocate one extra byte to store the null terminator.

C - memory management
---------------------

- Most C implementations do not have garbage collectors. All memory
  management is done explicitly

- Calls involved are ``malloc()``, ``calloc()``, and ``free()``.

- In this method, 'msg' is stored on the stack and 'msg2' is stored on
  the heap.

- stack variables are freed automatically, heap variables are not

::

   #include «stdio.h»

   int main(int argc, char* argv[]) {
      const char *msg = "Hello world";
      char *msg2 = (char*)calloc(0, sizeof(char) * strlen(msg)+1);
      strncpy(msg2, msg, strlen(msg));
      printf("%s\n", msg2);
      free(msg2);
      return 0;
   }


C - pointers
------------

- Two important keywords in C are '*' and '&'
- * - pointer dereference
- & - value address

::

   int a = 55;            // integer variable - 55
   int q = 100;           // integer variable - 100
   int* b = &a;           // b now holds the address of a
   a = 56;                // a is now 56, b still has the same value (address of a)
   *b = 70;               // *b dereferences the pointer of b. *b = 70,
                          // sets the value of 70 to the address b points to.
                          // effectively, this sets a = 70
   b = &q;                // b now points to q.
   a = *b;                // *b dereferences pointer of b. *b = 100 because q = 100
                          // value of q is now assigned to a

C - pointers
------------

::

   typedef struct {
      int a;
      int b;
   } foobar;

   foobar* x = (foobar*)malloc(sizeof(foobar));

   x->a = 10;             // here we use '->' because x is an address
   x->b = 100;

   foobar y;
   y.a = 50;              // here we use '.' because y is a value type
   y.b = 500;

   (*x).a = 100;          // here we use '.' because (*x) is a dereferenced pointer
                          // and therefore a value type

   (*x).b = 20;

C - pointers
------------

::

   int x[] = {2,4,6,8,10};   // auto-initialized array of 5 elements
   int* a = &x[0];           // address of first element
   a + 1;                    // address of second element
   *(a + 1);                 // value of second element (4) 
   *(a + 3) = 20;            // value of 3rd element is now 20

   for (int* b = &x[0]; *b != 10; b += 1) {
      printf("%d\n", *b);
   }


C - functions
-------------

- in C, functions have a name, return type, and parameter list
- since C is a single pass compiler, the definition of a function must
  appear before the reference to the function:


Example where the functions are declared before use::

   int add(int x, int y) {
      int sum = x + y;
      return sum;
   }

   int main(int argc, char* argv[]) {
      printf("2+2=%d\n", sum(2,2));
   }

Example where a function is defined but not fully declared before
first use::

   int add(int x, int y);

   int main(int argc, char* argv[]) {
      printf("2+2=%d\n", sum(2,2));
   }

   int add(int x, int y) {
      int sum = x + y;
      return sum;
   }

C - functions / headers
-----------------------

- A good place to put function declarations is in a .h or .hh file

- Since C is a single pass compiler, you have to take some care to make sure that a header is not read twice. macros help with this:

- The macro 'ifndef' means if not defined. This causes the C compiler to only continue parsing if 'FUNC_H' is not yet defined


myfunc.h contents::

   #ifndef FUNC_H
   #define FUNC_H

   int add(int x, int y);

   #endif

C - macros
----------

- C has a macro preprocessor. C processes the source file and expands
  all macros and then compiles the expanded source
- Be sure to be careful about parenthesis!

::

   #define add(x, y) x + y
   #define mul(x,y) x * y

   int foo() {
      int x = add(2,3);
      int y = mul(4+2,5);
   }

expands to (approximately)::

   int foo() {
      int x = 2 + 3;
      int y = 4 + 2 * 5;
   }

   #define add(x, y) (x) + (y)
   #define mul(x,y) (x) * (y)

   int foo() {
      int x = add(2,3);
      int y = mul(4+2,5);
   }

expands to (approximately):: 

   int foo() {
      int x = (2) + (3);
      int y = (4+2) * (5);
   }

Crash course on GCC
-------------------

- GCC is the GNU Compiler Collection
- Several languages exist: C, C++, ADA, Fortran, Java, Objective-C, and Go
- GCC was one of the early GNU projects


Where to find it?
-------------------

You'll want to make sure both of these commands are available::

   $ which gcc
   /usr/bin/gcc

   $ which make
   /usr/bin/make


What if I cannot find them?

- Install Linux or Windows Subsystem for Linux on a PC
- Install Homebrew (on MacOS)
- Install a cloud-based shell

These topics are covered in COMP 141 (Tools course).

Installing on Ubuntu
---------------------

::
   apt install build-essential

OS X
-------

- Has a mostly working `gcc` setup via Xcode
- Use Homebrew to install `gcc`, `make`, and more current versions of open source software.

Compiling C with GCC
--------------------

- Compilation with GCC has two phases:
  - source to object file translation (compilation)
  - object file to executable or library translation (linking)

Example compilation command (compiles hello.c to hello.o)::

   $ gcc -c hello.c -o hello.o
   $ gcc -c helper.c -o helper.o

Example linker command (links hello.o and helper.o to program hello)::

   $ gcc -o hello hello.o helper.o

Example with libraries in linker (links hello.o and helper.o, references pthreads shared library)::

   $ gcc -o hello hello.o helper.o -lpthread


Accessing assembly code with GCC
--------------------------------

- Modern compilers have several phases. GCC has a:

  #. lexer / parser (produces an AST)
  #. intermediate code generator (produces IL byte code)
  #. assembly code generator (produces machine specific code)
  #. object file generator (generates encoded machine instructions)
  #. linker (assembles many object files into a program)

- With GCC, it is possble to access the output of phases 3, 4, and 5.

- To get assembly output, use the following command line:
    - gcc -S test.c -o test.s
    - test.s will contain machine specific assembly source

Useful options in GCC (designed to *help* you)
----------------------------------------------

- -Wall : makes GCC print all language warnings
- -O2: for the linker, turns on the optimizer
- -g: generates debug symbols (useful for the debugger)
- -fpic: generates object files that can be used in a shared library
- -I adds a folder to search for header files

Crash Course on GNU Makefiles
-----------------------------

- A makefile is a list of rules. Each rule contains:
  - a target
  - dependent files or rules
  - commands to produce the target

- Makefiles manage source file dependencies. They also allow for rapid
  builds because the make command can detect if something has already
  been built or if it is out of date.

- The form of a make file rule is as follows::

    target: dependent_file1 dependent_file2
       command to build target

- The concept of a makefile might at first glance appear *old school*
  but the same ideas are used in modern IDEs, including Visual Studio,
  MonoDevelop, and Eclipse and is required for developing *in the large*.

Simple makefile example
-----------------------

::

   CC=/usr/pkg/bin/g++
   CFLAGS= -g -Wall -I.
   LINKFLAGS= -g -Wall

   all: demo

   demo: list.o main.o tests.o debug.o
      $(CC) $(LINKFLAGS) -o demo list.o main.o tests.o debug.o

   debug.o: debug.cc debug.hh
      $(CC) $(CFLAGS) -c debug.cc -o debug.o

   tests.o: tests.cc debug.hh list.hh
      $(CC) $(CFLAGS) -c tests.cc -o tests.o

   list.o: list.hh list.cc debug.hh
      $(CC) $(CFLAGS) -c list.cc -o list.o

   main.o: main.cc list.hh debug.hh tests.hh
      $(CC) $(CFLAGS) -c main.cc -o main.o

   clean:
      rm -f *.o demo

Running makefiles
-----------------

- Makefiles are run with the ``make`` command.

- The make command will parse the Makefile and build what is not built
  taking dependencies into account

- Example uses:
  - ``make all`` -- runs the target 'all'
  - ``make`` -- runs default target (all)
  - ``make clean`` -- runs target 'clean'

History of Operating Systems
----------------------------

What is an Operating System?

- An image of the machine to a program
  - Defines contracts for various types of hardware into categories
  - Each program doesn't have to know how to work with every type of
  network card, they each just have to interact with a networking
  library

- A resource manager
  - Manages permissions, scheduling, and access to hardware resources

- A collection of programs
  - OS Kernel
  - Device drivers
  - System runtime libraries
  - Graphical environments
  - Service Management

First Generation Operating Systems
----------------------------------

- Single program machines
- Vacuum tubes, plug boards, punch cards
- Stored program concept introduced in EDVAC, 1945, but would be many years before we had an OS.
- All programming done in machine language
- No programming languages
- No operating systems
- All programs are numerical calculations
- Up to 1950's

Second Generation Systems
-------------------------

- 1950s - 1960s
- Introduction of transistors
- Era of the mainframe and batch processing system
- Assembly languages and FORTRAN
- Batch systems composed of:
  - Card readers
  - Processors
  - Printers

Third Generation Systems
------------------------

- 1960s - 1980s
- Integrated circuits
- Beginning of the concept of backwards compatibility and machine images / abstractions.
- IBM System/360, Z series machines
- Multi-programming
  - When one job was waiting for I/O operations to complete, it would
  be paused and allow another job to run
  - Accomplished by partitioning memory into pre-defined segments

Fourth Generation Systems
-------------------------

- 1980s - present
- LSI / VLSI - integrated circuits
- Era of mini-computers and micro-processors
- Early UNIX operating systems
- Early MS-DOS operating systems
- 1980s - UNIX, OS/2, Amiga, RiscOS, MS-DOS, MacOS
- 1990s - MacOS, Linux, Windows, Solaris, BSDs, HP-UX, AIX, zOS
- 2000s - MacOS, Linux, Windows

History of Windows NT/XP/Vista/7
--------------------------------
- Windows NT 3/4
  - NT 3.1 was first created as a direct competitor to OS/2
  - Had Win32 / POSIX / OS-2 subsystems
  - First Windows OS to support virtual memory
  - Preemptively multi-tasking kernel
  - First version of Windows with NTFS
  - Generally outperformed Linux operating systems until 2.4.x Linux
  kernels (http://www.mindcraft.com/whitepapers/openbench1.html)
  - Beginning of Microsoft's replacement of UNIX Market

- Windows 2000
  - Next generation of OS kernel, faster, Win 9x GUI
  - NTFS 3.x - native encryption / compression, junctions, sparse files, disk quotas
  - Active Directory!
  - Beginning of MS security woes for IIS and worms in general

- Windows XP

  - Longest, largest install base of any Windows OS
  - Various kernel improvements over 2000
  - SP0 - SP2, MS experienced many security nightmares
  - Around 2003, with Linux 2.6, Linux began to perform better than
    Windows XP in many non GUI operations
  - Stability improved greatly in XP, many kernel subsystems were
    pulled into user-mode (csrss.exe)
  - Consolidation of development frameworks around .NET and Visual
    Studio began to gain steam.
  - Improved NTFS: volume shadow copy, better dynamic volume support

- Vista

  - released after two false starts in development
  - display manager moved from kernel to user mode
  - introduction of Aero
  - much more advanced memory manager
    - very incorrectly criticized too - unused memory is wasted memory!
    - largest performance problems were due to how much "hot memory"
    was kept available
  - first non-linux desktop system to support NUMA
  - new version of NTFS 
    - dynamic resizing possible
    - transaction support
  - UAC
  - new driver model - vendors caught by surprise

- Windows 7

  - Mostly a refreshed version of Vista bu tan important one nonetheless
  - Fixes to memory manager's "hot memory" problem
  - Mainstream 64-bit support by hardware vendors (finally!)
  - Many UI cleanups that Vista failed on
  - Virtualization support
  - New taskbar!


- Windows 10

  - Continued improvements to virtualization
  - Windows Terminal
  - Windows Subsystem For Linux (1 and 2)
  - Native Linux Kernel Support for Windows


History of Linux
----------------

- 89 - 93 -- 0.x:
  - Inspired by Dr. Tanenbaum's Minix operating system.
  - Used Minix FS format
  - Developed on various 386/486 machines
  - Protected memory, no swapper
  - Processes, no kernel theads

- 94 -- 96 1.x:
  - Torvalds declares Linux to be "fully matured".
  - Support for X-11
  - Support for non x86 begins to appear: SPARC, PPC, etc...

- 96 - 01 -- 2.0/2.2
  - Begins to become usable by advanced to intermediate hobbyists
  - SMP support. Linux "scales" to 4-8 CPUs
  - No real kernel thread support
  - Greatly improved FS implementations
  - Other vendors begin taking interest. Hardware support improves greatly

- 01-03 -- 2.4
  - much improved FS support - journaling, picks up legacy UNIX FS
  implementations (XFS from IRIX especially)
  - much improved hardware vendor support, IBM, HP, and others
  - much improved, but ever changing virtual memory implementations
  - begins to be competitive with Windows / UNIX in terms of performance
  - still no real kernel thread support.

- 03-11 -- 2.6
  - Much improved SMP support. By mid 2000s, support for CPU counts seen in Windows Server / Solaris / AIX were a reality for Linux.
  - Huge server install base. Killer app: Apache
  - non-technical user base increases by leaps and bounds
  - non personal computer / server installations - cell phones, media centers, industrial, etc... increase by leaps and bounds
  - Genuine kernel threads support
  - Surpasses Windows / UNIX performance for non-GUI operations
  - Much better FS support- ext3, ext4, reiser, xfs; still room for
  improvement based on NTFS progress
  - Filesystm stability comes into parity with other operating systems
  - Much better GUI performance - mostly from driver support
  - Much better virtual memory performance
  - Early competitor to the 2nd generation of virtualization support
  (IBM and zOS being the 1st generation)

Types of computer systems
-------------------------

- batch processing
- personal / parallel computers
- clusters
- grid computers
- distributed systems
- gaming consoles
- handheld/tablet computers
- smart phones

batch processing systems
------------------------

- very typical of early systems
- offline processing
- program and data are prepared
- program and data are loaded onto machine
- machine executes process
- machine stores results
- results are interpreted and output

Batch systems remain important, especially in CPU-bound environments where predictable performance is a requirement.

time sharing systems
--------------------

- multiple users share the computer simultaneously
- operating system gives a small portion of CPU time to each user
- main use cases for telnet and the internet in the past
- possible to run interactive programs such as editors, compilers, take user input, etc...

personal / parallel computers
-----------------------------

- used to be the case that personal and parallel computers were in a different category
- machine has many CPUs. ranges from as few as two to as many as 64
- have shared memory across the processors (memory hierarchies)
- operating system use symmetric multi-processing (SMP)
- each CPU runs a copy of the OS
- cost of machines is typically O(n^2) where N is the CPU count
- programs that require a high amount of communication run best on these types of systems (OpenMP programs for example)

clusters / clouds / grids
-------------------------

- alternative to parallel systems
- clusters
  - cost of system is O(n), where n is the CPU count
  - typically a collection of inexpensive PCs
  - doesn't have shared memory across all processors
  - all machines are typically in one location

- clouds - Azure, EC2, etc...

  - commercial version of a cluster - company builds and maintains a large set of machines in one or more clusters.
  - makes money by renting out one or more nodes in the cluster

- grids - typically universities, volunteer computing

  - not all at one location, perhaps across the internet
  - donation of idle CPU time - SETI, Folding@Home, etc...

Types of operating systems
--------------------------

- Single program / OS
- Real - time
- Microkernel
- Monolithic kernel
- Distributed

Single program / simple OS
--------------------------

- Usually exist for single purpose computers such as older cellular phones, industrial machinery, automobiles, ATMs, older cash registers, etc...

- Composed of either

  - a combined operating system / application 
  - a single operating system program and single application

- Examples:
  - MS-DOS or CP/M + simple program
  - Some uses of OS/2 - especially in ATMs

Real-time OS
------------

- Used where application's most important concern is completing a task on a deadline and at regular intervals rather than performance
  - Sensor measurement / scanners, especially medical
  - Industrial applications
  - Automobile applications such as anti-lock brakes, traction control
- Typically make use of cooperative multitasking 
- Examples:
  - QNX
  - MicroC
  - VxWorks
  - Layered systems on top of MS-D

Microkernel OS
---------------

- Basic operating system services including swapper, driver management, process scheduling, and system call handlers are separated into N kernel processes that pass messages to each other through the microkernel
- Microkernel is very small
- Typically have much more context switch overhead
- Message passing schemes present an additional engineering problem 
- Successful examples:
  - Minix (still a work in progress)
  - OS-X (via Mach)
  - Hurd (via Mach, then L4)

Monolithic Kernel OS
--------------------

- Single kernel process that manages swapper, drivers, system calls, etc...
- Examples:
  - Earlier versions of Windows
  - UNIX: AIX, HP-UX
  - Linux, Free/Net/OpenBSD
- Lower context switch overhead
- Many believe that they are easier to develop for

Distributed OS
--------------

- Presents to an application a single system image of more than one
  computer on a network
- Examples:
  - Amoeba
  - Plan 9
  - Mosix extensions to Linux
- Commercially successful examples: none
- Most distributed computing is accomplished by compilers and runtime
  libraries outside of the operating system

