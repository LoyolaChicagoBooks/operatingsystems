Mutual Exclusion
================

Critical Sections
-----------------

::

	int count = 0;
	void increment(int n) {
		for(int i = 0; i < n; i++) {
			int x = count;
			x = x + 1;
			count = x;
		}
	}

	int main(int argc, char* argv[]) {
		pthread_t t1, t2;
		....
		pthread_create(&t1, ..., increment)
		pthread_create(&t2, ..., increment)
	}

- What will happen when more than one thread executes the increment method?
- Which orders can the instructions be executed in?
- What is the expected value of count?


Identifying the Critical Section
--------------------------------

- This is the critical section of the increment method

::

	int x = count;
	x = x + 1;
	count = x;

- The critical section is the section of code that only one thread may enter at one time for the program's result to be definitely correct.
- In some critical sections, it is possible for more than one thread to execute at the same time, but correctness is not guaranteed.


Execution Orders and Atomicity
------------------------------

- Code for the critical section

::

	int x = count;
	x = x + 1;
	count = x;

- Pseudo-assembly for the critical section

::

	load &count, r0
	set r0, r1
	add 1, r1
	set r1, r0
	store &count, r0

- Key points to remember:
	- compilers aren't always going to do what you expect
	- after the statement "count = x", even up until several program statements later, you cannot be 100% guaranteed that the value has been stored to count
	- because of compiler optimizations, the value of count could be stored in a register for much longer than you expect. to deal with these scenarios, 'volatile' keywords will guarantee that stores are inserted earlier
	- don't count on one program statement being translated into one atomic machine instruction. most are not. only single machine instructions can be atomic.


Execution Orders and Atomicity
------------------------------

- What are some possible execution orders for our pseudo-assembly?
- With two threads on two different CPUs

::

	t0: load &count, r0
	t0: set r0, r1
	t1: load &count, r0
	t0: add 1, r1
	t1: set r0, r1
	t0: set r1, r0
	t0: store &count, r0
	t1: add 1, r1 
	t1: set r1, r0
	t1: store &count, r0

- In this case, instead of being 2, count would have a value of 1
- Given the pseudo assembly program and two threads, how many permutations of the program would be equivalent to serial execution of one thread executing after the other in serial?
- To achieve program correctness, we must eliminate all of the permutations that don't have results equivalent to serial execution.

- Solutions differ on:
	- Whether execution re-tries are required
	- The amount of coordination / cooperation required
	- The underlying assumptions of the parallelism of the program (such as user vs. kernel threads).


Characteristics of a Good Locking Solution
------------------------------------------

- No two processes may be simultaneously inside their critical section
- No assumptions may be made about speeds or the number of CPUs
- No processes running outside of its critical section may block other processes
- No process should have to wait forever to enter its critical section.


Achieving Atomicity / Serializability
-------------------------------------

- There are two possible models to consider when writing programs: pessimistic locking and optimistic locking
- Pessimistic locking:
	- Locks on all resources needed for a critical section are obtained, the critical section is executed, then all locks are released
- Optimistic locking:
	- A snapshot of the resource is made or a log of alterations is kept. The critical section is executed, then the snapshot or log are examined to see if the operation was atomic. If it was, then any alterations are committed, otherwise they are abandoned.
- For most frameworks and languages, pessimistic locking is the more used / accepted solution. With higher core counts and RAM sizes, optimistic locks are becoming more popular.

