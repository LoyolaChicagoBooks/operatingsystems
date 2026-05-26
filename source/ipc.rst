IPC Topics
==========

.. index::
   single: interprocess communication (IPC)
   single: IPC mechanisms
   pair: UNIX; IPC

Interprocess communication, or IPC, is the set of mechanisms processes
use to exchange data or coordinate activity. UNIX systems provide many
IPC mechanisms because different programs need different tradeoffs among
speed, structure, persistence, and portability.

The main IPC mechanisms covered here are pipes, named pipes, signals,
shared memory, memory mapped files, files, UNIX domain sockets, Solaris
doors, and TCP/IP sockets.

IPC Performance Hierarchy
-------------------------

.. index::
   single: shared memory
   single: memory mapped files
   pair: IPC; performance hierarchy

IPC mechanisms differ mostly in how much kernel involvement they need.

Shared memory and locks in shared memory are usually fastest after setup
because ordinary reads and writes do not require a system call. Memory
mapped files are also fast because they use the virtual memory system.
Pipes, FIFOs, signals, and domain sockets require system calls and
therefore context switches. Files and network sockets can be slower
because they may involve filesystem or network activity.

Pipes
-----

.. index::
   single: pipes
   single: pipe()
   pair: IPC; pipes
   single: half-duplex

A pipe is a half-duplex byte stream between related processes. Data flows
in one direction, from the write end to the read end.

Pipes are among the oldest UNIX IPC mechanisms. A parent process creates
a pipe and then child processes inherit the pipe file descriptors across
``fork()``. Passing data through a pipe requires system calls, but pipes
are still fast enough for many command-line pipelines and process
coordination tasks.

Pipe Descriptors
----------------

.. index::
   single: pipe(); file descriptors
   pair: Windows; CreatePipe()

A pipe has two file descriptors: one for reading and one for writing.

On Linux, the call is:

::

   int pipe(int pipefd[2]);

``pipefd[0]`` is the read-only end. ``pipefd[1]`` is the write-only end.

On Windows, the comparable call is:

::

   BOOL CreatePipe(
      HANDLE readHandle,
      HANDLE writeHandle,
      LPSECURITY_ATTRIBUTES attr,
      DWORD nSize);

The handles represent the read and write ends. The security attributes
control inheritance and permissions, and ``nSize`` gives a suggested
buffer size.

Pipe Example
------------

This local pipe example creates a pipe, forks, and redirects standard
input and output through the pipe.

.. literalinclude:: ipc/code/pipes.c
   :language: c
   :linenos:

Key points:

- ``pipe(pipes)`` creates the read and write file descriptors.
- The parent redirects standard input to the read end with ``dup2()``.
- The child redirects standard output to the write end with ``dup2()``.
- Each process closes the pipe end it does not use.
- The parent reads a value that the child wrote through standard output.

Fork and Pipe Example
---------------------

The ``systems-code-examples/fork_pipe`` example shows the same basic
pattern as a standalone submodule example.

.. literalinclude:: ../examples/systems-code-examples/fork_pipe/main.c
   :language: c
   :linenos:

Key points:

- The pipe is created before ``fork()`` so both processes inherit it.
- The parent uses the read end of the pipe.
- The child uses the write end of the pipe.
- ``dup2()`` lets ordinary ``scanf()`` and ``printf()`` operate through
  the pipe.
- The example depends on the parent and child closing the unused pipe
  halves.

Pipes and Context Switches
--------------------------

.. index::
   pair: pipes; context switches

Pipe I/O crosses the user-kernel boundary. A write enters the kernel,
the kernel buffers the data, and a read later copies the data into the
receiving process.

.. figure:: ipc/diagrams/pipes-context-switches.*
   :align: center
   :width: 600px
   :alt: Pipes and Context Switching

This extra copying and scheduling work is the cost of getting a simple
kernel-managed byte stream.

Named Pipes and FIFOs
---------------------

.. index::
   single: named pipes
   single: FIFOs
   single: mkfifo
   pair: IPC; named pipes
   single: PIPE_BUF

A named pipe, or FIFO, is a pipe that has a name in the filesystem.

Named pipes differ from ordinary pipes in three useful ways. They live in
the filesystem namespace, they can outlive the processes that use them,
and normal filesystem permissions can control access to them.

Regular and Named Pipe Usage
----------------------------

Ordinary shell pipelines use anonymous pipes:

::

   $ cat file | gzip -c9 > file.gz

Named pipes make the pipe visible as a filesystem entry:

