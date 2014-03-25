// A Mutex represents an exclusive lock.
// emulates leveldb

#ifndef BASE_MUTEX_H_
#define BASE_MUTEX_H_

#include "base/macros.h"

namespace base {

// A Mutex is a non-reentrant (aka non-recursive) mutex.  At most one thread T
// may hold a mutex at a given time.  If T attempts to Lock() the same Mutex
// while holding it, T will deadlock.
class Mutex {
 public:
  // Create a Mutex that is not held by anybody.
  Mutex();

  // Destructor
  ~Mutex();

  // Block if necessary until this Mutex is free, then acquire it exclusively.
  void Lock();

  // Release this Mutex.  Caller must hold it exclusively.
  void Unlock();

  // Crash if this Mutex is not held exclusively by this thread.
  // May fail to crash when it should; will never crash when it should not.
  void AssertHeld();

 private:
  friend class CondVar;
  struct Internal;
  Internal* mInternal;

  DISALLOW_COPY_AND_ASSIGN(Mutex);
};

class CondVar {
 public:
  explicit CondVar(Mutex* mu);
  ~CondVar();

  // Atomically release *mu and block on this condition variable until
  // either a call to SignalAll(), or a call to Signal() that picks
  // this thread to wakeup.
  // REQUIRES: this thread holds *mu
  void Wait();

  // If there are some threads waiting, wake up at least one of them.
  void Signal();

  // Wake up all waiting threads.
  void SignalAll();
 private:
  struct InternalCond;
  InternalCond* cInternal;
  Mutex* mu_;
  DISALLOW_COPY_AND_ASSIGN(CondVar);
};

}  // namespace base

#endif // BASE_MUTEX_H_
