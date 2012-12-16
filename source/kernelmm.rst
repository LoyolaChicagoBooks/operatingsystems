Virtual Memory
==============

Virtual Memory lecture...

What is Virtual Memory?
-----------------------

-  Virtual memory is responsible for many capabilities in an operating
   system. Among them are:

   -  Allowing one or more programs that require more bytes of memory
      than are available to continue to run.

   -  To improve performance of I/O operations by supporting buffering
      operations.

   -  To reduce overall memory usage by allowing processes to share
      pages of memory

   -  To manage process memory protection and sandboxing (To provide a
      virtual sub-machine to a process).

   -  To translate virtual memory addresses to physical memory addresses
      and to maintain the domain of virtual memory addresses.

Memory Management Units
-----------------------

-  A Memory Management Unit (MMU hereafter) is a hardware component that
   is responsible for:

   -  Translation of virtual addresses to physical addresses

   -  Memory protection

   -  Translation Lookaside Buffer

   -  Page table entries

-  In some modern CPUs, the MMU is a part of the CPU.

-  When the processor has a cache miss and needs to access a page of
   memory, it makes the request to the MMU

Pages and Page Tables
---------------------

-  Every process gets a view of the machine that makes it appear that
   the process has all of the machine’s address space available to it.

-  In reality, the process is only using a part of it. The parts (pages)
   being used have to be maintained in a list by the operating system.

Pages and Page Tables
---------------------

-  So, how is memory allocated in the operating system?

   -  If we allocated 128 byte pages and had to maintain 4GB worth of
      128 byte pages, we would need to maintain a list of:

   -  128 bytes - 7 bits to address, 25 bits for page offset, 4GB / 128B
      = 33,554,432 entries \* 25 bits = 100MB per process in page table
      entries.

   -  4k bytes - 12 bits to address, 20 bits for page offset, 4GB / 4K =
      1,048,576 entries \* 20 bits = 2.5MB per process

   -  In 32-bit x86, the entries are 32 bits because other pieces of
      information are needed other than the address. 1,048,576 \* 32bits
      = 4MB per process.

Pages and Page Tables
---------------------

-  Even at 4k pages, 2.5MB per process is pretty extreme. Imagine
   launching a process that needs 100k of memory but has 2.5MB of
   overhead! Imagine if you had to map more than 4GB of memory!

-  What are some ways we can solve this?

What’s in a Page Table Entry?
-----------------------------

-  If we use 4k pages, then we need to have 20 bits to address those
   pages in a 4GB address space (or more if we use a 64-bit address
   space)

-  We also need to know if the page has the following attributes:

   -  is executable

   -  is writable

   -  is modified

   -  is present

-  Additionally, we might want to store information about the process
   ID, statistics, etc...

-  In a 32-bit x86, we have 32-bits to work with. 20-bits go to the page
   address and 12 go to anything else we need.

Page Table Entries
------------------

-  So, if we need 32-bit page table entries on the 32-bit x86, how can
   we avoid having so many and having so much overhead?

-  The key part is how the first 20-bits of the 32-bit entry is managed.
   How can we use those 20-bits?

-  The solution in x86 is the use of a 2 or 3 level page table.

Page Table Entries
------------------

-  In a 2 level page table, the first 10-bits is used for the first
   level and the second 20-bits is used for the second level.

-  This doesn’t reduce the size of the entry, but reduces the number of
   entries we need to store per process.

-  Basically, this works by having one level of the page table manage a
   larger range. If we’re using 10-bits, then the first level page table
   is mapping 4MB pages, then the 2nd level divides it into 4k pages. We
   only create entries in the 2nd level if any exist.

Two Level Page Table (thanks Wikipedia)
---------------------------------------

    .. figure:: kernelmm/diagrams/two_level_page_table.*
       :align: center
       :alt: image

       image

MMU - Address Translation Algorithm
-----------------------------------

