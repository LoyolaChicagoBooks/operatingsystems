About the Book
=====================

This book grew out of an effort that began nearly fifteen years ago with
George K. Thiruvathukal, the lead editor, students in his research
laboratory, and faculty colleagues.

Dr. Thiruvathukal has been teaching operating systems and distributed
systems since the early 1990s, when he taught Operating System Design and
Implementation at the Illinois Institute of Technology while completing
his PhD.

The motivation for the book is practical. Many operating systems
textbooks are encyclopedic. They cover a large number of topics, but they
often spend less time on the implementation ideas that students need in
order to understand how operating systems are actually built. This book
focuses on the ideas that generally made it into modern operating-system
implementation, backed by coding examples that we hope actually work.

The book is designed as a companion for instructors who want to give
students a concise introduction to operating systems and the practice of
learning C programming without requiring an expensive textbook. C is not
the only systems language, and modern practice is increasingly interested
in Rust, Go, and other options. We introduce those ideas gradually where
they fit. Still, C remains useful because it exposes pointers, memory
layout, calling conventions, system calls, and hardware-facing details
with relatively little abstraction. In that sense, it can be thought of
as a portable assembly language. We also expect C to remain important in
embedded systems, accelerators, firmware, device drivers, and hardware
contexts where direct control still matters.

Approach
----------

The chapters are meant to be read alongside code. The goal is not to
catalog every operating-system feature. The goal is to explain the core
ideas clearly enough that students can connect the concept, the system
call or data structure, and the behavior they observe when running a
program.

Most examples are written in C, with C++ used where it does not distract
from the systems idea. Some examples and case studies use other
languages, but the main programming model is deliberately close to the
machine.

Book Organization
------------------

Introduction
^^^^^^^^^^^^

The :doc:`Introduction <introduction>` chapter gives historical context.
It traces operating systems from early batch systems through UNIX,
personal computers, and modern systems.

Systems and Discrete Math Recap
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

The :doc:`Systems and Discrete Math Recap <systems-recap>` chapter
reviews background material students are expected to know before the main
operating-systems chapters. It includes Boolean logic, binary numbers,
bit operations, memory layout, pointers, and small C examples.

Introduction to Processes
^^^^^^^^^^^^^^^^^^^^^^^^^

The :doc:`Introduction to Processes <processes>` chapter explains
processes, address spaces, process creation, ``fork()``, ``exec()``, and
related process-control ideas.

Files and I/O
^^^^^^^^^^^^^

The :doc:`Files and I/O <files-io>` chapter introduces the UNIX file
abstraction. It explains file descriptors, regular files, directories,
devices, pipes, sockets, and the common operations used to work with
them.

Process/Thread Scheduling
^^^^^^^^^^^^^^^^^^^^^^^^^

The :doc:`Process/Thread Scheduling <scheduling>` chapter explains how an
operating system decides what runs next. It covers user mode, kernel
mode, interrupts, context switching, scheduling goals, and scheduling
policies.

Mutual Exclusion
^^^^^^^^^^^^^^^^

The :doc:`Mutual Exclusion <mutualexclusion>` chapter introduces shared
state, critical sections, atomicity, locks, semaphores, monitors, and
other synchronization mechanisms used to make concurrent programs
correct.

Deadlock
^^^^^^^^

The :doc:`Deadlock <deadlock>` chapter explains how threads can become
stuck waiting for one another. It covers the dining philosophers problem,
lock graphs, deadlock conditions, and practical ways to avoid deadlock.

IPC Topics
^^^^^^^^^^

The :doc:`IPC Topics <ipc>` chapter covers interprocess communication.
It compares pipes, named pipes, signals, shared memory, memory-mapped
files, UNIX domain sockets, Solaris doors, and network sockets.

Virtual Memory
^^^^^^^^^^^^^^

The :doc:`Virtual Memory <kernelmm>` chapter explains the kernel side of
memory management. It covers address spaces, MMUs, page tables, page
replacement, demand paging, and case studies drawn from kernel memory
systems.

Userland Memory Management
^^^^^^^^^^^^^^^^^^^^^^^^^^

The :doc:`Userland Memory Management <userlandmm>` chapter explains how
language runtimes and libraries manage memory inside a process. It
covers heaps, allocation, garbage collection, smart pointers, reference
counting, and stack allocation.

Storage and Devices
^^^^^^^^^^^^^^^^^^^

The :doc:`Storage and Devices <storage>` chapter introduces storage
hardware and the operating-system policies used to manage it. It covers
device interfaces, buses, disks, flash storage, disk scheduling, RAID,
and storage services.

Implementing Files and Folders
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

The :doc:`Implementing Files and Folders <fs>` chapter explains how
filesystems represent files, directories, metadata, free space, and
paths. It also includes case studies of filesystem designs and research
systems.

Book Index
^^^^^^^^^^

The :doc:`Book Index <bookindex>` provides index entries for important
terms, system calls, abstractions, and examples used throughout the book.

Key Download Links
-------------------

This book automatically builds and deploys using GitHub Actions (a continuous integration system).
All commits trigger a rebuild of the book and, if successful, a deployment to GitHub pages.

Book Web Site
   `os.cs.luc.edu <https://os.cs.luc.edu>`__

Book PDF (Latest Release)
   `github.com/LoyolaChicagoBooks/operatingsystems/releases/latest/download/OperatingSystems.pdf <https://github.com/LoyolaChicagoBooks/operatingsystems/releases/latest/download/OperatingSystems.pdf>`__

Book Source Code
   `github.com/LoyolaChicagoBooks/operatingsystems <https://github.com/LoyolaChicagoBooks/operatingsystems>`__


Book Info
----------

This book is hosted on GitHub Pages.
It is written using the delightful Python Sphinx documentation tool.
In addition, we use the Sphinx Book Theme.

You can checkout the book from the Book Source Code and build it yourself.
This could be useful if you wish to help us correct bugs/errors by issuing a pull request.
While we welcome pull requests, we ask that you make sure your request can build successfully, especially if it requires changes to the book and any of the accompanying repositories.

Book Examples
--------------

Although we write most examples in 100% pure C code, there are occasions where we mix C and C++, especially if the C++ code does not get in the way of the systems pedagogy.
That said, we also have some *emerging* examples in Go, Java, C#, Python, and Scala, since we are constantly trying to find a way to apply high-level langauges and software engineering ideas to systems programming.
While the most promising we have found so far is Go, we don't think it is going to fully displace C/C++ for the foreseeable future.
Nevertheless, early testing has shown some good performance!
The future of systems remains bright!

C/C++ Examples
   `github.com/SoftwareSystemsLaboratory/systems-code-examples <https://github.com/SoftwareSystemsLaboratory/systems-code-examples>`__

Java Examples
   `github.com/LoyolaChicagoCode/hpjpc-source-java <https://github.com/LoyolaChicagoCode/hpjpc-source-java>`__

Java Multicast Example
   `github.com/gkthiruvathukal/multicast-java <https://github.com/gkthiruvathukal/multicast-java>`__


Contact
--------

If you have any questions, please contact gkt@cs.luc.edu. George K. Thiruvathukal acts as the maintainer and lead author of this book.
