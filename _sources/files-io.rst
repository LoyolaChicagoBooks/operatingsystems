Files and I/O
===========================

.. index::
   single: files; UNIX
   pair: UNIX; file abstraction
   single: file descriptor
   single: open()
   single: read()
   single: write()
   single: close()
   single: select()

UNIX systems use files as a common interface for many resources. Regular
files, directories, devices, pipes, and sockets have different behavior,
but they are all named in the filesystem and many of them support the
same basic operations: ``open()``, ``read()``, ``write()``, ``close()``,
and ``select()``.

Common attributes of all (UNIX) files
-------------------------------------

.. index::
   single: filesystem namespace
   single: file permissions
   single: file attributes
   pair: UNIX; file attributes
   single: extended attributes

Files live in the filesystem namespace under ``/``. UNIX paths do not use
drive letters. Each file has a name, an owning user, an owning group, and
permission bits for the owner, group, and other users.

Many filesystems also store extended attributes, creation time, last
access time, and modification time. Beyond these shared attributes, file
types vary widely in structure and behavior.

Types of Files in Unix
----------------------

.. index::
   single: file types; UNIX
   single: regular files
   single: symbolic links
   single: directories
   single: block device files
   single: character device files
   single: named pipes
   single: UNIX domain sockets

UNIX file types include regular files, symbolic links, directories, block
device files, character device files, named pipes, UNIX domain sockets,
and, on Solaris, doors.

Regular Files
-------------

.. index::
   single: regular files
   single: sparse files
   pair: files; random access

Regular files persist program data in a filesystem. They have ownership,
permissions, and a defined size. They can normally be accessed
sequentially or with random access through seek operations.

Sparse files are an important exception to the simple size model. A
filesystem may represent large unwritten regions without allocating disk
blocks for every byte.

Folders
-------

.. index::
   single: directories
   pair: filesystem; directories
   pair: permissions; execute bit

Directories map names to filesystem entries. Early UNIX implementations
represented directories as special files that listed other files. Modern
systems still expose some of that model, but programs modify directories
through directory-specific system calls rather than by writing directory
bytes directly.

Directories do not behave like regular files. The execute bit controls
whether a process can traverse the directory and use it in path
resolution. Read permission controls whether entries can be listed.

Symbolic Links
--------------

.. index::
   single: symbolic links
   pair: filesystem; symbolic links
   single: unlink()

A symbolic link is a file whose contents point to another file or
directory. The target may be an absolute path or a relative path. If the
target does not exist, the symbolic link is broken.

Most filesystem operations follow a symbolic link and operate on its
target. ``unlink()`` is the main exception: it removes the symbolic link
itself.

Block Device Files
------------------

.. index::
   single: block device files
   single: devfs
   single: sysfs
   pair: Linux; device files

Block device files expose devices that operate in fixed-size blocks.
Common examples are disks, optical drives, USB media, and RAM disks.
Block devices usually support random access and buffered I/O.

Operating systems expose block devices through special filesystems or
device-management tools. Linux has used several mechanisms over time,
including device files and special filesystems such as ``devfs`` and
``sysfs``.

Character Device Files
----------------------

.. index::
   single: character device files
   single: terminal
   single: serial ports
   pair: devices; character devices

Character device files expose stream-oriented devices. Common examples
include terminals, serial ports, modems, network interfaces, sound
devices, video devices, and tape drives.

Most character devices do not support random access. When they do,
seeking is often expensive or device-specific.

Named Pipes/FIFOs
-----------------

.. index::
   single: named pipes
   single: FIFOs
   pair: IPC; named pipes

Named pipes are pipes with names in the filesystem. They let unrelated
processes communicate even when the processes do not share a parent-child
relationship. We cover pipes in more detail in the IPC chapter.

Unix Domain Sockets
-------------------

.. index::
   single: UNIX domain sockets
   pair: IPC; UNIX domain sockets

UNIX domain sockets are sockets with names in the filesystem. They are
similar to named pipes, but can support stream or datagram communication.
Unlike network sockets, they do not use TCP/IP or UDP/IP.

Filesystem System Calls
-----------------------

.. index::
   single: filesystem system calls
   single: creat()
   single: lseek()
   single: fcntl()
   single: ioctl()
   single: stat()
   single: access()
   single: umask()

Most UNIX systems have many file-oriented system calls. The basic pattern
is simple: open a resource, perform operations on its file descriptor,
and close the descriptor when finished.

Filesystem System Calls
-------------------------

