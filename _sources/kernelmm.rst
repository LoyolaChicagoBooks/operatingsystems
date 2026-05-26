Virtual Memory
==============

.. index::
   single: virtual memory
   pair: memory; virtual memory

Virtual memory is the operating-system layer that gives each process its
own address space. It lets the kernel separate process memory from
physical memory and decide which pages should be resident, shared,
protected, or backed by storage.

What is Virtual Memory?
-----------------------

.. index::
   single: address space
   pair: virtual memory; address space

Virtual memory gives a process a private view of memory that does not
directly match the machine's physical memory.

This supports several operating-system goals:

- Programs can run even when their total virtual memory use is larger
  than available physical memory.
- File I/O can be buffered and cached in memory.
- Processes can share pages, such as executable code or shared library
  text.
- The kernel can enforce memory protection between processes.
- Virtual addresses can be translated to physical addresses by hardware
  and kernel-managed tables.

Memory Management Units
-----------------------

.. index::
   single: memory management unit (MMU)
   single: MMU
   single: translation lookaside buffer (TLB)
   pair: CPU; MMU

The memory management unit, or MMU, is the hardware that translates
virtual addresses into physical addresses.

The MMU also enforces memory protection and works with structures such
as page tables and the translation lookaside buffer. In modern systems,
the MMU is usually part of the CPU. When a process accesses memory, the
CPU and MMU use the current process's page tables to decide where the
access goes and whether it is allowed.

Pages and Page Tables
---------------------

.. index::
   single: pages
   single: page tables
   pair: virtual memory; pages
   pair: virtual memory; page tables

A page is a fixed-size block of virtual memory. A page table records how
virtual pages map to physical pages.

Every process appears to have access to a large address space, but only
some of that address space is actually mapped and resident at any given
time. The operating system maintains page tables so the MMU can translate
virtual addresses and enforce permissions.

Page Table Size
---------------

.. index::
   single: page size
   pair: page tables; overhead

Page size affects page-table overhead.

If a 32-bit system used 128-byte pages, it would need more than
33 million page entries to describe a 4 GB address space. Even with very
compact entries, that overhead would be too large for each process.

With 4 KB pages, a 4 GB address space has 1,048,576 virtual pages. On
32-bit x86, each page-table entry is 32 bits, so a flat page table would
still require about 4 MB per process. That is too much overhead for a
small process.

Page Table Entries
------------------

.. index::
   single: page table entries
   pair: page tables; entries

A page-table entry stores the physical page number and attributes for a
virtual page.

For a 4 KB page in a 32-bit address space, 20 bits identify the page and
12 bits remain for flags and other information. Common attributes include
whether the page is present, writable, executable, and modified.

Multi-Level Page Tables
-----------------------

.. index::
   single: multi-level page tables
   pair: page tables; multi-level

Multi-level page tables reduce memory overhead by allocating lower-level
tables only for address ranges that are actually used.

In a two-level page table, part of the virtual address selects a first
level entry, and another part selects a second level entry. The first
level covers large regions. The second level is created only when a
process actually uses pages in that region.

Two-Level Page Table
--------------------

The two-level page-table diagram shows how a virtual address can be split
into indexes and an offset.

.. figure:: kernelmm/diagrams/two_level_page_table.*
   :align: center
   :alt: Two-level page table

MMU Address Translation Algorithm
---------------------------------

.. index::
   single: address translation
   single: TLB
   single: TLB miss
   pair: MMU; address translation

Address translation first checks the TLB. If the translation is not
cached, the MMU and kernel page-table structures are used to find the
physical page.

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
         if(physical_page >> RESIDENT_OFFSET & 0x01 == 0) {
            generate page fault;
         }
         addr = (physical_page << 0xc) & (v_addr & 0xc);
         tlb[v_addr] = addr;
      }
      return addr;
   }

Key points:

- The TLB caches recent virtual-to-physical translations.
- A TLB miss requires a page-table lookup.
- Page-table entries contain both page addresses and permission or
  status bits.
- A nonresident page causes a page fault.
- Once translation succeeds, the result can be cached in the TLB.

Page Faults
-----------

.. index::
   single: page fault
   single: page fault handler
   pair: virtual memory; page fault

A page fault is an exception raised when a memory reference cannot be
completed using the current page-table entry.

Common causes include accessing a page that is not resident in physical
memory, writing to a read-only page, or executing from a non-executable
page. The operating system decides whether the fault can be repaired or
whether the process must be notified or terminated.

