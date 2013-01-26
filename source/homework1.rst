Homework 1
==========

Summary
-------
The purpose of this assignment is to learn to work with Linux / Minix and familiarize yourself with basic data structures, string manipulation/parsing, etc. We will be writing a program that takes the output of the "ps" command and presents the list of processes as a proper tree. It will be left to your imagination to decide how to best present the tree of processes, but a simple indentation scheme should be sufficient.

The first part of this assignment will be to get this program working on Linux. This means:
 #. You will need access to an existing Linux machine or you will need to setup Linux in a virtual machine.
 #. Make sure you know how to edit, compile, and run C programs on this platform. This will require you learn a simple editor (e.g. nano), the C/C++ compiler (gcc/g++), and a debugger (gdb)
 #. Make sure you know how to setup version control for this and all future assignments.

The second part of this assignment will be to port the program from Linux to Minix. This means:
 #. You will need to setup a Minix virtual machine. See the first lecture's notes for reference.
 #. You will need to get a copy of your code running on Minix. Minix has a slightly different output format for the ps command and a slightly different process tree structure. You will have to make your program flexible enough to handle this.

Details
-------
Your job in this assignment will be:
 #. You must use C or C++ on Linux and Minix for this project. 
 #. A Makefile to build your project.
 #. A README.txt file describing how to run your program.
 #. Use the popen() call to establish a pipe to the "ps" command. We'll use this to capture a snapshot of the list of currently running processes on your system. You will need to figure out how the options of the PID and PPID in Minix. For example, on Linux ps, ps -ef shows the PID and PPID. On Minix, it's slightly different (of course), so you'll want to read the Minix man page for ps.
 #. A big part of the job is to learn how "ps" works (read man page!) and figure out what all of the columns represent.
 #. Write a program that builds a process tree.
     - Read one line of input at a time but exclude the header line. You will need to use strtok() or write your own parser to extract data from the input
     - Extract the PID (process ID) and PPID (parent process ID) from each line
     - Create a process info "node" and insert it into the tree, such that the new "node" is linked to the strtok()'d parent "node".
     - Once all of the lines of "ps" have been consumed, print the tree. To keep this simple, you may use indentation to indicate that a process is the parent of another process.
 #. You may submit your assignment using Google Code, BitBucket, or GitHub. It is ok to use a public repository.

Tentative Due Date
------------------
Sunday, February 17th at midnight. As that date approaches I will check to see how everyone is doing and adjust the date if necessary. Start working on this project right away! 

Deliverables
------------
You will need to setup a public source control repository either though Google Code, BitBucket, or GitHub. You are welcome to use either Subversion, Mercurial, or Git. Subversion and Git work well in both Linux and Minix. Currently, there is no working version of Mercurial in Minix. If you choose to use Mercurial, you will have to find a way to transfer files into your Minix VM. 

Before the due date, you will need to e-mail your source repository URL to joekaylor@gmail.com. If you choose to use a private repository, you will need to add access for me to get to your code. I have accounts on all three services with the username joekaylor or joekaylor@gmail.com. When grading, I will pull the source code from your repository as of the latest version prior to the deadline.

How You Will be Graded
----------------------
 #. 1pt  - Does your program compile in Linux and Minix (Makefile) ?
 #. 1pt  - README.txt
 #. 10pt - Does your program print an accurate tree-like structure in Linux?
 #. 10pt - Does your program print an accurate tree-like structure in Minix?
 #. 4pt  - Is your code readable?
		- No extremely long functions (I shouldn't need to scroll up or down to read your function)
		- Use of indentation
		- Good choices of function and variable names (no strict rules, just be readable)
 #. 4pt  - Does your program have some degree of unit test coverage?

Notes
-----
You are allowed to talk to others as you work on this. Keep in mind, however, that you will need to be able to answer questions about what you learned in this assignment -- and all future assignments -- in the exams. So please make sure you spend time thinking about and doing the assignment on your own.


