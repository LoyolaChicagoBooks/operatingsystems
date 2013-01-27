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


Semaphores - some history
-------------------------
- The concept of a semaphore pre-dates the computing era.
- The concept of semaphores was first introduced in the railroad industry. You can still see these used on the Chicago CTA or Metra
- The most recent CTA derailment on the Green Line involved a semaphore
- Many software locking techniques owe their inspiration from rail roads and industrial automation


Semaphores
----------
- Semaphores support N-many threads entering a critical section at one time.
- Semaphores are generalizations of spin locks
	- Spin locks have two states: 0 - unlocked, 1 - locked
	- Semaphores have N states: 0 - locked, 1-N - unlocked
- Useful for protecting queue like data structures (more later)
- Useful for protecting N-reader M-writer data structures (more later)
- Semaphores have different operations than spin locks:
	- Spin locks have lock, unlock
	- Semaphores have:
		- up() - increases value of semaphore
		- down() - decreases value of semaphore - blocks while it is zero


Semaphores - Implementation
---------------------------
- Semaphores can be implemented on top of spin locksthat protect a count variable

::

	int lock = 0;
	int semaphore = 0;

	void up() {
		spin_lock(&lock);
		semaphore += 1;
		spin_unlock(&lock);
	}

	void down() {
		spin_lock(&lock);
		while(_count == 0) {
			spin_unlock(&lock);
			spin_lock(&lock);
		}
		_count -= 1;
		spin_unlock(&lock);
	}


Binary semaphores
-----------------
- Emulate mutexes
- Initial state is 1.
- Possible states are 1: unlocked, 0: locked
- Example:

::

	Semaphore *mutex = new Semaphore(1); 
	int count = 0;
	void increment(int n) {
		for(int i = 0; i < n; i++) {
			mutex->down();
			count += 1;
			mutex->up();
		}
	}


Event semaphores
----------------
- Used for signaling events.
- Initial value is 0
- Possible states are: 0 - unsignaled. 1 - signaled
- Example: 

::

	Semaphore *event = new Semaphore(0);
	Semaphore *done = new Semaphore(0); 
	int a = 0, b = 0, total = 0;
	void producer() {
		start(consumer);
		a = 50;
		b = 50;
		event->up();
		done->down();
		printf("total: %d\n", total);
	}

	void consumer() {
		event->down();
		total = a + b;
		done->up(); 
	}


- producer starts consumer
- producer sets up values
- producer signals consumer and waits
- consumer receives signal
- consumer performs operation
- consumer signals producer
- producer wakes back up and prints result


Semaphores - Implementation
---------------------------
- a good use case for semaphores is a producer / consumer problem:

::

	Queue *queue = new Queue(5); 
	void producer() {
		while(1) {
			int item = produce_item();
			queue->Enqueue(item);
		}
	}

	void consumer() {
		while(1) {
			int item = queue->Dequeue();
			consume_item(item);
		}
	}

- a class of problems exists here:
- how does the consumer know an item is available in the queue?
- how does the producer know that the queue isn't full (5 items)?
- how do we guarantee access to the queue is atomic - that queue and dequeue operations don't interfere with each other? 


Semaphores - Implementation
---------------------------
- Bounded buffer example
	- mutex is a binary semaphore (states of 0 - locked, 1 - unlocked)
	- empty protects the condition of the queue being empty
	- full protects the condition of the queue being full

::

	Queue *queue = new Queue(5);
	Semaphore *mutex = new Semaphore(1);
	Semaphore *empty = new Semaphore(5);
	Semaphore *full = new Semaphore(0);

	void producer() {
		while(1) {
			empty->down();
			mutex->down(); 
			int item = produce_item();
			queue->Enqueue(item);
			mutex->up();
			full->up();
		}
	}

	void consumer() {
		while(1) {
			full->down();
			mutex->down();
			int item = queue->Dequeue();
			consume_item(item);
			mutex->up();
			empty->up();
		}
	}


Semaphores - reader / writer locks
----------------------------------
- In some cases, it is desirable to allow N-readers and M-writers on a shared resource.
- The most typical case is N-readers and 1-writer. In this case, as many threads can read a value or 1 thread may write to the value at any time
- Using reader-writer locks can allow for much higher concurrency than the use of binary semaphores or mutexes. 
- Reader / writer Lock Implementation

