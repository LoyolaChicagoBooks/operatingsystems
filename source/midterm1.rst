Midterm Exam Prep
=================

Introduction
------------

- Following are a set of questions that may or may not show up in a similar form on the midterm exam
- Feel free to discuss these on the Piazza site
- The midterm exam will be closed-book, closed-notes, closed-computer and held in class.

How to do well on the exam
--------------------------

- The exam will be in March. Spend a small amount of time each week answering the questions below. A smaller, but sustained effort will help you absorb the material better and keep stress to a minimum.
- Ask questions on Piazza! Share and discuss answers!
- Become comfortable with your project #1 code!

Questions
---------

 #. How do monolithic-kernel operating systems and micro-kernel operating systems differ?

 #. Describe the role of a modern operating system on a computer. What guarantees are given to programs?

 #. List and describe the memory sections of a process.

 #. What are the steps that the loader takes in loading and launching the program?

 #. What are the steps the loader takes when loading a shared library (.so file)?

 #. Describe what the fork() system call does. Why does the fork() call need to return two different values when used within what seems to be the same program?

 #. What are some ways that a scheduler can make a system get the most possible work done? In other words, what can a scheduler do to achieve the best throughput?

 #. What are some ways that a scheduler can make a system more responsive? In other words, what can a scheduler do to achieve the best latency?

 #. How does a scheduler know if a process is interactive or I/O intensive? How does a scheduler know if a process is CPU intensive or non-interactive?

 #. Give an example of a real-time scheduling scenario that is schedulable and one that is not. Show your math. (Make your life easy by drawing a timeline!)

 #. Assuming one thread is executing the producer() function and one thread is executing the consumer() function, circle the lines that are the critical section.

::

	int queue[10];
	int queueSize = 0;

	void producer() {
	  int item = 0;
	  while(1) {
	    item += 1;
	    if(queueSize < 10) {
	       queue[queueSize] = item;
	       queueSize += 1;
	    }
	  }
	}

	void consumer() {
	   while(1) {
 	    if(queueSize > 0) {
 	       int item = queue[queueSize];
 	       queueSize -= 1;
  	      printf(“item: %d\n”, item);
  	   }
 	  }
	}

