Mutual Exclusion
================

.. index::
   single: mutual exclusion
   pair: concurrency; mutual exclusion

Mutual exclusion is the requirement that only one thread or process at a
time may execute a particular part of a program. We need mutual exclusion
when shared state can be read and written by more than one execution
context.

Critical Sections
-----------------

.. index::
   single: critical section
   pair: concurrency; critical section

A critical section is a region of code that accesses shared state and
must not be executed concurrently unless the program has been designed to
permit that concurrency.

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

If two threads call ``increment()`` at the same time, both threads read
and write the shared variable ``count``. The expected final value depends
on how many increments are requested, but the actual value can be lower
if the operations interleave.

Identifying the Critical Section
--------------------------------

The critical section in ``increment()`` is the sequence that reads,
modifies, and writes ``count``.

::

   int x = count;
   x = x + 1;
   count = x;

The program only behaves like a serial program if this sequence is
treated as one indivisible operation. If two threads enter this section
together, each thread can observe a stale value and overwrite the
other thread's update.

Execution Orders and Atomicity
------------------------------

.. index::
   single: atomicity
   pair: concurrency; atomicity

Atomicity means that an operation behaves as though it happens all at
once. Most source-level statements are not atomic at the machine level.

::

   int x = count;
   x = x + 1;
   count = x;

One possible pseudo-assembly expansion is:

::

   load &count, r0
   set r0, r1
   add 1, r1
   set r1, r0
   store &count, r0

The compiler and CPU are not required to preserve the simple mental
model of one C statement becoming one machine instruction. Values may
remain in registers longer than expected, instructions may be reordered
within the rules of the language and architecture, and a store may occur
later than the programmer assumes.

Interleavings
-------------

.. index::
   single: interleavings
   pair: concurrency; interleaving
   single: race condition

An interleaving is one possible ordering of instructions from two or
more threads. Some interleavings are equivalent to serial execution, and
some are not.

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

In this interleaving, both threads can read the same old value. Each
thread computes the same new value and stores it, so the final result is
``1`` instead of ``2``. A correct locking design eliminates the
interleavings that are not equivalent to some serial execution.

Characteristics of a Good Locking Solution
------------------------------------------

.. index::
   single: locking; requirements

A good locking solution protects correctness without adding unnecessary
assumptions about scheduling, CPU speed, or the number of processors.

The main requirements are:

- No two threads may be inside the same protected critical section at
  the same time.
- A thread outside the critical section should not prevent another
  thread from entering it.
- The solution should not depend on relative thread speeds.
- A thread should not have to wait forever to enter the critical
  section.

Achieving Atomicity and Serializability
---------------------------------------

.. index::
   single: serializability
   single: pessimistic locking
   single: optimistic locking
   pair: concurrency; serializability

Serializability means that concurrent execution produces a result that
is equivalent to some serial ordering of the same operations.

There are two common models:

- Pessimistic locking obtains the necessary locks before entering the
  critical section, performs the operation, and then releases the locks.
- Optimistic locking performs the operation against a snapshot or log and
  commits the result only if no conflicting operation occurred.

Most operating systems material in this chapter uses pessimistic locking.
Optimistic techniques are important in databases, transactional memory,
and some high-concurrency data structures.

Types of Pessimistic Locks
--------------------------

.. index::
   single: pessimistic locking; types
   single: spin locks
   single: mutexes
   single: semaphores
   single: monitors
   single: reader-writer locks

Pessimistic locking assumes that a conflict may happen and prevents the
conflict before it can corrupt shared state.

Common techniques include disabling interrupts, strict alternation, spin
locks, mutexes, semaphores, condition variables, monitors, and
reader-writer locks.

Types of Optimistic Locks
-------------------------

.. index::
   single: optimistic locking; types
   single: compare-and-swap
   single: software transactional memory (STM)

Optimistic locking assumes that conflicts are uncommon and checks for a
conflict before committing the result.

Most non-database implementations use some kind of software
transactional memory or compare-and-swap based retry loop. This approach
can work well when conflicts are rare, communication is expensive, or
the protected data structure is sparse.

Software Transactional Memory Example
-------------------------------------

.. index::
   single: software transactional memory (STM)
   pair: concurrency; transactional memory

Software transactional memory lets code make tentative changes and then
commit them only if the original data has not changed.

The ``systems-code-examples/stm`` example implements a small optimistic
transaction around a shared counter.

.. literalinclude:: ../examples/systems-code-examples/stm/main.cc
   :language: cpp
   :linenos:

Key points:

