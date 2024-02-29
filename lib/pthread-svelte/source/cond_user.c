#include "_pthread-svelte_internal.h"

static const pthread_svelte_cond_t _cond_static_initializer = PTHREAD_SVELTE_COND_INITIALIZER;
ALWAYS_INLINE int is_statically_allocated(const pthread_svelte_cond_t * cond) {
    return sceClibMemcmp(&_cond_static_initializer, cond, sizeof(pthread_svelte_cond_t)) == 0;
}

static SceKernelLwMutexWork _mother_mutex = {{ MAGIC_NOT_INITED }};

#define MAX_COND_NUMBER 1024
#define COND_ARRAY_SIZE sizeof(cond_object_t) * MAX_COND_NUMBER

static cond_object_t g_conds[MAX_COND_NUMBER];

ALWAYS_INLINE void _mother_mutex_check_init() {
    if (_mother_mutex.data[0] == MAGIC_NOT_INITED) {
        sceKernelCreateLwMutex(&_mother_mutex, "pthread_cond", 0, 0, NULL);
        sceKernelLockLwMutex(&_mother_mutex, 1, NULL);
        sceClibMemset(g_conds, 0, COND_ARRAY_SIZE);
        sceKernelUnlockLwMutex(&_mother_mutex, 1);
    }
}

ALWAYS_INLINE void _lock() {
    _mother_mutex_check_init();
    sceKernelLockLwMutex(&_mother_mutex, 1, NULL);
}

ALWAYS_INLINE void _unlock() {
    sceKernelUnlockLwMutex(&_mother_mutex, 1);
}

ALWAYS_INLINE uint8_t _is_valid_cond_ptr(cond_object_t * ptr) {
    return ptr >= &g_conds[0] && ptr < (&g_conds[0] + COND_ARRAY_SIZE);
}

ALWAYS_INLINE cond_object_t * _get_free_cond() {
    for (int i = 0; i < MAX_COND_NUMBER; ++i) {
        if (g_conds[i].is_initialized == 0) {
            return &g_conds[i];
        }
    }
    return NULL;
}


int pthread_svelte_cond_init(pthread_svelte_cond_t * cond, const pthread_svelte_condattr_t * attr) {
    _lock();
    pthread_cond_internal_t * internal = (pthread_cond_internal_t *) cond;

#ifdef EXTRA_ERROR_HANDLING
    if (is_statically_allocated(cond)) {
        _unlock();
        LOG_INFO("An attempt to initialize a statically allocated condition "
                 "variable, ignoring.");
        return 0;
    } else if (_is_valid_cond_ptr(internal->data) == 1) {
        _unlock();
        LOG_ERROR("The implementation has detected an attempt to reinitialise "
                  "the object referenced by cond, a previously initialised, "
                  "but not yet destroyed, condition variable. "
                  "Contents: %p", internal->data);
        return EBUSY;
    }
#endif

    internal->data = _get_free_cond();

#ifdef EXTRA_ERROR_HANDLING
    if (internal->data == NULL) {
        _unlock();
        LOG_ERROR("Insufficient memory exists to initialize "
                  "the condition variable.");
        return ENOMEM;
    }
#endif

    if (attr != NULL) internal->data->clock = COND_GET_CLOCK(*attr);
    internal->data->is_initialized = 1;

    _unlock();
    return 0;
}

int pthread_svelte_cond_destroy(pthread_svelte_cond_t * cond) {
    _lock();
    pthread_cond_internal_t * internal = (pthread_cond_internal_t *) cond;

#ifdef EXTRA_ERROR_HANDLING
    if (_is_valid_cond_ptr(internal->data) == 0) {
        _unlock();
        return EINVAL;
    }
#endif

    sceClibMemset(internal->data, 0, sizeof(cond_object_t));
    _unlock();
    return 0;
}

int pthread_svelte_cond_signal(pthread_svelte_cond_t * cond) {
    _lock();
    pthread_cond_internal_t * internal = (pthread_cond_internal_t *) cond;

#ifdef EXTRA_ERROR_HANDLING
    if (internal == NULL) {
        _unlock();
        LOG_ERROR("The value specified by cond is invalid (1).");
        return EINVAL;
    }
#endif

    if (_is_valid_cond_ptr(internal->data) == 0) {
        _unlock();
        LOG_INFO("The value specified by cond is not initialized (1).");
        return 0;
    }

    if (internal->data->mutex == NULL) {
        _unlock();
        LOG_INFO("The value specified by cond is an initialized but never used "
                 "condition variable (1).");
        return 0;
    }

    internal->data->signal = 1;
    if (internal->data->waiters > 0) {
        // wake up the latest waiter in the queue
        internal->data->waiters--;
    }

    _unlock();
    return 0;
}

