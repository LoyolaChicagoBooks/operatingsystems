Processes 1/2
=============

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

Examining process memory layout
-------------------------------

-  Command to try in Linux:

-  using code at
   http://code.google.com/p/operating-system-code-samples/source/

-  run c\_intro/layout:

Loading Programs
----------------

-  loader allocates memory for executable's text, data, bss, heap, and
   stack. and loads program's image into memory

-  loader gets information from OS where shared libraries are already
   allocated in memory and loads the ones that are not already loaded.
   each shared library has its own text, data, and bss

-  loader goes through executable and adjusts the list of external
   symbols to point to the correct spots in memory (to shared libraries)
   - try the 'nm' command

-  once program is ready, loader invokes \_start() method

-  \_start() calls \_init() for each shared library

-  \_start() initializes static constructors of objects defined as
   global variables

-  \_start() calls main() and program begins

Loading shared libraries (.so)
------------------------------

-  Libraries also have a data, bss, and text segment

-  Memory references in shared libraries are position independent (GCC
   -fpic/-fPIC flags)

-  -fpic option (see next slide for details)

-  Linker must resolve all of these position independent memory accesses
   to local accesses. This is accomplished by writing the GOT for each
   linked process.

-  The need for the memory addresses to be position independent is
   because the offset that a shared library will be loaded will differ
   between executions of the same program and among other programs.

-  Also, the same loaded shared library will be shared with other
   processes without reloading. So, the same library may have different
   offsets for different programs

more about -fpic option
-----------------------

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

-  Static libraries are simply a collection of unlinked .o (object)
   files

-  The dynamic linker simply loads the text, data, and bss sections of
   each object file into the program's address space

Position Independent Code Example: main.cc
------------------------------------------

::

	#include "list.hh"
	#include "debug.hh"
	#include "tests.hh"
	#include <stdio.h>
	
	int main(int argc, char *argv[]) {
		int passCount = runTests();
		printf("%d tests passed\n", passCount);
		return 0;
	}


gcc -S main.c
-------------

For the entire output, please run ``gcc -S main.c`` on your computer.

::


		.file	"main.cc"
		.section	.rodata
	.LC0:
		.string	"%d tests passed\n"
		.text
		.globl	main
		.type	main, @function
	main:
	.LFB0:
		.cfi_startproc
		pushq	%rbp
		.cfi_def_cfa_offset 16
		.cfi_offset 6, -16
		movq	%rsp, %rbp
		.cfi_def_cfa_register 6
		subq	$32, %rsp
		movl	%edi, -20(%rbp)
		movq	%rsi, -32(%rbp)
		call	_Z8runTestsv
		movl	%eax, -4(%rbp)
		movl	-4(%rbp), %eax
		movl	%eax, %esi
		movl	$.LC0, %edi
		movl	$0, %eax
		call	printf
		movl	$0, %eax
		leave
		.cfi_def_cfa 7, 8
		ret
		.cfi_endproc
	.LFE0:
		.size	main, .-main
		.ident	"GCC: (Ubuntu/Linaro 4.6.3-1ubuntu5) 4.6.3"
		.section	.note.GNU-stack,"",@progbits


gcc -S -fpic main.c
-------------------

::


		.file	"main.cc"
		.section	.rodata
	.LC0:
		.string	"%d tests passed\n"
		.text
		.globl	main
		.type	main, @function
	main:
	.LFB0:
		.cfi_startproc
		pushq	%rbp
		.cfi_def_cfa_offset 16
		.cfi_offset 6, -16
		movq	%rsp, %rbp
		.cfi_def_cfa_register 6
		subq	$32, %rsp
		movl	%edi, -20(%rbp)
		movq	%rsi, -32(%rbp)
		call	_Z8runTestsv@PLT
		movl	%eax, -4(%rbp)
		movl	-4(%rbp), %eax
		movl	%eax, %esi
		leaq	.LC0(%rip), %rdi
		movl	$0, %eax
		call	printf@PLT
		movl	$0, %eax
		leave
		.cfi_def_cfa 7, 8
		ret
		.cfi_endproc
	.LFE0:
		.size	main, .-main
		.ident	"GCC: (Ubuntu/Linaro 4.6.3-1ubuntu5) 4.6.3"
		.section	.note.GNU-stack,"",@progbits