Page Faults in UNIX
-------------------

.. index::
   single: SIGSEGV
   pair: UNIX; page faults

UNIX systems usually handle nonresident pages by loading the page and
retrying the instruction.

If the page cannot be loaded because memory is exhausted, the process may
fail or the system may take stronger action. If the access violates
permissions, the kernel normally sends ``SIGSEGV`` to the process.

Page Faults in Windows
----------------------

.. index::
   pair: Windows; page faults

Windows also handles nonresident pages by loading the page and retrying
the instruction when possible.

If the fault cannot be repaired, Windows raises an exception in the
faulting process. Permission faults are also reported through the
exception mechanism.

Page Replacement and Swapping
-----------------------------

.. index::
   single: page replacement
   single: swapper
   single: swap file
   single: swap partition
   pair: virtual memory; page replacement

Page replacement is the policy for choosing which physical page to reuse
when memory is needed.

The swapper moves pages between physical memory and slower backing
storage. It handles heap and stack pages, and it may also demand-page
program text so execution can begin before the full program is loaded.

When the Swapper Runs
---------------------

The swapper runs when a needed virtual page is not resident or when the
system needs more free physical pages.

The operating system may also reclaim memory when a page would be more
useful for another process, the filesystem cache, or another kernel
purpose. The swapper is therefore part of both memory protection and
overall system performance.

Physical Page Contenders
------------------------

.. index::
   single: filesystem cache
   single: DMA buffers
   pair: memory; physical page contenders

Many uses compete for physical pages.

Important contenders include the filesystem cache, shared memory regions,
memory mapped files, executable and library text, process stacks, process
heaps, and device DMA buffers. Some DMA regions must not be swapped
because a device is using the physical address directly.

Swapper Algorithms
------------------

.. index::
   single: page replacement algorithms
   single: working set
   pair: virtual memory; swapper algorithms

A page replacement algorithm tries to minimize expensive page faults.

Useful measures include the total number of page faults, the page faults
an optimal algorithm would have produced, and the working set of pages a
process is actively using. No real algorithm can predict the future, so
replacement policies approximate likely future behavior from recent
history.

Page Classification
-------------------

.. index::
   single: page classification
   pair: page replacement; reference bit

Many processors record whether a page has been referenced or modified.

These bits divide pages into four classes:

- Not referenced and not modified.
- Not referenced and modified.
- Referenced and not modified.
- Referenced and modified.

The operating system can clear reference bits periodically to estimate
which pages have been used recently.

Not Recently Used
-----------------

.. index::
   single: Not Recently Used (NRU)
   single: NRU
   pair: page replacement; NRU

Not Recently Used, or NRU, evicts pages from the lowest available page
class.

NRU is simple. It prefers pages that have not been referenced and have
not been modified. It is inexpensive, but it is only a rough estimate of
future page use.

FIFO Replacement
----------------

.. index::
   single: FIFO page replacement
   pair: page replacement; FIFO

First In First Out, or FIFO, evicts the oldest resident page.

FIFO assumes the oldest page is least likely to be used again. That
assumption is often wrong, so FIFO is rarely used directly in production
systems.

Second-Chance FIFO
------------------

.. index::
   single: second-chance FIFO
   pair: page replacement; second-chance

Second-chance FIFO improves FIFO by checking the reference bit before
evicting a page.

If the oldest page has not been referenced, it can be evicted. If it has
been referenced, the algorithm clears the reference bit and gives the
page another chance. This avoids evicting heavily used pages simply
because they are old.

Clock Replacement
-----------------

.. index::
   single: clock replacement algorithm
   pair: page replacement; clock

The clock algorithm is an efficient implementation of second-chance
replacement.

It stores pages in a circular list and keeps a pointer to the next
candidate. If the pointed-to page has a clear reference bit, it is
evicted. If the bit is set, the algorithm clears it and advances the
pointer.

Least Recently Used
-------------------

.. index::
   single: Least Recently Used (LRU)
   single: LRU
   pair: page replacement; LRU

Least Recently Used, or LRU, evicts the page that has not been used for
the longest time.

LRU is often close to optimal because programs tend to reuse pages they
used recently. A perfect LRU implementation is expensive because the
kernel would need to update ordering information on every memory access.

Not Frequently Used
-------------------

.. index::
   single: Not Frequently Used (NFU)
   single: NFU
   pair: page replacement; NFU