{language=C, basicstyle=, indent=xleftmargin}

::

       physical_address translate(virtual_address v_addr) {
        physical_address addr;
        if(tlb.contains(v_addr)) {
            addr = tlb[v_addr];
        } else {
            first10 = (v_addr >> 0x16) & 0xa;
            second10 = (v_addr >> 0xc) & 0xa;
            level_1 = page_table_1[first10];
            entry = level_1[second10];
            physical_page = (v_addr >> 0xc) & 0x14;
            if(physical_page >> RESIDENT_OFFSET & 0x01 -- 0)) {
                generate page fault
            }
            addr = (physical_page << 0xc) & (v_addr & 0xc);
            tlb[v_addr] = addr;
        }
        return addr;
      }

Page Faults
-----------

-  A page fault is generated by the MMU or by the CPU when:

   -  An instruction references a virtual address that is not resident
      in physical memory.

   -  An instruction writes to a virtual address that is not writable

   -  An instruction branches/jumps to an address that is not executable

-  Each operating system has a different implementation / reaction to
   each type of page fault.

Page Faults - UNIX
------------------

-  Not resident - invoke swapper, retry instruction if successful, crash
   due to out of memory if it fails.

-  Not writable / readable - sends a signal to the process: SIGSEGV.
   Crashes by default, if handled, the process won’t crash

Page Faults - Windows
---------------------

-  Not resident - invoke swapper, retry instruction if sucessful. Raise
   exception if it fails.

-  Not writable / readable - raises exception to the process.

Page Replacement / Swapping
---------------------------

-  To support more optimal use of physical memory, operating systems
   implement swappers.

-  A swapper is a program that swaps pages from physical memory to and
   from persistent and slower storage.

-  The swapper is the program that handles page in/out operations for
   the stack and heap segements.

-  Many implementations will also demand page in text segments of
   programs to allow execution to begin before a program is fully
   loaded.

Page Replacement / Swapping
---------------------------

-  The swapper is invoked under the following conditions:

   -  The OS tries to translate a virtual address to a physical address,
      but the physical page is not resident

   -  The OS has exhausted or nearly exhausted physical memory and needs
      to move physical pages to slower storage.

   -  The OS has determined that a region of memory would be better used
      for another purpose:

      -  For another program that is more active

      -  For the filesystem cache

Page Replacement / Swapping
---------------------------

-  Who are the contenders for physical pages?

-  The block / FS cache

   -  Where recently read / written files are kept in memory by the OS

   -  Promotes better I/O scheduling decisions by allowing write-behind
      and read-ahead

   -  Improves file operation performance

-  Shared memory regions / memory mapped files

-  Program library and executable files

-  Program stack and heap segments

-  Device driver DMA (Direct Memory Access) regions

   -  Some exist outside of virtual / physical translation

   -  These regions are typically off-limits to the swapper.

   -  Some devices implement IO-MMUs

Swapper Algorithms
------------------

-  Key measures to consider in a swapping algorithm:

   -  Total page faults - during a period of time, how many page faults
      occur?

   -  Optimal page faults - given an optimal algorithm (that can predict
      the future) what were the minimum number of page faults?

      -  b.t.w. no such algorithm exists for programs subject to the
         halting problem.

   -  Working Set - the set of pages in a program that are most often
      and recently used.

Swapper Algorithms - Page Classification
----------------------------------------

-  Most computers record how each page has been accessed.

-  Typically, most hardware records whether a page has been read or
   modified in two bit fields with the ability to reset these bits. This
   yields four classes of pages:

   -  1 - Not referenced, not modified

   -  2 - Not referenced, modified

   -  3 - Referenced, not modified

   -  4 - Referenced, modified

-  Some hardware implementations will periodically clear the read bit to
   help determine which pages have been recently read. This is how you
   can get class 2 above.

Swapper Algorithms - NRU
------------------------

-  NRU - Not Recently Used