.. note:: See the difference on line 23 of both assembly outputs.

Shared vs. Static LIbraries
---------------------------

Shared - advantages

-  Reduced memory footprint. If two programs load the same shared
   library, the .text segment is reused across processes thanks to the
   GOT

Shared - disadvantages:

-  Requires a more advanced virtual memory implementation in the
   operating system. Sometimes not practical for simple or embedded
   systems

-  Requires more advanced compiler code generators. Different processors
   have special features regarding memory offset registers or function
   table size limitations.

Static - advantages:

-  Makes sense when re-use is not desired. A good example would be
   installer executables with very large .data segments.

-  Faster first load loading time than shared libraries.

-  Fewer instructions generated for GOT lookups (minor issue)

Libraries vs. Statically-Linked Programs
----------------------------------------

Dynamic linking - advantages:

-  Memory footprint

-  Code reuse

-  Improvement with new versions of shared libraries

-  Smaller executables

Static linking - advantages:

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
   parameter passing standards, stack management, etc...

-  A crash, exception, resource starvation, deadlock, or other fault in
   one process does not directly affect other processes

-  While mapped to the same address space, the process cannot modify
   kernel memory or memory otherwise protected by the operating system
   (such as text pages).

Process Creation with fork()
----------------------------

"man fork": creates a new process by duplicating the calling process.
The new process, referred to as the child, is an exact duplicate of the
calling process, referred to as the parent, except for the following
points:

-  the child has its own unique process id (PID)

-  the child's parent PID is the same as the parent's PID

-  the parent's threads are not recreated on the child

interesting point: in Linux, fork() != fork(); fork() calls clone() From
the man page:

-  fork() returns the child PID to the parent

-  fork() returns 0 to the child

-  fork() returns -1 if the child cannot be created

fork() example
--------------

Source:
http://code.google.com/p/operating-system-code-samples/source/browse/trunk/fork/main.cc.

::

    #include <stdio.h>
    #include <unistd.h>
    #include <fcntl.h>
    #include <string.h>

    int main(int argc, char* argv[]) {
            int SomeValue = 100;
            int pid = fork();
            int fd = open("test_file", O_WRONLY|O_CREAT|O_TRUNC, 0666);

            const char *parentMessage = "1111111";
            const char *childMessage =  "22222222222222\n";

            if(pid > 0) {
                    printf("hello from the parent process, chid pid = %d\n", pid);
                    sleep(2);
                    printf("parent's SomeValue = %d\n", SomeValue);
                    write(fd, parentMessage, strlen(parentMessage) * sizeof(char));
            } else if(pid == 0) {
                    printf("hello from the child process\n");
                    SomeValue = 200;
                    printf("child's SomeValue = %d\n", SomeValue);
                    write(fd, childMessage, strlen(childMessage) * sizeof(char));
            } else {
                    printf("fork() failed!!\n");
            }

            close(fd);

            return 0;
    }

Process Creation with clone()
-----------------------------

-  similar to fork() in that a child process is created.

-  clone() allows different parts of the parent process to be shared
   with the child process

-  flags for creating a light weight process (kernel thread):

   -  CLONE\_FS - share FS information (chroot, chdir, umask)

   -  CLONE\_FILES - share file descriptor table

   -  CLONE\_SIGHAND - share signal handlers

   -  CLONE\_VM - share page table

-  many more flags exist - don't forget this little known capability!

-  glibc's version of fork, calls clone without any of these flags

-  clone() not present in every UNIX OS (available in Linux but not
   Minix)

Windows CreateProcess() and CreateThread()
------------------------------------------

-  Different from UNIX fork()/clone() - parts of processes are not
   shared

-  Windows has two flavors:

   -  CreateProcess() - creates a new process, equivalent of calling
      fork() then execve() in UNIX

   -  CreateThread() - equivalent of creating clone() with thread flags

