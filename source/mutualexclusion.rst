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


Types of Pessimistic Locks
--------------------------
- Disabling Interrupts
- Strict Alternation
- Spin locks / mutexes
- Semaphores
- Condition variables / monitors
- Reader / writer locks

Types of Optimistic Locks
-------------------------

- Most non-database implementations make use of some kind of software transactional memory library

When to Consider Optimistic Locking
-----------------------------------
- If your data structure is sparse. 
	- Example: you're reading from and writing to a few values out of a 1-GB array.
- The number of likely serial equivalent permutations of your code is high relative to the non serial.
- The cost of communication is high relative to the cost of re-trying execution
	- communication over networks
	- communication over NUMA systems with high CPU counts
	- when you have very high CPU counts

Disabling Interrupts
--------------------
- This is the simplest solution to making critical sections atomic.
- Disabling interrupts prevents the scheduler from being invoked and therefore any code executed between disabling and enabling interrupts will be atomic.
- Disabling interrupts is undesirable because:
	- It is the least optimal solution in terms of resource utilization
	- Disabling interrupts is a privileged operation (only the operating system may do this)
	- If a program crashes while interrupts are disabled, interrupts will not be restored, the scheduler will not be invoked, and the entire computer will be essentially stopped.
	- In most architectures, disabling interrupts only happens on one CPU.
- Disabling interrupts is useful in operating system kernels to achieve mutual exclusion

Strict Alternation
------------------
- One or more processes take turns entering a critical section. This is controlled either by an external scheduler or by the use of a 'turn' variable.
- In general this is a very bad solution: basically, one process can effectively lock the other by being slow in its non critical section of code.

::

	while(TRUE) {
		while(turn != 0) {}
		critical_section();
		turn = 1;
		non_critical_section();
	}

::

	while(TRUE) {
		while(turn != 1) {}
		critical_section();
		turn = 0;
		non_critical_section();
	}


Spin Locks
----------
- Spin locks allow at most one owner of the lock. If a spin lock is agreed upon for a critical section of code, then locking a spin lock before entering and unlocking after exiting will make the critical section atomic.

::

	int lock = 0;
	void increment(int n) {
		for(int i = 0; i < n; i++) {
			spin_lock(&lock);
			int x = count;
			x = x + 1;
			count = x;
			spin_unlock(&lock);
		}
	}


- This critical section is now atomic.
- What are the possible orders of execution now?


Spin Locks - Implementation
---------------------------
- Spin locks have two states: locked and not-locked.
- The following is a pseudo algorithm for spin_lock and spin_unlock

::

	spin_lock(int* lock) {
		while(*lock == 1) { }
		*lock = 1;
	}

	spin_unlock(int* lock) {
		*lock = 0;
	}



- while lock == 1, someone else holds it. once it switches to 0, it is unlocked, we set it to 1 and we've acquired the lock
- the while loop is where the spin lock get's its "spin" name
- to unlock, we just set lock back to 0
- what is the critical section in spin_lock()?
- will the pseudo implementation work? why? or why not?


Spin Locks - Implementation
---------------------------
- The pseudo implementation will not work. Its critical section is the entire method. 
- For example, if two threads are in spin_lock, when lock goes to 0, both threads may break out of the while loop and both set lock to 1. Then both threads would acquire the lock.
- So, what we need to do, is to collapse the while loop into a single atomic operation.
- This atomic operation will have to both test the value of lock and set it to 1 if it is ever zero

::

	spin_lock(int* lock) {
		while(test_and_set(lock) == 1){}
	}


Spin Locks - Implementation
---------------------------
- So, how do we implement test_and_set? 
	- There is no C/C++ keyword for this. 
	- We have no guarantee that any C/C++ statement will be reduced to one atomic instruction.
- For this, we need to rely on a specialized machine instruction. On the x86, this is the xchg instruction.
- x86 assembly version of spin_lock:

::

	lock: 
		dd    0
	spin_lock:
		mov   eax, 1
	loop:
		xchg  eax, [lock]               
		test  eax, eax                 
		jnz   loop                        
		ret

- with the xchg instruction, our test and set operation is atomic


Spin Locks - Implementation
---------------------------

::

	unsigned long test_and_set(unsigned long* lock) {
		unsigned long newval = 0;
		asm volatile("lock: cmpxchgl %2, %0"
			: "+m" (*lock), "+a" (newval)
			: "r" (1)
			: "cc");
		return newval;
	}

	void spin_lock(unsigned long *lock) {
		while(test_and_setlock) == 1) {}
	}

	void spin_unlock(unsigned long *lock) {
		*lock = 0;
	}


Pthreads - Mutex
----------------
- The pthreads library in Linux and Minix offer an efficient implementation of mutexes.
- Rather than wasting CPU time by 'spinning', the thread is put to sleep if it cannot obtain the lock immediately.

::

	int main(int argc, char* argv[]) {
		pthread_mutex_t mutex;
		pthread_mutex_init (&mutex, NULL);

		pthread_mutex_lock(&mutex);

		//critical section

		pthread_mutex_unlock(&mutex);

	}


Mutexes in Windows
------------------
- Windows also provides an efficient implementation for mutexes.
- WaitForSingleObject locks mutexes (along with other lock primitives)
- ReleaseMutex unlocks the mutex

::

	int main(int argc, char* argv[]) {
		HANDLE mutex = CreateMutex(NULL, FALSE, NULL);
    
		WaitForSingleObject(mutex, INFINITE);

		//critical section

		ReleaseMutex(mutex);

		CloseHandle(mutex);
	}



