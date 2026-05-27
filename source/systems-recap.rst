Systems and Discrete Math Recap
===============================

This chapter reviews ideas students are expected to know before starting
the main operating systems material. It is not a replacement for
Discrete Structures or Computer Systems. It is a short reminder of the
parts of those courses that show up repeatedly in operating systems.

Boolean Logic
-------------

Boolean logic works with truth values and logical operations. The
operations most often used in systems code are AND, OR, NOT, and XOR.

Truth tables are a simple way to check all possible inputs to a logical
expression. This example is written in C.

.. code-block:: c

   #include <stdio.h>
   #include <stdbool.h>

   int main(void) {
       printf("AND Truth Table\n");
       printf("true AND true = %d\n", true && true);
       printf("true AND false = %d\n", true && false);
       printf("false AND true = %d\n", false && true);
       printf("false AND false = %d\n", false && false);
       return 0;
   }

Operating systems use Boolean logic in conditionals, permission checks,
state tests, and lock conditions.

Logical Equivalence
-------------------

Two logical expressions are equivalent when they have the same truth
value for every possible input. This example is written in C.

.. code-block:: c

   #include <stdio.h>
   #include <stdbool.h>

   int main(void) {
       bool a = true;
       bool b = false;
       printf("%d\n", (a && b) == (b && a));
       return 0;
   }

Logical equivalence matters when simplifying conditions. In systems
code, a small change to a condition can alter when a process sleeps,
wakes, enters a critical section, or handles an error.

De Morgan's Laws
----------------

De Morgan's laws are useful transformations for negated logical
expressions. This example is written in C.

.. code-block:: c

   #include <stdio.h>
   #include <stdbool.h>

   int main(void) {
       bool a = true;
       bool b = false;
       printf("%d\n", (!a || !b) == !(a && b));
       printf("%d\n", (!a && !b) == !(a || b));
       return 0;
   }

These laws often appear when code tests the opposite of a condition. For
example, a scheduler or monitor may wait while a condition is not true.

Short-Circuit Evaluation
------------------------

Short-circuit evaluation means the second operand is evaluated only when
the first operand does not already determine the result. This example is
written in C.

.. code-block:: c

   #include <stdio.h>
   #include <stdbool.h>

   bool expensive_test(void) {
       printf("expensive_test ran\n");
       return false;
   }

   int main(void) {
       bool a = true;
       printf("%d\n", a || expensive_test());
       return 0;
   }

This matters in systems code because the second expression may read a
pointer, call a function, or touch shared state. The order of evaluation
can be part of the program's correctness.

Bitwise Operations
------------------

Bitwise operations apply logic to individual bits. They are used for
flags, masks, permissions, page-table entries, device registers, and
parity. This example is written in C.

.. code-block:: c

   unsigned int flags = 0;
   const unsigned int READABLE = 1u << 0;
   const unsigned int WRITABLE = 1u << 1;

   flags |= READABLE;
   flags |= WRITABLE;

   if ((flags & WRITABLE) != 0) {
       printf("page is writable\n");
   }

The shift operator creates a mask with one bit set. The AND operation
tests whether that bit is present.

XOR and Parity
--------------

Exclusive OR, or XOR, returns true when exactly one input is true. At the
bit level, XOR is useful because the same operation can create and
recover parity. This example is written in C.

.. code-block:: c

   unsigned char a = 0x35;
   unsigned char b = 0x7a;
   unsigned char parity = a ^ b;
   unsigned char recovered_b = a ^ parity;

This is the same idea used in the RAID parity example later in the book.

Number Systems and Data Representation
--------------------------------------

Systems code frequently uses decimal, hexadecimal, octal, and binary
views of the same value. This example is written in C.