-  Is this a disadvantage?

   -  For most use cases and most programs, no.

   -  The vast majority of calls to clone() in UNIX are equivalent to
      CreateThread()

   -  The vast majority of calls to fork() in UNIX are equivalent to
      CreateProcess()

Emulating fork() on Windows
---------------------------

A well known system, Cygwin, implements fork() on Windows as follows:

1. cygwin.dll calls CreateProcess() to create a suspended child process

2. parent process calls setjmp to save registers

3. parent process copies its BSS and DATA sections to the child's
   address space.

4. parent wakes child up and waits on a named mutex

5. child wakes up, realizes it was a forked process, then longjumps to
   the saved jump buffer. child unlock's

6. parent's named mutex and waits on a second mutex

7. parent wakes up, copies its stack and heap to the child process.
   release's child's named mutex

8. Child wakes up and copies any memory mapped regions the parent
   signals to the child through shared memory

9. fork() system call in Cygwin does not use copy on write, but "copy on
   fork". this is similar to fork() implementations in early UNIX
   operating systems

Causes of process termination
-----------------------------

-  Normal exit–return from main(...)

-  Error exit–return from main(...) with an error code

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

::

    int main(int argc, char* argv[]) {
        pid_t pid = fork();
        if(pid == 0) {
            abort();    //child process exits
        }
        int status;
        wait(&status); // wait for child to exit
        if(WIFEXITED(status)) {
            printf("normal exit. exit code = %d\n", WEXITSTATUS(status));
        } else if(WIFSIGNALED(status)) {
            printf("abnormal termination, signal number = %d\n", WTERMSIG(status));
        } else if(WIFSTOPPED(status)) {
            printf("child stopped, signal number = %d\n", WSTOPSIG(status));
        }
    }

Files and I/O
-------------


Common attributes of all (UNIX) files
-------------------------------------

-  All files:

   -  Live in the filesystem namespace (under '/')

   -  Have a name

   -  Implement read, write, open, close, and select system calls.

-  All can be contained in either normal or 'special' folders

-  All have a concept of a:

   -  Owning user and group

-  Read/write/execute bits for the owning user/group, and for other
   users/groups

-  A list of custom extended attributes

-  Creation date/time

-  Last accessed date/time

-  Beyond these few things, there's a great degree of variety in
   semantics and structure for various file types


Types of Files in Unix
----------------------

-  Regular files

-  Symbolic links

-  Folders

-  Block device files

-  Character device files

-  Named pipes/FIFOs

-  UNIX domain sockets

-  Doors (Solaris only)


Regular Files
-------------

-  Persist data from programs. Reside in filesystems.

-  In addition to owner/permissions. Regular files have:

   -  A committed and defined size (which differ for filesystems that
      support sparse files)

   -  Can be accessed sequentially

   -  Can be accessed in random order

-  Exceptions exist for device restrictions such as exit for tape drives


Folders
-------

-  In early UNIX implementations, folders were files that listed other
   files and had a special bit set to make them folders.

-  Folders were modified by reading from and writing to the file.

-  Some of these semantics still exist

-  Early operating systems did not support folders:

   -  Macintosh file system (circa 1984)

   -  CP/M file system (predecessor to MS-DOS and FAT)

-  Folders do not have a file size

-  The execute bit for a folder determines:

   -  If the contents of the folder may be listed

   -  If a program may change use it as its working folder

Symbolic Links
--------------

-  Symbolic links are a type of file that points to another file or
   folder.

-  The pointer can be either a relative or absolute path.

-  Support exists in many modern operating systems (OS X, UNIX, and
   Windows)

-  A symbolic link that refers to a file or folder that does not exist
   is called 'broken'

-  File system operations on a symbolic link work on the file that they
   point to with the exception of the unlink system call (to delete the
   symbolic link)

-  Other system calls exist to help determine if a file/folder is a
   symbolic link or a real file/folder

Block Device Files
------------------

-  Block device files are file abstractions for devices exposed by the
   operating system.

-  Common device block files are:

   -  Hard disks

   -  CD/DVD/Blu-Ray drives

   -  Floppy drives

   -  USB media

   -  Mapped memory devices (RAM disks, or diagnostic devices)