::

	Semaphore *read = new Semaphore(0);
	Semaphore *write = new Semaphore(0);
	Semaphore *lock = new Semaphore(1);
	int readWait = 0, writeWait = 0;
	int activeRead = 0, activeWrite = 0;

	void StartWrite() {
		lock->down();
		if(activeWrite+activeRead+writeWait == 0) {
			write->up();
			activeWrite += 1; 
		} else {
			writeWait += 1;
		}
		lock->up();
		write->down();
	}

	void EndWrite() {
		lock->down();
		activeWrite -= 1;
		if(writeWait > 0) {
			write->up();
			activeWrite += 1;
			writeWait -= 1;
		} else {
			while(readWait>0) {
				read->up();
				activeRead += 1;
				readWait -= 1;
			}
		}
		lock->up();
	}

	void BeginRead() {
		lock->down();
		if(activeWrite+writeWait == 0) {
			read->up();
			activeRead++;
		} else {
			readWait += 1;
		}
		lock->up();
		read->down();
	}

	void EndRead() {
		lock->down();
		activeRead -= 1;
		if(activeRead == 0 && writeWait > 0)) {
			write->up();
			activeWrite += 1;
			writeWait -= 1;
		}
		lock->up();
	}


Semaphores - reader/writer locks
--------------------------------
- As you can see from the previous slide, the implementation of a reader/writer lock can be complex.
- Implementation details will vary depending on the conditions of the lock.
- Some implementation considerations
	- Who gets higher priority: readers or writers?
	- Do we allow users to downgrade or upgrade locks? i.e. can a read lock be translated to a write lock and can a write lock be translated to a read lock?
- reader / writer locks can have some obscure and complex starvation / deadlock scenarios (more later)


Semaphores - pthreads
---------------------
- int sem_init(sem_t *sem, int pshared, unsigned int value)
- sem is the semaphore
- pshared - where the semaphore can be shard:
	- 0 - threads of a process - must be located in the heap 
	- not 0 - between processes - must be located in shared memory
- value - the initial value of a semaphore. Typical cases include:
	- 0 used for event semaphores
	- 1 used for a binary semaphore
	- N used for a normal semaphore

::

	int main(int argc, char* argv[]) {
		sem_t sema;
		sem_init(&sema, 0, 1);
		sem_wait(&sema);
		//critical section
		sem_post(&sema);
	}


Windows - Semaphores
--------------------
- Windows also provides an efficient semaphore implementation.
- Windows shares semaphores through names. The 4th paramenter in CreateSemaphore accepts a name. If you want to use an exising named semaphore, you call OpenSemaphore.

::

	int main(int argc, char* argv[]) {
		const int minCount = 0;
		const int maxCount = 20;
		HANDLE sema = CreateSemaphore(NULL, minCount, maxCount, NULL);

		WaitForSingleObject(sema, INFINITE);

		//critical section

		ReleaseSemaphore(sema, 1, NULL);
	}


Monitors / Condition Variables
------------------------------
- Monitors are a higher level locking abstraction
- Monitors have 4 operations:
	- enter -lock the monitor
	- exit - unlock the monitor
	- wait - release the lock and put the thread to sleep. when wait() returns, automatically obtain the lock again
	- pulse - tell other threads to wake up and re-obtain the monitor lock
- Monitors permit much more complex locking conditions with simpler code than is possible with spin locks and semaphores in many cases


Monitors
--------
- Monitor locks typically follow the following pattern:
	#. enter()
	#. while( condition to proceed is not true ) { wait(); }
	#. do operation now that condition is true
	#. if( condition has changed ) { pulse(); }
	#. exit();

- At the beginning of a critical section, enter() is called, at the end, exit() is called
- After the call to enter() and just before the critical section, a while loop checks that it is okay to proceed while calling wait() on each iteration
- After the while loop ends, we enter the critical section
- If after the critical section, the condition for the critical section changes, call pulse()
- after a possible call to pulse(), call exit() to end the lock and exit the critical section
- The conditions in the while loop of the monitor lock can be very simple to very complex.
- Even if the conditions become very complex, the operations on the monitor remain simple
- The danger with complex conditions is that you need to make sure that all threads in their while loops will at some point break out of the while loop. Otherwise your -program will freeze up.
- The performance for monitors is typically very good. spin locks and semaphores will outperform monitors in some cases


Monitor - bounded buffer
------------------------
- Semaphore implementation:

::

	Queue *queue = new Queue(5);
	Semaphore *mutex = new Semaphore(1);
	Semaphore *empty = new Semaphore(5);
	Semaphore *full = new Semaphore(0);

	void producer() {
		while(1) {
			empty->down();
			mutex->down(); 
			int item = produce_item();
			queue->Enqueue(item);
			mutex->up();
			full->up();
		}
	}

	void consumer() {
		while(1) {
			full->down();
			mutex->down();
			int item = queue->Dequeue();
			consume_item(item);
			mutex->up();
			empty->up();
		}
	}