::

   $ mkfifo pipe_file
   $ gzip -c9 < pipe_file > file.gz
   $ cat file > pipe_file
   $ rm -f pipe_file

This pattern can adapt programs that expect files to a streaming data
source.

Named Pipe Uses
---------------

Named pipes are commonly used for local client-server designs and for
adapting file-oriented programs to streaming input.

For example, a program that can only read from a local file can sometimes
read from a FIFO while another process fetches, decrypts, decompresses,
or generates the data. This works best for sequential I/O. It does not
work well when the program needs random access.

Named Pipe Atomicity
--------------------

.. index::
   pair: named pipes; atomicity
   single: PIPE_BUF

Writes to a pipe are atomic only up to a system-defined size.

If each ``write()`` call is no larger than ``PIPE_BUF``, the writes are
atomic and are not interleaved with writes from other processes. If a
write is larger than ``PIPE_BUF``, the kernel may split the write and
interleave it with other writers. A safe rule is to keep each logical
message at or below ``PIPE_BUF`` or provide a separate synchronization
mechanism.

Signals
-------

.. index::
   single: signals
   single: signal handler
   pair: IPC; signals
   single: signal disposition

A signal is a software interrupt delivered to a process.

Signals are used for asynchronous events such as child termination,
terminal interrupts, timers, broken pipes, and invalid memory accesses.
Each signal has a disposition: ignore it, catch it with a handler, or use
the default action.

Important Signals
-----------------

.. index::
   single: SIGABRT
   single: SIGALRM
   single: SIGBUS
   single: SIGCHLD
   single: SIGCONT
   single: SIGFPE
   single: SIGHUP
   single: SIGILL
   single: SIGINT
   single: SIGKILL
   single: SIGPIPE
   single: SIGSEGV
   single: SIGSTOP

Many UNIX systems define more than thirty signals. The following table
lists common ones used in systems programming.

.. csv-table:: List of Important Signals
   :header: "Signal", "Description"
   :widths: 15,65

   "SIGABRT", "abnormal termination, generated by abort(). Default action terminates and may dump core."
   "SIGALRM", "generated when a timer set by alarm() expires."
   "SIGBUS", "hardware fault or memory access fault."
   "SIGCHLD", "sent to a parent process when a child process terminates."
   "SIGCONT", "sent to a stopped process when it is continued."
   "SIGEMT", "general hardware fault on systems that define it."
   "SIGFPE", "arithmetic exception, such as divide by zero."
   "SIGHUP", "terminal disconnected or controlling session ended."
   "SIGILL", "illegal instruction."
   "SIGINT", "terminal interrupt, usually Ctrl-C."
   "SIGKILL", "cannot be caught or ignored; kills the process."
   "SIGPIPE", "write to a pipe whose reader has closed."
   "SIGSEGV", "segmentation violation."
   "SIGSTOP", "cannot be caught or ignored; stops the process."

Handling Signals
----------------

.. index::
   single: signal()
   single: pause()
   pair: signals; signal handler registration

A process can register a handler for a catchable signal.

.. literalinclude:: ipc/code/sighandler.c
   :language: c
   :linenos:

Key points:

- ``signal()`` registers the handler for ``SIGUSR1`` and ``SIGINT``.
- ``pause()`` blocks until a signal is caught.
- The handler receives the signal number and branches on it.
- A real handler should only call async-signal-safe functions.

Sending Signals
---------------

.. index::
   single: kill()
   single: raise()
   single: alarm()
   pair: signals; sending

Signals can be sent from the shell with ``kill``.

::

   $ firefox &
   [1] 5050
   $ kill -USR1 5050
   $ kill 5050

In C, ``kill(pid, signo)`` sends a signal to another process,
``raise(signo)`` sends a signal to the current process, ``alarm(seconds)``
sets a timer for ``SIGALRM``, and ``pause()`` waits for a signal.

Interrupted System Calls
------------------------

.. index::
   single: EINTR
   single: SA_RESTART
   single: sigaction()
   pair: signals; interrupted system calls

A signal can interrupt a blocking system call.

Older UNIX systems could interrupt almost any blocking system call.
Modern systems still treat reads and writes on pipes, terminals, and
network devices as slow calls because they can block indefinitely. If a
signal handler runs while such a call is blocked, the call may return
with ``errno`` set to ``EINTR``. Code that catches signals must be ready
to retry interrupted operations, unless ``sigaction()`` was configured
with ``SA_RESTART`` and the system call is restartable.

