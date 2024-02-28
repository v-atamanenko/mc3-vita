#include "_pthread-svelte_internal.h"

static const pthread_svelte_cond_t _cond_static_initializer = PTHREAD_SVELTE_COND_INITIALIZER;
ALWAYS_INLINE int is_statically_allocated(const pthread_svelte_cond_t * cond) {
    return memcmp(&_cond_static_initializer, cond, sizeof(pthread_svelte_cond_t)) == 0;
}

static const pthread_svelte_mutex_t _mutex_static_initializer = PTHREAD_SVELTE_MUTEX_INITIALIZER;
static const pthread_svelte_mutex_t _mutex_recursive_static_initializer = PTHREAD_SVELTE_RECURSIVE_MUTEX_INITIALIZER_NP;
static const pthread_svelte_mutex_t _mutex_errorcheck_static_initializer = PTHREAD_SVELTE_ERRORCHECK_MUTEX_INITIALIZER_NP;

ALWAYS_INLINE int is_statically_allocated_mutex(const pthread_svelte_mutex_t * mutex) {
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

int pthread_svelte_cond_init(pthread_svelte_cond_t * cond, const pthread_svelte_condattr_t * attr) {
    pthread_cond_internal_t * internal = (pthread_cond_internal_t *) cond; // acquire `internal`

    if (internal->data != NULL && is_statically_allocated(cond)) {
        LOG_INFO("An attempt to initialize a statically allocated condition "
                 "variable, ignoring.");
        return 0;
    } /* else if (internal->data != NULL) {
        LOG_ERROR("The implementation has detected an attempt to reinitialise "
                  "the object referenced by cond, a previously initialised, "
                  "but not yet destroyed, condition variable. "
                  "Contents: 0x%x", internal->data);
        return EBUSY;
    } */ // Commented this out, because if we really wanna do this, we need to
         // keep track of all objects we *really* allocated, otherwise we will
         // end up confusing uninitialised malloc() memory with existing objects

    internal->data = malloc(sizeof(cond_object_t));

    if (internal->data == NULL) {
        LOG_ERROR("Insufficient memory exists to initialize "
                  "the condition variable.");
        return ENOMEM;
    }

    if (attr != NULL) internal->data->clock = COND_GET_CLOCK(*attr);

    // No actual initialization SceKernelLwCondWork initialization happens here,
    // since it would require a mutex which we will only get access to in later
    // calls due to the architecture of pthread.
    internal->data->magic = MAGIC_NOT_INITED;
    internal->data->sce_mutex = NULL;

    LOG_INFO("Preinitialization of a condition variable succeeded.");

    return 0;
}

int pthread_svelte_cond_destroy(pthread_svelte_cond_t * cond) {
    pthread_cond_internal_t * internal = (pthread_cond_internal_t *) cond; // acquire `internal`

    if (internal == NULL)
        return EINVAL;

    if (internal->data == NULL)
        return EINVAL;

    if (is_statically_allocated(cond)) {
        return 0;
    }

    if (internal->data->magic == MAGIC_INITED) {
        sceKernelDeleteLwCond(&internal->data->sce_cond);
        // TODO: Extra debugging options
    }

    free(internal->data);
    internal->data = NULL;

    return 0;
}

int pthread_svelte_cond_signal(pthread_svelte_cond_t * cond) {
    pthread_cond_internal_t * internal = (pthread_cond_internal_t *) cond; // acquire `internal`

    if (internal == NULL) {
        LOG_ERROR("The value specified by cond is invalid (1).");
        return EINVAL;
    }

    if (internal->data == NULL) {
        LOG_INFO("The value specified by cond is an initialized but never used "
                 "condition variable (1).");
        return 0;
    }

    if (internal->data->magic != MAGIC_INITED) {
        LOG_INFO("The value specified by cond is an initialized but never used "
                 "condition variable (2).");
        return 0;
    }

    sceKernelSignalLwCond(&internal->data->sce_cond);

    // TODO: Extra debugging options

    LOG_INFO("Condition variable signal completed.");
    return 0;
}

int pthread_svelte_cond_broadcast(pthread_svelte_cond_t * cond) {
    pthread_cond_internal_t * internal = (pthread_cond_internal_t *) cond; // acquire `internal`

    if (internal == NULL) {
        LOG_ERROR("The value specified by cond is invalid (1).");
        return EINVAL;
    }

    if (internal->data == NULL) {
        LOG_INFO("The value specified by cond is an initialized but never used "
                 "condition variable (1).");
        return 0;
    }

    if (internal->data->magic != MAGIC_INITED) {
        LOG_INFO("The value specified by cond is an initialized but never used "
                 "condition variable (2).");
        return 0;
    }

    sceKernelSignalLwCondAll(&internal->data->sce_cond);

    // TODO: Extra debugging options

    LOG_INFO("Condition variable broadcast completed.");
    return 0;
}

int pthread_svelte_cond_wait(pthread_svelte_cond_t * cond, pthread_svelte_mutex_t * mutex) {
    pthread_cond_internal_t * internal = (pthread_cond_internal_t *) cond; // acquire `internal`
    pthread_mutex_internal_t * mut_internal = (pthread_mutex_internal_t *) mutex; // acquire `mut_internal`

    if (internal == NULL) {
        LOG_ERROR("The value specified by cond is invalid (1).");
        return EINVAL;
    }

    if (is_statically_allocated(cond)) {
        internal->data = NULL;
        if (pthread_svelte_cond_init(cond, NULL) != 0) {
            LOG_ERROR("The value specified by cond was a statically allocated "
                      "condition variable that failed to undergo the actual "
                      "initialization procedure.");
            return EINVAL;
        }
    }

    if (internal->data == NULL) {
        LOG_ERROR("The value specified by cond is invalid (2).");
        return EINVAL;
    }

    if (mut_internal == NULL) {
        LOG_ERROR("The value specified by mutex is invalid.");
        return EINVAL;
    }

    int t = is_statically_allocated_mutex(mutex);
    if (mut_internal->data == NULL) {
        if (pthread_svelte_mutex_init(mutex, NULL) != 0) {
            LOG_ERROR("The value specified by mutex was a statically allocated "
                      "mutex that failed to undergo the actual "
                      "initialization procedure.");
            return EINVAL;
        }
    } else if (t != -1) {
        LOG_INFO("Statically allocated mutex detected, will perform actual "
                 "initialization now.");

        mut_internal->data = NULL;

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

    int reinitialization_required = 0;

    if (internal->data->magic == MAGIC_INITED) {
        if (internal->data->sce_mutex != NULL) {
            pthread_mutex_internal_t * old_mutex = (pthread_mutex_internal_t *) internal->data->sce_mutex;
            if (&old_mutex->data->sce_mutex != &mut_internal->data->sce_mutex) {
                SceKernelLwCondInfo info;
                sceKernelGetLwCondInfo(&internal->data->sce_cond, &info);
                if (info.numWaitThreads > 0) {
                    LOG_ERROR("Different mutexes were supplied for concurrent "
                              "pthread_cond_wait() or pthread_cond_timedwait() "
                              "operations on the same condition variable.");
                    return EINVAL;
                } else {
                    reinitialization_required = 1;
                }
            }
        }
    }

    if (internal->data->magic == MAGIC_NOT_INITED || reinitialization_required) {
        if (internal->data->magic == MAGIC_INITED) {
            // In case there is a new mutex to be assigned to an old cond, we
            // need to recreate the kernel object
            sceKernelDeleteLwCond(&internal->data->sce_cond);
        }

        internal->data->sce_mutex = &mut_internal->data->sce_mutex;

        sceKernelCreateLwCond(&internal->data->sce_cond, "PTHREAD_COND",
                              SCE_KERNEL_LW_COND_ATTR_TH_FIFO,
                              internal->data->sce_mutex, NULL);
    }

    sceKernelWaitLwCond(&internal->data->sce_cond, NULL);

    // TODO: Extra debugging options

    LOG_INFO("Condition variable wait completed.");

    return 0;
}

int pthread_svelte_cond_timedwait(pthread_svelte_cond_t * cond, pthread_svelte_mutex_t * mutex, const struct timespec * abstime) {
    pthread_cond_internal_t * internal = (pthread_cond_internal_t *) cond; // acquire `internal`
    pthread_mutex_internal_t * mut_internal = (pthread_mutex_internal_t *) mutex; // acquire `mut_internal`

    if (internal == NULL) {
        LOG_ERROR("The value specified by cond is invalid (1).");
        return EINVAL;
    }

    if (is_statically_allocated(cond)) {
        internal->data = NULL;
        if (pthread_svelte_cond_init(cond, NULL) != 0) {
            LOG_ERROR("The value specified by cond was a statically allocated "
                      "condition variable that failed to undergo the actual "
                      "initialization procedure.");
            return EINVAL;
        }
    }

    if (internal->data == NULL) {
        LOG_ERROR("The value specified by cond is invalid (2).");
        return EINVAL;
    }

    if (mut_internal == NULL) {
        LOG_ERROR("The value specified by mutex is invalid.");
        return EINVAL;
    }

    if (abstime == NULL) {
        LOG_ERROR("The value specified by abstime is invalid.");
        return EINVAL;
    }

    int t = is_statically_allocated_mutex(mutex);
    if (mut_internal->data == NULL) {
        if (pthread_svelte_mutex_init(mutex, NULL) != 0) {
            LOG_ERROR("The value specified by mutex was a statically allocated "
                      "mutex that failed to undergo the actual "
                      "initialization procedure.");
            return EINVAL;
        }
    } else if (t != -1) {
        LOG_INFO("Statically allocated mutex detected, will perform actual "
                 "initialization now.");

        mut_internal->data = NULL;

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

    int reinitialization_required = 0;

    if (internal->data->magic == MAGIC_INITED) {
        if (internal->data->sce_mutex != NULL) {
            pthread_mutex_internal_t * old_mutex = (pthread_mutex_internal_t *) internal->data->sce_mutex;
            if (&old_mutex->data->sce_mutex != &mut_internal->data->sce_mutex) {
                SceKernelLwCondInfo info;
                sceKernelGetLwCondInfo(&internal->data->sce_cond, &info);
                if (info.numWaitThreads > 0) {
                    LOG_ERROR("Different mutexes were supplied for concurrent "
                              "pthread_cond_wait() or pthread_cond_timedwait() "
                              "operations on the same condition variable.");
                    return EINVAL;
                } else {
                    reinitialization_required = 1;
                }
            }
        }
    }

    if (internal->data->magic == MAGIC_NOT_INITED || reinitialization_required) {
        if (internal->data->magic == MAGIC_INITED) {
            // In case there is a new mutex to be assigned to an old cond, we
            // need to recreate the kernel object
            sceKernelDeleteLwCond(&internal->data->sce_cond);
        }

        internal->data->sce_mutex = &mut_internal->data->sce_mutex;

        sceKernelCreateLwCond(&internal->data->sce_cond, "PTHREAD_COND",
                              SCE_KERNEL_LW_COND_ATTR_TH_FIFO,
                              internal->data->sce_mutex, NULL);
    }

    uint32_t timeout = 0; // useconds
    struct timespec now;
    clock_gettime(internal->data->clock, &now);

    int seconds = abstime->tv_sec - now.tv_sec;
    int nanoseconds = abstime->tv_nsec - now.tv_nsec;
    timeout = (nanoseconds / 1000) + (seconds * 1000000);

    int ret = sceKernelWaitLwCond(&internal->data->sce_cond, &timeout);

    if (ret == SCE_KERNEL_ERROR_WAIT_TIMEOUT) {
        LOG_INFO("Condition variable timed out.");
        return ETIMEDOUT;
    }

    // TODO: Extra debugging options

    LOG_INFO("Condition variable timed wait completed.");
    return 0;
}

int pthread_svelte_cond_clockwait(pthread_svelte_cond_t * cond, pthread_svelte_mutex_t * mutex, clockid_t clk, const struct timespec * abstime) {
    pthread_cond_internal_t * internal = (pthread_cond_internal_t *) cond; // acquire `internal`
    pthread_mutex_internal_t * mut_internal = (pthread_mutex_internal_t *) mutex; // acquire `mut_internal`

    if (internal == NULL)
        return EINVAL;

    if (is_statically_allocated(cond)) {
        internal->data = NULL;
        if (pthread_svelte_cond_init(cond, NULL) != 0)
            return EINVAL;
    }

    if (internal->data == NULL)
        return EINVAL;

    if (mut_internal == NULL)
        return EINVAL;

    if (abstime == NULL)
        return EINVAL;

    if (mut_internal->data == NULL)
        pthread_svelte_mutex_init(mutex, NULL);

    int reinitialization_required = 0;

    if (internal->data->magic == MAGIC_INITED) {
        if (internal->data->sce_mutex != NULL) {
            pthread_mutex_internal_t * old_mutex = (pthread_mutex_internal_t *) internal->data->sce_mutex;
            if (&old_mutex->data->sce_mutex != &mut_internal->data->sce_mutex) {
                SceKernelLwCondInfo info;
                sceKernelGetLwCondInfo(&internal->data->sce_cond, &info);
                if (info.numWaitThreads > 0) {
                    // Different mutexes were supplied for concurrent
                    // pthread_svelte_cond_wait() or pthread_svelte_cond_timedwait()
                    // operations on the same condition variable.
                    return EINVAL;
                } else {
                    reinitialization_required = 1;
                }
            }
        }
    }

    if (internal->data->magic == MAGIC_NOT_INITED || reinitialization_required) {
        if (internal->data->magic == MAGIC_INITED) {
            // In case there is a new mutex to be assigned to an old cond, we
            // need to recreate the kernel object
            sceKernelDeleteLwCond(&internal->data->sce_cond);
        }

        internal->data->sce_mutex = &mut_internal->data->sce_mutex;

        sceKernelCreateLwCond(&internal->data->sce_cond, "PTHREAD_COND",
                              SCE_KERNEL_LW_COND_ATTR_TH_FIFO,
                              internal->data->sce_mutex, NULL);
    }

    uint32_t timeout = 0; // useconds
    struct timespec now;
    clock_gettime(clk, &now);

    int seconds = abstime->tv_sec - now.tv_sec;
    int nanoseconds = abstime->tv_nsec - now.tv_nsec;
    timeout = (nanoseconds / 1000) + (seconds * 1000000);

    int ret = sceKernelWaitLwCond(&internal->data->sce_cond, &timeout);

    if (ret == SCE_KERNEL_ERROR_WAIT_TIMEOUT)
        return ETIMEDOUT;

    // TODO: Extra debugging options

    return 0;
}