- Each thread increments the counter in chunks.
- ``Begin()`` returns a handle containing a snapshot of the current
  value.
- The thread updates the handle instead of updating the shared counter
  directly.
- ``TryCommit()`` commits the new value only if no other transaction has
  committed since the snapshot was taken.
- A failed commit causes the loop to retry.

.. literalinclude:: ../examples/systems-code-examples/stm/trans.cc
   :language: cpp
   :linenos:

Key points:

- ``_event`` is the version number for the shared value.
- ``Begin()`` copies the current value and event while holding the
  internal mutex.
- ``TryCommit()`` compares the saved event with the current event.
- A successful commit writes the new value and advances the event.
- A failed commit increments the rollback count.

Disabling Interrupts
--------------------

.. index::
   single: disabling interrupts
   pair: mutual exclusion; disabling interrupts

Disabling interrupts prevents timer interrupts from invoking the
scheduler on the current CPU. This can make a short kernel critical
section atomic on that CPU.

This technique is not suitable for ordinary application code. Disabling
interrupts is privileged, can hurt resource utilization, and can stop the
system if interrupts are not restored. On multiprocessor systems it also
does not automatically protect the same data on other CPUs.

Strict Alternation
------------------

.. index::
   single: strict alternation
   pair: mutual exclusion; strict alternation

Strict alternation lets threads take turns entering a critical section,
usually through a shared ``turn`` variable.

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

Strict alternation is usually a poor solution. A slow thread in its
non-critical section can prevent another thread from entering the
critical section even when the critical section is free.

Spin Locks
----------

.. index::
   single: spin locks
   pair: mutual exclusion; spin locks

A spin lock is a lock where a waiting thread repeatedly checks whether
the lock is available. The waiting thread stays runnable while it waits.

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

The lock and unlock calls make the update to ``count`` behave as a
single protected operation. Spin locks are useful for very short waits,
especially inside kernels, but they waste CPU time if the wait is long.

Spin Lock Implementation
------------------------

.. index::
   single: test_and_set
   single: xchg instruction
   single: cmpxchg instruction
   pair: x86; atomic instructions
   single: atomic operations

A spin lock needs an atomic operation that tests the old value and sets a
new value as one indivisible operation.

::

   spin_lock(int* lock) {
      while(*lock == 1) { }
      *lock = 1;
   }

   spin_unlock(int* lock) {
      *lock = 0;
   }

This pseudo-code is not correct. Two threads can both observe ``lock`` as
``0`` and then both set it to ``1``. The test and the store must be one
atomic operation.

::

   spin_lock(int* lock) {
      while(test_and_set(lock) == 1) {}
   }

On x86 systems, this kind of operation can be implemented with an atomic
instruction such as ``xchg`` or ``cmpxchg``.

Spin Lock Example
-----------------

The ``systems-code-examples/spin_lock`` example demonstrates the race in
the increment program and provides a spin lock implementation.

.. literalinclude:: ../examples/systems-code-examples/spin_lock/main.c
   :language: c
   :linenos:

Key points:

- Two pthreads both call ``increment()`` and update the shared global
  ``count``.
- The program checks whether the final value is ``2 * n``.
- The calls to ``spin_lock()`` and ``spin_unlock()`` are present but
  commented out, which leaves the race visible.
- Uncommenting the lock calls protects the read-modify-write sequence.

.. literalinclude:: ../examples/systems-code-examples/spin_lock/lock.c
   :language: c
   :linenos:

Key points:

- ``test_and_set()`` uses inline assembly to perform an atomic
  compare-and-exchange operation.
- ``spin_lock()`` loops while another thread owns the lock.
- ``pthread_yield()`` gives the scheduler a chance to run another
  thread while this thread waits.
- ``spin_unlock()`` releases the lock by storing ``0``.

Mutexes
-------

.. index::
   single: mutex
   single: pthread_mutex_t
   single: pthread_mutex_lock()
   single: pthread_mutex_unlock()
   single: pthread_mutex_init()
   pair: POSIX; mutex

A mutex is a mutual exclusion lock that allows only one owner at a time.
Unlike a simple spin lock, a system mutex can put a waiting thread to
sleep instead of making it burn CPU cycles.

The pthreads library provides mutexes on POSIX systems:

::

   pthread_mutex_t mutex;
   pthread_mutex_init(&mutex, NULL);

   pthread_mutex_lock(&mutex);
   /* critical section */
   pthread_mutex_unlock(&mutex);

.. index::
   single: CreateMutex()
   single: WaitForSingleObject()
   single: ReleaseMutex()
   pair: Windows; mutex