Reentrant Signal Handlers
-------------------------

.. index::
   single: async-signal-safe functions
   single: reentrant functions
   pair: signals; reentrancy

A signal handler interrupts whatever the process was doing at that
moment. The interrupted code might have been inside ``malloc()``,
``printf()``, or another function with internal state.

For that reason, signal handlers must only call reentrant, async-signal
safe functions. System calls and low-level wrappers such as ``read()``,
``write()``, ``close()``, ``fork()``, ``execve()``, ``kill()``, and
``waitpid()`` are generally safe. Library functions that allocate memory
or use mutable static data are not safe in a signal handler.

Sleep with Alarm
----------------

.. index::
   single: SIGALRM
   pair: signals; alarm()

Early UNIX implementations used ``SIGALRM`` to implement sleep-like
behavior.

.. literalinclude:: ipc/code/sleep+alarm.c
   :language: c
   :linenos:

Key points:

- The function installs a ``SIGALRM`` handler.
- ``alarm(numSeconds)`` schedules a signal in the future.
- ``pause()`` waits until a signal is delivered.
- ``alarm(0)`` cancels any remaining alarm and returns unslept time.
- The race between ``alarm()`` and ``pause()`` makes this style fragile.

Better Sleep with Longjmp
-------------------------

.. index::
   single: setjmp()
   single: longjmp()
   single: sigsetjmp()
   single: siglongjmp()
   pair: signals; longjmp

One attempted improvement is to use ``setjmp()`` and ``longjmp()`` so
the signal handler can escape the interrupted wait.

.. literalinclude:: ipc/code/bettersleep.c
   :language: c
   :linenos:

Key points:

- ``setjmp()`` records a return point before the process blocks.
- The signal handler calls ``longjmp()`` to return to that point.
- This avoids some forms of blocking after the alarm fires.
- Jumping out of a signal handler is still delicate.
- ``sigsetjmp()`` and ``siglongjmp()`` are safer choices when signal
  masks matter.

Signal Timing Problem
---------------------

.. index::
   pair: signals; timing hazards

Signal-based sleep code is difficult because one signal can interrupt
another handler or arrive between two operations that looked adjacent in
the source.

.. literalinclude:: ipc/code/sleepforproblem.c
   :language: c
   :linenos:

Key points:

- ``SIGINT`` runs a handler while the program is also using
  ``sleepFor()``.
- The handler performs a long computation to make timing effects easier
  to observe.
- The example shows why signal handlers should do as little work as
  possible.
- Complicated signal interactions are a common source of subtle bugs.

Using Alarm for Time Limits
---------------------------

``alarm()`` can also place an upper time limit on a blocking operation.

For example, a process reading from a named pipe can set an alarm before
the read. If no writer appears, the signal interrupts the read and gives
the program a chance to recover. This pattern is useful, but it requires
careful handling of ``EINTR`` and careful signal-handler design.

Memory Mapped Files
-------------------

.. index::
   single: memory mapped files
   single: mmap()
   single: munmap()
   single: MAP_SHARED
   pair: IPC; memory mapped files
   pair: virtual memory; mmap

A memory mapped file maps a file into a process's virtual address space.
After setup, the program can read and write the mapped region with normal
memory operations.

The file is the backing store for the mapped memory. If two processes map
the same file with shared mappings, they can communicate by reading and
writing the mapped bytes. Their virtual addresses may be different, so
shared data structures must not depend on process-local pointer values.

Virtual Addressing in Mapped Files
----------------------------------

.. index::
   pair: memory mapped files; position-independent layout

Memory mapped regions should use position-independent data layouts.

Pointers store virtual addresses, and the same mapped file can appear at
different virtual addresses in different processes. Data stored in a
mapped region should use fixed-size arrays, offsets, or indexes instead
of raw pointers to process-local memory.

Absolute Memory Addressing
--------------------------

This example stores pointers inside a structure.

.. literalinclude:: ipc/code/absolute.c
   :language: c
   :linenos:

Key points:

- ``Car`` stores pointers to ``Wheel`` objects.
- The wheel objects are allocated separately from the ``Car`` object.
- The pointer values are meaningful only in the process that created
  them.
- This layout is not suitable for sharing through a mapped file.

Relative Memory Addressing
--------------------------

This example stores the related objects inside one contiguous structure.

.. literalinclude:: ipc/code/relative.c
   :language: c
   :linenos:

Key points:

- ``Car`` contains the wheel array directly.
- ``Make`` and ``Model`` are fixed-size arrays inside the same object.
- The data can be interpreted relative to the start of the mapped region.
- This layout is much more suitable for memory mapped IPC.

