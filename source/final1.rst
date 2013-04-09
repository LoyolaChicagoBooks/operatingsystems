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