.. list-table:: Filesystem Calls
   :widths: 10 20
   :header-rows: 1

   * - Function
     - Description

   * - ``open()``
     - opens or creates files and returns a file descriptor

   * - ``creat()``
     - creates new files

   * - ``close()``
     - closes a file descriptor

   * - ``lseek()``
     - updates a file descriptor's current file offset

   * - ``read()``
     - reads data from a file descriptor into a buffer

   * - ``write()``
     - writes data from a buffer to a file descriptor

   * - ``dup()``
     - duplicates one file descriptor

   * - ``dup2()``
     - updates a file descriptor to point to another one

   * - ``fcntl()``
     - changes file properties such as asynchronous I/O and file locks

   * - ``ioctl()``
     - handles device-specific control operations

   * - ``stat()``
     - returns permissions, size, timestamps, and other metadata

   * - ``access()``
     - tests for read, write, execute, or existence of a file

   * - ``umask()``
     - updates the file creation mask

   * - ``chmod()``
     - updates permission bits

More Filesystem System Calls
------------------------------

.. index::
   single: truncate()
   single: link()
   single: rmdir()
   single: remove()
   single: rename()
   single: symlink()
   single: readlink()
   single: utime()
   single: opendir()
   single: readdir()
   single: rewinddir()
   single: closedir()
   single: chdir()
   single: getcwd()
   single: sync()

.. list-table:: Filesystem Calls
   :widths: 10 20
   :header-rows: 1

   * - Function
     - Description

   * - ``chown()``
     - changes file user or group ownership

   * - ``truncate()``
     - changes the length of a file

   * - ``link()``
     - creates a hard link

   * - ``unlink()``
     - removes a name from the filesystem

   * - ``rmdir()``
     - deletes an empty directory

   * - ``remove()``
     - combines ``unlink()`` and ``rmdir()`` behavior

   * - ``rename()``
     - renames a file, possibly moving it to another directory

   * - ``symlink()``
     - creates a symbolic link

   * - ``readlink()``
     - reads the value stored in a symbolic link

   * - ``utime()``
     - updates access and modification times

   * - ``mkdir()``
     - creates a directory

   * - ``opendir()``
     - opens a directory for reading

   * - ``readdir()``
     - reads the next directory entry

   * - ``rewinddir()``
     - resets directory reading to the beginning

   * - ``closedir()``
     - closes a directory descriptor

   * - ``chdir()``
     - changes the current working directory

   * - ``getcwd()``
     - gets the current working directory

   * - ``sync()``
     - flushes filesystem buffers to disk

Opening Files with open()
-------------------------

.. index::
   single: open(); flags
   single: O_APPEND
   single: O_CREAT
   single: O_TRUNC
   single: O_SYNC
   single: O_DIRECT
   single: O_ASYNC
   single: mode_t
   pair: open(); permission bits

``open()`` opens a file and returns a file descriptor.

.. code-block:: c

   int open(const char *pathname, int flags, mode_t mode);
   int open(const char *pathname, int flags);

``pathname`` is the path to the file. ``flags`` controls how the file is
opened. Common flags include ``O_APPEND``, ``O_ASYNC``, ``O_CREAT``,
``O_DIRECT``, ``O_SYNC``, and ``O_TRUNC``.

When ``O_CREAT`` is used, ``mode`` supplies permission bits. These are
usually written in octal. For example, ``0700`` gives the owner read,
write, and execute permission, while group and other users receive no
permissions. ``0660`` gives the owner and group read/write permission.

The return value is a file descriptor on success and ``-1`` on error.

Closing files with close()
--------------------------

.. index::
   single: close()
   pair: system call; close

``close()`` releases a file descriptor.

.. code-block:: c

   int close(int fd);

The argument is a descriptor returned by calls such as ``open()``,
``dup()``, or ``pipe()``. The return value is ``0`` on success and ``-1``
on failure.

Writing to a File
-----------------

.. index::
   single: write(); partial writes
   pair: system call; write

``write()`` copies bytes from a user buffer to a file descriptor.

.. code-block:: c

   ssize_t write(int fd, const void *buf, size_t count);

``fd`` is the file descriptor, ``buf`` is the source buffer, and
``count`` is the number of bytes to write. A return value of ``-1`` means
an error occurred. A successful call usually returns ``count``, but code
that needs to be robust should handle partial writes.

Writing Example
---------------

The ``systems-code-examples/read-write-demo`` directory contains small
separate examples for direct ``read()`` and ``write()`` calls.

.. literalinclude:: ../examples/systems-code-examples/read-write-demo/write-demo.c
   :language: c
   :linenos:

Key points:

- ``open()`` creates or truncates the file named ``file``.
- ``strlen()`` is used to compute the number of bytes to write.
- The loop handles the possibility that ``write()`` writes fewer bytes
  than requested.
- ``offset`` advances through the source string as bytes are written.
- ``close()`` releases the descriptor after the write loop finishes.

Reading from a File
-------------------

.. index::
   single: read(); end of file
   pair: system call; read

``read()`` copies bytes from a file descriptor into a user buffer.

.. code-block:: c

   ssize_t read(int fd, void *buf, size_t count);

The return value is ``-1`` on error, ``0`` at end of file, and otherwise
the number of bytes read. The number of bytes returned may be smaller
than ``count``.

Reading Example
---------------

.. literalinclude:: ../examples/systems-code-examples/read-write-demo/read-demo.c
   :language: c
   :linenos:

Key points:

- ``open()`` opens the existing file for reading.
- The buffer is deliberately small so the loop has to run more than once
  for longer input.
- ``read()`` returns ``0`` at end of file, which terminates the loop.
- ``write(1, ...)`` writes the bytes to standard output.
- This example omits detailed error handling so the read/write pattern is
  easy to see.

Complete File I/O Example
-------------------------

The ``systems-code-examples/file_rw`` example combines creation, writing,
reading, closing, and removing a file.

.. literalinclude:: ../examples/systems-code-examples/file_rw/main.c
   :language: c
   :linenos:

Key points:

- The first ``open()`` creates ``temp.dat``, truncating any existing file
  with the same name.
- The write loop keeps going until all bytes from ``file_data`` are
  written.
- The file is closed before it is reopened for reading.
- The read loop writes each chunk to standard output as it is read.
- ``unlink()`` removes the file's name from the filesystem at the end of
  the example.

Seeking within a File
---------------------

.. index::
   single: lseek()
   single: SEEK_SET
   single: SEEK_CUR
   single: SEEK_END
   pair: system call; lseek
   pair: files; seeking

Not all files support seeking. Regular files usually do. Pipes, sockets,
and many character devices do not.

``lseek()`` changes the current file offset for a descriptor.

.. code-block:: c

   off_t lseek(int fd, off_t offset, int whence);

``whence`` is usually ``SEEK_SET`` for the beginning of the file,
``SEEK_CUR`` for the current offset, or ``SEEK_END`` for the end of the
file. Seeking beyond the current end of a file can create a sparse file
on filesystems that support sparse allocation.

Standard File Descriptors
-------------------------

.. index::
   single: standard file descriptors
   single: stdin
   single: stdout
   single: stderr
   pair: file descriptor; standard

Every process starts with three standard file descriptors.

stdin
    Standard input. The default descriptor value is ``0``.

stdout
    Standard output. The default descriptor value is ``1``.

stderr
    Standard error. The default descriptor value is ``2``.

The parent process may redirect any of these before the child program
runs.

Duplicating File Descriptors
----------------------------

.. index::
   single: dup()
   pair: system call; dup
   pair: file descriptor; duplication

``dup()`` duplicates a file descriptor and returns a new descriptor
number.

.. code-block:: c

   int dup(int fd);

Programs use duplicated descriptors when they need another reference to
the same open file. They are also useful in shell-style redirection.

Redirecting File Descriptors
----------------------------

.. index::
   single: dup2()
   pair: system call; dup2
   pair: shell; I/O redirection

``dup2()`` redirects one descriptor to refer to another open file.

.. code-block:: c

   int dup2(int oldfd, int newfd);

If ``newfd`` is already open, ``dup2()`` closes it first. After the call,
``newfd`` refers to the same open file description as ``oldfd``. This is
how shells redirect standard input, output, and error.

Reading Folders
---------------

.. index::
   single: opendir()
   single: readdir()
   single: closedir()
   single: DIR*
   pair: directory; traversal

Directories are read with directory-specific library calls such as
``opendir()``, ``readdir()``, and ``closedir()``.

Directory Traversal in C
------------------------

The ``systems-code-examples/dir_list`` example lists entries in the root
directory.

.. literalinclude:: ../examples/systems-code-examples/dir_list/main.c
   :language: c
   :linenos:

Key points:

- ``opendir()`` returns a ``DIR*`` handle for reading directory entries.
- ``readdir()`` returns one ``struct dirent`` at a time.
- The loop ends when ``readdir()`` returns ``NULL``.
- ``closedir()`` releases the directory handle.
- The example prints entry names, not full paths.

Directory Traversal in C++
--------------------------