Not Frequently Used, or NFU, approximates LRU with counters.

At each clock interrupt, the operating system scans pages and increments
a counter for each page whose reference bit is set. On a page fault, a
page with a low counter is a candidate for eviction.

NFU is not forgetful enough. A page that was used heavily in the past can
keep a high counter long after it stops being useful.

Aging
-----

.. index::
   single: aging (page replacement)
   pair: page replacement; aging

Aging improves NFU by making old references matter less over time.

At each clock interrupt, the operating system shifts each counter right.
If the page was referenced, it sets the most significant bit. The result
is a small history of recent use. Aging still has limited precision, but
it tracks recency better than plain NFU.

Belady's Anomaly
----------------

.. index::
   single: Belady's anomaly
   pair: page replacement; Belady's anomaly

Belady's anomaly is the surprising result that adding more physical pages
can increase page faults for some replacement algorithms and access
patterns.

For example, FIFO with the reference string ``3 2 1 0 3 2 4 3 2 1 0 4``
can produce 9 page faults with 3 physical pages and 10 page faults with
4 physical pages. More memory does not always help FIFO.

Belady's Anomaly Diagram
------------------------

The diagram shows a reference string where FIFO performs worse with more
physical pages.

.. figure:: kernelmm/diagrams/beladys_anomoly.*
   :align: center
   :alt: Belady's anomaly

Modeling Page Replacement
-------------------------

.. index::
   single: reference string
   pair: page replacement; modeling

Page replacement algorithms can be modeled with a reference string and a
fixed number of physical pages.

The reference string is the ordered list of page accesses. The model
tracks which pages are in physical memory and which pages have been
referenced but are not resident. On each reference, the algorithm either
finds the page in memory or handles a page fault.

Modeling LRU
------------

.. index::
   single: LRU; stack property
   pair: LRU; Belady's anomaly

LRU has the stack property: with more physical pages, the set of pages in
memory is always a superset of the set that would be present with fewer
physical pages.

.. figure:: kernelmm/diagrams/lru_reference_string.*
   :align: center
   :alt: LRU reference string

This property means LRU is not subject to Belady's anomaly. Adding more
physical pages cannot make LRU perform worse for the same reference
string.

Distance Strings
----------------

.. index::
   single: distance strings
   pair: page replacement; distance strings

A distance string records how far a referenced page is from the top of
the algorithm's conceptual stack.

Pages not yet referenced have infinite distance. The distance string can
estimate page faults for different physical memory sizes by counting how
often the distance exceeds the number of available frames.

::

   Fm = Sum(k = m+1, n, Ck) + Cinf

Here, ``Ck`` is the number of times distance ``k`` occurs, ``Cinf`` is
the number of infinite distances, ``m`` is the number of physical pages,
and ``n`` is the number of virtual pages.

Distance String Example
-----------------------

The distance-string counts can be read from the LRU model.

.. figure:: kernelmm/diagrams/lru_reference_string.*
   :align: center
   :alt: LRU reference string

In this example, ``C1 = 4``, ``C2 = 2``, ``C3 = 1``, ``C4 = 3``,
``C5 = 2``, ``C6 = 2``, ``C7 = 1``, and ``Cinf = 8``. Therefore:

::

   F1 = 2 + 1 + 3 + 2 + 2 + 1 + 8 = 19
   F2 = 1 + 3 + 2 + 2 + 1 + 8 = 17
   F5 = 2 + 1 + 8 = 11
   F6 = 1 + 8 = 9

Design Considerations for Paging Systems
----------------------------------------

.. index::
   pair: virtual memory; paging design

A naive paging system could start a process with none of its pages in
memory, but that would cause many page faults at startup.

Real systems try to avoid unnecessary faults. They use locality,
read-ahead, working-set estimates, and page replacement policies to keep
the pages a process is likely to use in memory.

Working Sets
------------

.. index::
   single: working set
   single: locality of reference
   pair: virtual memory; working set

A working set is the set of pages a process is actively using during a
period of execution.

Many programs show locality of reference. If a process is using one page
now, it is likely to use the same page or nearby pages soon. Stacks,
heaps, loops, and data structures often create these patterns.

Taking Advantage of Locality
----------------------------

Operating systems use locality to reduce page faults.

They may load adjacent pages from executable files or libraries, continue
loading pages asynchronously after a fault, and prefer evicting pages far
from the current working set. These techniques trade a little extra work
now for fewer faults later.

Costs of Paging Page Classes
----------------------------

