#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include "pthread_svelte_types.h"

enum {
  PTHREAD_SVELTE_MUTEX_NORMAL = 0,
  PTHREAD_SVELTE_MUTEX_RECURSIVE = 1,
  PTHREAD_SVELTE_MUTEX_ERRORCHECK = 2,

  PTHREAD_SVELTE_MUTEX_ERRORCHECK_NP = PTHREAD_SVELTE_MUTEX_ERRORCHECK,
  PTHREAD_SVELTE_MUTEX_RECURSIVE_NP  = PTHREAD_SVELTE_MUTEX_RECURSIVE,

  PTHREAD_SVELTE_MUTEX_DEFAULT = PTHREAD_SVELTE_MUTEX_NORMAL
};

#define PTHREAD_SVELTE_MUTEX_INITIALIZER { { ((PTHREAD_SVELTE_MUTEX_NORMAL & 3) << 14) } }
#define PTHREAD_SVELTE_RECURSIVE_MUTEX_INITIALIZER_NP { { ((PTHREAD_SVELTE_MUTEX_RECURSIVE & 3) << 14) } }
#define PTHREAD_SVELTE_ERRORCHECK_MUTEX_INITIALIZER_NP { { ((PTHREAD_SVELTE_MUTEX_ERRORCHECK & 3) << 14) } }

#define PTHREAD_SVELTE_COND_INITIALIZER  { { 0 } }
#define PTHREAD_SVELTE_COND_INITIALIZER_MONOTONIC_NP  { { 1 << 1 } }

#define PTHREAD_SVELTE_RWLOCK_INITIALIZER  { { 0 } }

enum {
  PTHREAD_SVELTE_RWLOCK_PREFER_READER_NP = 0,
  PTHREAD_SVELTE_RWLOCK_PREFER_WRITER_NONRECURSIVE_NP = 1,
};

#define PTHREAD_SVELTE_ONCE_INIT 0

#define PTHREAD_SVELTE_STACK_MIN (2 * PAGE_SIZE)

#define PTHREAD_SVELTE_CREATE_DETACHED 1
#define PTHREAD_SVELTE_CREATE_JOINABLE 0

#define PTHREAD_SVELTE_EXPLICIT_SCHED 0
#define PTHREAD_SVELTE_INHERIT_SCHED 1

#define PTHREAD_SVELTE_PRIO_NONE 0
#define PTHREAD_SVELTE_PRIO_INHERIT 1

#define PTHREAD_SVELTE_PROCESS_PRIVATE 0
#define PTHREAD_SVELTE_PROCESS_SHARED 1

#define PTHREAD_SVELTE_SCOPE_SYSTEM 0
#define PTHREAD_SVELTE_SCOPE_PROCESS 1

int       pthread_svelte_cond_broadcast(pthread_svelte_cond_t *);
int       pthread_svelte_cond_clockwait(pthread_svelte_cond_t *, pthread_svelte_mutex_t *, clockid_t, const struct timespec*); // Bionic-only
int       pthread_svelte_cond_destroy(pthread_svelte_cond_t *);
int       pthread_svelte_cond_init(pthread_svelte_cond_t *, const pthread_svelte_condattr_t *);
int       pthread_svelte_cond_signal(pthread_svelte_cond_t *);
int       pthread_svelte_cond_timedwait(pthread_svelte_cond_t *, pthread_svelte_mutex_t *, const struct timespec *);
int       pthread_svelte_cond_wait(pthread_svelte_cond_t *, pthread_svelte_mutex_t *);
int       pthread_svelte_condattr_destroy(pthread_svelte_condattr_t *);
int       pthread_svelte_condattr_getclock(const pthread_svelte_condattr_t *, clockid_t *); // Bionic-only
int       pthread_svelte_condattr_getpshared(const pthread_svelte_condattr_t *, int *);
int       pthread_svelte_condattr_init(pthread_svelte_condattr_t *);
int       pthread_svelte_condattr_setclock(pthread_svelte_condattr_t *, clockid_t); // Bionic-only
int       pthread_svelte_condattr_setpshared(pthread_svelte_condattr_t *, int);

int       pthread_svelte_mutex_destroy(pthread_svelte_mutex_t *);
int       pthread_svelte_mutex_getprioceiling(const pthread_svelte_mutex_t *, int *);
int       pthread_svelte_mutex_init(pthread_svelte_mutex_t *, const pthread_svelte_mutexattr_t *);
int       pthread_svelte_mutex_lock(pthread_svelte_mutex_t *);
int       pthread_svelte_mutex_setprioceiling(pthread_svelte_mutex_t *, int, int *);
int       pthread_svelte_mutex_trylock(pthread_svelte_mutex_t *);
int       pthread_svelte_mutex_unlock(pthread_svelte_mutex_t *);
int       pthread_svelte_mutexattr_destroy(pthread_svelte_mutexattr_t *);
int       pthread_svelte_mutexattr_getprioceiling(const pthread_svelte_mutexattr_t *, int *);
int       pthread_svelte_mutexattr_getprotocol(const pthread_svelte_mutexattr_t *, int *);
int       pthread_svelte_mutexattr_getpshared(const pthread_svelte_mutexattr_t *, int *);
int       pthread_svelte_mutexattr_gettype(const pthread_svelte_mutexattr_t *, int *);
int       pthread_svelte_mutexattr_init(pthread_svelte_mutexattr_t *);
int       pthread_svelte_mutexattr_setprioceiling(pthread_svelte_mutexattr_t *, int);
int       pthread_svelte_mutexattr_setprotocol(pthread_svelte_mutexattr_t *, int);
int       pthread_svelte_mutexattr_setpshared(pthread_svelte_mutexattr_t *, int);
int       pthread_svelte_mutexattr_settype(pthread_svelte_mutexattr_t *, int);

int       pthread_svelte_once(pthread_svelte_once_t *, void (*)(void));

int       pthread_svelte_rwlock_destroy(pthread_svelte_rwlock_t *);
int       pthread_svelte_rwlock_init(pthread_svelte_rwlock_t *, const pthread_svelte_rwlockattr_t *);
int       pthread_svelte_rwlock_rdlock(pthread_svelte_rwlock_t *);
int       pthread_svelte_rwlock_tryrdlock(pthread_svelte_rwlock_t *);
int       pthread_svelte_rwlock_trywrlock(pthread_svelte_rwlock_t *);
int       pthread_svelte_rwlock_unlock(pthread_svelte_rwlock_t *);
int       pthread_svelte_rwlock_wrlock(pthread_svelte_rwlock_t *);
int       pthread_svelte_rwlockattr_destroy(pthread_svelte_rwlockattr_t *);
int       pthread_svelte_rwlockattr_getpshared(const pthread_svelte_rwlockattr_t *, int *);
int       pthread_svelte_rwlockattr_init(pthread_svelte_rwlockattr_t *);
int       pthread_svelte_rwlockattr_setpshared(pthread_svelte_rwlockattr_t *, int);

#ifdef __cplusplus
}
#endif