Using mmap
----------

.. index::
   single: mmap(); usage pattern
   single: memcpy()

The basic ``mmap()`` pattern is to create a file, size it, map it, and
then treat the mapped region as memory.

The producer process creates and writes the mapped file.

.. literalinclude:: ipc/code/mmap.c
   :language: c
   :linenos:

Key points:

- The file is opened with ``O_CREAT``, ``O_TRUNC``, and ``O_RDWR``.
- ``lseek()`` and ``write()`` extend the file to the desired size.
- ``mmap()`` maps the file with read and write permissions.
- ``memcpy()`` writes data into the mapped region.
- ``munmap()`` and ``close()`` clean up the mapping and descriptor.

The consumer process maps the same file and copies the data out.

.. literalinclude:: ipc/code/mmap2.c
   :language: c
   :linenos:

Key points:

- ``stat()`` obtains the file size.
- The file is opened with read-write access.
- ``mmap()`` maps the existing file into memory.
- ``memcpy()`` copies the mapped bytes into process-local memory.
- The process unmaps and closes the file after reading it.

Memory Mapped Example
---------------------

The ``systems-code-examples/mmap`` example provides complete producer and
consumer programs for a shared mapped file.

.. literalinclude:: ../examples/systems-code-examples/mmap/producer.c
   :language: c
   :linenos:

Key points:

- The producer creates ``shared.dat`` and sizes it for the string.
- ``MAP_SHARED`` makes updates visible through the backing file.
- ``memcpy()`` writes ``Hello World`` into the mapped region.
- The program waits for input before unmapping so another process can
  inspect the file.

.. literalinclude:: ../examples/systems-code-examples/mmap/consumer.c
   :language: c
   :linenos:

Key points:

- The consumer opens the existing ``shared.dat`` file.
- ``stat()`` determines how much memory to map.
- The mapped bytes are copied into a local buffer.
- The consumer prints the string produced by the other process.

Memory Mapped Atomicity
-----------------------

.. index::
   pair: memory mapped files; atomicity

Memory mapped I/O does not make reads and writes atomic.

The system calls ``mmap()`` and ``munmap()`` set up and tear down the
mapping. Ordinary reads and writes to mapped memory do not enter the
kernel. That is why mapped memory is fast, but it also means the program
must provide its own synchronization when multiple processes write the
same region.

Shared Memory Locking
---------------------

.. index::
   single: shared memory locking
   pair: POSIX; process-shared semaphores

Locks used inside shared memory must also be safe to share between
processes.

POSIX semaphores can be placed in shared memory when initialized with
the process-shared argument enabled. Data structures in mapped memory
must also avoid raw process-local pointers, because each process may map
the file at a different address.

Memory Mapped Queue Example
---------------------------

The local bounded-buffer snippets show the structure of a queue stored in
mapped memory.

.. literalinclude:: ipc/code/boundedbuffer.c
   :language: cpp
   :linenos:

Key points:

- ``Message`` stores semaphores directly in the shared object.
- ``_lock`` protects the queue state.
- ``_empty`` counts available messages.
- ``_full`` counts available queue slots.
- The messages are stored as fixed-size arrays rather than pointers.

.. literalinclude:: ipc/code/boundedbuffer2.c
   :language: cpp
   :linenos:

Key points:

- The constructor initializes process-shared semaphores.
- ``CopyToMemoryMappedFile()`` sizes the file and maps it.
- A temporary ``Message`` object is copied into the mapped region.
- The returned pointer refers to the mapped shared object.

.. literalinclude:: ipc/code/boundedbuffer3.c
   :language: cpp
   :linenos:

Key points:

- ``GetFromMemoryMappedFile()`` maps an existing shared file.
- The mapped memory is interpreted as a ``Message`` object.
- ``ReleaseFile()`` unmaps the shared object.
- The file descriptor remains the caller's responsibility.

.. literalinclude:: ipc/code/boundedbuffer4.c
   :language: cpp
   :linenos:

Key points:

- ``EnqueueMessage()`` waits for a free slot and then locks the queue.
- ``DequeueMessage()`` waits for an available message and then locks the
  queue.
- The lock semaphore protects ``_current`` and the message array.
- The counting semaphores coordinate producer and consumer progress.

Memory Mapped Queue Programs
----------------------------

The local producer and consumer snippets show how separate programs use
the mapped queue.

