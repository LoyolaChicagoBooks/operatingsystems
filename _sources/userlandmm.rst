Userland Memory Management
==========================

Why Userland Memory Management?
-------------------------------

-  Why discuss user land memory management in an OS course?

-  After all, user land memory management is almost always an
   implementation detail of a standard library or virtual machine.

-  The reason that this is important is that the OS allocates to user
   programs memory as units of contiguous virtual pages.

-  The unit of the page is not very useful for many programs. Most
   programs work in terms of structured objects that are composed of
   primitive data types and pointers to other structured objects or
   primitive data types.

Why Userland Memory Management?
-------------------------------

-  Because programs need a view of the machine as something other than
   pages, we can consider the topic of user land memory management as a
   detail of the virtual machine that the system provides.

-  Because the details of the memory management system differ based on
   language needs, it is impractical for this component to exist within
   the operating system implementation.

Heap Management
---------------

The Heap
--------

-  The heap is the portion of a process’ memory that is used for data
   that lives beyond the lifetime of any one function or method. The
   lifetime of this data is defined by when it is allocated and when the
   heap manager either explicitly deletes it or garbage collects it.

-  Languages like C or C++ allocate chunks of memory manually through
   explicit statements like malloc, free, new, delete, etc...

-  Languages like C# or Java allocate chunks of memory through
   statements like new and deallocate memory with garbage collectors

The Memory Manager
------------------

-  The memory manager has the following responsibilities:

   -  Manages list of allocated sections of memory

   -  Manages list of free regions in memory

   -  Allocation - When a program manages memory for an object, the
      memory manager will find a chunk of contiguous memory at least as
      large as the object. If not enough contiguous memory is available,
      it asks the operating system for more heap space

   -  Deallocation - When a program deallocates heap memory, the memory
      manager returns the deallocated space to the free list. Memory is
      typically not returned to the operating system in most
      implementations.

Characteristics of Memory Allocation
------------------------------------

-  Requests from a program for chunks of the heap are typically not of
   the same size

-  Deallocation is not predictable. In the stack, memory is allocated
   and freed in First In Last Out order. This is not true of the heap.

-  Therefore, the memory manager must be prepared to handle requests of
   differing size in unpredictable order

-  Most of the time, the majority of requests are for small regions of
   memory that are measured in bytes or kilo-bytes.

Characteristics of Memory Allocation
------------------------------------

-  Although not optimal, it is acceptable for an allocator to return a
   region of memory larger than the requested size.

-  Many allocators will split the heap into buckets of a different chunk
   sizes.

-  So, for example an allocator may service a request for 31 bytes of
   memory with a region of memory that is 32 bytes in size.s

Characteristics of a Good Memory Manager
----------------------------------------

-  Space Efficiency - The memory manager should minimize the total heap
   needed by a program. This is accomplished by minimizing heap
   fragmentation.

-  Program Efficiency - Allocations should be arranged in memory to
   preserve locality

-  Low Overhead - Memory allocations / deallocations are frequent
   operations in a program. Because of this, the overhead of the memory
   manager should be minimized. Efficiency for smaller memory
   allocations should be a priority over efficiency for larger
   allocations since smaller allocations happen more frequently.

A Refresher on the Memory Hierarchy
-----------------------------------

-  Memory Type / Memory Size / Access Time

-  Registers / 32 words / 1 ns

-  L1 Cache / 16 - 64KB / 5-10 ns

-  L2 Cache / 128KB - 4MB / 40-60 ns

-  Physical Memory / 512MB - 8GB / 100 - 150 ns

-  Swap File / 512MB - 8GB / 3 - 15 ms

Program Locality
----------------

-  Often, programs contain many instructions that are never executed.

-  Programs built with reusable libraries, often only use a small
   fraction of the capability of these libraries.

-  Programs typically spend most of their time executing innermost loops
   and recursive functions.

-  Only a small fraction of code that could be executed is actually
   executed. This means, that within any given method, there are basic
   blocks of code dedicated to exception and error handling that is
   often not invoked.

Fragmentation
-------------

-  Internal fragmentation - When a request for memory is answered with a
   region of memory that is larger than the request, the remainder is
   wasted.

-  External fragmentation - Regions of contiguous memory that are not
   large enough to service requests.

-  External fragmentation = 1 - (largest free region / total free
   memory)

-  Fragmentation comes about because while decisions can be made about
   where heap allocations are placed, the order of their eventual
   deallocations cannot be controlled or predicted.

Reducing Fragmentation of the Heap
----------------------------------

-  The heap starts as one contiguous free space

-  As the program allocates and deallocates memory, the free space will
   no longer be contiguous

-  After a while, there may be regions of free space that are not large
   enough to service a memory allocation request.

-  If the free space becomes sufficiently fragmented, then it is
   possible that even though there is enough total free memory, a
   request for more memory could fail because there isn’t a contiguous
   free region that is large enough.

