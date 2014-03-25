// Shutdown support.

#ifndef BASE_SHUTDOWN_H_
#define BASE_SHUTDOWN_H_

#include "base/once.h"
#include "base/mutexlock.h"

#include <vector>
using std::vector;

namespace base {

namespace internal {

typedef void OnShutdownFunc();
vector<void (*)()>* shutdown_functions = NULL;
Mutex* shutdown_functions_mutex = NULL;
GOOGLE_DECLARE_ONCE(shutdown_functions_init);

void InitShutdownFunctions() {
  shutdown_functions = new vector<void (*)()>;
  shutdown_functions_mutex = new Mutex;
}

inline void InitShutdownFunctionsOnce() {
  GoogleOnceInit(&shutdown_functions_init, &InitShutdownFunctions);
}

void OnShutdown(void (*func)()) {
  InitShutdownFunctionsOnce();
  MutexLock lock(shutdown_functions_mutex);
  shutdown_functions->push_back(func);
}

}  // namespace internal

void ShutdownLibrary() {
  internal::InitShutdownFunctionsOnce();

  // We don't need to lock shutdown_functions_mutex because it's up to the
  // caller to make sure that no one is using the library before this is
  // called.

  // Make it safe to call this multiple times.
  if (internal::shutdown_functions == NULL) return;

  for (int i = 0; i < internal::shutdown_functions->size(); i++) {
    internal::shutdown_functions->at(i)();
  }
  delete internal::shutdown_functions;
  internal::shutdown_functions = NULL;
  delete internal::shutdown_functions_mutex;
  internal::shutdown_functions_mutex = NULL;
}

}  // namespace base

#endif // BASE_SHUTDOWN_H_