.. literalinclude:: ipc/code/bb-producer.c
   :language: cpp
   :linenos:

Key points:

- The producer creates and truncates ``shared.dat``.
- ``CopyToMemoryMappedFile()`` initializes the queue in the mapped file.
- The producer enqueues one hundred messages.
- The process waits before releasing the mapping so a consumer can run.

.. literalinclude:: ipc/code/bb-consumer.c
   :language: cpp
   :linenos:

Key points:

- The consumer opens the existing mapped file.
- ``GetFromMemoryMappedFile()`` attaches to the shared queue.
- ``DequeueMessage()`` blocks until a message is available.
- The loop prints messages as they are removed.

Submodule Memory Mapped Lock Example
------------------------------------

The ``systems-code-examples/mmap_locks`` example is the complete version
of the mapped queue design.

.. literalinclude:: ../examples/systems-code-examples/mmap_locks/message.hh
   :language: cpp
   :linenos:

Key points:

- The shared object contains semaphores, queue state, and message
  storage.
- Fixed-size arrays keep the object self-contained inside the mapped
  file.
- Static helper functions create, attach to, and release the mapping.

.. literalinclude:: ../examples/systems-code-examples/mmap_locks/message.cc
   :language: cpp
   :linenos:

Key points:

- ``sem_init()`` uses process-shared semaphores.
- ``CopyToMemoryMappedFile()`` creates and initializes the shared object.
- ``GetFromMemoryMappedFile()`` maps an existing object.
- ``EnqueueMessage()`` and ``DequeueMessage()`` use semaphores to
  coordinate access across processes.
- The queue data itself lives in the mapped file.

.. literalinclude:: ../examples/systems-code-examples/mmap_locks/producer.cc
   :language: cpp
   :linenos:

Key points:

- The producer creates the shared backing file.
- It initializes the mapped ``Message`` queue.
- It enqueues a sequence of numbered messages.
- The program waits before releasing the file so the consumer can read.

.. literalinclude:: ../examples/systems-code-examples/mmap_locks/consumer.cc
   :language: cpp
   :linenos:

Key points:

- The consumer opens the existing shared file.
- It maps the ``Message`` queue created by the producer.
- It repeatedly dequeues and prints messages.
- Synchronization occurs through semaphores stored in the mapped region.

Memory Mapped I/O Performance
-----------------------------

.. index::
   pair: memory mapped files; performance
   pair: I/O; mmap vs write

Memory mapped I/O can be faster than ordinary file I/O because it avoids
an explicit copy between a user buffer and a kernel buffer.

With ``write()``, the process enters the kernel and the kernel copies
data from the user buffer into filesystem buffers. With ``mmap()``, the
process writes into mapped memory and the virtual memory system writes
dirty pages back to the file later. This works especially well when the
file format already matches the program's in-memory data model.

Files as IPC
------------

.. index::
   single: files as IPC
   pair: IPC; files
   single: lock files
   single: spool directories

Files are the oldest and most portable form of IPC.

File-based IPC appears in several patterns: persistent state, exposing
current state, spool directories, lock files, and simple queues. Files
are slower and less structured than specialized IPC mechanisms, but they
are durable and easy to inspect.

Exposing Current State
----------------------

.. index::
   single: /proc filesystem
   pair: Linux; /proc

The ``/proc`` filesystem exposes kernel and process state as files.

This gives tools and developers a file-oriented interface to information
that would otherwise require custom system calls.

.. csv-table:: List of /proc-style Filesystems
   :header: "Filesystem", "Description"
   :widths: 15,45

   "/proc/vmstat", "virtual memory statistics and configuration"
   "/proc/cpuinfo", "CPU information"
   "/proc/<PID>", "individual process information"
   "/proc/loadavg", "moving average of ready process load"

Spool Folders
-------------

.. index::
   single: spool directories
   pair: IPC; spool directories

A spool folder is a directory used as a persistent work queue.

Mail daemons, print systems, and scheduled job systems often use spool
folders. A daemon watches the directory, processes new files, and then
moves or deletes completed work. If the daemon crashes, the queued work
remains on disk.

Cron Spool Folders
------------------

.. index::
   single: cron
   pair: cron; spool directories

Cron uses spool-like directories for scheduled tasks.

Common examples include ``/etc/cron.daily``, ``/etc/cron.hourly``,
``/etc/cron.monthly``, and ``/etc/cron.weekly``. At the scheduled time,
cron runs executable files in the appropriate directory.

CUPS Spool Folders
------------------

