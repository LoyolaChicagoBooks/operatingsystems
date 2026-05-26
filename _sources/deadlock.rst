Deadlock
========

Deadlock occurs when two or more threads are each waiting for resources
held by another thread in the same waiting cycle. Once the cycle forms,
the threads cannot make progress unless something external breaks the
cycle.

Deadlock may not happen immediately in a program that contains the
conditions for it. The bug may depend on timing, scheduling, input, or
the order in which threads reach the locks.

Dining Philosophers Problem
---------------------------

The dining philosophers problem is a small model of resource contention.
Each philosopher alternates between thinking and eating, and each needs
two forks to eat.

The usual rules are:

- A philosopher can be eating or thinking.
- Philosophers do not communicate with each other.
- A philosopher needs both the left fork and the right fork before
  eating.
- Each fork can be held by only one philosopher at a time.

Dining Philosophers Deadlock Scenario
-------------------------------------

A deadlock can occur if every philosopher picks up one fork and then
waits for the other fork.

::

   eat() {
      pick up right fork;
      pick up left fork;
      proceed with eating;
   }

One possible execution is:

- At ``t = 0``, every philosopher picks up the fork on the right.
- At ``t = 1``, philosopher 1 tries to pick up the left fork, but
  philosopher 5 holds it.
- At ``t = 2``, philosopher 2 tries to pick up the left fork, but
  philosopher 1 holds it.
- The same pattern continues around the table.

Every philosopher holds one fork and waits for a fork held by another
philosopher. No philosopher can finish eating and release a fork, so the
program is deadlocked.

Dining Philosophers Lock Graph
------------------------------

A lock graph shows the possible ordering relationships among locks. If
the graph contains a cycle, a deadlock is possible.

In the first dining philosophers algorithm, forks can be acquired in an
order that forms a cycle.

.. figure:: deadlock/undirected_lock_graph.*
   :align: center
   :width: 450px
   :alt: Undirected Lock Graph

Avoiding Deadlock with Try-and-Release
--------------------------------------

One way to avoid deadlock is to acquire both needed resources as a single
logical operation. If the second resource is not available, the thread
releases the first resource and tries again later.

::

   eat() {
      while(does not have both forks) {
         pickup left fork;
         if(can pick up right fork) {
            pick up right fork;
         } else {
            put down left fork;
         }
      }
      proceed with eating;
   }

This avoids the deadlock cycle because a philosopher does not continue
holding one fork while waiting forever for the other fork. The tradeoff
is that the implementation is more complex and can still need fairness
rules to avoid starvation or livelock.

Avoiding Deadlock with Lock Ordering
------------------------------------

Another way to avoid deadlock is to assign a global order to resources
and require every thread to acquire resources in that order.

::

   eat() {
      pick up fork with lower number;
      pick up fork with higher number;
      proceed with eating;
   }

This works because a global order removes cycles from the lock graph. If
all threads acquire the lower-numbered lock before the higher-numbered
lock, no thread can create a circular wait.

.. figure:: deadlock/directed_lock_graph.*
   :align: center
   :width: 450px
   :alt: Directed Lock Graph

Dijkstra's Solution and the Banker's Algorithm
----------------------------------------------

Dijkstra's solution to the dining philosophers problem uses resource
ordering. The same basic idea appears in the Banker's algorithm: do not
enter a resource allocation state that can lead to deadlock.

The main advantage is simplicity. Lock ordering is easy to state, easy to
review, and often easy to implement. A common approach is to lock mutexes
in address order or in the order they appear in a table.

The main disadvantage is reduced concurrency. Some lock acquisition
orders would be safe in a specific execution, but a strict global order
forbids them anyway.

Deadlock Avoidance Implementation
---------------------------------

Deadlock avoidance prevents unsafe lock acquisition by enforcing a
consistent order.

::

   typedef struct {
      int LockNumber;
      void* LockObject;
   } lock;

   void multi_lock(lock* locks, int count) {
      sort(locks, count);
      for(int i = 0; i < count; i++) {
         lock(locks[i]);
      }
   }

