Files and I/O
===========================

Common attributes of all (UNIX) files
-------------------------------------

-  All files:

   -  Live in the filesystem namespace (under root, or ``/``). No drive letters!

   -  Have a name

   -  Implement read, write, open, close, and select system calls.

-  All can be contained in either normal or *special* folders

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

Filesystem System Calls
-------------------------

.. list-table:: Filesystem Calls
   :widths: 10 20
   :header-rows: 1

   * - Function
     - Description

   * - ``open()``
     - opens/creates files and returns a file descriptor

   * - ``creat()``
     - creates new files

   * - ``close()``
     - closes a file descriptor (reduces references to the file)

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
     - changes file properties (asynchronous I/O, file locks)

   * - ``ioctl()``
     - a 'catch all' interface that interacts with device files, setting atypical properties, etc...

   * - ``stat()``
     - returns rwx bits, size, timestamps, and other details

   * - ``access()``
     - tests for read, write, execute, or existence of a file

   * - ``umask()``
     - updates file creation mask

   * - ``chmod()``
     - updates rwx bits

More Filesystem System Calls
------------------------------

.. list-table:: Filesystem Calls
   :widths: 10 20
   :header-rows: 1

   * - ``chown()``
     - changes file user/group ownership

   * - ``truncate()``
     - change the length of a file (grow or shrink)

   * - ``link()``
     - create a hard link

   * - ``unlink()``
     - remove a name in the filesystem and possibly the file it refers to (no processes have the file open)

   * - ``rmdir()``
     - deletes empty directories

   * - ``remove()``
     - combines unlink/rmdir into one call

   * - ``rename()``
     - renames a file, possibly changing its parent folder

   * - ``symlink()``
     - creates a symbolic link

   * - ``readlink()``
     - reads the value of a symbolic link

   * - ``utime()``
     - updates the access and modification time

   * - ``mkdir()``
     - creates a folder

   * - ``opendir()``
     - opens a folder for reading

   * - ``readdir()``
     - reads the next entry in a folder

   * - ``rewinddir()``
     - resets directory entry to beginning

   * - ``closedir()``
     - closes a directory descriptor

   * - ``chdir()``
     - changes current working directory

   * - ``getcwd()``
     - gets current working directory

   * - ``sync()``
     - flushes buffer cache for filesystem to disk

Opening Files with open()
-------------------------

.. code-block:: c

   int open(const char *pathname, int flags, mode_t mode)
   int open(const char *pathname, int flags)

-  *pathname* is the path to the file

-  *flags* can be combinations of:

   -  ``O_APPEND``: open in append mode

   -  ``O_ASYNC``: use signal driven asynchronous I/O

   -  ``O_CREAT``: create the file if it does not exist

   -  ``O_DIRECT``: minimize use of the buffer cache

   -  ``O_SYNC``: opened for synchronous I/O - block until write calls are
      committed to hardware

   -  ``O_TRUNC``: if file already exists, truncate it to length 0

   -  and many others...

-  *mode* is used for ``O_CREAT`` and is typically passed as an octal:

   -  ``0XYZ``, ``X`` is for user, ``Y`` is for group, ``Z`` is for others

   -  each digit, being an octal digit is composed of three bits

   -  the most significant bit is read permissions

   -  the next most significant bit is write permissions

   -  the least significant bit is execute permissions

   -  ``0700`` means user has rwx, group and other have no access

   -  ``0660`` means user/group have rw, other has no acess

-  return value of ``open()`` is the file descriptor, or -1 if an error
   happens

Closing files with close()
--------------------------

.. code-block:: c

   int close(int fd)

-  *fd* argument is a file descriptor returned by a call to: open, dup,
   pipe, etc...

-  return value is 0 on success or -1 on failure (bad file descriptor,
   interrupted by signal)

Writing to a File
-----------------

.. code-block:: c

   ssize_t write(int fd, const void *buf, size_t count);

-  fd is an opened file descriptor

-  but is a buffer

-  count is the number of bytes from that buffer to write to the file at
   the current offset

-  the return value of the method will be

   -  `return == -1` if an error is encountered

   -  `return == count` in most successful cases

   -  `return < count` in some implementations (network filesystems in some cases)

Typical Write Algorithm
-----------------------

.. code-block:: c

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

.. code-block:: c

   size_t read(int fd, void *buf, size_t count);

-  takes as arguments a file descriptor, a destination buffer, and the
   number of bytes to read into that buffer

-  the return values of the method will be:

   -  ``return == -1`` if an error occurred

   -  ``return == 0`` if EOF is encountered

   -  ``return == count`` in most success cases

Typical Read Algorithm
----------------------

.. code-block:: c

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

-  ``off_t lseek(int fd, off_t offset, int whence)``

   -  fd is a file descriptor

   -  offset is the number of bytes relative to whence

   -  whence is one of ``SEEK_SET`` (beginning of file), ``SEEK_CUR`` (current
      position of the file descriptor), or ``SEEK_END`` (end of the file)

   -  The ``off_t`` type is typically a 64-bit signed integer. It is possible
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

.. code-block:: c

   int dup(int fd) : duplicate a file descriptor

-  accepts a file descriptor and returns a copy of it with a new id

-  the duplicated file descriptor has an independent file offset and
   reference to the file

-  reasons to duplicate file descriptors:

   -  for use in multi-threading, to avoid calls to lseek()

   -  one call necessary for redirecting stdin/stdout/stderr

Redirecting File Descriptors
----------------------------

.. code-block:: c

   int dup2(int oldfd, int newfd) : redirect a file descriptor

-  makes ``newfd`` be a copy of ``oldfd``

-  if ``newfd`` is open, it is automatically closed

-  This call differs from ``dup()`` in that both of the file descriptors in
   this case share the same file offset.

-  So, calling ``lseek()`` on one will cause the offset of the other to
   change.

-  ``dup()`` and ``dup2()`` are used to redirect **stdin**, **stdout**, and **stderr** on the
   command line (sometimes to combine them)

Redirecting File Descriptors code example
-----------------------------------------

.. code-block:: c

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

.. code-block:: c

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

In general:

-   Increasing block size improves performance.

-   This is a single run of ``dd`` for each block size. Multiple runs would likely result in higher average throughput.

-   System load at any given time can impact the observed performance numbers.

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

.. code-block:: c

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


