/*
 * Copyright (C) 2023      Rinnegatamante
 * Copyright (C) 2023-2024 Volodymyr Atamanenko
 *
 * This software may be modified and distributed under the terms
 * of the MIT license. See the LICENSE file for details.
 */

#include <vitasdk.h>
#include <vitaGL.h>
#include <stdio.h>

#include "utils/trophies.h"
#include "utils/logger.h"

static char comm_id[12] = {0};
static char signature[160] = {0xb9,0xdd,0xe1,0x3b,0x01,0x00};

static int trp_ctx;
static int plat_id = -1;

typedef struct {
    int sdkVersion;
    SceCommonDialogParam commonParam;
    int context;
    int options;
    uint8_t reserved[128];
} SceNpTrophySetupDialogParam;

typedef struct {
    uint32_t unk[4];
} SceNpTrophyUnlockState;
SceNpTrophyUnlockState trophies_unlocks;

int sceNpTrophyInit(void *unk);
int sceNpTrophyCreateContext(int *context, char *commId, char *commSign, uint64_t options);
int sceNpTrophySetupDialogInit(SceNpTrophySetupDialogParam *param);
SceCommonDialogStatus sceNpTrophySetupDialogGetStatus();
int sceNpTrophySetupDialogTerm();
int sceNpTrophyCreateHandle(int *handle);
int sceNpTrophyDestroyHandle(int handle);
int sceNpTrophyUnlockTrophy(int ctx, int handle, int id, int *plat_id);
int sceNpTrophyGetTrophyUnlockState(int ctx, int handle, SceNpTrophyUnlockState *state, uint32_t *count);

bool trophies_available = false;

volatile int trp_id;
SceUID trp_request_mutex, trp_delivered_mutex;
int trophies_unlocker(SceSize args, void *argp) {
    for (;;) {
        sceKernelWaitSema(trp_request_mutex, 1, NULL);
        int local_trp_id = trp_id;
        int trp_handle;
        int res = sceNpTrophyCreateHandle(&trp_handle);
        if (res < 0) {
            l_error("sceNpTrophyCreateHandle returned 0x%08X", res);
        }

        res = sceNpTrophyUnlockTrophy(trp_ctx, trp_handle, local_trp_id, &plat_id);
        if (res < 0) {
            l_error("sceNpTrophyUnlockTrophy returned 0x%08x", res);
        }

        sceKernelSignalSema(trp_delivered_mutex, 1);
        sceNpTrophyDestroyHandle(trp_handle);
    }
}

extern int8_t game_idx;
int trophies_init() {
    // Starting sceNpTrophy
    sprintf(comm_id, "%s", TITLE_ID);
    sceSysmoduleLoadModule(SCE_SYSMODULE_NP_TROPHY);
    sceNpTrophyInit(NULL);
    int res = sceNpTrophyCreateContext(&trp_ctx, comm_id, signature, 0);
    if (res < 0) {
#ifdef DEBUG_SOLOADER
        l_error("sceNpTrophyCreateContext returned 0x%08X", res);
#endif
        return res;
    }
    SceNpTrophySetupDialogParam setupParam;
    sceClibMemset(&setupParam, 0, sizeof(SceNpTrophySetupDialogParam));
    _sceCommonDialogSetMagicNumber(&setupParam.commonParam);
    setupParam.sdkVersion = PSP2_SDK_VERSION;
    setupParam.options = 0;
    setupParam.context = trp_ctx;

    res = sceNpTrophySetupDialogInit(&setupParam);
    if (res < 0) {
        if (res == 0x80020436) {
            l_error("sceNpTrophySetupDialogInit error: GXM not initialized");
            return res;
        } else {
            l_error("sceNpTrophySetupDialogInit returned 0x%08X", res);
            return res;
        }
    }

    static int trophy_setup = SCE_COMMON_DIALOG_STATUS_RUNNING;
    while (trophy_setup == SCE_COMMON_DIALOG_STATUS_RUNNING) {
        trophy_setup = sceNpTrophySetupDialogGetStatus();
        vglSwapBuffers(GL_TRUE);
    }
    sceNpTrophySetupDialogTerm();

    // Starting trophy unlocker thread
    trp_delivered_mutex = sceKernelCreateSema("trps delivery", 0, 1, 1, NULL);
    trp_request_mutex = sceKernelCreateSema("trps request", 0, 0, 1, NULL);
    SceUID tropies_unlocker_thd = sceKernelCreateThread("trophies unlocker", &trophies_unlocker, 0x10000100, 0x10000, 0, 0, NULL);
    sceKernelChangeThreadCpuAffinityMask(tropies_unlocker_thd, 0x80000); // use 4th core if available
    sceKernelStartThread(tropies_unlocker_thd, 0, NULL);

    // Getting current trophy unlocks state
    int trp_handle;
    uint32_t dummy;
    res = sceNpTrophyCreateHandle(&trp_handle);
    if (res < 0) {
        l_error("sceNpTrophyCreateHandle returned 0x%08X", res);
        return res;
    }
    res = sceNpTrophyGetTrophyUnlockState(trp_ctx, trp_handle, &trophies_unlocks, &dummy);
    if (res < 0) {
        l_error("sceNpTrophyGetTrophyUnlockState returned 0x%08X", res);
        return res;
    }
    sceNpTrophyDestroyHandle(trp_handle);

    trophies_available = true;
    return res;
}

bool trophies_is_unlocked(uint32_t id) {
    if (trophies_available) {
        return (trophies_unlocks.unk[id >> 5] & (1 << (id & 31))) > 0;
    }
    return false;
}

void trophies_unlock(uint32_t id) {
    if (!trophies_available || trophies_is_unlocked(id)) {
        return;
    }

    trophies_unlocks.unk[id >> 5] |= (1 << (id & 31));
    sceKernelWaitSema(trp_delivered_mutex, 1, NULL);

    trp_id = id;
    sceKernelSignalSema(trp_request_mutex, 1);
}