int pthread_svelte_cond_broadcast(pthread_svelte_cond_t * cond) {
    _lock();
    pthread_cond_internal_t * internal = (pthread_cond_internal_t *) cond;

#ifdef EXTRA_ERROR_HANDLING
    if (internal == NULL) {
        _unlock();
        LOG_ERROR("The value specified by cond is invalid (1).");
        return EINVAL;
    }
#endif

    if (_is_valid_cond_ptr(internal->data) == 0) {
        _unlock();
        LOG_INFO("The value specified by cond is not initialized (2).");
        return 0;
    }

    if (internal->data->mutex == NULL) {
        _unlock();
        LOG_INFO("The value specified by cond is an initialized but never used "
                 "condition variable (1).");
        return 0;
    }

    internal->data->broadcast = 1;
    if (internal->data->waiters > 0) {
        // wake up the latest waiter in the queue
        internal->data->waiters--;
    }

    _unlock();
    return 0;
}

int pthread_svelte_cond_wait(pthread_svelte_cond_t * cond, pthread_svelte_mutex_t * mutex) {
    _lock();
    pthread_cond_internal_t * internal = (pthread_cond_internal_t *) cond;

#ifdef EXTRA_ERROR_HANDLING
    if (internal == NULL) {
        _unlock();
        LOG_ERROR("The value specified by cond is invalid (1).");
        return EINVAL;
    }
#endif

    if (is_statically_allocated(cond)) {
        internal->data = _get_free_cond();

#ifdef EXTRA_ERROR_HANDLING
        if (internal->data == NULL) {
            _unlock();
            LOG_ERROR("The value specified by cond was a statically allocated "
                      "condition variable that failed to undergo the actual "
                      "initialization procedure (1).");
            return EINVAL;
        }
#endif

        internal->data->is_initialized = 1;
    }

    if (_is_valid_cond_ptr(internal->data) == 0) {
        internal->data = _get_free_cond();

#ifdef EXTRA_ERROR_HANDLING
        if (internal->data == NULL) {
            _unlock();
            LOG_ERROR("The value specified by cond was a statically allocated "
                      "condition variable that failed to undergo the actual "
                      "initialization procedure (2).");
            return EINVAL;
        }
#endif

        internal->data->is_initialized = 1;
    }

    if (pthread_svelte_mutex_check_init(mutex) != 0) {
        _unlock();
        LOG_ERROR("The value specified by mutex is invalid.");
        return EINVAL;
    }

#ifdef EXTRA_ERROR_HANDLING
    if (internal->data->mutex != NULL && internal->data->mutex != mutex) {
        _unlock();
        LOG_ERROR("Different mutexes were supplied for concurrent "
                  "pthread_cond_wait() or pthread_cond_timedwait() "
                  "operations on the same condition variable.");
        return EINVAL;
    }
#endif

    internal->data->mutex = mutex;

    uint16_t self = internal->data->waiters; // marks our place in the queue
    internal->data->waiters++;

    uint32_t backoff = 100; // us
    while (
            (internal->data->broadcast == 0) ||
            (internal->data->signal == 0) ||
            (internal->data->broadcast == 1 && internal->data->waiters > self) ||
            (internal->data->signal == 1 && internal->data->waiters > self)
            )
    {
        pthread_svelte_mutex_unlock(mutex);
        _unlock();
        sceKernelDelayThread(backoff);
        backoff = backoff + 10;
        _lock();
        pthread_svelte_mutex_lock(mutex);
    }

    if (internal->data->signal == 1) {
        // only one waiter receives the signal
        internal->data->signal = 0;
    }

    if (internal->data->waiters == 0) {
        // every waiter got the signal / broadcast at this point
        internal->data->signal = 0;
        internal->data->broadcast = 0;
        internal->data->mutex = NULL;
    } else if (internal->data->broadcast == 1) {
        // allow the next waiter in the queue to wake up in case of broadcast
        internal->data->waiters--;
    }

    _unlock();
    return 0;
}