Windows provides the same idea through mutex handles:

::

   HANDLE mutex = CreateMutex(NULL, FALSE, NULL);
   WaitForSingleObject(mutex, INFINITE);
   /* critical section */
   ReleaseMutex(mutex);
   CloseHandle(mutex);

Mutex Example
-------------

The ``systems-code-examples/mutex`` example wraps a low-level lock in a
small C++ ``Mutex`` class and uses it to protect the shared counter.

.. literalinclude:: ../examples/systems-code-examples/mutex/main.cc
   :language: cpp
   :linenos:

Key points:

- ``lock->Lock()`` is called before the shared counter is read.
- ``lock->Unlock()`` is called after the updated value is stored.
- The critical section is the read, increment, and write of ``count``.
- The two worker threads can run concurrently, but only one may update
  ``count`` at a time.

.. literalinclude:: ../examples/systems-code-examples/mutex/mutex.cc
   :language: cpp
   :linenos:

Key points:

- The class stores the lock state in ``_lock``.
- ``Lock()`` uses the same atomic test-and-set idea as the spin lock.
- ``Unlock()`` releases ownership by setting ``_lock`` back to ``0``.
- This example is useful for seeing how a lock can be wrapped behind a
  smaller interface.

Semaphores
----------

.. index::
   single: semaphore
   single: binary semaphore
   single: counting semaphore
   single: event semaphore
   pair: synchronization; semaphore

A semaphore is a synchronization object with an integer count. A thread
decrements the count before proceeding and increments it when it releases
or signals the resource.

Binary semaphores have only two useful states and can emulate mutexes.
Counting semaphores allow up to ``N`` units of a resource to be used at
the same time. Event semaphores often begin at ``0`` and are used to
signal that something has happened.

Semaphore Implementation
------------------------

.. index::
   pair: semaphore; implementation

A semaphore can be built from a lock that protects the count and
operations that adjust the count.

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
      while(semaphore == 0) {
         spin_unlock(&lock);
         spin_lock(&lock);
      }
      semaphore -= 1;
      spin_unlock(&lock);
   }

This simple version shows the idea but still spins while waiting. System
semaphores normally block the waiting thread.

Semaphore Example
-----------------

The ``systems-code-examples/semaphore`` example uses a semaphore to
coordinate worker threads that consume work from a queue.

.. literalinclude:: ../examples/systems-code-examples/semaphore/main.cc
   :language: cpp
   :linenos:

Key points:

- ``populateQueue()`` adds work items and calls ``semaphore->Up()`` for
  each new item.
- Worker threads call ``semaphore->Down()`` before removing an item from
  the queue.
- The mutex protects the queue while a thread checks for work, removes
  work, or marks the queue as done.
- The final count should equal the number of queued work items.

.. literalinclude:: ../examples/systems-code-examples/semaphore/semaphore.cc
   :language: cpp
   :linenos:

Key points:

- ``Semaphore`` stores the count in ``_count``.
- ``Up()`` increments the count while holding the internal mutex.
- ``Down()`` decrements the count only when a unit is available.
- This version returns ``false`` instead of blocking when the count is
  zero, so the caller remains responsible for retry behavior.

Producer-Consumer with Semaphores
---------------------------------

.. index::
   single: producer-consumer problem
   pair: concurrency; producer-consumer
   single: bounded buffer

The producer-consumer problem uses one or more producers to place items
in a bounded queue and one or more consumers to remove them.

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

The ``empty`` semaphore counts open slots. The ``full`` semaphore counts
available items. The ``mutex`` semaphore protects the queue itself.

Bounded Buffer Example
----------------------

The ``systems-code-examples/bounded-buffer`` example implements the
producer-consumer pattern with pthread mutexes and condition variables.

.. literalinclude:: ../examples/systems-code-examples/bounded-buffer/bounded-buffer.c
   :language: c
   :linenos:

Key points:

- The program creates supplier threads and consumer threads.
- Each thread receives thread-specific data containing the shared bounded
  buffer, command-line options, and its own id.
- Suppliers allocate entries and place them into the bounded buffer.
- Consumers remove entries, process them, and free their memory.
- The main thread joins all worker threads before cleaning up the buffer.

.. literalinclude:: ../examples/systems-code-examples/bounded-buffer/bbuffer.c
   :language: c
   :linenos:

Key points:

- ``bounded_buffer_init()`` initializes the circular buffer, mutex, and
  two condition variables.
