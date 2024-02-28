#pragma once

#include "pthread-svelte/include/pthread_svelte.h"
#include <psp2/kernel/threadmgr.h>

//fixme: this is temp
#include "utils/logger.h"

#include <assert.h>
#include <string.h>
#include <errno.h>
#include <time.h>
#include <stdlib.h>

/*
* Sce stuff
*/


// Uncomment to have extra error handling like NULL mutexes or deadlocks
// #define EXTRA_ERROR_HANDLING

#define LOG_INFO(fmt)
#define LOG_WARN(fmt)
#define LOG_DBG(...)

//#define LOG_INFO(fmt)   _log_info(__FILE__, __LINE__, __func__, fmt)
//#define LOG_WARN(fmt)   _log_warn(__FILE__, __LINE__, __func__, fmt)
//#define LOG_DBG(...) _log_debug(__FILE__, __LINE__, __func__, __VA_ARGS__)
#define LOG_ERROR(...) _log_print(LT_ERROR,   __VA_ARGS__)

#define SCE_KERNEL_ERROR_LW_MUTEX_FAILED_TO_OWN 0x80028185
#define SCE_KERNEL_ERROR_RW_LOCK_FAILED_TO_LOCK 0x800281E6
#define SCE_KERNEL_ERROR_LW_MUTEX_NOT_OWNED     0x80028186
#define SCE_KERNEL_ERROR_LW_MUTEX_LOCK_OVF      0x80028183
#define SCE_KERNEL_ERROR_LW_MUTEX_UNLOCK_UDF    0x80028184
#define SCE_KERNEL_ERROR_WAIT_TIMEOUT           0x80028005
#define SCE_KERNEL_ERROR_UNKNOWN_LW_MUTEX_ID    0x80028181
#define SCE_KERNEL_ERROR_UID_RL_OVERFLOW        0x8002450d

#ifndef SCE_KERNEL_LW_MUTEX_ATTR_TH_FIFO
#define SCE_KERNEL_LW_MUTEX_ATTR_TH_FIFO (0x00000000U)
#endif

#ifndef SCE_KERNEL_LW_COND_ATTR_TH_FIFO
#define SCE_KERNEL_LW_COND_ATTR_TH_FIFO (0x00000000U)
#endif

#ifndef SCE_KERNEL_LW_MUTEX_ATTR_RECURSIVE
#define SCE_KERNEL_LW_MUTEX_ATTR_RECURSIVE (0x00000002U)
#endif

#ifndef SCE_KERNEL_RW_LOCK_ATTR_TH_FIFO
#define SCE_KERNEL_RW_LOCK_ATTR_TH_FIFO (0x00000000U)
#endif

typedef struct SceKernelLwCondInfo {
    SceSize size;
    SceUID uid;
    char name[32];
    SceUInt32 attr;
    SceKernelLwCondWork *pWork;
    SceKernelLwMutexWork *pLwMutex;
    SceUInt32 numWaitThreads;
} SceKernelLwCondInfo;

SceInt32 sceKernelGetLwCondInfo(SceKernelLwCondWork *pWork, SceKernelLwCondInfo *pInfo);

/*
* Sce stuff end
*/

#define ALWAYS_INLINE static inline __attribute__((always_inline))

/*
 * Cond
 */

#define COND_SHARED_MASK 0x0001
#define COND_CLOCK_MASK 0x0002
#define COND_COUNTER_STEP 0x0004
#define COND_FLAGS_MASK (COND_SHARED_MASK | COND_CLOCK_MASK)
#define COND_COUNTER_MASK (~COND_FLAGS_MASK)

#define COND_IS_SHARED(c) (((c) & COND_SHARED_MASK) != 0)
#define COND_GET_CLOCK(c) (((c) & COND_CLOCK_MASK) >> 1)
#define COND_SET_CLOCK(attr, c) ((attr) | (c << 1))

#define MAGIC_INITED     0x0D15EA5E
#define MAGIC_NOT_INITED 0xFEE1DEAD

typedef struct cond_object_t {
    pthread_svelte_mutex_t * mutex;
    uint16_t waiters;
    uint8_t broadcast;
    uint8_t signal;
    int32_t  owner;
    int32_t  type;
    uint8_t  is_initialized;
    clockid_t clock;
} __attribute__((aligned(8))) cond_object_t;

typedef struct pthread_cond_internal_t {
    cond_object_t * data;
} __attribute__((aligned(4))) pthread_cond_internal_t;

static_assert(sizeof(pthread_svelte_cond_t) == sizeof(pthread_cond_internal_t),
              "pthread_svelte_cond_t size must be equal to pthread_cond_internal_t");

/*
 * Key
 */

typedef struct key_object_t {
    unsigned key;
    void (*destructor) (void *);
    pthread_svelte_mutex_t key_lock;
    void *threads;
} __attribute__((aligned(8))) key_object_t;

typedef struct pthread_key_internal_t {
    key_object_t * data;
} __attribute__((aligned(4))) pthread_key_internal_t;

static_assert(sizeof(pthread_svelte_key_t) == sizeof(pthread_key_internal_t),
              "pthread_key_t size must be equal to pthread_key_internal_t");

/*
 * Mutex
 */

typedef struct mutex_object_t {
    uint16_t lock_count;
    int32_t  owner;
    int32_t  type;
    uint8_t  is_initialized;
} mutex_object_t;

typedef struct pthread_mutex_internal_t {
    mutex_object_t * data;
} __attribute__((aligned(4))) pthread_mutex_internal_t;

static_assert(sizeof(pthread_svelte_mutex_t) == sizeof(pthread_mutex_internal_t),
              "pthread_svelte_mutex_t size must be equal to pthread_mutex_internal_t");

int pthread_svelte_mutex_check_init(pthread_svelte_mutex_t *mutex);

/*
 * RWLock
 */

typedef struct rwlock_object_t {
    SceKernelLwMutexWork sce_mutex;
    int type;
} __attribute__((aligned(8))) rwlock_object_t;

typedef struct pthread_rwlock_internal_t {
    SceUID sce_rwlock;
    int32_t magic;
    int32_t __padding[8];
} __attribute__((aligned(4))) pthread_rwlock_internal_t;

static_assert(sizeof(pthread_svelte_rwlock_t) == sizeof(pthread_rwlock_internal_t),
              "pthread_svelte_rwlock_t size must be equal to pthread_rwlock_internal_t");