-  Block devices support:

   -  Random access

   -  Buffered read/write (through some characteristic block size)

   -  Block device files are either automatically exposed by the
      operating system through special file systems or are user created
      through special system programs and system calls. Approaches vary.

   -  Early Linux depended upon special programs

   -  Modern Linux uses special filesystems (devfs, sysfs)

Character Device Files
----------------------

-  Character device files are file abstractions for devices exposed by
   the operating system.

-  Common character devices are:

   -  terminals

   -  serial ports

   -  modems

   -  network cards

   -  video/sound devices

   -  tape drives

-  Most character devices do not support random access.

-  Those that do, typically have a high cost for seek operations

Named Pipes/FIFOs
-----------------

-  Named pipes are pipes that exist in the filesystem.

-  Allow for pipe operations in sets of programs that have different
   lifetimes such as client server programs.

-  We will dig into more detail on pipes when we discuss inter-process
   communication.

Unix Domain Sockets
-------------------

-  Domain sockets are sockets that have a name in the filesystem.

-  Similar to named pipes except they can be created in a streaming or
   datagram mode

-  Unlike regular sockets, domain sockets do not have an underlying
   TCP/IP or UDP/IP protocol

Filesystem System Calls
-----------------------

-  A majority of the system calls in a UNIX operating system exist to
   operate upon files

-  The acronym MS-DOS expands to MicroSoft Disk Operating System. The
   DOS part of this acronym seems to greatly apply to all operating
   systems.

A Nearly (In)complete LIst
--------------------------

open() creat() close() lseek() read() write() dup() dup2() fcntl()
ioctl() stat() fstat() lstat() chmod() chown() truncate() link()
unlink() remove() rename() symlink() readlink() utime() mkdir() rmdir()
opendir() readdir() rewinddir() closedir() chdir() getcwd() sync()
umask() access()

Filesystem Calls
----------------

open(): opens/creates files and returns a file descriptor

creat(): creates new files

close(): closes a file descriptor (reduces references to the file)

lseek(): updates a file descriptor's current file offset

read(): reads data from a file descriptor into a buffer

write(): writes data from a buffer to a file descriptor

dup(): duplicates one file descriptor

dup2(): updates a file descriptor to point to another one

fcntl(): changes file properties (asynchronous I/O, file locks)

ioctl(): a 'catch all' interface that interacts with device files,
setting atypical properties, etc...

stat(): returns rwx bits, size, timestamps, and other details

access(): tests for read, write, execute, or existence of a file

umask(): updates file creation mask

chmod(): updates rwx bits

Filesystem System Calls
-----------------------

chown(): changes file user/group ownership

truncate(): change the length of a file (grow or shrink)

link(): create a hard link

unlink(): remove a name in the filesystem and possibly the file it
refers to (no processes have the file open)

rmdir(): deletes empty directories

remove(): combines unlink/rmdir into one call

rename(): renames a file, possibly changing its parent folder

symlink(): creates a symbolic link

readlink(): reads the value of a symbolic link

utime(): updates the access and modification time

mkdir(): creates a folder

opendir(): opens a folder for reading

readdir(): reads the next entry in a folder

rewinddir(): resets directory entry to beginning

closedir(): closes a directory descriptor

chdir() - changes current working directory

getcwd() - gets current working directory

sync() - flushes buffer cache for filesystem to disk

Opening Files with open()
-------------------------

int open(const char \*pathname, int flags, mode\_t mode)

int open(const char \*pathname, int flags)

-  *pathname* is the path to the file

-  *flags* can be combinations of:

   -  O\_APPEND: open in append mode

   -  O\_ASYNC: use signal driven asynchronous I/O

   -  O\_CREAT: create the file if it does not exist

   -  O\_DIRECT: minimize use of the buffer cache

   -  O\_SYNC: opened for synchronous I/O - block until write calls are
      committed to hardware

   -  O\_TRUNC: if file already exists, truncate it to length 0

   -  and many others...

