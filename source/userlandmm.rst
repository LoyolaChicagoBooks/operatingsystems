Userland Memory Management
==========================

Userland memory management is the layer that turns pages from the
operating system into objects, buffers, and runtime data structures used
by programs.

Why Userland Memory Management?
-------------------------------

Operating systems allocate memory to processes in units of virtual
pages. Most programs do not work directly in pages.

Programs usually allocate objects, arrays, strings, and linked data
structures. A C program may request 24 bytes for a structure or 1024
bytes for a buffer. A Java or C# program may allocate an object with
``new``. The operating system provides the virtual address space, but the
language runtime or standard library manages the smaller allocations
inside that space.

Userland Memory as Runtime Policy
---------------------------------

Userland memory management belongs in libraries and language runtimes
because allocation policy depends on the language.

C and C++ need explicit allocation and deallocation. Java, C#, Python,
and many other languages need allocation plus garbage collection. These
policies are too language-specific to live entirely inside the kernel,
but they still depend on operating-system services such as pages,
``sbrk()``, and ``mmap()``.

Heap Management
---------------

The heap is the part of a process address space used for data whose
lifetime is not tied to one stack frame.

Stack memory is allocated and freed as functions call and return. Heap
memory is allocated and freed according to program logic. In C and C++,
the program manages this lifetime explicitly with calls such as
``malloc()``, ``free()``, ``new``, and ``delete``. In garbage-collected
languages, the runtime decides when unreachable heap objects can be
reclaimed.

The Memory Manager
------------------

A memory manager tracks allocated regions and free regions inside the
heap.

When a program requests memory, the manager finds a contiguous free
region large enough for the request. If no suitable region exists, the
manager asks the operating system for more heap space. When memory is
freed, the manager returns the region to its free-space data structures.
Most allocators do not immediately return every freed region to the
operating system.

Characteristics of Allocation
-----------------------------

Heap allocation requests vary in size and are freed in unpredictable
orders.

Unlike stack allocation, heap allocation is not first-in, last-out. A
program may allocate many small objects, free some of them, allocate a
large buffer, and keep other objects alive for the rest of the process.
The allocator must handle this pattern efficiently.

Over-Allocation and Buckets
---------------------------

Allocators often return a block that is slightly larger than the request.

For example, a request for 31 bytes may be served from a 32-byte bucket.
This wastes a small amount of memory inside the block, but it simplifies
allocation and can make free-list management faster.

Characteristics of a Good Memory Manager
----------------------------------------

A good memory manager balances space efficiency, speed, and locality.

Space efficiency means minimizing fragmentation and total heap size.
Program efficiency means placing related allocations close enough to
benefit from cache locality. Low overhead matters because allocations
and deallocations are frequent operations, especially for small objects.

Memory Hierarchy Refresher
--------------------------

Memory performance depends on where data is located in the hierarchy.

Registers are fastest and smallest. L1 cache is larger but slower. L2
and later caches are slower still. Physical memory is much slower than
cache, and swap is slower by orders of magnitude. A memory manager cannot
control all of these layers, but allocation placement can affect cache
locality and page behavior.

Program Locality
----------------

Programs usually use a small part of their code and data heavily.

Most execution time is spent in inner loops, recursive calls, and common
paths. Libraries may contain many functions that a particular program
never calls. Good memory management tries to preserve this locality by
keeping related objects near each other and avoiding unnecessary heap
growth.

Fragmentation
-------------

Fragmentation is wasted memory caused by allocation size and placement.

Internal fragmentation occurs when an allocator returns a block larger
than the request. External fragmentation occurs when the heap has enough
total free memory, but the free memory is split into regions too small to
satisfy a request.

One way to express external fragmentation is:

::

   1 - (largest free region / total free memory)

Reducing Heap Fragmentation
---------------------------

The heap starts as one contiguous free region, but allocation and
deallocation split it over time.

After enough activity, the heap may contain many small free regions that
cannot serve larger requests. An allocator reduces fragmentation by
choosing where to place allocations, coalescing adjacent free regions,
and using size classes or buckets for common request sizes.

First Fit and Best Fit
----------------------