Best-Fit vs Next-Fit
--------------------

-  The best-fit allocation policy is to fit each request into the free
   memory region that is the smallest that will satisfy the request

-  The first-fit allocation policy is to fit each request into the first
   free memory region encountered in a search that will satisfy the
   request

-  Overall, the best-fit policy provides the best performance in most
   cases.

-  The downside to the best-fit policy is that it results in a more
   complex allocator and there is some cost to the search for the best
   fitting free region (although there are ways around this).

Best-Fit
--------

-  To avoid the costs associated with searching for the best-fit free
   space region for an allocation, best-fit allocators commonly will
   split up the heap into different buckets.

-  Each of these buckets will be responsible for serving different sized
   requests.

-  In the GNU Standard C Library, there are buckets of size 8-byte,
   16-byte, 24-byte, .... 512-byte. For larger allocations, there are
   additional buckets for sizes spread out logarithmically (512, 576,
   640, ....)

-  To service an allocation request, the allocator will use the free
   list in the bucket that is the smallest that is also the same size or
   larger than the request. If that bucket is empty, then the next
   largest one is used. If all buckets are full, then the allocator asks
   the operating system for more heap space.

Managing Free Space
-------------------

-  There are three approaches to managing free space:

   -  If buckets are being used, then a bitmap of which items in each
      bucket are free can be maintained. This is pretty straightforward.

   -  Boundary Tags - At the high and low end of each chunk, we keep a
      bit telling us if the chunk is free or allocated and how big the
      chunk is

   -  Doubly Linked, Embedded Free List - The free chunks contain
      pointers to a previous free chunk and a next free chunk. Often
      these lists are sorted to help best-fit algorithms.

Bugs and Explicit Allocators
----------------------------

-  For languages like C, when programs write beyond the boundaries of
   allocated sections of the memory explicitly allocated, it is possible
   that these overwrites will write into structures used by the
   allocator.

-  Because of this possibility, different allocators are more or less
   susceptible to different types of errors. In general, Boundary Tags,
   and Doubly Linked Embedded Free List are fairly vulnerable to these
   types of bugs. Bucket allocators with bitmaps are less susceptible.

-  This reality can be of use during debugging. There are some
   debug-only allocators that return extra large regions of memory for
   each allocation and pad the front and back of each region. At
   deallocation time, if the padded values are altered, then the
   allocator can print an error to the console.

Debugging malloc / free
-----------------------

-  An excellent way to debug common malloc / free issues is to use the
   dmalloc library.

-  This library is available at http://dmalloc.com. It can be used in
   existing applications without modification.

-  This library can help you discover:

   -  Where un-freed objects were originally allocated.

   -  Where any buffer overruns are if they are encountered.

Expanding the Heap
------------------

-  There are two ways that UNIX allocators expand the heap:

   -  sbrk - Asks the OS to increase the data segment by N bytes.

   -  mmap/mfree - Requests from the OS to allocate a separate
      non-contiguous region of memory. Since these regions are
      non-contiguous, it is simpler to return them to the OS. mmap/mfree
      have a fairly large overhead. Often, mmap/mfree are used to
      service requests for larger allocations that don’t fit into a
      bucket (> 1MB).

Recommended Reading
-------------------

-  Article by Doug Lea, the author of the GNU libc malloc/realloc/free -
   http://g.oswego.edu/dl/html/malloc.html

Problems with manual deallocation
---------------------------------

-  Manual memory deallocation can lead to memory leaks. Memory leaks are
   defined as memory that has been allocated, but not deallocated and
   will not be referenced at any point in the future.

-  Memory leaks do not cause a program to be incorrect or to crash as
   long as enough free memory is available. For short lived programs
   this is not a problem. For longer lived programs this is a problem.

Problems with manual deallocation
---------------------------------

-  Manual memory deallocation can lead to problems if after the
   deallocation, a pointer refers to the deallocated space and the
   program dereferences that pointer in the future. This can lead to
   modification of memory that was allocated to different pointers or to
   memory that has been released to the operating system. This can lead
   to crashes or incorrect program behavior.

Avoiding problems with manual deallocation
------------------------------------------

-  When designing an application, make sure each allocated object has a
   defined owner. This can be an instance of a class, or a function.
   This is applicable when the owner can be determined statically.

-  When the owner cannot be determined statically, using reference
   counters is advisable. Whenever a reference to an object is created,
   we increment the reference counter. Whenever a reference goes away,
   we decrease the reference counter. When the counter reaches zero, we
   deallocate the object. This runs into trouble with cyclic structures.

A simple Malloc / Free Implementation (Example from IBM Developer Works)
------------------------------------------------------------------------

{language=C, basicstyle=, indent=xleftmargin}