The important part is not the sorting function itself. The important
part is that every caller uses the same ordering rule before acquiring
more than one lock.

Deadlock Prevention Implementation
----------------------------------

Deadlock prevention can also be done by acquiring locks with a
try-and-release rule. If any lock cannot be acquired, the thread releases
what it already holds and tries again.

::

   typedef struct {
      int LockNumber;
      void* LockObject;
   } lock;

   void multi_lock(lock* locks, int count) {
      while(1) {
         int i = 0;
         for(i = 0; i < count; i++) {
            if(!try_lock(locks[i])) {
               for(int j = 0; j < i; j++) {
                  unlock(locks[j]);
               }
               break;
            }
         }
         if(i == count) {
            return;
         }
      }
   }

This approach can allow more concurrency than strict ordering, but it can
also be harder to reason about. Without backoff or fairness, two threads
can repeatedly interfere with each other.

Dining Philosophers Example
---------------------------

The ``systems-code-examples/dining-philosophers`` example can run with
or without resource ordering. The default mode can deadlock. The
``DINING_POLICY=avoid_deadlock`` mode reorders forks to avoid the cycle.

.. literalinclude:: ../examples/systems-code-examples/dining-philosophers/diner-demo.c
   :language: c
   :linenos:

Key points:

- The program creates five shared ``fork_t`` objects and five
  ``diner_t`` objects.
- Each diner receives pointers to the two forks it needs.
- The default order gives each diner its normal left and right fork.
- When the policy is ``avoid_deadlock``, the program reorders the fork
  pointers so each diner follows the same resource order.
- The main thread prints each diner's state so deadlock can be observed.

.. literalinclude:: ../examples/systems-code-examples/dining-philosophers/diners.c
   :language: c
   :linenos:

Key points:

- Each fork is protected by a ``pthread_mutex_t``.
- ``fork_pickup()`` locks the fork mutex, and ``fork_putdown()``
  unlocks it.
- ``diner_run()`` repeatedly thinks, picks up the left fork, picks up the
  right fork, eats, and releases both forks.
- The short sleep between fork acquisitions makes the deadlock easier to
  reproduce.
- ``get_dining_policy()`` reads the environment variable that selects
  the deadlock-avoidance policy.

.. literalinclude:: ../examples/systems-code-examples/dining-philosophers/diners.h
   :language: c
   :linenos:

Key points:

- ``fork_t`` stores the fork id and the mutex protecting that fork.
- ``diner_t`` stores the diner id, current state, fork pointers, and
  pthread handle.
- The ``dining_policy_t`` enum records whether fork reordering is active.
- The header separates the shared data model from the implementation in
  ``diners.c``.

Example of Deadlock in Nested Calls
-----------------------------------

Deadlock can be harder to see when locks are acquired through nested
method calls.

::

   void method1() {
      a.lock();
      method2();
      a.unlock();
   }

::

   void method2() {
      b.lock();
      c.lock();
      b.unlock();
      c.unlock();
   }

::

   void method3() {
      c.lock();
      method2();
      method4();
      c.unlock();
   }

::

   void method4() {
      d.lock();
      d.unlock();
   }

Here, ``method3()`` locks ``c`` and then calls ``method2()``, which tries
to lock ``c`` again after locking ``b``. Depending on the lock type and
the rest of the program, this can produce self-deadlock or contribute to
a larger lock-ordering cycle.

Multi-Lock Solutions in Windows
-------------------------------

Windows provides ``WaitForMultipleObjects()`` for waiting on multiple
kernel objects.

The objects can include events, mutexes, semaphores, timers, and other
waitable handles. This gives Windows programs a system-level way to wait
for one or more synchronization objects.

Multi-Lock Solutions in Linux and Minix
---------------------------------------

Linux and Minix do not provide a general multi-lock operation for normal
pthread mutexes.

The usual guidance is to avoid multi-locking when possible. When multiple
locks are necessary, use a consistent ordering rule. For shared-memory
semaphores, virtual address order is not reliable across processes, so
the order should come from a shared table, array index, or explicit lock
number.

Deadlock with Correct Lock Ordering
-----------------------------------