.. index::
   pair: C++; filesystem library
   single: std::filesystem::directory_iterator

The ``systems-code-examples/dir_list_cpp`` example performs the same
basic operation with C++17 filesystem support.

.. literalinclude:: ../examples/systems-code-examples/dir_list_cpp/main.cpp
   :language: c++
   :linenos:

Key points:

- ``std::filesystem::directory_iterator`` wraps the directory traversal
  pattern in a C++ range-like interface.
- The exception handler catches filesystem errors, such as permission
  problems.
- ``entry.path().filename()`` extracts just the entry name.
- The underlying work still depends on operating system directory
  operations.

Text Input
----------

.. index::
   single: text input
   pair: C; line input
   pair: C; buffer management

Text-oriented programs often read a line, split it into words, and then
process those words. C gives you low-level tools for this, which means
programs must be careful about buffer sizes, terminators, and ownership
of allocated memory.

Bounded Line Input Example
--------------------------

The ``systems-code-examples/better_gets`` example implements a bounded
line input function. The surrounding program also demonstrates signal and
child-process handling, but the relevant part for this chapter is
``better_gets()``.

.. literalinclude:: ../examples/systems-code-examples/better_gets/main.c
   :language: c
   :linenos:

Key points:

- ``better_gets()`` accepts both a buffer and a maximum length.
- The loop stops before the buffer is full so there is room for the null
  terminator.
- Input stops at newline, end of file, or the configured length.
- The implementation reads from descriptor ``0``, which is standard
  input.
- The rest of the example shows why reusable input helpers matter in
  larger systems programs.

Tokenizing Input with strtok()
------------------------------

.. index::
   single: strtok()
   pair: C library; strtok
   single: string tokenization

The ``systems-code-examples/strtok`` example splits a string into tokens
using whitespace delimiters.

.. literalinclude:: ../examples/systems-code-examples/strtok/main.c
   :language: c
   :linenos:

Key points:

- ``strtok()`` modifies the input string by replacing delimiters with
  null terminators.
- The first call receives the original string. Later calls pass ``NULL``
  to continue tokenizing the same string.
- The example stores pointers into the original string; it does not copy
  each token.
- The test function makes the expected token sequence explicit.

Dynamic String Buffer
---------------------

.. index::
   single: dynamic string buffer
   single: realloc()
   pair: C library; realloc
   single: strbuffer_t

The ``systems-code-examples/strbuffer_t`` example builds a small dynamic
string buffer. It supports appending characters and returning a
heap-allocated C string.

.. literalinclude:: ../examples/systems-code-examples/strbuffer_t/strbuffer.h
   :language: c
   :linenos:

.. literalinclude:: ../examples/systems-code-examples/strbuffer_t/strbuffer.c
   :language: c
   :linenos:

Key points:

- ``strbuffer_t`` stores the backing array, capacity, resize increment,
  and current position.
- ``strbuffer_append()`` checks whether the buffer needs to grow before
  storing the next character.
- ``realloc()`` is used to grow the buffer while preserving existing
  contents.
- ``strbuffer_tostring()`` returns a separate heap allocation, so callers
  must eventually free it.
- ``strbuffer_getline()`` builds a line one character at a time.

String Buffer Input Example
---------------------------

The ``systems-code-examples/strbuffer-demo`` directory shows how the
string buffer can be used to read lines and split them into words.

.. literalinclude:: ../examples/systems-code-examples/strbuffer-demo/strbuffer_getline_demo.c
   :language: c
   :linenos:

Key points:

- The example uses ``strbuffer_getline()`` so line length is not fixed by
  a small stack buffer.
- Each returned line is tokenized with ``strtok()``.
- The loop exits when the input function reports end of file.
- Each heap-allocated line is freed after it is processed.

Regular Expression Example
--------------------------

.. index::
   single: regular expressions
   single: regcomp()
   single: regfree()
   pair: POSIX; regular expressions

The ``systems-code-examples/regex-demo`` example reads text and extracts
matches using the POSIX regular expression API.

.. literalinclude:: ../examples/systems-code-examples/regex-demo/try-regex.c
   :language: c
   :linenos:

Key points:

- If the program receives an argument, it processes that argument as the
  input text.
- If there is no argument, it reads a line from standard input with the
  string buffer helper.
- ``regcomp()`` and related POSIX regex calls are wrapped in helper
  functions from the example.
- The match list is explicitly initialized, printed, and deleted.
- ``regfree()`` releases resources owned by the compiled regular
  expression.

Character List Case Study
-------------------------

