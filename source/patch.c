/*
 * Copyright (C) 2023 Volodymyr Atamanenko
 *
 * This software may be modified and distributed under the terms
 * of the MIT license. See the LICENSE file for details.
 */

/**
 * @file  patch.c
 * @brief Patching some of the .so internal functions or bridging them to native
 *        for better compatibility.
 */

#include <kubridge.h>
#include <so_util/so_util.h>
#include <stdio.h>
#include <psp2/kernel/clib.h>
#include <string.h>
#include <malloc.h>
#include <psp2/kernel/threadmgr/thread.h>
#include "utils/logger.h"
#include "utils/utils.h"

extern so_module so_mod;

int logWrapper(const char * fmt, ...) {
    //if (sceClibStrcmp(fmt, "Error: Cannot open file %s") == 0)
    //    return 0;

    va_list list;
    char text[1024];

    va_start(list, fmt);
    sceClibVsnprintf(text, sizeof(text), fmt, list);
    va_end(list);

    if (text[strlen(text) - 1] == '\n')
        l_info("[GLITCH] %s", text);
    else
        l_info("[GLITCH] %s\n", text);

    return 0;
}

int logWrapper2(int a1, const char * fmt, ...) {
    //if (sceClibStrcmp(fmt, "Error: Cannot open file %s") == 0)
    //    return 0;

    va_list list;
    char text[2048];

    va_start(list, fmt);
    sceClibVsnprintf(text, sizeof(text), fmt, list);
    va_end(list);

    if (text[strlen(text) - 1] == '\n')
        l_info("[GLITCH2] %s", text);
    else
        l_info("[GLITCH2] %s\n", text);

    return 0;
}

typedef struct ShaderClass {
    void * f1;
    void * f2;
    void * f3;
    void * f4;
    void * f5;
    void * f6;
    void * f7;
    char * filename;
    void * f9;
    int glShader;
} ShaderClass;

known_shaders_struct known_shaders[256];
int known_shaders_count = 0;

so_hook Shader_SubmitSource_hook;
void Shader_SubmitSource(ShaderClass *this,int shaderType,char **string,int count) {
    char ** real_string = string;
    char * backup_string = *string;
    int faked = 0;

    char * hash = str_sha1sum(this->filename, strlen(this->filename));

    for (int i = 0; i < known_shaders_count; ++i) {
        if (strcmp(known_shaders[i].engine_name, hash) == 0) {
            faked = 1;
            *real_string = malloc(45);
            strncpy(*real_string, "sha:", 4);
            strncpy(*real_string + 4, known_shaders[i].real_name, 40);
            (*real_string)[44] = '\0';
            break;
        }
    }
    if (!faked) {
        strncpy(known_shaders[known_shaders_count].engine_name, hash, 40);
        known_shaders[known_shaders_count].engine_name[40] = '\0';
        known_shaders_count++;

        if (!*string) {
            l_error("We NEED to use faked shader but we don't know it. Name is %s", this->filename);
        } else {
            l_info("Prepared to fake shader id %i sha %s", known_shaders_count - 1, hash);
        }
    }

    SO_CONTINUE(void *, Shader_SubmitSource_hook, this, shaderType, real_string, count);

    free(hash);
    if (faked) {
        free(*real_string);
        *real_string = backup_string;
    }
}

so_hook FUN_00083594_hook;
uint32_t FUN_00083594(char * param_1, int param_2) {
    uint32_t ret = SO_CONTINUE(uint32_t, FUN_00083594_hook, param_1, param_2);
    sceClibPrintf("hash [%s] : [%u]\n", param_1, ret);
    return ret;
}

so_hook audioThread_hook;
void * audioThread(void * arg) {
    // Move audio thread to 4th core if available
    int ret = sceKernelChangeThreadCpuAffinityMask(sceKernelGetThreadId(), 0x80000);
    return SO_CONTINUE(void *, audioThread_hook, arg);
}

void so_patch(void) {
    for (int i = 0; i < so_mod.text_size; i += 4) {
        uint32_t value = *(uint32_t *)(so_mod.text_base + i);
        if (value == 0xFFFF0FA0) {
            value = 0x9A000FA0;
            kuKernelCpuUnrestrictedMemcpy((void *)(so_mod.text_base + i), &value, sizeof(uint32_t));
        } else if (value == 0xFFFF0FC0) {
            value = 0x9A000FC0;
            kuKernelCpuUnrestrictedMemcpy((void *)(so_mod.text_base + i), &value, sizeof(uint32_t));
        }
    }

    //FUN_006ab354_hook = hook_addr((uintptr_t)so_mod.text_base + 0x006ab354, (uintptr_t)&FUN_006ab354);
    //FUN_006ab588_hook = hook_addr((uintptr_t)so_mod.text_base + 0x006ab588, (uintptr_t)&FUN_006ab588);
    //FUN_005f74fc_hook = hook_addr((uintptr_t)so_mod.text_base + 0x005f74fc, (uintptr_t)&FUN_005f74fc);

    // Uncomment next line to get hash-filename pairs for OBBs
    //FUN_00083594_hook = hook_addr((uintptr_t)so_mod.text_base + 0x00083594, (uintptr_t)&FUN_00083594);

    audioThread_hook = hook_addr((uintptr_t)so_mod.text_base + 0x003b4ae0, (uintptr_t)&audioThread);

    //hook_addr((uintptr_t)so_mod.text_base + 0x00702230, (uintptr_t)&logWrapper);
    //hook_addr((uintptr_t)so_mod.text_base + 0x0058bd84, (uintptr_t)&logWrapper2);
    //hook_addr((uintptr_t)so_mod.text_base + 0x0058b940, (uintptr_t)&logWrapper);

#if 0 // 1.1.2
    Shader_SubmitSource_hook = hook_addr((uintptr_t)so_mod.text_base + 0x0040ed68 + 1, (uintptr_t)&Shader_SubmitSource);
#else // 1.1.7g
    Shader_SubmitSource_hook = hook_addr((uintptr_t)so_mod.text_base + 0x006b271c, (uintptr_t)&Shader_SubmitSource);
#endif
}