int pthread_svelte_cond_timedwait(pthread_svelte_cond_t * cond, pthread_svelte_mutex_t * mutex, const struct timespec * abstime) {
    if (abstime == NULL) {
        LOG_ERROR("The value specified by abstime is invalid.");
        return EINVAL;
    }

    _lock();
    pthread_cond_internal_t * internal = (pthread_cond_internal_t *) cond;

#ifdef EXTRA_ERROR_HANDLING
    if (internal == NULL) {
        _unlock();
        LOG_ERROR("The value specified by cond is invalid (1).");
        return EINVAL;
    }
#endif

    if (is_statically_allocated(cond)) {
        internal->data = _get_free_cond();

#ifdef EXTRA_ERROR_HANDLING
        if (internal->data == NULL) {
            _unlock();
            LOG_ERROR("The value specified by cond was a statically allocated "
                      "condition variable that failed to undergo the actual "
                      "initialization procedure.");
            return EINVAL;
        }
#endif

        internal->data->is_initialized = 1;
    }

    if (_is_valid_cond_ptr(internal->data) == 0) {
        internal->data = _get_free_cond();

#ifdef EXTRA_ERROR_HANDLING
        if (internal->data == NULL) {
            _unlock();
            LOG_ERROR("The value specified by cond was a statically allocated "
                      "condition variable that failed to undergo the actual "
                      "initialization procedure (2).");
            return EINVAL;
        }
#endif

        internal->data->is_initialized = 1;
    }

    if (pthread_svelte_mutex_check_init(mutex) != 0) {
        _unlock();
        LOG_ERROR("The value specified by mutex is invalid.");
        return EINVAL;
    }

#ifdef EXTRA_ERROR_HANDLING
    if (internal->data->mutex != NULL && internal->data->mutex != mutex) {
        _unlock();
        LOG_ERROR("Different mutexes were supplied for concurrent "
                  "pthread_cond_wait() or pthread_cond_timedwait() "
                  "operations on the same condition variable.");
        return EINVAL;
    }
#endif

    internal->data->mutex = mutex;

    uint16_t self = internal->data->waiters; // marks our place in the queue
    internal->data->waiters++;

    uint32_t timeout = 0; // useconds
    struct timespec now;
    clock_gettime(internal->data->clock, &now);

    int seconds = abstime->tv_sec - now.tv_sec;
    int nanoseconds = abstime->tv_nsec - now.tv_nsec;
    timeout = (nanoseconds / 1000) + (seconds * 1000000);

    uint32_t slept_for = 0;
    uint32_t backoff = 100; // us
    while (
            (
                (internal->data->broadcast == 0) ||
                (internal->data->signal == 0) ||
                (internal->data->broadcast == 1 && internal->data->waiters > self) ||
                (internal->data->signal == 1 && internal->data->waiters > self)
            ) &&
            slept_for < timeout
            )
    {
        pthread_svelte_mutex_unlock(mutex);
        _unlock();
        sceKernelDelayThread(backoff);
        slept_for += backoff;
        backoff = backoff + 10;
        _lock();
        pthread_svelte_mutex_lock(mutex);
    }

    if (slept_for >= timeout) {
        _unlock();
        return ETIMEDOUT;
    }

    if (internal->data->signal == 1) {
        // only one waiter receives the signal
        internal->data->signal = 0;
    }

    if (internal->data->waiters == 0) {
        // every waiter got the signal / broadcast at this point
        internal->data->signal = 0;
        internal->data->broadcast = 0;
        internal->data->mutex = NULL;
    } else if (internal->data->broadcast == 1) {
        // allow the next waiter in the queue to wake up in case of broadcast
        internal->data->waiters--;
    }

    _unlock();
    return 0;
}

