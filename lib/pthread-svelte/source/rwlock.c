#include "_pthread-svelte_internal.h"

static const pthread_svelte_rwlock_t _rwlock_static_initializer = PTHREAD_SVELTE_RWLOCK_INITIALIZER;
ALWAYS_INLINE int is_statically_allocated(const pthread_svelte_rwlock_t * rwlock) {
    return memcmp(&_rwlock_static_initializer, rwlock, sizeof(pthread_svelte_rwlock_t)) == 0;
}

int pthread_svelte_rwlockattr_init(pthread_svelte_rwlockattr_t * attr) {
    if (attr != NULL) {
        *attr = (pthread_svelte_rwlockattr_t){0};
    }

    return 0;
}

int pthread_svelte_rwlockattr_destroy(pthread_svelte_rwlockattr_t * attr) {
    return 0;
}

int pthread_svelte_rwlockattr_getpshared(const pthread_svelte_rwlockattr_t * attr, int * pshared) {
    if (attr == NULL)
        return EINVAL;

    if (pshared != NULL) {
        if (*attr == PTHREAD_SVELTE_PROCESS_SHARED || *attr == PTHREAD_SVELTE_PROCESS_PRIVATE)
            *pshared = *attr;
        else
            *pshared = PTHREAD_SVELTE_PROCESS_PRIVATE;
    }

    return 0;
}

int pthread_svelte_rwlockattr_setpshared(pthread_svelte_rwlockattr_t * attr, int pshared) {
    if (attr == NULL || (pshared != PTHREAD_SVELTE_PROCESS_SHARED && pshared != PTHREAD_SVELTE_PROCESS_PRIVATE))
        return EINVAL;

    if (attr != NULL)
        *attr = pshared;

    return 0;
}

int pthread_svelte_rwlock_init(pthread_svelte_rwlock_t * rwlock, const pthread_svelte_rwlockattr_t * attr) {
    pthread_rwlock_internal_t * internal = (pthread_rwlock_internal_t *) rwlock; // acquire `internal`

    if (internal->magic == MAGIC_INITED || is_statically_allocated(rwlock)) {
        // The implementation has detected an attempt to re-initialise
        // the object referenced by `rwlock`, a previously initialised but
        // not yet destroyed read-write lock.
        return EBUSY;
    }

    // We completely ignore `attr` since `pshared` is not applicable to the Vita
    SceUID sce_rwlock = sceKernelCreateRWLock("PTHREAD_RWLOCK", SCE_KERNEL_RW_LOCK_ATTR_TH_FIFO, NULL);

    if (sce_rwlock < 0) {
        internal->magic = MAGIC_NOT_INITED;
        return EAGAIN;
    }

    internal->sce_rwlock = sce_rwlock;
    internal->magic = MAGIC_INITED;

    return 0;
}

int pthread_svelte_rwlock_destroy(pthread_svelte_rwlock_t * rwlock) {
    pthread_rwlock_internal_t * internal = (pthread_rwlock_internal_t *) rwlock; // acquire `internal`

    if (internal->magic == MAGIC_NOT_INITED || is_statically_allocated(rwlock)) {
        // Nothing to destroy here.
        return 0;
    }

    int ret = sceKernelDeleteRWLock(internal->sce_rwlock);

    // TODO: Extra debugging options

    return 0;
}

int pthread_svelte_rwlock_rdlock(pthread_svelte_rwlock_t * rwlock) {
    pthread_rwlock_internal_t * internal = (pthread_rwlock_internal_t *) rwlock; // acquire `internal`

    if (internal->magic == MAGIC_NOT_INITED || is_statically_allocated(rwlock)) {
        SceUID sce_rwlock = sceKernelCreateRWLock("PTHREAD_RWLOCK_STATIC", SCE_KERNEL_RW_LOCK_ATTR_TH_FIFO, NULL);

        if (sce_rwlock < 0) {
            internal->magic = MAGIC_NOT_INITED;
            return EINVAL;
        }

        internal->sce_rwlock = sce_rwlock;
        internal->magic = MAGIC_INITED;
    }

    int ret = sceKernelLockReadRWLock(internal->sce_rwlock, NULL);

    // TODO: Extra debugging options

    return 0;
}

