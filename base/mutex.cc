// emulates google3/base/mutex.cc

#include "base/mutex.h"

#include "base/logging.h"
#include <pthread.h>

namespace base {

#ifdef _WIN32

struct Mutex::Internal {
  CRITICAL_SECTION mutex;
#ifndef NDEBUG
  // Used only to implement AssertHeld().
  DWORD thread_id;
#endif
};

Mutex::Mutex()
  : mInternal(new Internal) {
  InitializeCriticalSection(&mInternal->mutex);
}

Mutex::~Mutex() {
  DeleteCriticalSection(&mInternal->mutex);
  delete mInternal;
}

void Mutex::Lock() {
  EnterCriticalSection(&mInternal->mutex);
#ifndef NDEBUG
  mInternal->thread_id = GetCurrentThreadId();
#endif
}

void Mutex::Unlock() {
#ifndef NDEBUG
  mInternal->thread_id = 0;
#endif
  LeaveCriticalSection(&mInternal->mutex);
}

void Mutex::AssertHeld() {
#ifndef NDEBUG
  DCHECK_EQ(mInternal->thread_id, GetCurrentThreadId());
#endif
}

//#elif defined(HAVE_PTHREAD)
#else

struct Mutex::Internal {
  pthread_mutex_t mutex;
};

Mutex::Mutex()
  : mInternal(new Internal) {
  pthread_mutex_init(&mInternal->mutex, NULL);
}

Mutex::~Mutex() {
  pthread_mutex_destroy(&mInternal->mutex);
  delete mInternal;
}

void Mutex::Lock() {
  int result = pthread_mutex_lock(&mInternal->mutex);
  if (result != 0) {
    LOG(FATAL) << "pthread_mutex_lock: " << strerror(result);
  }
}

void Mutex::Unlock() {
  int result = pthread_mutex_unlock(&mInternal->mutex);
  if (result != 0) {
    LOG(FATAL) << "pthread_mutex_unlock: " << strerror(result);
  }
}

void Mutex::AssertHeld() {
  // pthreads dosn't provide a way to check which thread holds the mutex.
  // TODO(kenton):  Maybe keep track of locking thread ID like with WIN32?
}

struct CondVar::InternalCond {
  pthread_cond_t cond;
};

CondVar::CondVar(Mutex* mu)
    : mu_(mu), cInternal(new InternalCond) {
  pthread_cond_init(&cInternal->cond, NULL);
}

CondVar::~CondVar() {
  pthread_cond_destroy(&cInternal->cond);
  delete cInternal;
}

void CondVar::Wait() {
  int result = pthread_cond_wait(&cInternal->cond, &mu_->mInternal->mutex);
  if (result != 0) {
    LOG(FATAL) << "pthread_cond_wait: " << strerror(result);
  }
}

void CondVar::Signal() {
  int result = pthread_cond_signal(&cInternal->cond);
  if (result != 0) {
    LOG(FATAL) << "signal: " << strerror(result);
  }
}

void CondVar::SignalAll() {
  int result = pthread_cond_broadcast(&cInternal->cond);
  if (result != 0) {
    LOG(FATAL) << "broadcast: " << strerror(result);
  }
}

#endif

}  // namespace base