.. code-block:: c

   #include <stdio.h>

   int main() {
       int num = 42;
       printf("Decimal: %d\n", num);
       printf("Hexadecimal: 0x%x\n", num);
       printf("Octal: %o\n", num);
       printf("Binary: ");
       for (int i = sizeof(num) * 8 - 1; i >= 0; i--) {
           printf("%d", (num >> i) & 1);
       }
       printf("\n");
       return 0;
   }

Hexadecimal is common because each hex digit represents four bits. This
makes addresses, masks, and binary layouts easier to read.

Signed and Unsigned Values
--------------------------

Signed and unsigned integers use the same bits but interpret them
differently. This example is written in C.

.. code-block:: c

   int signed_value = -1;
   unsigned int unsigned_value = (unsigned int)signed_value;
   printf("%d\n", signed_value);
   printf("%u\n", unsigned_value);

This matters for sizes, offsets, system call arguments, and error values.
Many system interfaces use unsigned types for sizes and signed types for
return values.

Modular Arithmetic
------------------

Modulo arithmetic is arithmetic with wraparound. This example is written
in C.

.. code-block:: c

   int mod(int a, int b) {
       return a - (a / b) * b;
   }

Circular buffers, periodic timers, hash tables, and scheduler queues all
use wraparound behavior. When an index reaches the end of a fixed-size
array, modulo arithmetic brings it back to the beginning.

Sets
----

A set is a collection of distinct values. Sets are useful for describing
membership, ownership, permissions, and active resources. This example is
written in C++.

.. code-block:: c++

   std::set<int> ready;
   ready.insert(12);
   ready.insert(19);

   if (ready.contains(12)) {
       std::cout << "process 12 is ready\n";
   }

Operating systems use set-like ideas for ready processes, open files,
allocated pages, mounted filesystems, and held locks.

Power Sets
----------

The power set of a set is the set of all subsets. This example is written
in C.

.. code-block:: c

   #include <stdio.h>

   void print_power_set(int set[], int n) {
       int power_set_size = 1 << n;

       for (int mask = 0; mask < power_set_size; mask++) {
           printf("{ ");
           for (int i = 0; i < n; i++) {
               if ((mask & (1 << i)) != 0) {
                   printf("%d ", set[i]);
               }
           }
           printf("}\n");
       }
   }

The implementation uses bits to decide whether each element is in a
subset. This is a direct connection between set theory and bit masks.

Relations and Partial Orders
----------------------------

A relation describes which pairs of values are connected. A partial
order is a relation that can describe a consistent ordering without
requiring every pair to be comparable.

Lock ordering is a partial-order idea. If every thread acquires locks in
the same order, the program avoids circular wait and therefore avoids
one common cause of deadlock.

Graphs
------

A graph has nodes and edges. Edges may be directed or undirected.

Deadlock analysis uses graphs. A node can represent a process or a
resource, and an edge can represent ownership or waiting. A cycle in a
resource graph is a warning sign for deadlock. This example is written in
C++.

.. code-block:: c++

   std::map<std::string, std::vector<std::string>> graph;
   graph["thread1"].push_back("lockA");
   graph["lockA"].push_back("thread2");

The graph representation is simple. The hard part is deciding what each
node and edge means in the system being modeled.

Trees
-----

A tree is a graph with a root and no cycles. Trees show up throughout
operating systems.

Directory hierarchies are trees or tree-like graphs. Multi-level page
tables are trees. Search trees and indexed structures appear in file
systems, caches, and memory managers.

Counting and Interleavings
--------------------------

Counting is useful when reasoning about how many orders of execution are
possible.

With concurrency, the number of interleavings grows quickly. Even a
short critical section can have many possible instruction orderings when
two threads run at the same time. This is why systems code uses locks,
atomic operations, and careful ordering rules.

Basic Complexity Thinking
-------------------------

Complexity thinking asks how time and space costs grow as input size
grows.

This matters for schedulers, allocators, page replacement algorithms,
filesystem lookup, and cache design. A policy that is acceptable for ten
items may be too expensive for one million items.