- ``bounded_buffer_put()`` waits while the buffer is full.
- ``bounded_buffer_get()`` waits while the buffer is empty.
- ``has_space`` wakes a producer when a consumer removes an item.
- ``has_items`` wakes a consumer when a producer adds an item.

Reader-Writer Locks
-------------------

.. index::
   single: reader-writer locks
   pair: synchronization; reader-writer locks

A reader-writer lock allows many readers to hold the lock at the same
time but gives writers exclusive access.

Reader-writer locks can improve concurrency when reads are common and
writes are less frequent. Their implementation is more complex than a
plain mutex because the lock must track active readers, active writers,
waiting readers, and waiting writers.

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

Reader-writer locks force design choices. A lock may give priority to
readers, priority to writers, or attempt to balance the two. Each choice
can create different starvation and deadlock risks.

POSIX Semaphores
----------------

.. index::
   single: sem_init()
   single: sem_wait()
   single: sem_post()
   pair: POSIX; semaphores

POSIX semaphores provide a system interface for counting semaphores.

The basic initialization call is:

::

   int sem_init(sem_t *sem, int pshared, unsigned int value);

The ``sem`` argument names the semaphore object. The ``pshared`` argument
controls whether the semaphore is shared only among threads in one
process or among processes through shared memory. The ``value`` argument
sets the initial count.

::

   sem_t sema;
   sem_init(&sema, 0, 1);
   sem_wait(&sema);
   /* critical section */
   sem_post(&sema);

Windows Semaphores
------------------

.. index::
   single: CreateSemaphore()
   single: ReleaseSemaphore()
   pair: Windows; semaphores
   single: named semaphores

Windows also provides semaphore objects through the Win32 API.

::

   const int minCount = 0;
   const int maxCount = 20;
   HANDLE sema = CreateSemaphore(NULL, minCount, maxCount, NULL);

   WaitForSingleObject(sema, INFINITE);
   /* critical section */
   ReleaseSemaphore(sema, 1, NULL);

Named semaphores can be shared by separate processes. A process creates
or opens a named semaphore and then waits on it with
``WaitForSingleObject()``.

Monitors and Condition Variables
--------------------------------

.. index::
   single: monitors
   pair: synchronization; monitors

A monitor combines mutual exclusion with a way for threads to wait until
a condition becomes true.

Monitor-style synchronization usually has four operations:

- ``enter`` locks the monitor.
- ``exit`` unlocks the monitor.
- ``wait`` releases the lock and blocks until another thread signals.
- ``pulse`` or ``signal`` wakes a waiting thread.

The usual pattern is:

::

   enter();
   while(condition_to_proceed_is_not_true) {
      wait();
   }
   do_operation();
   if(condition_has_changed) {
      pulse();
   }
   exit();

The condition is checked in a loop because a thread may wake up before
the condition it needs is actually true.

Monitor Example
---------------

The ``systems-code-examples/monitor`` example rewrites the queue
coordination problem with a monitor abstraction.

.. literalinclude:: ../examples/systems-code-examples/monitor/main.cc
   :language: cpp
   :linenos:

Key points:

- Worker threads enter the monitor before inspecting the shared queue.
- A worker waits while the queue is empty and more work may still arrive.
- ``populateQueue()`` pulses the monitor after it enqueues a new item.
- ``DoneAdding()`` lets waiting workers eventually stop waiting for more
  work.
- The monitor keeps the waiting condition close to the protected queue
  operations.

.. literalinclude:: ../examples/systems-code-examples/monitor/monitor.cc
   :language: cpp
   :linenos:

Key points:

- ``Enter()`` obtains the internal mutex and marks the monitor locked.
- ``Exit()`` releases the internal mutex.
- ``Wait()`` repeatedly releases and reacquires the monitor while waiting
  for a pulse.
- ``Pulse()`` records that one waiter may proceed.
- This implementation is intentionally small and shows the idea rather
  than a production condition-variable implementation.

Pthreads Condition Variables
----------------------------

.. index::
   single: pthread_cond_t
   single: pthread_cond_wait()
   single: pthread_cond_signal()
   single: pthread_cond_init()
   pair: POSIX; condition variables

Pthreads condition variables implement the monitor pattern with an
explicit mutex and condition variable.

::

   pthread_mutex_t mutex;
   pthread_cond_t cond;
   pthread_mutex_init(&mutex);
   pthread_cond_init(&cond);

   pthread_mutex_lock(&mutex);
   while(/* condition is not true */) {
      pthread_cond_wait(&cond, &mutex);
   }
   /* critical section */
   pthread_cond_signal(&cond);
   pthread_mutex_unlock(&mutex);