First fit and best fit are two basic policies for choosing a free block.

First fit uses the first free region large enough for the request. Best
fit uses the smallest free region that can satisfy the request. Best fit
can reduce wasted space, but it requires more searching unless the
allocator uses data structures that organize free blocks by size.

Best-Fit Buckets
----------------

Bucket allocators approximate best fit with size classes.

The allocator keeps separate free lists for common sizes. A request is
rounded up to the nearest size class, and the allocator uses that
bucket's free list. If the bucket is empty, the allocator tries a larger
bucket or asks the operating system for more heap space.

Managing Free Space
-------------------

Allocators need data structures that record which heap regions are free.

Common approaches include bitmaps for fixed-size buckets, boundary tags
at the start and end of chunks, and embedded doubly linked free lists
inside free chunks. Boundary tags and embedded lists are flexible, but
they are vulnerable to memory overwrites because allocator metadata sits
near program data.

Bugs and Explicit Allocators
----------------------------

Explicit allocators are vulnerable to bugs in the program using them.

If a C program writes past the end of an allocated block, it may corrupt
allocator metadata. If it frees the same pointer twice or uses a pointer
after freeing it, the allocator can later return overlapping or invalid
memory. Debug allocators often add padding and checks around allocations
to detect these errors.

Debugging malloc and free
-------------------------

Debugging allocators help find memory leaks, buffer overruns, double
frees, and use-after-free errors.

Tools such as ``dmalloc`` can be linked into existing C programs to
record allocation sites and check heap consistency. Modern toolchains
also provide sanitizers and leak detectors that serve the same purpose.

Expanding the Heap
------------------

UNIX allocators usually expand the heap with ``sbrk()`` or ``mmap()``.

``sbrk()`` moves the program break and grows the traditional contiguous
heap. ``mmap()`` can allocate separate non-contiguous regions, which are
often easier to return to the operating system. Many allocators use
``mmap()`` for large allocations because large regions do not fit well in
small-object buckets.

Simple malloc Case Study
------------------------

The ``systems-code-examples/malloc`` example implements a small allocator
using ``sbrk()``.

.. literalinclude:: ../examples/systems-code-examples/malloc/malloc.h
   :language: c
   :linenos:

Key points:

- The public interface mirrors the shape of ``malloc()`` and ``free()``.
- ``mymalloc()`` returns a pointer to usable memory.
- ``myfree()`` marks a previously allocated block as available.

.. literalinclude:: ../examples/systems-code-examples/malloc/malloc.c
   :language: c
   :linenos:

Key points:

- ``malloc_init()`` records the current program break as the start of the
  managed heap.
- Each allocation stores a small control block before the returned
  memory.
- ``mymalloc()`` scans existing blocks for a free block large enough for
  the request.
- If no block is available, ``sbrk()`` grows the heap.
- ``myfree()`` marks the block available but does not coalesce adjacent
  free blocks or return memory to the operating system.

.. literalinclude:: ../examples/systems-code-examples/malloc/demo.c
   :language: c
   :linenos:

Key points:

- The demo allocates enough memory for ``hello world`` and the null byte.
- The buffer is initialized before use.
- ``strncpy()`` copies the message into the allocated region.
- ``myfree()`` returns the block to the allocator's free list.

Manual Deallocation Problems
----------------------------

Manual memory management can create leaks and dangling pointers.

A memory leak occurs when allocated memory will never be freed and can no
longer be reached by the program. A dangling pointer occurs when a
pointer still refers to memory after that memory has been freed. Leaks
usually waste memory over time. Dangling pointers can corrupt data,
crash the program, or create security vulnerabilities.

Avoiding Manual Deallocation Problems
-------------------------------------

Explicit memory management works best when ownership is clear.

Each allocated object should have a defined owner responsible for freeing
it. When ownership cannot be determined statically, reference counting
can help. A reference count is incremented when a reference is created
and decremented when a reference is removed. When the count reaches zero,
the object can be freed. Reference counting does not handle cycles by
itself.

Garbage Collection
------------------

Garbage collection automatically reclaims heap objects that can no longer
be reached by the program.

