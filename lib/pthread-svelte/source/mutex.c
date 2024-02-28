#include "_pthread-svelte_internal.h"

#define MUTEXATTR_TYPE_MASK     0x000f
#define MUTEXATTR_SHARED_MASK   0x0010
#define MUTEXATTR_PROTOCOL_MASK 0x0020

static const pthread_svelte_mutex_t _mutex_static_initializer = PTHREAD_SVELTE_MUTEX_INITIALIZER;
static const pthread_svelte_mutex_t _mutex_recursive_static_initializer = PTHREAD_SVELTE_RECURSIVE_MUTEX_INITIALIZER_NP;
static const pthread_svelte_mutex_t _mutex_errorcheck_static_initializer = PTHREAD_SVELTE_ERRORCHECK_MUTEX_INITIALIZER_NP;

ALWAYS_INLINE int is_statically_allocated(const pthread_svelte_mutex_t * mutex) {
    if (memcmp(&_mutex_static_initializer, mutex, sizeof(pthread_svelte_mutex_t)) == 0) {
        return PTHREAD_SVELTE_MUTEX_NORMAL;
    }

    if (memcmp(&_mutex_recursive_static_initializer, mutex, sizeof(pthread_svelte_mutex_t)) == 0) {
        return PTHREAD_SVELTE_MUTEX_RECURSIVE;
    }

    if (memcmp(&_mutex_errorcheck_static_initializer, mutex, sizeof(pthread_svelte_mutex_t)) == 0) {
        return PTHREAD_SVELTE_MUTEX_ERRORCHECK;
    }

    return -1;
}

static int mutcount = 0;

int pthread_svelte_mutex_init(pthread_svelte_mutex_t *mutex, const pthread_svelte_mutexattr_t *attr) {
    pthread_mutex_internal_t * internal = (pthread_mutex_internal_t *) mutex; // acquire `internal`

    if (internal->data != NULL && is_statically_allocated(mutex) != -1) {
        LOG_INFO("An attempt to initialize a statically allocated mutex,"
                 "ignoring.");
        return 0;
    } /* else if (internal->data != NULL) {
        LOG_ERROR("The implementation has detected an attempt to reinitialise "
                  "the object referenced by mutex, a previously initialised, "
                  "but not yet destroyed, mutex.");
        return EBUSY;
    } */ // Commented this out, because if we really wanna do this, we need to
        // keep track of all objects we *really* allocated, otherwise we will
        // end up confusing uninitialised malloc() memory with existing objects

    internal->data = malloc(sizeof(mutex_object_t));

    if (internal->data == NULL) {
        LOG_ERROR("Insufficient memory exists to initialise the mutex.");
        return ENOMEM;
    }

    internal->data->type = PTHREAD_SVELTE_MUTEX_NORMAL;
    if (attr != NULL) pthread_svelte_mutexattr_gettype(attr, &internal->data->type);

    int ret;
    switch (internal->data->type) {
        case PTHREAD_SVELTE_MUTEX_RECURSIVE:
            LOG_INFO("Initializing a recursive mutex.");
            ret = sceKernelCreateLwMutex(&internal->data->sce_mutex,
                                         "PTHREAD_MUTEX_RECURSIVE",
                                         SCE_KERNEL_LW_MUTEX_ATTR_TH_FIFO |
                                         SCE_KERNEL_LW_MUTEX_ATTR_RECURSIVE,
                                         0, NULL);
            break;
        case PTHREAD_SVELTE_MUTEX_ERRORCHECK:
            LOG_INFO("Initializing an errorcheck mutex.");
            ret = sceKernelCreateLwMutex(&internal->data->sce_mutex,
                                         "PTHREAD_MUTEX_ERRORCHECK",
                                         0, 0, NULL);
            break;
        default:
            LOG_INFO("Initializing a normal mutex.");
            ret = sceKernelCreateLwMutex(&internal->data->sce_mutex,
                                         "PTHREAD_MUTEX_NORMAL",
                                         0, 0, NULL);
            break;
    }

    switch (ret) {
    case SCE_KERNEL_ERROR_UID_RL_OVERFLOW:
        LOG_ERROR("Mutex initialization failed: the number of opened UIDs "
                  "exceeds the maximum number");
        return EAGAIN;
    case 0:
        LOG_INFO("Mutex initialization completed.");
        return 0;
    default:
        LOG_ERROR("Mutex initialization failed: unexpected error 0x%x", ret);
        return EAGAIN;
    }
}