Monitor - bounded buffer
------------------------

::

	void consumer() {
		while(1) {
			monitor->Enter();
			while(queue->Size() == 0) {
				monitor->Wait();
			}
			int item = queue->Dequeue();
			consume_item(item);
			monitor->Pulse();
			monitor->Exit();
		}
	}

	Queue *queue = new Queue(5);
	Monitor *monitor = new Monitor();
	
	void producer() {
		while(1) {
			monitor->Enter();
			while(queue->Size() >= 5) {
				monitor->Wait();
			}
			int item = produce_item();
			queue->Enqueue(item);
			monitor->Pulse();
			monitor->Exit();
		}
	}


- In this example, we call pulse() before every exit. We do this because the size of the queue is changed in the critical section and may allow the other thread to operate.


Semaphore - reader/writer lock
------------------------------

::

	void StartWrite() {
		lock->down();
		if(activeWrite+activeRead+writeWait == 0) {
			write->up();
			activeWrite += 1; 
		}  else {
			writeWait += 1;
		}
		lock->up();
		write->down();
	}

	void EndWrite() {
		lock->down();
		activeWrite -= 1;
		if(writeWait > 0) {
			write->up();
			activeWrite += 1;
			writeWait -= 1;
		} else {
			while(readWait>0) {
				read->up();
				activeRead += 1;
				readWait -= 1;
			}
		}
		lock->up();
	}

	Semaphore *read = new Semaphore(0);
	Semaphore *write = new Semaphore(0);
	Semaphore *lock = new Semaphore(1);
	int readWait = 0, writeWait = 0;
	int activeRead = 0, activeWrite = 0;

	void BeginRead() {
		lock->down();
		if(activeWrite+writeWait == 0) {
			read->up();
			activeRead++;
		} else {
			readWait += 1;
		}
		lock->up();
		read->down();
	}

	void EndRead() {
		lock->down();
		activeRead -= 1;
		if(activeRead == 0 && writeWait > 0)) {
			write->up();
			activeWrite += 1;
			writeWait -= 1;
		}
		lock->up();
	}


Monitors - Reader / Writer locks
--------------------------------
- Monitor implementation is much simpler!

::

	Monitor *monitor = new Monitor();
	int readWait = 0, writeWait = 0;
	int activeRead = 0, activeWrite = 0;

	void BeginRead() {
		monitor->enter();
		readWait += 1;
		while(activeWrite+writeWait > 0) {
			monitor->wait();
		}
		readWait -= 1;
		activeRead += 1;
		monitor->pulse();
		monitor->exit();
	}

	void EndRead() {
		monitor->enter();
		activeRead -= 1;
		monitor->pulse();
		monitor->exit();
	}

	void StartWrite() {
		monitor->enter();
		writeWait += 1;
		while(activeWrite+activeRead > 0) {
			monitor->wait();
		}
		activeWrite += 1;
		monitor->pulse();
		monitor->exit();
	}

	void EndWrite() {
		monitor->enter();
		activeWrite -= 1;
		monitor->pulse();
		monitor->exit();
	}


Pthreads - Condition Variables
------------------------------
- pthreads uses condition variables to implement monitor like functions.
- to use them, you will need a mutex and a condition variable:

::

	int main(int argc, char* argv[]) {
		pthread_mutex_t mutex;
		pthread_cond_t cond;
		pthread_mutex_init(&mutex);
		pthread_cond_init(&cond);

		pthread_mutex_lock(&mutex);
		while(/* condition is not true */ ) {
			pthread_cond_wait(&cond, &mutex);
		}
		//critical section
		pthread_cond_signal(&cond);
		pthread_mutex_unlock(&mutex);
	}


Windows - Condition Variables
-----------------------------
- Available in Windows Vista, 7, and Server 2008
- Condition variables use CRITICAL_SECTION like pthread's condition variables use a mutex. In this case the object is a spin lock and not a mutex.

::

	int main(int argc, char* argv[]) {
		CONDITION_VARIABLE cond;
		CRITICAL_SECTION lock;

		InitializeConditionVariable(&cond);
		InitializeCriticalSection(&lock);

		EnterCriticalSection(&lock);
		while(/*condition is not true*/) {
			SleepConditionVariableCS(&cond, &lock, INFINITE);
		}
		//critical section
		WakeConditionVariable(&cond);
		LeaveCriticalSection(&lock);
	}