``pthread_cond_wait()`` releases the mutex while the thread waits and
reacquires it before returning. This lets another thread enter the
critical section and change the condition.

Windows Condition Variables
---------------------------

.. index::
   single: CONDITION_VARIABLE
   single: SleepConditionVariableCS()
   single: WakeConditionVariable()
   pair: Windows; condition variables

Windows condition variables provide the same basic pattern as pthread
condition variables.

::

   CONDITION_VARIABLE cond;
   CRITICAL_SECTION lock;

   InitializeConditionVariable(&cond);
   InitializeCriticalSection(&lock);

   EnterCriticalSection(&lock);
   while(/* condition is not true */) {
      SleepConditionVariableCS(&cond, &lock, INFINITE);
   }
   /* critical section */
   WakeConditionVariable(&cond);
   LeaveCriticalSection(&lock);

The condition variable works with a ``CRITICAL_SECTION`` so the waiting
thread can atomically release the lock and block.

C# Bounded Buffer
-----------------

.. index::
   pair: C#; bounded buffer
   pair: C#; semaphores

The ``systems-code-examples/threads_csharp`` directory includes a C#
bounded buffer implemented with .NET semaphores.

.. literalinclude:: ../examples/systems-code-examples/threads_csharp/threads/BoundBuffer.cs
   :language: csharp
   :linenos:
   :start-after: //begin-class-BoundBuffer
   :end-before: //end-class-BoundBuffer

Key points:

- ``_empty`` counts open slots in the buffer.
- ``_full`` counts items available to consumers.
- ``_lock`` is a binary semaphore that protects the queue.
- ``Enqueue()`` waits for space before adding an item.
- ``Dequeue()`` waits for an item before removing one.

C# Monitor Pipeline
-------------------

.. index::
   pair: C#; monitor
   single: Monitor.Wait()
   single: Monitor.Pulse()

The C# pipeline examples use ``lock``, ``Monitor.Wait()``, and
``Monitor.Pulse()`` to coordinate staged work.

.. literalinclude:: ../examples/systems-code-examples/threads_csharp/threads/PipelineComputation.cs
   :language: csharp
   :linenos:
   :start-after: //begin-class-PipelineComputation
   :end-before: //end-class-PipelineComputation

Key points:

- The reader, compressor, and writer run on separate threads.
- Each stage communicates through a queue protected by ``lock``.
- A stage waits when its input queue is empty or its output queue is too
  large.
- ``Monitor.Pulse()`` wakes a later stage when new work is available.
- The ``volatile`` flags communicate when earlier stages have finished.

C# Threaded Enumeration
-----------------------

The ``ThreadedList`` helper turns an ordinary enumerable into a
producer-consumer pipeline.

.. literalinclude:: ../examples/systems-code-examples/threads_csharp/threads/ThreadedList.cs
   :language: csharp
   :linenos:
   :start-after: //begin-class-ThreadedList
   :end-before: //end-class-ThreadedList

Key points:

- A background thread advances the wrapped enumerator.
- Produced values are placed in a bounded queue.
- ``MoveNext()`` waits until an item is available or enumeration is
  finished.
- ``Monitor.Wait()`` and ``Monitor.Pulse()`` coordinate the producer and
  consumer.
- ``Dispose()`` stops the worker thread and joins it.

Concise Pipeline with ThreadedList
----------------------------------

The concise pipeline example shows how the synchronization details can be
hidden behind ``ThreadedList``.

.. literalinclude:: ../examples/systems-code-examples/threads_csharp/threads/ConcisePipelineComputation.cs
   :language: csharp
   :linenos:
   :start-after: //begin-class-ConcisePipelineComputation
   :end-before: //end-class-ConcisePipelineComputation

Key points:

- ``FileReader()`` yields byte arrays from the input file.
- ``Compression()`` consumes blocks and yields compressed blocks.
- ``FileWriter()`` consumes the compressed stream.
- ``ThreadedList`` lets adjacent stages overlap execution.
- The pipeline reads like ordinary iterator code while still using
  threaded buffering internally.

Dining Philosophers
-------------------

.. index::
   single: dining philosophers problem
   single: deadlock
   pair: concurrency; dining philosophers
   single: lock ordering
   single: resource contention

The dining philosophers problem is a classic example of lock ordering,
resource contention, and deadlock.

The complete ``systems-code-examples/dining-philosophers`` example fits
best in the deadlock chapter. It will be used there as the main example
for circular wait and deadlock avoidance.