-  The NRU algorithm basically pages out pages from the lowest numbered
   class that has pages available.

-  This is the simplest algorithm.

Swapper Algorithms - FIFO
-------------------------

-  FIFO - First In First Out

-  When a page is loaded, it is added to the end of a list

-  When a page fault occurs and a new page needs to be loaded, the page
   in the front of the list is removed and swapped out

-  FIFO works on the premise that the oldest page is the least likely to
   be used in the future.

-  This algorithm is rarely used as is because this assumption is often
   faulty

Swapper Algorithms - Second Chance FIFO
---------------------------------------

-  Second chance FIFO improves over FIFOs deficiency of paging out
   heavily used pages by taking into account the read and write bits

-  Second chance FIFO will scan the list in order for a page with both
   read/write bits set to zero. If it finds a page in this class, it
   will swap that page out. If it fails to find such a page, it will
   swap out the first page in the list.

Swapper Algorithms - Clock
--------------------------

-  The clock algorithm improves upon second chance FIFO

-  Second chance FIFO suffers from many modifications to its internal
   list.

-  The clock algorithm uses a uses a circular list and stores a pointer
   to the oldest page. When a page fault occurs, the page pointed to is
   inspected. If its read bit is 0, it is evicted. If the read bit is 1,
   it is set to 0 and the pointer advances.

-  In reality, the clock algorithm is only very slightly better than
   second chance FIFO.

Swapper Algorithms - LRU
------------------------

-  LRU - Least Recently Used

-  LRU in practice is often close to optimal

-  LRU assumes:

   -  Pages that have been heavily used recently will be heavily used in
      the near future

   -  Pages that have not been used recently will not be used in the
      near future

-  To maintain the data necessary to implement LRU, the OS would have to
   maintain a linked list of all pages in physical memory. This list
   would have the most recently used page in the head and the least
   recently used page in the tail. This is not cheap. Every access
   requires a search of the list. Also the list can be very big.

Swapper Algorithms - LRU/NFU
----------------------------

-  NFU: Not Frequently Used - a software implementation of LRU

-  Each page gets a counter in the page table.

-  At each clock interrupt, the OS scans the page table and for each
   page with the read bit = 1, increments the counter

-  When a page fault occurs, the page with the lowest counter is evicted

-  Problems with NFU

   -  NFU isn’t forgetful enough

   -  If a single page is very heavily accessed and then never again, it
      will take a long time for it to be evicted (if ever).

Swapper Algorithms - LRU/NFU - Aging
------------------------------------

-  NFU can be improved with an approach called aging

-  NFU+Aging is a commonly used algorithm

-  Aging changes NFU slightly:

-  When a clock interrupt occurs, two things happen:

   -  For each page with a read bit set to 1, the most significant bit
      in the counter for that page is set.

   -  Each page has its counter value shifted to the right, thereby
      decreasing it.

Swapper Algorithms - LRU/NFU - Aging
------------------------------------

-  When a page fault occurs, the page with the lowest counter is
   evicted.

-  This policy more closely approximates LRU by favoring recently
   accessed pages and penalizing pages that have not been recently
   access by decreasing their value.

-  This algorithm falls short of LRU in two ways:

   -  The number of bits in the counter are finite. This allows for two
      pages two have the value of zero, but one of them being more
      recently used.

   -  The algorithm is constrained to the grain of a clock interrupt.
      All pages accessed between two successive interrupts are
      considered to be as recently accessed as each other.

Belady’s Anomoly
----------------

-  A seemingly obvious assumption to make is that having more physical
   pages will reduce the total number of page faults.

-  This assumption isn’t true for all page replacement algorithms and
   all access patterns.

-  Example - Assume that there are 5 virtual pages numbered from 0 to 4
   and these pages are accessed with the following pattern using FIFO:

   -  3 2 1 0 3 2 4 3 2 1 0 4

   -  In this case, having 3 physical pages will result in 9 page faults
      and having 4 physical pages will lead to 10 page faults!

