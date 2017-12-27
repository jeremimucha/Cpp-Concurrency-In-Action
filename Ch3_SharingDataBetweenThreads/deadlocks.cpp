/* 
** Deadlocks occur when multiple locks are aquired by multiple threads
** to performa an operation. Each thread holds one mutex and is waiting for
** another to be released, which never occurs. 
** 
** Avoiding deadlocks:
** - always acquire mutexes in the same order - easy with the help of
**   std::lock (example below)
** - Deadlocks can be created by calls to join() - two threads are waiting
**   for each other to finish. Avoid by not waiting for a thread if there's
**   a slightest chance that it might be waiting for you (this thread).
** - Avoid nested locks alltogether if possible
** - Avoid calling user-supplied code while holding a lock - it might lock
**   other data.
** - Acquire locks in a fixed order - can be conceptual / by design, or can
**   be enforced by a hirearchical mutex.
*/

// TODO: add deadlock examples (avoiding deadlocks)