int pthread_svelte_rwlock_tryrdlock(pthread_svelte_rwlock_t * rwlock) {
    pthread_rwlock_internal_t * internal = (pthread_rwlock_internal_t *) rwlock; // acquire `internal`

    if (internal->magic == MAGIC_NOT_INITED || is_statically_allocated(rwlock)) {
        SceUID sce_rwlock = sceKernelCreateRWLock("PTHREAD_RWLOCK_STATIC", SCE_KERNEL_RW_LOCK_ATTR_TH_FIFO, NULL);

        if (sce_rwlock < 0) {
            internal->magic = MAGIC_NOT_INITED;
            return EINVAL;
        }

        internal->sce_rwlock = sce_rwlock;
        internal->magic = MAGIC_INITED;
    }

    int ret = sceKernelTryLockReadRWLock(internal->sce_rwlock);

    // TODO: Extra debugging options
    if (ret == SCE_KERNEL_ERROR_RW_LOCK_FAILED_TO_LOCK) {
        return EBUSY;
    }

    return 0;
}

int pthread_svelte_rwlock_wrlock(pthread_svelte_rwlock_t * rwlock) {
    pthread_rwlock_internal_t * internal = (pthread_rwlock_internal_t *) rwlock; // acquire `internal`

    if (internal->magic == MAGIC_NOT_INITED || is_statically_allocated(rwlock)) {
        SceUID sce_rwlock = sceKernelCreateRWLock("PTHREAD_RWLOCK_STATIC", SCE_KERNEL_RW_LOCK_ATTR_TH_FIFO, NULL);

        if (sce_rwlock < 0) {
            internal->magic = MAGIC_NOT_INITED;
            return EINVAL;
        }

        internal->sce_rwlock = sce_rwlock;
        internal->magic = MAGIC_INITED;
    }

    int ret = sceKernelLockWriteRWLock(internal->sce_rwlock, NULL);

    // TODO: Extra debugging options

    return 0;
}

int pthread_svelte_rwlock_trywrlock(pthread_svelte_rwlock_t * rwlock) {
    pthread_rwlock_internal_t * internal = (pthread_rwlock_internal_t *) rwlock; // acquire `internal`

    if (internal->magic == MAGIC_NOT_INITED || is_statically_allocated(rwlock)) {
        SceUID sce_rwlock = sceKernelCreateRWLock("PTHREAD_RWLOCK_STATIC", SCE_KERNEL_RW_LOCK_ATTR_TH_FIFO, NULL);

        if (sce_rwlock < 0) {
            internal->magic = MAGIC_NOT_INITED;
            return EINVAL;
        }

        internal->sce_rwlock = sce_rwlock;
        internal->magic = MAGIC_INITED;
    }

    int ret = sceKernelTryLockWriteRWLock(internal->sce_rwlock);

    // TODO: Extra debugging options
    if (ret == SCE_KERNEL_ERROR_RW_LOCK_FAILED_TO_LOCK) {
        return EBUSY;
    }

    return 0;
}


int pthread_svelte_rwlock_unlock(pthread_svelte_rwlock_t * rwlock) {
    pthread_rwlock_internal_t * internal = (pthread_rwlock_internal_t *) rwlock; // acquire `internal`

    if (internal->magic != MAGIC_INITED) {
        return EINVAL;
    }

    // TODO: Carefully test if calling both is not a problem
    int ret_1 = sceKernelUnlockWriteRWLock(internal->sce_rwlock);
    int ret_2 = sceKernelUnlockReadRWLock(internal->sce_rwlock);

    // TODO: Extra debugging options

    return 0;
}