-  *mode* is used for O\_CREAT and is typically passed as an octal:

   -  0XYZ, X is for user, Y is for group, Z is for others

   -  each digit, being an octal digit is composed of three bits

   -  the most significant bit is read permissions

   -  the next most significant bit is write permissions

   -  the least significant bit is execute permissions

   -  0700 means user has rwx, group and other have no access

   -  0660 means user/group have rw, other has no acess

-  return value of open() is the file descriptor, or -1 if an error
   happens

Closing files with close()
--------------------------

int close(int fd)

-  *fd* argument is a file descriptor returned by a call to: open, dup,
   pipe, etc...

-  return value is 0 on success or -1 on failure (bad file descriptor,
   interrupted by signal)

Writing to a File
-----------------

ssize\_t write(int fd, const void \*buf, size\_t count);

-  fd is an opened file descriptor

-  but is a buffer

-  count is the number of bytes from that buffer to write to the file at
   the current offset

-  the return value of the method will be

   -  :math:`$return == - 1$` if an error is encountered

   -  :math:`$return == count$` in most successful cases

   -  :math:`$return < count$` in some implementations (network
      filesystems in some cases)

Typical Write Algorithm
-----------------------

::

    const char *data = "foobar";
    int fd = open("file", O_CREAT | O_TRUNC | O_RDWR, 0666);
    size_t length = strlen(data), offset = 0;
    while(length > 0) {
       size_t written = write(fd, data + offset, length);
       offset += written;
       length -- written;
    }
    close(fd)

Reading from a File
-------------------

size\_t read(int fd, void \*buf, size\_t count);

-  takes as arguments a file descriptor, a destination buffer, and the
   number of bytes to read into that buffer

-  the return values of the method will be:

   -  :math:`$return == -1$` if an error occurred

   -  :math:`$return == 0$` if EOF is encountered

   -  :math:`$return == count$` in most success cases

Typical Read Algorithm
----------------------

::

    int fd = open("file", O_RDONLY, 0666);
    char buffer[5];
    while((length = read(fd, &buffer[0], 5)) != 0) {
        write(1, &buffer[0], length);
    }
    close(fd);

Seeking within a File
---------------------

-  Not all files support seeking.

-  The use of seek calls is how random access I/O is performed

-  The use of seek calls have performance implications (more later...)

-  off\_t lseek(int fd, off\_t offset, int whence)

   -  fd is a file descriptor

   -  offset is the number of bytes relative to whence

   -  whence is one of SEEK\_SET (beginning of file), SEEK\_CUR (current
      position of the file descriptor), or SEEK\_END (end of the file)

   -  The off\_t type is typically a 64-bit signed integerIt is possible
      to seek both within and outside of a file.

-  Seeking outside of a file will cause the value of 0 to be written
   from the end of the file to the seek position.

-  Filesystems that support sparse files, will optimize this to prevent
   unnecessary write operations.

Standard File Descriptors
-------------------------

stdin
    standard input. default is the input pipe from the console; default
    value is 0

stdout
    standard output. default is the output pipe to the console; default
    value is 1

stderr
    standard error. default is the output pipe to the console; default
    value is 2

every program is initialized with these three file descriptors open by
default. their specific targets may have been redirected by the parent
program (more later...)

Duplicating File Descriptors
----------------------------

int dup(int fd) : duplicate a file descriptor

-  accepts a file descriptor and returns a copy of it with a new id

-  the duplicated file descriptor has an independent file offset and
   reference to the file

-  reasons to duplicate file descriptors:

   -  for use in multi-threading, to avoid calls to lseek()

   -  one call necessary for redirecting stdin/stdout/stderr

Redirecting File Descriptors
----------------------------

int dup2(int oldfd, int newfd) : redirect a file descriptor

-  makes newfd be a copy of oldfd

-  if newfd is open, it is automatically closed

-  This call differs from dup() in that both of the file descriptors in
   this case share the same file offset.

-  So, calling lseek() on one will cause the offset of the other to
   change.

-  dup and dup2 are used to redirect stdin, stdout, and stderr on the
   command line (sometimes to combine them)

Redirecting File Descriptors code example
-----------------------------------------