.. index::
   pair: page replacement; eviction cost

Not all page evictions cost the same amount.

Unmodified pages can usually be dropped and reloaded later from their
original backing store. Modified pages must be written to swap or another
backing store before their physical page can be reused.

Reducing Paging Cost
--------------------

The kernel can reduce future paging cost by cleaning modified pages in
the background.

When the disk is otherwise idle, the system may write dirty pages that
are likely eviction candidates. This makes later eviction cheaper because
the page is already clean. Replacement policies also prefer clean pages
when the performance tradeoff is reasonable.

Local and Global Paging
-----------------------

.. index::
   single: global page replacement
   single: local page replacement
   pair: page replacement; local vs global

Page replacement can be local or global.

In global replacement, a faulting process may cause the kernel to evict a
page from any process. In local replacement, the kernel prefers to evict
pages owned by the process that faulted. Local replacement can improve
fairness, while global replacement can improve overall memory use.

Page Locking
------------

.. index::
   single: page locking
   single: pinned pages
   pair: virtual memory; page locking
   single: DMA

Page locking pins a page in physical memory so the swapper cannot evict
it.

Pinned pages are needed for operations such as DMA, where a device reads
from or writes to a specific physical memory region. During the DMA
operation, the kernel must ensure that the physical page remains present
and stable.

Copy on Write
-------------

.. index::
   single: copy-on-write (COW)
   single: COW
   pair: virtual memory; copy-on-write

Copy on write, or COW, lets related processes share pages until one of
them writes to a shared page.

After ``fork()`` or ``clone()``, the kernel can copy page-table entries
instead of copying every page. The shared entries are marked read-only.
When the parent or child writes to one of those pages, the write causes a
page fault. The kernel then copies the physical page, updates the
faulting process's page table, and marks the new page writable.

Backing Store
-------------

.. index::
   single: backing store
   single: swap file
   single: swap partition
   pair: virtual memory; backing store

Backing store is the persistent or recoverable location that backs a
virtual page.

For heap, stack, and data pages, the backing store is usually a swap file
or swap partition. Simpler systems often prefer swap partitions because
the kernel can address the disk directly. Modern systems can also use
swap files efficiently because the filesystem can provide stable block
locations.

Hibernation
-----------

.. index::
   single: hibernation
   pair: virtual memory; hibernation

Hibernation stores the machine's memory state so the system can power
down and later resume.

To hibernate, the operating system writes used physical pages to a
hibernation file or swap area. On the next boot, the kernel detects the
hibernated state, restores enough core state to resume, rebuilds page
tables, and pages data back in as needed.

Hot Memory
----------

.. index::
   single: hot memory
   pair: memory; free page reserve

Hot memory is free memory kept available so the system can satisfy small
bursts of memory demand without immediate page replacement.

Modern systems often keep some physical pages free even though using all
memory for cache might seem optimal. This reserve reduces jitter when
processes allocate memory, files are read, or the filesystem cache grows
and shrinks.

Hot Memory Example
------------------

The following diagram shows memory use on a long-running Linux system.

.. figure:: kernelmm/diagrams/hot_memory.*
   :align: center
   :alt: Hot memory

The example includes filesystem cache, software memory, buffers, swapped
pages, and a small amount of free or recently freed memory. That free
area gives the kernel room to respond quickly to new allocation requests.

Summary: Page Fault Handling
----------------------------

.. index::
   pair: page fault; handling sequence

Page fault handling is the path from a faulting memory instruction back
to a runnable process.

The usual sequence is:

1. The hardware traps into the kernel and saves the process state.
2. The kernel identifies the faulting address and the reason for the
   fault.
3. If the access is invalid, the kernel signals or terminates the
   process.
4. If the access is valid, the kernel finds or creates a free physical
   page.
5. If a dirty page must be evicted, the kernel schedules it to be written
   to backing storage.
6. The kernel loads the needed page from backing storage if needed.
7. The page-table entry is updated to mark the page resident with the
   correct permissions.
8. The saved registers are restored.
9. The faulting instruction is retried, and the process becomes runnable
   again.

Linux Kernel Module Case Study
------------------------------

.. index::
   single: Linux kernel module
   single: MODULE_LICENSE
   single: module_init
   single: module_exit
   single: printk()
   pair: Linux; kernel modules

This case study shows the smallest structure of a Linux loadable kernel
module.