.. index::
   single: CUPS
   pair: CUPS; spool directories

CUPS uses spool files to represent print jobs.

The spool directory is typically ``/var/spool/cups``. Data files contain
the bytes to print, while control files describe the job. The naming
convention gives the print system a simple domain model for queue
processing.

Lock Files
----------

.. index::
   single: lock files
   pair: IPC; lock files

A lock file uses atomic filesystem operations to represent ownership of
a resource.

For example, a server might create ``/var/lock/http_80`` before binding
to port 80. If another process tries to create the same lock file at the
same time, only one creation succeeds. The lock file should be removed
when the owning process exits normally.

Doors
-----

.. index::
   single: doors (Solaris)
   single: door_create()
   single: door_call()
   single: fattach()
   single: fdetach()
   pair: Solaris; doors

Doors are a Solaris IPC mechanism that exposes procedure calls through
filesystem entries.

They are not portable, but they are useful historically because they show
one way to build RPC-like behavior into a filesystem namespace.

Door Server
-----------

The server creates a door and attaches it to a filesystem path.

.. literalinclude:: ipc/code/doorserver.c
   :language: c
   :linenos:

Key points:

- ``door_create()`` registers the server function.
- ``creat()`` creates the filesystem entry.
- ``fdetach()`` removes any previous attachment at the path.
- ``fattach()`` attaches the door to the path.
- ``pause()`` keeps the server process alive.

Door Client
-----------

The client opens the door path and calls through it.

.. literalinclude:: ipc/code/doorclient.c
   :language: c
   :linenos:

Key points:

- The client opens the door path like a file.
- ``door_arg_t`` packages the argument data.
- ``door_call()`` invokes the server through the door.
- The filesystem path names the IPC endpoint.

Domain Sockets
--------------

.. index::
   single: UNIX domain sockets
   single: socket()
   single: bind()
   single: listen()
   single: accept()
   single: connect()
   single: PF_UNIX
   single: SOCK_STREAM
   pair: IPC; domain sockets

A UNIX domain socket is a local socket addressed through the filesystem
namespace.

Domain sockets are similar to named pipes, but they can be full-duplex,
support multiple clients, and support stream or datagram communication.
They use familiar socket calls such as ``socket()``, ``bind()``,
``listen()``, ``accept()``, and ``connect()``.

Domain Socket Example
---------------------

The local domain socket snippets show the server, handler, and client
pieces separately.

.. literalinclude:: ipc/code/domainsocketserver.c
   :language: c
   :linenos:

Key points:

- ``socket(PF_UNIX, SOCK_STREAM, 0)`` creates a local stream socket.
- ``unlink()`` removes any stale socket file.
- ``bind()`` attaches the socket to a filesystem path.
- ``listen()`` marks the socket as a server endpoint.
- ``accept()`` receives client connections.

.. literalinclude:: ipc/code/domainsocketserverhandler.c
   :language: c
   :linenos:

Key points:

- The handler reads a request from the connected socket.
- It prints the client message.
- It writes a response back to the client.
- It closes the connected socket when finished.

.. literalinclude:: ipc/code/domainsocketclient.c
   :language: c
   :linenos:

Key points:

- The client creates a UNIX stream socket.
- ``connect()`` attaches the client to the server's socket path.
- The client writes a message and then reads a response.
- The same connected socket supports both directions of communication.

Submodule Domain Socket Example
-------------------------------

The ``systems-code-examples/domain_sock`` directory contains a complete
domain socket client and server.

.. literalinclude:: ../examples/systems-code-examples/domain_sock/server.c
   :language: c
   :linenos:

Key points:

- The server binds a ``PF_UNIX`` socket to the path provided on the
  command line.
- Each accepted connection is handled in a child process.
- The child reads the client message and writes a response.
- The parent closes the connected socket and continues accepting clients.
- The socket file is unlinked when the server exits normally.

.. literalinclude:: ../examples/systems-code-examples/domain_sock/client.c
   :language: c
   :linenos:

Key points:

- The client receives the socket path on the command line.
- It connects to the server with ``connect()``.
- It sends a short request string.
- It reads and prints the server response.
- The client closes the socket before exiting.

TCP/IP
------

.. index::
   single: TCP/IP sockets
   pair: IPC; TCP/IP
   single: network sockets

TCP/IP sockets are the networked counterpart to local socket IPC.

They use many of the same programming ideas as domain sockets, but the
address is a network address and port rather than a local filesystem
path. TCP/IP sockets are usually the right choice when communication must
cross machine boundaries.
