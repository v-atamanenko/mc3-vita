#include "_pthread-svelte_internal.h"

#define MUTEXATTR_TYPE_MASK     0x000f
#define MUTEXATTR_SHARED_MASK   0x0010
#define MUTEXATTR_PROTOCOL_MASK 0x0020

static const pthread_svelte_mutex_t _mutex_static_initializer = PTHREAD_SVELTE_MUTEX_INITIALIZER;
static const pthread_svelte_mutex_t _mutex_recursive_static_initializer = PTHREAD_SVELTE_RECURSIVE_MUTEX_INITIALIZER_NP;
static const pthread_svelte_mutex_t _mutex_errorcheck_static_initializer = PTHREAD_SVELTE_ERRORCHECK_MUTEX_INITIALIZER_NP;
ALWAYS_INLINE int is_statically_allocated(const pthread_svelte_mutex_t * mutex) {
    if (sceClibMemcmp(&_mutex_static_initializer, mutex, sizeof(pthread_svelte_mutex_t)) == 0) {
        return PTHREAD_SVELTE_MUTEX_NORMAL;
    }

    if (sceClibMemcmp(&_mutex_recursive_static_initializer, mutex, sizeof(pthread_svelte_mutex_t)) == 0) {
        return PTHREAD_SVELTE_MUTEX_RECURSIVE;
    }

    if (sceClibMemcmp(&_mutex_errorcheck_static_initializer, mutex, sizeof(pthread_svelte_mutex_t)) == 0) {
        return PTHREAD_SVELTE_MUTEX_ERRORCHECK;
    }

    return -1;
}

static SceKernelLwMutexWork _mother_mutex = {{ MAGIC_NOT_INITED }};

#define MAX_MUTEX_NUMBER 3072
#define MUTEX_ARRAY_SIZE sizeof(mutex_object_t) * MAX_MUTEX_NUMBER

static mutex_object_t g_mutexes[MAX_MUTEX_NUMBER];