Computer Hardware Overview
--------------------------

Computer systems combine CPU, memory, I/O devices, and software.

The operating system manages these resources and gives programs a safer,
more regular interface. Programs use system calls and libraries rather
than directly controlling every hardware device.

Memory Hierarchy
----------------

The memory hierarchy moves from small and fast storage to large and slow
storage.

Registers are fastest. Caches are next. Physical memory is slower.
Secondary storage and swap are much slower. Operating systems try to
make good use of this hierarchy through caching, paging, and locality.

Machine-Level Data
------------------

At the machine level, data is stored as bytes and words at addresses.

C exposes this model more directly than many higher-level languages.
Pointers hold addresses. Structs describe memory layouts. Arrays are
contiguous regions. These ideas are necessary for understanding process
memory, page tables, and system call interfaces.

Instruction Execution and Assembly
----------------------------------

Assembly language shows the machine-facing view of execution. It uses
registers, instructions, comparisons, branches, and memory operations.
This example is written in ARM assembly.

.. code-block:: asm

   MOV R0, #1
   MOV R1, #1
   CMP R0, R1
   BEQ equivalent

The comparison sets condition flags, and the branch uses those flags.
This is the low-level version of an ``if`` statement.

Bitwise Assembly
----------------

Assembly also makes bit operations explicit. This example is written in
ARM assembly.

.. code-block:: asm

   MOV R0, #1
   MOV R1, #0
   EOR R2, R0, R1

   AND R3, R0, R1
   MVN R3, R3

``EOR`` computes XOR. ``AND`` computes bitwise AND. ``MVN`` moves the
bitwise NOT of a value. These operations are the same ideas used in
flags, masks, and parity.

C as a Systems Language
-----------------------

C is central to systems programming because it exposes memory, pointers,
layout, and system interfaces directly. This example is written in C.

.. code-block:: c

   struct process {
       int pid;
       int state;
       void *stack;
   };

   struct process p;
   p.pid = 100;
   p.stack = NULL;

C++ adds abstraction while preserving low-level control. Rust is designed
in part as a better C, and it remains essential in many high-performance
domains. The experience of using C++ can vary across platforms,
compilers, libraries, and project conventions. Knowing C well also helps
when learning C++, because many of C++'s lower-level abstractions build
directly on ideas from C.

Modern practice is increasingly interested in newer systems languages
such as Rust and Go. Both are promising and already see significant use,
especially in systems and software engineering tools. Rust and Go are
unlikely to replace C everywhere, especially in embedded systems,
microcontrollers, accelerators, device drivers, firmware, and other
specialized hardware contexts.

There is still value in the clarity of C's programming model. With
discipline, careful testing, and tools such as Valgrind, it is possible
to write safer C code. Even when a modern language provides an unsafe
mode or a foreign-function interface, you still need to understand the
lower-level programming abstractions that C exposes directly: pointers,
memory layout, calling conventions, and sometimes assembly language. For
this book, C remains the main language because it exposes the operating
system concepts with the least abstraction.

User Mode, Kernel Mode, and System Calls
----------------------------------------

User mode is where ordinary programs run. Kernel mode is where the
operating system can execute privileged operations.

A system call is a controlled entry from user mode into kernel mode.
Programs use system calls to create processes, open files, allocate
memory mappings, communicate with devices, and wait for events.

Files, Processes, and Interfaces
--------------------------------

Operating systems expose many resources through interfaces that look
like files, processes, memory regions, and synchronization objects.

This course builds on those interfaces. The early chapters start with
processes and files, then move to scheduling, synchronization, memory,
IPC, storage, and filesystems.

What to Review Before Continuing
--------------------------------

Before continuing, students should be comfortable with Boolean logic,
bitwise operations, binary and hexadecimal notation, pointers, arrays,
structs, basic assembly, graphs, trees, and the idea that a system call
crosses from user mode into the kernel.