int pthread_svelte_cond_clockwait(pthread_svelte_cond_t * cond, pthread_svelte_mutex_t * mutex, clockid_t clk, const struct timespec * abstime) {
    if (abstime == NULL) {
        LOG_ERROR("The value specified by abstime is invalid.");
        return EINVAL;
    }

    _lock();
    pthread_cond_internal_t * internal = (pthread_cond_internal_t *) cond;

    if (internal == NULL) {
        _unlock();
        LOG_ERROR("The value specified by cond is invalid (1).");
        return EINVAL;
    }

    if (is_statically_allocated(cond)) {
        internal->data = _get_free_cond();

        if (internal->data == NULL) {
            _unlock();
            LOG_ERROR("The value specified by cond was a statically allocated "
                      "condition variable that failed to undergo the actual "
                      "initialization procedure.");
            return EINVAL;
        }

        internal->data->is_initialized = 1;
    }

    if (_is_valid_cond_ptr(internal->data) == 0) {
        internal->data = _get_free_cond();

        if (internal->data == NULL) {
            _unlock();
            LOG_ERROR("The value specified by cond was a statically allocated "
                      "condition variable that failed to undergo the actual "
                      "initialization procedure (2).");
            return EINVAL;
        }

        internal->data->is_initialized = 1;
    }

    if (pthread_svelte_mutex_check_init(mutex) != 0) {
        _unlock();
        LOG_ERROR("The value specified by mutex is invalid.");
        return EINVAL;
    }

    if (internal->data->mutex != NULL && internal->data->mutex != mutex) {
        _unlock();
        LOG_ERROR("Different mutexes were supplied for concurrent "
                  "pthread_cond_wait() or pthread_cond_timedwait() "
                  "operations on the same condition variable.");
        return EINVAL;
    }

    internal->data->mutex = mutex;

    uint16_t self = internal->data->waiters; // marks our place in the queue
    internal->data->waiters++;

    uint32_t timeout = 0; // useconds
    struct timespec now;
    clock_gettime(clk, &now);

    int seconds = abstime->tv_sec - now.tv_sec;
    int nanoseconds = abstime->tv_nsec - now.tv_nsec;
    timeout = (nanoseconds / 1000) + (seconds * 1000000);

    uint32_t slept_for = 0;
    uint32_t backoff = 100; // us
    while (
            (
                    (internal->data->broadcast == 0) ||
                    (internal->data->signal == 0) ||
                    (internal->data->broadcast == 1 && internal->data->waiters > self) ||
                    (internal->data->signal == 1 && internal->data->waiters > self)
            ) &&
            slept_for < timeout
            )
    {
        pthread_svelte_mutex_unlock(mutex);
        _unlock();
        sceKernelDelayThread(backoff);
        slept_for += backoff;
        backoff = backoff + 10;
        _lock();
        pthread_svelte_mutex_lock(mutex);
    }

    if (slept_for >= timeout) {
        _unlock();
        return ETIMEDOUT;
    }

    if (internal->data->signal == 1) {
        // only one waiter receives the signal
        internal->data->signal = 0;
    }

    if (internal->data->waiters == 0) {
        // every waiter got the signal / broadcast at this point
        internal->data->signal = 0;
        internal->data->broadcast = 0;
        internal->data->mutex = NULL;
    } else if (internal->data->broadcast == 1) {
        // allow the next waiter in the queue to wake up in case of broadcast
        internal->data->waiters--;
    }

    _unlock();
    return 0;
}

int pthread_svelte_condattr_init(pthread_svelte_condattr_t* attr) {
    *attr = 0;
    *attr |= PTHREAD_SVELTE_PROCESS_PRIVATE;
    *attr |= (CLOCK_REALTIME << 1);
    return 0;
}

int pthread_svelte_condattr_getpshared(const pthread_svelte_condattr_t* attr, int* pshared) {
    *pshared = (int) COND_IS_SHARED(*attr);
    return 0;
}

int pthread_svelte_condattr_setpshared(pthread_svelte_condattr_t* attr, int pshared) {
    if (pshared != PTHREAD_SVELTE_PROCESS_SHARED && pshared != PTHREAD_SVELTE_PROCESS_PRIVATE) {
        return EINVAL;
    }

    *attr |= pshared;
    return 0;
}

int pthread_svelte_condattr_getclock(const pthread_svelte_condattr_t* attr, clockid_t* clock) {
    *clock = COND_GET_CLOCK(*attr);
    return 0;
}

int pthread_svelte_condattr_setclock(pthread_svelte_condattr_t* attr, clockid_t clock) {
    if (clock != CLOCK_MONOTONIC && clock != CLOCK_REALTIME) {
        return EINVAL;
    }

    *attr = COND_SET_CLOCK(*attr, clock);
    return 0;
}

int pthread_svelte_condattr_destroy(pthread_svelte_condattr_t* attr) {
    *attr = MAGIC_NOT_INITED;
    return 0;
}