Lock ordering prevents circular wait, but it does not prevent every way a
program can stop making progress.

A thread can still lock a resource and fail to release it. Common causes
include missing unlock calls, crashes while holding a lock, infinite
loops inside critical sections, and monitor conditions that never become
true. These failures are often harder to handle because the lock order
may be correct while the program state is not.

Starvation
----------

Starvation occurs when one or more threads wait indefinitely because
other threads keep getting access to the resource first.

Starvation is closely related to scheduling fairness. A program can avoid
deadlock and still be unfair enough that one thread makes little or no
progress.

Starvation Example
------------------

This example holds a lock while performing blocking I/O. That can make
other threads wait much longer than necessary.

::

   Queue _queue = new Queue();
   Mutex _mutex = new Mutex();

   void add() {
      int value = 0;
      while(1) {
         _mutex->Lock();
         while(_queue->count() > 0) {
            value += _queue->Dequeue();
            printf("current value = %d\n", value);
         }
         _mutex->Unlock();
      }
   }

::

   void read_values() {
      while(1) {
         int value = 0;
         _mutex->Lock();
         scanf("%d\n", &value);
         _queue->Enqueue(value);
         _mutex->Unlock();
      }
   }

The first thread holds the lock while calling ``printf()``. The second
thread holds the lock while calling ``scanf()``. Both operations can
block, and while either one blocks the other thread cannot enter the
critical section.

Reducing Starvation
-------------------

One simple improvement is to keep blocking I/O outside the critical
section.

::

   Queue _queue = new Queue();
   Mutex _mutex = new Mutex();

   void add() {
      int value = 0;
      while(1) {
         _mutex->Lock();
         while(_queue->count() > 0) {
            value += _queue->Dequeue();
            _mutex->Unlock();
            printf("current value = %d\n", value);
            _mutex->Lock();
         }
         _mutex->Unlock();
      }
   }

::

   void read_values() {
      while(1) {
         int value = 0;
         scanf("%d\n", &value);
         _mutex->Lock();
         _queue->Enqueue(value);
         _mutex->Unlock();
      }
   }

This version does not hold the lock during ``printf()`` or ``scanf()``.
The critical section is shorter, and the average wait time for the lock
is reduced.

Guidelines to Avoid Starvation
------------------------------

Starvation is less likely when locks are held for short, predictable
periods.

Useful guidelines include:

- Keep critical sections short.
- Avoid blocking I/O while holding a lock.
- If a computation is long-running, design it so the lock can be released
  periodically.
- Copy the needed data while holding the lock, then do expensive work
  after releasing it.
- Use synchronization libraries that provide fairness guarantees when
  fairness matters.

Livelock
--------

Livelock occurs when threads keep running but repeatedly take actions
that prevent progress.

It can happen in deadlock-avoidance code when two threads both release
and retry at the same time.

::

   void thread1() {
      while(1) {
         a.lock();
         if(b.trylock()) {
            /* do work */
            b.unlock();
         }
         a.unlock();
      }
   }

::

   void thread2() {
      while(1) {
         b.lock();
         if(a.trylock()) {
            /* do work */
            a.unlock();
         }
         b.unlock();
      }
   }

One possible sequence is:

- ``thread1`` locks ``a``.
- ``thread2`` locks ``b`` and fails to lock ``a``.
- ``thread1`` fails to lock ``b`` and releases ``a``.
- ``thread2`` releases ``b``.

If the pattern repeats, both threads continue executing but neither
completes the protected work. Backoff, ordering, or a coordinator can
break the livelock.

Lock Fairness
-------------

Lock fairness means that threads waiting for a lock have similar average
wait times.

Fair locks can reduce starvation and make execution more predictable.
Common approaches include FIFO wait queues and, less commonly, schedulers
that use timing history.

Fairness Tradeoffs
------------------

Fairness has a cost because selecting the next owner of a lock is not the
same as immediately running that thread.

A fair lock can reduce throughput when a thread repeatedly locks and
unlocks a short critical section. An unfair lock may let the same thread
reacquire the lock quickly, which can improve locality and throughput but
can also starve other threads.