The first garbage collector was implemented for Lisp in 1958. Since then,
garbage collection has become a common feature of language runtimes. It
reduces many manual deallocation errors, but it introduces runtime costs
and does not eliminate every kind of memory leak.

Requirements for Garbage Collectors
-----------------------------------

Garbage collectors need reliable information about references.

Most collectors require type safety and a way to find the root set of
references. They also need references to point to recognizable objects,
not arbitrary locations inside objects. These guarantees are difficult in
C because pointer arithmetic and casts can hide references from the
collector.

Goals for a Garbage Collector
-----------------------------

A garbage collector should reclaim memory without causing unacceptable
pause times or memory overhead.

Important goals include low total collection time, low space overhead,
short maximum pauses, and good locality after collection. Different
collectors make different tradeoffs among these goals.

Disadvantages of Garbage Collection
-----------------------------------

Garbage collection consumes CPU time and can run at inconvenient moments.

Many collectors pause program execution at least briefly. If collection
runs too late, garbage can accumulate and increase memory pressure. A
collector also may not know about system-wide low-memory pressure early
enough to prevent paging.

Reachability
------------

Reachability determines whether an object is still live.

The root set includes references from thread stacks, registers, and
global variables. Objects reachable from the root set are live. Objects
not reachable from the root set are garbage. Allocations, parameter
passing, assignments, and function returns all change the reachability
graph.

Reference Counting
------------------

Reference counting frees an object when its count reaches zero.

New objects start with a reference count. Passing, copying, or storing a
reference increments the count. Removing or overwriting a reference
decrements the count. When the count reaches zero, the object is freed
and references held by that object are decremented.

Reference Counting Tradeoffs
----------------------------

Reference counting is simple but expensive in busy programs.

Every reference update requires bookkeeping. Function calls and returns
can touch many references. Reference counting also cannot collect cycles:
two unreachable objects can keep each other's counts above zero.

Mark-and-Sweep Garbage Collection
---------------------------------

Mark-and-sweep collection finds reachable objects and frees the rest.

The collector starts from the root set and traverses references to mark
live objects. After marking finishes, allocated objects that were not
marked are moved to the free list. Mark-and-sweep can collect cycles, but
it can pause the program and leave the heap fragmented.

Mark-and-Compact Garbage Collection
-----------------------------------

Mark-and-compact collectors move live objects so free memory becomes
contiguous.

Copying collectors are a common form of compacting collection. Live
objects are copied into a new contiguous region. After the copy, the old
region becomes free. This improves allocation speed and locality, but it
requires updating references to moved objects.

Garbage Collection Costs
------------------------

Collector cost depends on what the collector must scan or move.

Mark-and-sweep cost is related to the number of reachable objects.
Compacting and copying collector cost is related to the total size of
reachable objects. Large live heaps are therefore expensive even when
little garbage is present.

Incremental Garbage Collection
------------------------------

Incremental collectors divide collection work into smaller pieces.

Instead of stopping all program threads for a full collection, an
incremental collector does part of the work at a time. This can reduce
pause times and improve responsiveness, but it makes the collector more
complex.

Why Incremental Collection Works
--------------------------------

Many objects die young.

For many programs, most allocations become unreachable soon after they
are created. Incremental collectors benefit from this pattern because
they can reclaim young garbage quickly. The downside is that objects that
survive early collections may be copied or scanned more than once.

Generational Garbage Collection
-------------------------------

Generational garbage collection divides the heap by object age.

Young objects are collected frequently because most objects die young.
Objects that survive are promoted to older generations, which are scanned
less often. Java, .NET, Python, and many other runtimes use generational
ideas in their collectors.

Buyer Beware
------------

Garbage collection does not make memory leaks impossible.

A program can still hold references longer than necessary. If an object
remains reachable, the collector must keep it, even if the program will
never use it again. Large reachable structures and long-lived caches can
therefore still exhaust memory.

Recommended Reading
-------------------

Doug Lea's allocator article is a useful follow-up for explicit memory
management and the design of ``malloc()`` implementations:
http://gee.cs.oswego.edu/dl/html/malloc.html