::

    int main(int argc, char* argv[]) {
        int pipes[2];
        pipe(pipes);
        int input = pipes[0], output = pipes[1];
        int pid = fork();
        if(pid > 0) {            //parent process
            dup2(input, 0)   //redirect stdin
            close(output);    //close unused half of pipe
            scanf("%d\n", &value);
            printf("child sent value = %d\n", value);
        } else if(pid == 0) {  //child process
            dup2(output, 1); //redirect STDOUT
            close(input);        //close unused half of pipe
            printf("%d\n", 5000);
        }
        return 0;
    }

Reading Folders
---------------

::

    int main(int argc, char* argv[]) {
        const char *dir = "/";
        DIR *d = opendir(dir);

        struct dirent *de;
        while((de = readdir(d)) != NULL) {
            printf("name %s\n", de->d_name);
        }
        closedir(d);
        return 0;
    }

Looking Ahead: I/O Performance
------------------------------

Performance
-----------

-  Achieving good I/O performance is about choosing the right buffering
   strategy.

-  Reading/Writing with small buffers will lead to lower throughput.

-  Reading/Writing with large buffers will create a longer wait for
   read/write to return.

-  This time could be used processing the data.

-  balance must be achieved.

-  Producer/Consumer models are advantageous:

   -  One process/thread reads a file (producer)

   -  Another process/thread runs computation (consumer)

   -  This way, you're computing and performing I/O at the same
      timeConsider memory mapped I/O - (more later when we talk about
      IPC)

Simple I/O Performance Experiment
---------------------------------

::

    dd if=/dev/zero of=tmp.dat bs=1 count=1000000 - 671 kB/s
    dd if=/dev/zero of=tmp.dat bs=10 count=100000 - 5.9 MB/s
    dd if=/dev/zero of=tmp.dat bs=100 count=10000 - 38.9 MB/s
    dd if=/dev/zero of=tmp.dat bs=1000 count=1000 - 244 MB/s
    dd if=/dev/zero of=tmp.dat bs=10000 count=100 - 537 MB/s
    dd if=/dev/zero of=tmp.dat bs=100000 count=10 - 834 MB/s
    dd if=/dev/zero of=tmp.dat bs=1000000 count=1 - 461 MB/s

Reading/Writing Performance
---------------------------

-  Another approach to consider is Vectored I/O a.k.a. Gather-Scatter

-  Programs will often separate reads/writes into different calls.

-  One example would be a program that writes a header and then the
   content in two separate calls.

-  Additional calls involve additional context switches and decreased
   performance.

-  Vectored I/O allows several read/write calls to be combined.

-  Smart operating system implementations will also allow them to be
   read/written out of order.

-  This can make for significant performance gains.

-  We'll see more about this when we study the elevator algorithm as we
   look deeper into storage topics.

Performance Example
-------------------

::

    char *file_data1 = "1234567890";
    char *file_data2 = "abcdefghijk";
    char *file_data3 = "lmnopqrstuvwxyz";
    const char *file_name = "temp.dat";
    int main(int argc, char* argv[]) {

            int fd = open(file_name, O_CREAT|O_TRUNC|O_RDWR, 0666);
            if(fd == (-1)) {
                    printf("open returned (-1)\n");
                    return (-1);
            }

            struct iovec buffers[3];
            buffers[0].iov_base = file_data1;
            buffers[0].iov_len = strlen(file_data1);
            buffers[1].iov_base = file_data2;
            buffers[1].iov_len = strlen(file_data2);
            buffers[2].iov_base = file_data3;
            buffers[2].iov_len = strlen(file_data3);

            int written = writev(fd, buffers, 3);
            if(written == (-1)) {
                    printf("writev returned (-1)\n");
                    return (-1);
            }
            printf("wrote %d bytes\n", written);

            close(fd);
            return 0;
    }

Vectored I/O in Windows
-----------------------

-  Windows has a similar facility to readv and writev: ReadFileScatter
   and WriteFileScatter are analogous.

-  Windows also has additional asynchronous I/O capabilities for these
   and related functions.

-  This allows programs to be notified when individual buffers have
   completed reading or writing.

-  This capability was an important part of the performance improvements
   in recent editions of SQL Server.