int pthread_svelte_mutex_destroy(pthread_svelte_mutex_t *mutex) {
    pthread_mutex_internal_t * internal = (pthread_mutex_internal_t *) mutex; // acquire `internal`

    if (internal == NULL) {
        LOG_ERROR("The value specified by mutex is invalid (1).");
        return EINVAL;
    }

    if (internal->data == NULL || is_statically_allocated(mutex) != -1) {
        return 0;
        LOG_ERROR("The value specified by mutex is invalid (2).");
        return EINVAL;
    }

    int ret = sceKernelDeleteLwMutex(&internal->data->sce_mutex);
    switch (ret) {
    case SCE_KERNEL_ERROR_UNKNOWN_LW_MUTEX_ID:
        LOG_ERROR("Mutex destruction failed: unknown LW mutex ID.");
        ret = EINVAL;
        break;
    case 0:
        LOG_INFO("Mutex destroyed successfully.\n");
        break;
    default:
        LOG_ERROR("Mutex destruction failed: unexpected error 0x%x.", ret);
        ret = 0;
        break;
    }
    mutcount--;
    free(internal->data);
    internal->data = NULL;
    return ret;
}

int pthread_svelte_mutex_lock(pthread_svelte_mutex_t *mutex) {
    LOG_DBG("Called from %p, target %p\n", __builtin_return_address(0), mutex);

    pthread_mutex_internal_t * internal = (pthread_mutex_internal_t *) mutex; // acquire `internal`

    if (internal == NULL) {
        LOG_ERROR("The value specified by mutex does not refer to "
                  "an initialised mutex object.");
        return EINVAL;
    }

    int t = is_statically_allocated(mutex);
    if (t != -1) {
        LOG_INFO("Statically allocated mutex detected, will perform actual "
                 "initialization now.");

        internal->data = NULL;

        pthread_svelte_mutexattr_t a;
        pthread_svelte_mutexattr_init(&a);
        pthread_svelte_mutexattr_settype(&a, t);
        if (pthread_svelte_mutex_init(mutex, &a) != 0) {
            LOG_ERROR("The value specified by mutex was a statically allocated "
                      "mutex that failed to undergo the actual "
                      "initialization procedure.");
            return EINVAL;
        }
    }

    if (internal->data == NULL) {
        LOG_WARN("Data pointer in mutex is NULL, will try to initialize still");
        if (pthread_svelte_mutex_init(mutex, NULL) != 0) {
            LOG_ERROR("The value specified by mutex was a statically allocated "
                      "mutex that failed to undergo the actual "
                      "initialization procedure.");
            return EINVAL;
        }
    }

    if (internal->data->type == PTHREAD_SVELTE_MUTEX_RECURSIVE) {
        LOG_INFO("Trying to lock a recursive mutex.");
        int ret = sceKernelTryLockLwMutex(&internal->data->sce_mutex, 1);

        if (ret == 0) {
            LOG_INFO("Locking a recursive mutex succeeded.");
            return 0;
        }

        switch(ret) {
            case SCE_KERNEL_ERROR_LW_MUTEX_FAILED_TO_OWN:
            case SCE_KERNEL_ERROR_LW_MUTEX_NOT_OWNED:
                LOG_ERROR("Locking a recursive mutex failed: "
                          "The current thread does not own the mutex.");
                return EPERM;
            case SCE_KERNEL_ERROR_LW_MUTEX_LOCK_OVF:
            default:
                LOG_ERROR("Locking a recursive mutex failed: "
                          "The mutex could not be acquired because the "
                          "maximum number of recursive locks for mutex has "
                          "been exceeded (0x%x).", ret);
                return EAGAIN;
        }
    }

    if (internal->data->type == PTHREAD_SVELTE_MUTEX_ERRORCHECK) {
        LOG_INFO("Trying to lock an errorcheck mutex.");
        int ret = sceKernelTryLockLwMutex(&internal->data->sce_mutex, 1);

        if (ret == 0) {
            LOG_INFO("Locking an errorcheck mutex succeeded.");
            return 0;
        }

        switch(ret) {
            case SCE_KERNEL_ERROR_LW_MUTEX_FAILED_TO_OWN:
            case SCE_KERNEL_ERROR_LW_MUTEX_NOT_OWNED:
                LOG_ERROR("Locking an errorcheck mutex failed: "
                          "The current thread does not own the mutex.");
                return EPERM;
            default:
                LOG_ERROR("Locking an errorcheck mutex failed: "
                          "The current thread already owns the mutex.");
                return EDEADLK;
        }
    }

    LOG_INFO("Trying to lock a normal mutex.");
    sceKernelLockLwMutex(&internal->data->sce_mutex, 1, NULL);
    LOG_INFO("Locked a normal mutex.");
    return 0;
}

