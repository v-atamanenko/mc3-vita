#include "_pthread-svelte_internal.h"

static SceKernelLwMutexWork _once_mutex = {{ MAGIC_NOT_INITED }};

int pthread_svelte_once(pthread_svelte_once_t *once_control, void (*init_routine) (void)) {
    pthread_svelte_once_t once;

    if (once_control == NULL || init_routine == NULL) {
        return EINVAL;
    }

    if (_once_mutex.data[0] == MAGIC_NOT_INITED) {
        sceKernelCreateLwMutex(&_once_mutex, "pthread_once_mutex", 0, 0, NULL);
    }

    sceKernelLockLwMutex(&_once_mutex, 1, NULL);

    once = *once_control;
    *once_control = 1;

    sceKernelUnlockLwMutex(&_once_mutex, 1);

    if (!once) {
        init_routine();
    }

    return 0;
}
