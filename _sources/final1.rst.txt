:orphan:

Final Exam Prep
===============


Introduction
------------

- Following are a set of questions that may or may not show up in a similar form on the exam
- Feel free to discuss these on the Piazza site
- The exam will be closed-book, closed-notes, closed-computer and held in class.

How to do well on the exam
--------------------------

- The exam will be on April 30th. Spend a small amount of time each week answering the questions below. A smaller, but sustained effort will help you absorb the material better and keep stress to a minimum.
- Ask questions on Piazza! Share and discuss answers!


Topics Covered
--------------

- Virtual Memory
- Storage and Devices
- Implementing Files and Folders


Likely Extra-Credit Topics
--------------------------

- User-Mode memory management
- Midterm lecture material


Questions - Inter-process Communication
---------------------------------------

#. List the following forms of IPC from faster performance to slower performance: files, named pipes and memory mapped files

#. What guarantees does a named-pipe give you? 

#. What guarantee do I lose if I send too much data at once over a named-pipe?

#. Is it safe to use pointers in memory mapped regions? Why?

#. Re-write these structs so that they’ll work properly with memory mapped files:


.. literalinclude:: ipc/code/absolute.c
   :language: c
   :linenos:


#. Why is it dangerous to call functions like malloc() or free() inside a signal handler? What methods should you limit yourself to?


Questions - Virtual Memory
--------------------------

#. What is Virtual Memory responsible for? 

#. How does Virtual Memory help a system to perform better?

#. What is a physical address? What is a virtual address?

#. What role does the MMU serve?

#. What role does the page table serve?

#. How does a two or three level page table save memory? What kind of data structure is it?

#. How does a page fault occur?

#. What are the four classifications of pages as used by a page replacement algorithm?

#. Which of the two algorithms is subject to Belady's Anomaly - NFU or FIFO

#. If a page replacement algorithm is not subject to Belady's Anomoly, what does this imply when more physical memory is added to a system?

#. If I have the following distance string - 4, 4, 1, inf., 7, 2, 2. What is the page fault rate for a physical memory size of 2? For 5? (See 8.35 in the notes)

#. What is a working set?

#. What is the difference between local and global paging?

#. Why does DMA need its pages to be locked?

#. Explain how COW (Copy on Write) works when a program calls fork() and then writes to a data structure.

#. List the steps in the process of a page fault (8.51, 52)


Questions - Storage and Devices
-------------------------------

#. Is a hard disk a character or a block device?

#. What is RAID 0?

#. What is RAID 1?

#. What is RAID 5?

#. Explain how to calculate the parity of two bytes into a third byte.

#. What does parity have to do with RAID 5?

#. What is the policy of the Shortest Seek First disk scheduling algorithm?

#. What is the down side of Shortest Seek First?

#. What is the policy of the Elevator Algorithm?

#. Which part of the disk does the Elevator Algorithm favor in terms of average response time?

#. How does FSCAN modify Shortest Seek First to get rid of its disadvantage?


Questions - Implementing Files and Folders
------------------------------------------

#. Linked lists are used to map files onto some sequence of blocks in some filesystem implementations. What are the advantages / disadvantages of this?

#. What change does FAT make from the linked list approach?

#. What are the advantages to FAT? What are the disadvantages?

#. Explain what an inode is

#. Why are several pointers (zones) to the first few data blocks stored in the first inode of the Minix inode structure? What benefit does this give?

#. Explain what an indirect and double indirect zone are. Draw a diagram.

#. Why are block caches needed? What do they help?

#. What responsibilities does a VFS (Virtual Filesystem) manage?

#. What is FUSE? How does VFS enable it?


 