::

    #include <unistd.h>
    int has_initialized = 0;
    void *managed_memory_start;
    void *last_valid_address;

    struct mem_control_block {
      int is_available;
      int size;
    };

    void malloc_init()
    {
      last_valid_address = sbrk(0);
      managed_memory_start = last_valid_address;
       has_initialized = 1;
    }

A simple Malloc / Free Implementation
-------------------------------------

{language=C, basicstyle=, indent=xleftmargin}

::

    void myfree(void *firstbyte) {
      struct mem_control_block *mcb;
      mcb = (struct mem_control_block*)
               (firstbyte - sizeof(struct mem_control_block));
      mcb->is_available = 1;
      return;
    }

A simple Malloc / Free Implementation
-------------------------------------

{language=C, basicstyle=, indent=xleftmargin}

::

    void *mymalloc(long numbytes) {
      void *current_location;
      struct mem_control_block *current_location_mcb;
      void *memory_location;
      if(! has_initialized)   {
        malloc_init();
      }
      numbytes = numbytes + sizeof(struct mem_control_block);
      memory_location = 0;
      current_location = managed_memory_start;

A simple Malloc / Free Implementation
-------------------------------------

{language=C, basicstyle=, indent=xleftmargin}

::

      while(current_location != last_valid_address)
      {
        current_location_mcb = 
               (struct mem_control_block *)current_location;
        if(current_location_mcb->is_available)
        {
          if(current_location_mcb->size >= numbytes)
          {
            current_location_mcb->is_available = 0;
            memory_location = current_location;
            break;
          }
        }
        current_location = current_location + 
                          current_location_mcb->size;
      }

A simple Malloc / Free Implementation
-------------------------------------

{language=C, basicstyle=, indent=xleftmargin}

::

      if(!memory_location)
      {
        sbrk(numbytes);
        memory_location = last_valid_address;
        last_valid_address = last_valid_address + numbytes;
        current_location_mcb = 
                   (struct mem_control_block*)memory_location;
        current_location_mcb->is_available = 0;
        current_location_mcb->size = numbytes;
      }
      memory_location = memory_location + 
                  sizeof(struct mem_control_block);
      return memory_location;
    }

Garbage Collection
------------------

Garbage Collection
------------------

-  Data that can’t be referenced is considered garbage

-  One problem that explicit allocators have is that deallocation is the
   source of bugs

-  Some runtimes and programming languages provide automatic garbage
   collectors.

-  The first garbage collector implementation was for the LISP
   programming language in 1958.

Requirements for Garbage Collectors
-----------------------------------

-  Most garbage collectors require that a system be type safe to support
   a collector.

-  Most garbage collectors require that references / pointers refer to
   the beginning of an object and not somewhere in the middle

-  In general, these guarantees cannot be made for C programs.

-  Garbage collectors exist for C, but programmers have to limit their
   use of the full capability of C

Goals for a Garbage Collector
-----------------------------

-  Overall execution time for collection - Garbage collectors must
   access a lot of data. The faster they are the better

-  Space Usage - Garbage collectors must keep fragmentation to a minimum

-  Wait time - Many garbage collectors require that a program be
   temporarily halted while running. So, it is desirable that the
   maximum wait time be small.

-  Locality - Like explicit allocators, garbage collectors can help
   preserve locality by allocating memory from regions that are recently
   deallocated.

Disadvantages to Garbage Collection
-----------------------------------

-  Garbage collectors consume processing time determining which objects
   are garbage

-  Garbage collectors run interval is not predictable. This can cause
   trouble for:

   -  The garbage collector may run too late, allowing too much garbage
      to pile up

   -  Garbage collectors need to halt execution to one degree or
      another. This can degrade performance.

Disadvantages to Garbage Collection
-----------------------------------

-  Garbage collectors can’t detect when low physical memory conditions
   occur. With manual deallocation, the memory usage should always be
   the minimum needed, but with garbage collectors, this can be
   variable. So, it is possible that a garbage collector can encourage
   overuse of physical memory or unnecessary paging.

-  Garbage collectors cannot remove all possible memory leaks. Garbage
   collectors can only collect garbage.

Reachability
------------

-  Every program has a concept of a root set of references. The root set
   is the list of references held on the stack of every executing
   thread.

-  Some programming language and compiler support is required to make
   sure that this list is available to the garbage collector.

-  Operations that change reachability:

   -  Allocations - allocating new objects create new reachable objects

   -  Method "out" parameters and return values - Values returned from
      allocations within a method remain reachable by the caller

   -  Reference copying - Copying a reference from one reference
      variable to another.

   -  Method returns - When a method returns, all of the references on
      the stack that are not a part of the return are lost.

Reference Counting Garbage Collectors
-------------------------------------