int pthread_svelte_mutex_trylock(pthread_svelte_mutex_t *mutex) {
    pthread_mutex_internal_t * internal = (pthread_mutex_internal_t *) mutex; // acquire `internal`

    if (internal == NULL)
        return EINVAL;

    int t = is_statically_allocated(mutex);
    if (t != -1) {
        LOG_INFO("Statically allocated mutex detected, will perform actual "
                 "initialization now.");

        internal->data = NULL;

        pthread_svelte_mutexattr_t a;
        pthread_svelte_mutexattr_init(&a);
        pthread_svelte_mutexattr_settype(&a, t);
        if (pthread_svelte_mutex_init(mutex, &a) != 0) {
            LOG_ERROR("The value specified by mutex was a statically allocated "
                      "mutex that failed to undergo the actual "
                      "initialization procedure.");
            return EINVAL;
        }
    }

    if (internal->data == NULL) {
        LOG_WARN("Data pointer in mutex is NULL, will try to initialize still");
        if (pthread_svelte_mutex_init(mutex, NULL) != 0) {
            LOG_ERROR("The value specified by mutex was a statically allocated "
                      "mutex that failed to undergo the actual "
                      "initialization procedure.");
            return EINVAL;
        }
    }

    int ret = sceKernelTryLockLwMutex(&internal->data->sce_mutex, 1);

    if (ret == 0)
        return 0;

    switch(ret) {
        case SCE_KERNEL_ERROR_LW_MUTEX_FAILED_TO_OWN:
        case SCE_KERNEL_ERROR_LW_MUTEX_NOT_OWNED:
            // The current thread does not own the mutex.
            return EPERM;
        case SCE_KERNEL_ERROR_LW_MUTEX_LOCK_OVF:
        default:
            // The mutex could not be acquired because it was already locked.
            return EBUSY;
    }
}

int pthread_svelte_mutex_unlock(pthread_svelte_mutex_t *mutex) {
    pthread_mutex_internal_t * internal = (pthread_mutex_internal_t *) mutex; // acquire `internal`

    if (internal == NULL) {
        LOG_ERROR("The value specified by mutex does not refer to "
                  "an initialised mutex object (1).");
        return EINVAL;
    }

    if (is_statically_allocated(mutex) != -1) {
        LOG_ERROR("The value specified by mutex does not refer to "
                  "an initialised mutex object (2).");
        return EINVAL;
    }

    if (internal->data == NULL) {
        LOG_ERROR("The value specified by mutex does not refer to "
                  "an initialised mutex object (3).");
        return EINVAL;
    }

    int do_remove = 0;

    int ret = sceKernelGetLwMutexInfo(&internal->data->sce_mutex, &internal->data->sce_mutex_info);

    if (ret == 0) {
        if (internal->data->sce_mutex_info.numWaitThreads == 0) {
            do_remove = 1;
        }
    }
    if (do_remove == 1) {
        ret = sceKernelDeleteLwMutex(&internal->data->sce_mutex);
        free(internal->data);
        internal->data = NULL;
        switch (ret) {
        case SCE_KERNEL_ERROR_UNKNOWN_LW_MUTEX_ID:
            LOG_ERROR("Mutex destruction failed: unknown LW mutex ID (2).");
            break;
        case 0:
            LOG_INFO("Mutex destroyed successfully (2).\n");
            break;
        default:
            LOG_ERROR("Mutex destruction failed: unexpected error 0x%x (2).", ret);
            break;
        }
        return 0;
    } else {
        ret = sceKernelUnlockLwMutex(&internal->data->sce_mutex, 1);
        if (ret == 0 || internal->data->type != PTHREAD_SVELTE_MUTEX_ERRORCHECK) {
            LOG_INFO("Mutex unlocked.");
            return 0;
        }

        switch(ret) {
            case SCE_KERNEL_ERROR_LW_MUTEX_FAILED_TO_OWN:
            case SCE_KERNEL_ERROR_LW_MUTEX_NOT_OWNED:
                LOG_ERROR("The current thread does not own the mutex.");
                return EPERM;
            case SCE_KERNEL_ERROR_LW_MUTEX_UNLOCK_UDF:
                // FIXME: This is probably a lie.
                LOG_ERROR("The mutex could not be acquired because "
                          "the maximum number of recursive locks for mutex "
                          "has been exceeded. ");
                return EAGAIN;
            default:
                LOG_INFO("Mutex unlocked.");
                return 0;
        }
    }
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