.. literalinclude:: ../examples/systems-code-examples/linux-kernel/proc_module/simple.c
   :language: c
   :linenos:

Key points:

- ``MODULE_LICENSE()`` declares the module license to the kernel.
- ``module_init()`` registers the function called when the module loads.
- ``module_exit()`` registers the function called when the module
  unloads.
- ``printk()`` writes messages to the kernel log rather than standard
  output.

Linux /proc Module Case Study
-----------------------------

.. index::
   single: /proc filesystem
   single: vmalloc()
   single: copy_from_user()
   pair: Linux; /proc module

This case study shows a Linux kernel module that exposes state through a
``/proc`` entry.

.. literalinclude:: ../examples/systems-code-examples/linux-kernel/proc_module/fortune.c
   :language: c
   :linenos:

Key points:

- The module allocates kernel memory for stored fortune strings with
  ``vmalloc()``.
- ``create_proc_entry()`` creates the ``/proc/fortune`` interface.
- ``copy_from_user()`` copies data safely from a user buffer into kernel
  memory.
- The read handler returns one stored fortune at a time.
- Cleanup removes the proc entry and frees the allocated memory.

Linux Character Device Case Study
---------------------------------

.. index::
   single: character device driver
   single: register_chrdev()
   single: file_operations
   single: put_user()
   single: unregister_chrdev()
   pair: Linux; character device

This case study shows how a Linux character device exposes kernel code
through the file interface.

.. literalinclude:: ../examples/systems-code-examples/linux-kernel/simple_chardev/chardev.c
   :language: c
   :linenos:

Key points:

- ``register_chrdev()`` asks the kernel for a major device number.
- The ``file_operations`` structure connects file operations to driver
  functions.
- ``device_open()`` prepares the message returned by the device.
- ``put_user()`` copies bytes from kernel memory to the user buffer.
- ``unregister_chrdev()`` removes the device registration when the
  module unloads.

Linux System Call Case Study
----------------------------

.. index::
   single: system call table
   single: syscalls.h
   pair: Linux; adding a system call

This case study shows the pieces needed to add and test a simple Linux
system call.

.. literalinclude:: ../examples/systems-code-examples/linux-kernel/syscall/syscall.patch
   :language: diff
   :linenos:
   :start-at: diff -r linux-source-2.6.38-orig//arch/x86/ia32/ia32entry.S
   :end-before: Only in linux-source-2.6.38/: .config

Key points:

- The patch adds a system call number for the new call.
- The syscall table is updated so the kernel can dispatch to the new
  handler.
- The public syscall declaration is added to ``syscalls.h``.
- The implementation adds two integer arguments and returns the result.

.. literalinclude:: ../examples/systems-code-examples/linux-kernel/syscall/test/main.cc
   :language: cpp
   :linenos:

Key points:

- The test program defines the syscall number used by the patched
  kernel.
- ``syscall()`` invokes the kernel entry directly.
- The wrapper function gives the test a normal C function shape.
- ``perror()`` reports failure if the syscall returns ``-1``.

Minix Startup Case Study
------------------------

.. index::
   single: Minix
   single: sef_startup()
   single: System Event Framework (SEF)
   pair: Minix; startup

This case study shows a minimal Minix program using the System Event
Framework startup path.

.. literalinclude:: ../examples/systems-code-examples/minix-kernel/sample1/sample1.c
   :language: c
   :linenos:

Key points:

- ``sef_startup()`` initializes the Minix service environment.
- The program then runs ordinary user code.
- The example is useful as a minimal starting point for Minix service
  examples.

Minix Time Driver Case Study
----------------------------

.. index::
   single: Minix time driver
   pair: Minix; character driver
   single: CMOS

This case study shows a small Minix character driver that exposes the
current time from CMOS through a device interface.

.. literalinclude:: ../examples/systems-code-examples/minix-kernel/time/time.h
   :language: c
   :linenos:

Key points:

- ``TIME_MAJOR`` defines the major number for the time device.
- The header keeps the device number separate from the driver
  implementation.

.. literalinclude:: ../examples/systems-code-examples/minix-kernel/time/time.c
   :language: c
   :linenos:

Key points:

- The driver table connects Minix driver operations to local functions.
- ``read_register()`` and ``write_register()`` access CMOS registers
  through kernel calls.
- ``get_time()`` reads a stable RTC value and converts BCD fields.
- ``time_transfer()`` copies formatted time data to the requesting
  process.
- The SEF callbacks handle fresh startup, live update, and restart.