Belady’s Anomoly (thanks Wikipedia)
-----------------------------------

    .. figure:: kernelmm/diagrams/beladys_anomoly.*
       :align: center
       :alt: image

       image

Belady’s Anomoly
----------------

-  For two physical memory sizes it is possible to find an access order
   to get ratios worse than even 2:1

-  A paper by Fornai and Ivany showed that you can get any ratio with
   the correct access pattern

Modeling Page Replacement
-------------------------

-  While examining a particular page replacement algorithm, the
   following are considered:

   -  The reference string of the executing process

   -  The number of pages available in memory

-  The reference string is a time ordered list of page accesses from one
   or more processes. For simplicity, often only one process is
   considered.

Modeling Page Replacement
-------------------------

-  Notation for modeling a page replacement algorithm:

   -  M - an array that keeps track of the state of memory. M has n
      elements

   -  n - the number of virtual pages.

   -  M - divided into two parts: the first m entries are in physical
      memory, the last n-m have been referenced but are paged out.

-  As a reference string is read, entry by entry, the algorithm checks
   to see if the page is in memory (top part of M).

-  If not, a page fault occurs. If there is an empty slot (top part of
   M), the page is moved from the bottom into that slot.

-  If the top part of M is full, the page replacement algorithm is
   invoked to remove a page from memory.

Modeling LRU
------------

-  This is the LRU algorithm modeled with the reference string: 0 2 1 3
   5 4 6 3 7 4 7 3 3 5 5 3 1 1 1 7 2 3 4 1

    .. figure:: kernelmm/diagrams/lru_reference_string.*
       :align: center
       :alt: image

       image

Modeling LRU
------------

-  LRU has an interesting property when modeled this way.

-  For LRU M(m, r) is always a subset of or equivalent to M(m+1, r).

-  This means that at memory access ’r’, all of the pages in m will
   exist if there were an additional page of physical memory m+1

-  This means, that LRU always does as well or improves with more
   physical pages and is not subject to Belady’s anomaly

Modeling - Distance Strings
---------------------------

-  Another interesting measurement in this type of modeling is the
   distance string.

-  The distance string refers to the distance from the top of the
   ’stack’ to where the page is in the stack.

-  Pages not yet referenced get a distance of infinity.

-  The distance value depends upon both the reference string and the
   algorithm.

-  The optimal algorithm will minimize the values of the distance
   string.

Modeling - Distance Strings
---------------------------

-  The distance string can be used to estimate the number of page faults
   for different physical memory sizes using the following formula

-  :math:`$Fm = Sum(k = m+1, n, Ck) + Cinf$`

   -  :math:`$Ck$` = the occurrences of k in the distance string

   -  :math:`$Cinf$` = the occurrences of infinity in the distance
      string

   -  :math:`$m$` = the number of physical pages

   -  :math:`$n$` = the number of virtual pages

   -  :math:`$Fm$` = the predicted page fault rate for m physical pages

Modeling - Distance Strings
---------------------------

    .. figure:: kernelmm/diagrams/lru_reference_string.*
       :align: center
       :alt: image

       image

-  :math:`$C1 = 4$`, :math:`$C2 = 3$`, :math:`$C3 = 3$`,
   :math:`$C4 = 3$`, :math:`$C5 = 2$`, :math:`$C6 = 1$`,
   :math:`$C7 = 0$`, :math:`$Cinf = 8$`

-  So, for various memory sizes:

-  :math:`$F1 = 3+3+3+2+1+0+8 = 20$`

-  :math:`$F2 = 3+3+2+1+0+8 = 17$`

-  :math:`$F5 = 0+8 = 8$`

-  :math:`$F6 = 8 = 8$`

Design Considerations for Paging Systems
----------------------------------------

-  A naive paging implementation would start up a process with none of
   its pages in memory (libraries, program, data, bss, etc...).

