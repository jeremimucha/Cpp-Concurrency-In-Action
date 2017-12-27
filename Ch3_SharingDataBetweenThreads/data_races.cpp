/* 
** Race conditions occur when multiple threads acces shared data and at least
** one of the threads is modifying (writing to) the data.
** Race conditions can be avoided by:
** - using locking mechanisms (mutexes)
** - lock-free programming - modifying the data structures and its invariants
**   so that each modification is divided into indivisible changes, while
**   each change preserves the invariant.
** - Software transactional memory - each transaction is a single step, and
**   is commited only if the transaction wasn't interrupted by a different
**   thread.
**
** Using mutexes to avoid race conditions:
** - use RAII - std::lock_guard, std::lock (for multiple mutexes)
** - group data and associated mutex together in a class to encapsulate its use.
** - avoid returning references or pointers to the protected data
** - avoid passing protected data to user functions which might also save
**   references or pointers to it.
** - Keep in mind inherent race conditions of some data structures. E.g.
**   stack's state might change between the call to empty(), top() and pop(),
**   it might be necessary to provide a modified interface.
**   Possible modifications:
**   - return via reference - void pop(T& retval)
**   - require no-throw copy/move constructable 
**   - return a pointer to the poped item - avoids the need for no-throw copy/move
** */

// TODO: add a data-race example (avoiding data races)