ALWAYS_INLINE void _mother_mutex_check_init() {
    if (_mother_mutex.data[0] == MAGIC_NOT_INITED) {
        sceKernelCreateLwMutex(&_mother_mutex, "pthread_mutex", 0, 0, NULL);
        sceKernelLockLwMutex(&_mother_mutex, 1, NULL);
        sceClibMemset(g_mutexes, 0, MUTEX_ARRAY_SIZE);
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

ALWAYS_INLINE uint8_t _is_valid_mutex_ptr(mutex_object_t * ptr) {
    return ptr >= &g_mutexes[0] && ptr < (&g_mutexes[0] + MUTEX_ARRAY_SIZE);
}

ALWAYS_INLINE mutex_object_t * _get_free_mutex() {
    for (int i = 0; i < MAX_MUTEX_NUMBER; ++i) {
        if (g_mutexes[i].is_initialized == 0) {
            return &g_mutexes[i];
        }
    }
    return NULL;
}

int pthread_svelte_mutex_check_init(pthread_svelte_mutex_t *mutex) {
    pthread_mutex_internal_t * internal = (pthread_mutex_internal_t *) mutex;

    if (internal == NULL) {
        LOG_ERROR("The value specified by mutex is NULL (check_init).");
        return EINVAL;
    }

    _lock();

    int t = is_statically_allocated(mutex);

    if (t == -1 && _is_valid_mutex_ptr(internal->data) == 0) {
        // most likely, malloc artifact
        t = PTHREAD_SVELTE_MUTEX_NORMAL;
    }

    if (t != -1) {
        internal->data = _get_free_mutex();

        if (internal->data == NULL) {
            _unlock();
            LOG_ERROR("The value specified by mutex was a statically allocated "
                      "mutex that failed to undergo the actual "
                      "initialization procedure (check_init).");
            return EINVAL;
        }

        internal->data->owner = -1;
        internal->data->type = t;
        internal->data->is_initialized = 1;

        _unlock();
        return 0;
    }

    _unlock();
    return 0;
}

int pthread_svelte_mutex_init(pthread_svelte_mutex_t *mutex, const pthread_svelte_mutexattr_t *attr) {
    _lock();
    pthread_mutex_internal_t * internal = (pthread_mutex_internal_t *) mutex;

#ifdef EXTRA_ERROR_HANDLING
    if (is_statically_allocated(mutex) != -1) {
        _unlock();
        LOG_INFO("An attempt to initialize a statically allocated mutex,"
                 "ignoring.");
        return 0;
    } else if (_is_valid_mutex_ptr(internal->data) == 1) {
        _unlock();
        LOG_ERROR("The implementation has detected an attempt to reinitialise "
                  "the object referenced by mutex, a previously initialised, "
                  "but not yet destroyed, mutex.");
        return EBUSY;
    }
#endif

    internal->data = _get_free_mutex();

#ifdef EXTRA_ERROR_HANDLING
    if (internal->data == NULL) {
        _unlock();
        LOG_ERROR("Insufficient memory exists to initialise the mutex.");
        return ENOMEM;
    }
#endif

    internal->data->type = PTHREAD_SVELTE_MUTEX_NORMAL;
    if (attr != NULL)
        pthread_svelte_mutexattr_gettype(attr, &internal->data->type);

    internal->data->is_initialized = 1;
    internal->data->owner = -1;

    _unlock();
    LOG_INFO("Mutex initialization completed.");
    return 0;
}

int pthread_svelte_mutex_destroy(pthread_svelte_mutex_t *mutex) {
    pthread_mutex_internal_t * internal = (pthread_mutex_internal_t *) mutex;

#ifdef EXTRA_ERROR_HANDLING
    if (internal == NULL) {
        LOG_ERROR("The value specified by mutex is invalid (1).");
        return EINVAL;
    }
#endif

    _lock();

#ifdef EXTRA_ERROR_HANDLING
    if (_is_valid_mutex_ptr(internal->data) == 0) {
        _unlock();
        LOG_ERROR("The value specified by mutex is invalid (2).");
        return EINVAL;
    }
#endif

    sceClibMemset(internal->data, 0, sizeof(mutex_object_t));

    _unlock();
    LOG_INFO("Mutex destroyed successfully.");
    return 0;
}

int pthread_svelte_mutex_lock(pthread_svelte_mutex_t *mutex) {
    pthread_mutex_internal_t * internal = (pthread_mutex_internal_t *) mutex;

#ifdef EXTRA_ERROR_HANDLING
    if (internal == NULL) {
        LOG_ERROR("The value specified by mutex does not refer to "
                  "an initialised mutex object. (1)");
        return EINVAL;
    }
#endif

    _lock();

    int t = is_statically_allocated(mutex);
    if (t != -1) {
        LOG_INFO("Statically allocated mutex detected, will perform actual "
                 "initialization now.");

        internal->data = _get_free_mutex();

#ifdef EXTRA_ERROR_HANDLING
        if (internal->data == NULL) {
            _unlock();
            LOG_ERROR("The value specified by mutex was a statically allocated "
                      "mutex that failed to undergo the actual "
                      "initialization procedure.");
            return EINVAL;
        }
#endif

        internal->data->owner = -1;
        internal->data->type = t;
        internal->data->is_initialized = 1;
    }

#ifdef EXTRA_ERROR_HANDLING
    if (!_is_valid_mutex_ptr(internal->data)) {
        _unlock();
        LOG_ERROR("The value specified by mutex does not refer to "
                  "an initialised mutex object. (2)");
        return EINVAL;
    }
#endif

    int self = sceKernelGetThreadId();

#ifdef EXTRA_ERROR_HANDLING
    if (internal->data->owner == -1) {
        internal->data->owner = self;
        internal->data->lock_count++;

        _unlock();
        return 0;
    }

    if (internal->data->owner == self) {
        internal->data->lock_count++;

        if (internal->data->type == PTHREAD_SVELTE_MUTEX_RECURSIVE) {
            _unlock();
            return 0;
        } else {
            _unlock();
            LOG_ERROR("The current thread already owns the mutex: deadlock.");
            return EDEADLK;
        }
    }
#else
    if (internal->data->owner == -1 || internal->data->owner == self) {
        internal->data->owner = self;
        internal->data->lock_count++;

        _unlock();
        return 0;
    }
#endif

    // owner != self

    uint32_t backoff = 100; // us
    while (internal->data->lock_count != 0) {
        _unlock();
        sceKernelDelayThread(backoff);
        backoff = backoff + 8;
        _lock();
    }

    internal->data->owner = self;
    internal->data->lock_count++;

    _unlock();
    return 0;
}

int pthread_svelte_mutex_trylock(pthread_svelte_mutex_t *mutex) {
    pthread_mutex_internal_t * internal = (pthread_mutex_internal_t *) mutex;

#ifdef EXTRA_ERROR_HANDLING
    if (internal == NULL) {
        LOG_ERROR("The value specified by mutex does not refer to "
                  "an initialised mutex object. (1)");
        return EINVAL;
    }
#endif

    _lock();

    int t = is_statically_allocated(mutex);
    if (t != -1) {
        LOG_INFO("Statically allocated mutex detected, will perform actual "
                 "initialization now.");

        internal->data = _get_free_mutex();

#ifdef EXTRA_ERROR_HANDLING
        if (internal->data == NULL) {
            _unlock();
            LOG_ERROR("The value specified by mutex was a statically allocated "
                      "mutex that failed to undergo the actual "
                      "initialization procedure.");
            return EINVAL;
        }
#endif

        internal->data->owner = -1;
        internal->data->type = t;
        internal->data->is_initialized = 1;
    }

#ifdef EXTRA_ERROR_HANDLING
    if (!_is_valid_mutex_ptr(internal->data)) {
        _unlock();
        LOG_ERROR("The value specified by mutex does not refer to "
                  "an initialised mutex object. (2)");
        return EINVAL;
    }
#endif

    int self = sceKernelGetThreadId();

    if (internal->data->owner == -1) {
        internal->data->owner = self;
        internal->data->lock_count++;

        _unlock();
        return 0;
    }

    if (internal->data->owner == self) {
        if (internal->data->type == PTHREAD_SVELTE_MUTEX_RECURSIVE) {
            internal->data->lock_count++;
            _unlock();
            return 0;
        } else {
            _unlock();
            // The mutex could not be acquired because it was already locked.
            return EBUSY;
        }
    }

    // owner != self

    _unlock();
    // The current thread does not own the mutex.
    return EPERM;
}

int pthread_svelte_mutex_unlock(pthread_svelte_mutex_t *mutex) {
    pthread_mutex_internal_t * internal = (pthread_mutex_internal_t *) mutex;

#ifdef EXTRA_ERROR_HANDLING
    if (internal == NULL) {
        LOG_ERROR("The value specified by mutex does not refer to "
                  "an initialised mutex object (1).");
        return EINVAL;
    }
#endif

    _lock();

#ifdef EXTRA_ERROR_HANDLING
    if (_is_valid_mutex_ptr(internal->data) == 0) {
        _unlock();
        LOG_ERROR("The value specified by mutex does not refer to "
                  "an initialised mutex object (2).");
        return EINVAL;
    }
#endif

    int self = sceKernelGetThreadId();

#ifdef EXTRA_ERROR_HANDLING
    if (internal->data->owner != self) {
        _unlock();

        LOG_ERROR("The current thread does not own the mutex.");
        return EPERM;
    }
#endif

#ifdef EXTRA_ERROR_HANDLING
    if (internal->data->lock_count == 0) {
        _unlock();
        LOG_ERROR("An attempt to unlock an already unlocked mutex.");
        return EINVAL;
    }
#endif

    if (--internal->data->lock_count == 0) {
        internal->data->owner = -1;
    }

    _unlock();
    return 0;
}

int pthread_svelte_mutex_getprioceiling(const pthread_svelte_mutex_t *mutex, int *prioceiling) {
    return ENOSYS;
}

int pthread_svelte_mutex_setprioceiling(pthread_svelte_mutex_t *mutex, int prioceiling, int *old_ceiling) {
    return ENOSYS;
}

int pthread_svelte_mutexattr_init(pthread_svelte_mutexattr_t *attr) {
    *attr = PTHREAD_SVELTE_MUTEX_DEFAULT;
    return 0;
}

int pthread_svelte_mutexattr_destroy(pthread_svelte_mutexattr_t *attr) {
    *attr = -1;
    return 0;
}

int pthread_svelte_mutexattr_gettype(const pthread_svelte_mutexattr_t *attr, int *type_p) {
    int type = (*attr & MUTEXATTR_TYPE_MASK);

    if (type < PTHREAD_SVELTE_MUTEX_NORMAL || type > PTHREAD_SVELTE_MUTEX_ERRORCHECK) {
        return EINVAL;
    }

    *type_p = type;
    return 0;
}

int pthread_svelte_mutexattr_settype(pthread_svelte_mutexattr_t *attr, int type) {
    if (type < PTHREAD_SVELTE_MUTEX_NORMAL || type > PTHREAD_SVELTE_MUTEX_ERRORCHECK ) {
        return EINVAL;
    }

    *attr = (*attr & ~MUTEXATTR_TYPE_MASK) | type;
    return 0;
}

int pthread_svelte_mutexattr_setpshared(pthread_svelte_mutexattr_t *attr, int  pshared) {
    // Not supported but irrelevant on the Vita anyway
    return 0;
}

int pthread_svelte_mutexattr_getpshared(const pthread_svelte_mutexattr_t* attr, int* pshared) {
    // Not supported but irrelevant on the Vita anyway
    *pshared = PTHREAD_SVELTE_PROCESS_PRIVATE;
    return 0;
}

int pthread_svelte_mutexattr_setprotocol(pthread_svelte_mutexattr_t* attr, int protocol) {
    // Not supported but insignificant in most cases so stub instead of ENOSYS
    return 0;
}

int pthread_svelte_mutexattr_getprotocol(const pthread_svelte_mutexattr_t* attr, int* protocol) {
    // Not supported but insignificant in most cases so stub instead of ENOSYS
    *protocol = PTHREAD_SVELTE_PRIO_NONE;
    return 0;
}

int pthread_svelte_mutexattr_setprioceiling(pthread_svelte_mutexattr_t *attr, int prioceiling) {
    // Not supported but insignificant in most cases so stub instead of ENOSYS
    return 0;
}

int pthread_svelte_mutexattr_getprioceiling(const pthread_svelte_mutexattr_t *attr, int *prioceiling) {
    // Not supported but insignificant in most cases so stub instead of ENOSYS
    *prioceiling = 128;
    return 0;
}