-  When the process attempts to execute its first instruction it would
   immediately generate a page fault.

-  For the first few moments of a program’s execution it would generate
   many page faults until it was mostly loaded and then run without
   generating many page faults.

-  Generating many and unnecessary faults leads to poorly performing
   applications.

Working Sets
------------

-  Although not universally true, many applications exhibit a locality
   of reference.

-  This means that, if a process is working with a given page at one
   point in time, then just before that time and in the near future it
   is likely to continue to work with that page and pages that are near
   (in terms of virtual address distance).

-  Many programs will have one or more regions that they exhibit a
   locality of reference. Most commonly they will be one or more regions
   in the stack or heap.

-  The set of pages that a process is currently using is called the
   working set

Taking Advantage of Locality
----------------------------

-  If we take locality into account, how can we make paging systems
   faster?

-  Some approaches:

   -  When loading pages from a library or program file, adjacent pages
      are loaded at the same time.

   -  Often, after servicing a page fault, and operating system can
      continue to load program pages in asynchronously.

   -  When choosing pages to evict, if there is more than one page that
      is desirable to evict, the OS can choose to evict the page with
      the greatest distance from any pages in the working set.

Costs of Paging Different Page Classes
--------------------------------------

-  Recall, earlier we defined the following classes of pages:

   -  1 - Not referenced, not modified

   -  2 - Not referenced, modified

   -  3 - Referenced, not modified

   -  4 - Referenced, modified

-  These classes differ in terms of eviction cost.

Costs of Paging Different Page Classes
--------------------------------------

-  Text and other read-only pages will always be in classes 1 or 3 (not
   modified)

-  Stack and heap pages can be in any of 1-4.

-  When a page in class 1 or 3 (not modified) is evicted, the swapper
   only needs to mark the page as not resident and then reuse the
   physical page for a new entry.

-  When a page in class 2 or 4 is evicted, the swapper must also copy
   the contents of the page to a different storage system (typical a
   disc). This increases the cost of evicting these pages.

Costs of Paging Different Page Classes
--------------------------------------

-  How do we reduce cost?

-  In the background, when the disc is otherwise idle, we can commit
   modified pages to the disc to reduce future cost.

-  Often, this behavior is reserved for pages that are likely to be
   evicted rather than pages in the working set.

-  Chose unmodified pages over modified pages for eviction.

Local vs Global Paging
----------------------

-  In process scheduling, we try to be fair and give each runnable
   process an even share of the CPU(s).

-  What are some things we can do to be ’fair’ in our page replacement
   implementations?

-  One possibility to consider is local vs. global page replacement.

   -  In global page replacement, if a process page faults, we consider
      all of memory for page eviction.

   -  In local page replacement, we consider only the process that
      caused the page fault’s pages for page replacement or otherwise
      favor them.

   -  A local page replacement policy can help make sure that one
      process that causes many page faults does not interfere with other
      processes too much.

   -  The downside to a local policy is that it can hurt overall system
      performance

Page Locking
------------

-  For some operations, we need to guarantee that a page will remain in
   physical memory

-  The most common case is for regions of memory that are dedicated to
   buffering for devices or regions of memory that work with DMA.

-  DMA is basically a system by which an operating system kernel can
   tell a device to write the results of an operation directly to a
   specific region of memory without interacting directly with the CPU.

-  During the period of time a DMA operation is occuring, the OS must
   guarantee that the region of memory is not evicted by the swapper.

-  The best, but less ideal alternative to this is to only do DMA
   operations to operating system buffers and then copy them to program
   buffers.

COW: Copy on Write
------------------

-  In UNIX, we create processes by calling fork() or clone().

-  In either of these cases, regions of memory (from the program’s
   perspective) are copied.

-  To avoid unnecessary copying, the operating system will only copy
   page table entries.

-  When the page table entries are copied, they are all marked as
   read-only for both the parent and child process.