.. index::
   single: tail queue (TAILQ)
   pair: data structures; linked list

The ``systems-code-examples/charlist_t`` example stores characters in a
tail queue. It is a small data-structure case study for text processing.

.. literalinclude:: ../examples/systems-code-examples/charlist_t/charlist.c
   :language: c
   :linenos:

Key points:

- Each character is stored in a heap-allocated list entry.
- ``TAILQ_INSERT_TAIL`` appends entries while preserving order.
- ``charlist_tostring()`` converts the list back into a normal C string.
- ``charlist_delete()`` walks the list and frees each entry.
- This representation is flexible but much heavier than a contiguous
  string buffer.

Word Counting with Hash Tables
------------------------------

.. index::
   single: hash tables
   single: hsearch
   pair: data structures; hash table

The ``systems-code-examples/std-hsearch`` example uses the standard
``hsearch`` family to maintain word counts.

.. literalinclude:: ../examples/systems-code-examples/std-hsearch/wordtable-demo.c
   :language: c
   :linenos:

Key points:

- ``wordtable_init()`` prepares a hash table with a fixed capacity.
- Repeated words increment an existing count rather than creating a new
  logical word.
- The example looks up words after insertion and after decrementing
  counts.
- Cleanup is explicit because the table owns allocated entries.
- Hash tables are useful when fast lookup matters more than sorted order.

Word Counting with Trees
------------------------

.. index::
   single: binary search tree
   single: tsearch()
   single: twalk()
   single: tdestroy()
   pair: data structures; binary search tree

The ``systems-code-examples/std-tsearch`` example stores word information
in a binary search tree.

.. literalinclude:: ../examples/systems-code-examples/std-tsearch/tsearch-wordinfo-demo.c
   :language: c
   :linenos:

Key points:

- ``tsearch()`` inserts or finds an entry using the comparison function.
- The comparison function orders entries by word text.
- Repeated words find the existing node and increment its count.
- ``twalk()`` traverses the tree and calls a callback for each node.
- ``tdestroy()`` releases the tree at the end.

Looking Ahead: I/O Performance
------------------------------

.. index::
   single: I/O performance
   single: buffering strategy
   pair: I/O; buffering
   single: producer-consumer pattern

Good I/O performance depends on choosing the right buffering strategy.
Small buffers increase the number of system calls and lower throughput.
Very large buffers may cause longer waits before processing can continue.
Programs often need a balance.

Producer-consumer designs can improve throughput. One thread or process
reads data while another processes it. Memory-mapped I/O is another
approach, and we return to it in the IPC chapter.

Simple I/O Performance Experiment
---------------------------------

.. index::
   single: dd command
   pair: I/O; block size

The ``dd`` command gives a quick view of how block size can affect
throughput.

::

    dd if=/dev/zero of=tmp.dat bs=1 count=1000000 - 671 kB/s
    dd if=/dev/zero of=tmp.dat bs=10 count=100000 - 5.9 MB/s
    dd if=/dev/zero of=tmp.dat bs=100 count=10000 - 38.9 MB/s
    dd if=/dev/zero of=tmp.dat bs=1000 count=1000 - 244 MB/s
    dd if=/dev/zero of=tmp.dat bs=10000 count=100 - 537 MB/s
    dd if=/dev/zero of=tmp.dat bs=100000 count=10 - 834 MB/s
    dd if=/dev/zero of=tmp.dat bs=1000000 count=1 - 461 MB/s

Increasing block size usually improves performance until some other
limit dominates. These are single-run measurements, so system load and
cache state can affect the numbers.

Reading/Writing Performance
---------------------------

.. index::
   single: vectored I/O
   single: gather-scatter I/O
   pair: I/O; vectored

Vectored I/O, also called gather-scatter I/O, combines multiple buffers
into one read or write operation. This reduces the number of system calls
when a program has data split across several buffers.

Vectored I/O Example
--------------------

.. index::
   single: writev()
   single: iovec
   pair: system call; writev

The ``systems-code-examples/vectored_io`` example writes three separate
buffers with one ``writev()`` call.

.. literalinclude:: ../examples/systems-code-examples/vectored_io/main.c
   :language: c
   :linenos:

Key points:

- Each ``struct iovec`` entry describes one buffer and its length.
- ``writev()`` writes the buffers in order as one logical operation.
- This avoids three separate ``write()`` system calls.
- Vectored I/O is useful for data with a header, body, and trailer stored
  in separate buffers.
- The file descriptor is still opened and closed with the same calls used
  for ordinary file I/O.
