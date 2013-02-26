Homework 2
==========

Summary
-------
The purpose of this assignment will be to make use of multi-threading, multiple processes, locking mechanisms, and inter-process communication. We will be writing a simple console based chat client that will allow two users to communicate with each other over a console.

The components of the chat client will be the following:
 #. The client's main process thread will read lines of input from STDIN. Messages will be sent to other client(s) using some IPC mechanism.
 #. The client will have a background thread to monitor the IPC mechanism to read incoming messages.
 #. A message encoding scheme that includes: a chat handle, and the message.


Requirements for the implementation
-----------------------------------

#. The assignment must be done with C or C++.
#. The implementation must work on 64-bit Ubuntu 12.04 or 12.10.
#. You must provide a README.txt describing how to run your program.
#. You may not use any networking code (no TCP or UDP).
#. You may use shared memory, memory mapped files, UNIX domain sockets, or FIFOs. Pick your favorite.
#. The implementation must support two clients in the chat session. You may choose to support more if you like.


Due Date
--------
Friday March 29th at midnight.


Deliverables
------------
A link to your source repository e-mailed to joekaylor@gmail.com. You may re-use your repository from Homework 1, but be sure to e-mail the link either way.

How You Will be Graded
----------------------

#. 1pt - Does your program compile? Do you have a Makefile?
#. 1pt - README.txt
#. 10pt - Does your program support two chat clients without crashing / locking up?
#. 10pt - If I write a script to rapidly send messages through your client does it still work?
#. 4pt - Is your code readable
	- No extremely long functions
	- Use of indentation
	- Good choices of function and variable names
#. 4pt - Does your program have some degree of unit test coverage?


Notes
-----

You are allowed to talk to others as you work on this. Keep in mind, however that you will need to be able to answer questions about what you learned in this assignment in the exam. So make sure you spend time thinking about and doing the assignment on your own.