-  After the copy operation, the parent and child process will share
   each other’s physical pages.

COW: Copy on Write
------------------

-  COW comes into effect when write operations happen to a shared page.

-  The process that causes the page fault, will make a copy of the
   physical page into a new physical page and updates its page table
   entry to point to that page.

-  The new page will then be marked as writable.

-  In this way, new processes only use memory that is different from the
   parent process.

Backing Store
-------------

-  For heap, stack, and data pages the backing store in most operating
   systems is one of:

   -  Swap file

   -  Swap partition

-  In simpler operating systems, swap partitions are preferable because
   the operating system can interact directly with the disc and not FS
   code.

-  In more advanced operating systems (more recent versions of Linux or
   Windows), the FS implementation is advanced enough that the OS can
   guarantee the location of sectors of the swap file that there is no
   overhead to using a swap file.

-  Swap files have the advantage of being able to be resized on demand.
   In Linux, additional swap files can be created and then used with the
   swapon command.

-  Windows manages one or more swap files automatically.

Hibernation
-----------

-  Hibernation is a specific implementation of a swap file.

-  To hibernate, the operating system will page out all used physical
   pages to disc. Either a special hibernation file or the swap file
   will be used.

-  Then, the operating system will either shutdown the computer or put
   the computer in a special low power state.

-  When the computer boots back up, the operating system will notice
   that it was previously shutdown by hibernation.

-  After the core OS components are loaded, the OS will restore the page
   table from the hibernation file and then begin paging in from the
   hibernation file.

VM Performance - Hot Memory
---------------------------

-  Theoretically, the best use of physical memory is to use all of the
   physical memory if possible.

-  To improve performance and responsiveness of operations that need new
   memory (reading a new file, writing new data, allocating new pages to
   the heap or stack), many modern VM implementations will keep a few
   pages free at all times.

-  Both Windows and Linux will typically keep about 12-16MB free as a
   "hot memory" area.

-  This hot memory area has the effect of preventing page faults due to
   "jitters" of memory usage. So, if a process is increasing and
   decreasing its memory usage rapidly, it will not likely generate page
   faults.

Modeling - Distance Strings
---------------------------

    .. figure:: kernelmm/diagrams/hot_memory.*
       :align: center
       :alt: image

       image

-  Here, we can see a long running Linux OS.

-  453388K is used by the FS - Cache

-  178580K is used by software

-  52704K is being used as buffers

-  18272K is swapped out

-  7972K is being kept as "hot memory" (or is otherwise recently freed)

Summary: Page Fault Handling
----------------------------

-  1- The hardware interrupts the kernel. Program counter and registers
   are saved. Information necessary to restart the current instruction
   is also saved. The OS is then called.

-  2- The OS discovers a page fault has occurred. The OS inspects either
   a special register or inspects the saved instruction from 1 to figure
   out which page is needed.

-  3- Once the page is discovered, it determines the cause of the page
   fault. If the address is inconsistent with access rights or memory
   accessible to the process, a signal is sent to the process or the
   process is terminated.

-  4- If it is consistent, the OS tries to acquire a free physical page
   to load the necessary page into memory. If no physical page is free,
   the page replacement algorithm is invoked.

Summary: Page Fault Handling
----------------------------

-  5- If the evicted page is dirty, it is scheduled to be written to
   disc. In this case the faulting process is put to sleep and a context
   switch occurs.

-  6- As soon as the evicted page is clean, the OS schedules a disc
   operation to load the page. While waiting for the load, the faulting
   process is suspended and the scheduler will pick another process to
   run.

-  7- As soon as the page is loaded from disc, the page table entry is
   updated to reflect its position and updates the status of the page to
   resident

-  8- The OS restores the registers of the program, and depending on
   hardware details will retry the faulting instruction, updating the
   program counter accordingly.

-  9- The faulting process is then marked as runnable for the scheduler.