-  Reference counting garbage collectors work by detecting when an
   object’s reference count reaches zero and at that time deallocate the
   object. References are increased and decreased in the following
   manner:

   -  Allocation - New objects start with a reference count of 1

   -  Parameter passing - when a parameter is passed to a method, it’s
      reference count is incremented

   -  Reference assignments - If a copy of the reference is made, the
      reference count goes up by 1

   -  Method returns - when a method returns, all references on that
      method’s stack have their counts decremented by 1

   -  Transitive - if a reference count of an object reaches 0, then it
      also decrements the reference of every object it refers to.

Pros-Cons of Reference Counting Garbage Collectors
--------------------------------------------------

-  Pros

   -  The implementation is very simple.

-  Cons

   -  There is a lot of book keeping overhead. Every time a method is
      called and returned from, each parameter and stack variable has to
      be touched an additional time.

   -  Reference counting cannot collect unreachable, cyclic data
      structures.

Mark-and-Sweep Garbage Collection
---------------------------------

-  Mark-and-Sweep collectors work by stopping the execution of a program
   and use an algorithm to determine which objects are not reachable and
   return those unreachable objects to the free list.

-  The algorithm works by maintaining four sets

   -  The free list - a list of free memory regions

   -  The root set - a list of all references on the stacks of all
      executing threads and global variables.

   -  The referenced list - a list of objects that are referenced.

   -  The unscanned list - a list of allocated objects that have not yet
      been found to be referenced.

-  The algorithm then does a depth first search form all items in the
   root set. If an item is reached in the scan, it is removed from the
   unscanned list to the referenced list.

-  After the scan is complete, items remaining in the unscanned list are
   moved to the free list as garbage.

Mark-and-Compact Garbage Collection
-----------------------------------

-  Compacting garbage collectors take the additional step of moving
   objects around in memory after collection to make free space
   contiguous and to make allocated objects closer to each other in
   memory.

-  The most popular type of these collectors are copying collectors.

-  Copying collectors modify the Mark-and-Sweep collector algorithm by
   performing a copy operation to a contiguous free memory region that
   is set aside at collection time instead of adding the objects to the
   referenced list. Then, after collection is complete, the previous
   memory region becomes the free memory region.

-  For more details, see "Cheney’s Algorithm" in a paper by C.J. Cheney
   in the ACM library.

Garbage Collection Costs
------------------------

-  Mark-and-Sweep collectors costs are related to the number of
   reachable objects

-  Compacting (Copying) collectors costs are related to the total size
   of reachable objects.

Incremental Garbage Collection
------------------------------

-  The previously mentioned garbage collectors stop the execution of all
   threads in a program to run. This can cause programs to pause for
   long periods of time.

-  To keep programs responsive, incremental garbage collection is
   desirable.

-  Instead of collecting all garbage, incremental garbage collectors
   just collect objects that can be found to be unreachable without
   performing a mark and sweep algorithm.

-  Then, in subsequent collections, the collector will be able to
   collect objects that became unreachable by the previous collection in
   addition to any newly unreachable objects.

Incremental Garbage Collection
------------------------------

-  Why in general do incremental garbage collectors work well?

-  For most programs, objects become garbage rapidly. This means that
   most allocations in a program become unreferenced fairly soon after
   being allocated.

-  Often, the number of objects that become garbage rapidly is in the
   range of 80 - 95 percent. These objects are typically easily
   collected and at a high rate by an incremental garbage collector

-  The downside is that, very often an object that survives a first
   round of incremental collection, will often survive more than one
   round. In copying incremental collectors, this means that they will
   also be copied multiple times.

Generational Garbage Collection
-------------------------------

-  Generational garbage collectors are a specific way of implementing a
   copying incremental garbage collector.

-  Generational collectors split the heap into N sections. Section 0 is
   the oldest, 1 is the next oldest, ....

-  When section 0 becomes full, then the collector is invoked and copies
   reachable objects into section 1, making the entire section 0 free.

-  When any 1...N section becomes full, the same algorithm takes place.

-  Occasionally, garbage collection is run for some value of i between 1
   and N to free objects in sections 1...N.

-  This type of collection is very fast because it favors scanning young
   objects which are objects most likely to be garbage.

Generational Garbage Collection
-------------------------------

-  Generational garbage collectors are used in many platforms -

-  Java

-  .NET

-  Python

Buyer Beware
------------

-  Even with advanced and efficient garbage collection algorithms, it is
   still possible in theory to "leak" memory.

-  Programs that have poorly defined object lifetimes or hang on to
   references for too long can accumulate objects that will never be
   referenced in the future.

-  Programs like these will slowly run out of memory without creating
   any garbage for the collector to reclaim.

-  It is also important to remember that with incremental / generational
   collectors, not all garbage is collected every time. Also some of
   these collectors will never reclaim large objects (greater than 8KB)
   due to the cost. It is possible in these systems to run out of memory
   artificially.


