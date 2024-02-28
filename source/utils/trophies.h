/*
 * Copyright (C) 2022-2023 Volodymyr Atamanenko
 *
 * This software may be modified and distributed under the terms
 * of the MIT license. See the LICENSE file for details.
 */

/**
 * @file  trophies.h
 * @brief Provides methods to work with native trophies using NoTrpDrm
 */

#ifndef SOLOADER_TROPHIES_H
#define SOLOADER_TROPHIES_H

#include "stdbool.h"
#include "stdint.h"

#ifdef __cplusplus
extern "C" {
#endif

int trophies_init();
void trophies_unlock(uint32_t id);
bool trophies_is_unlocked(uint32_t id);

#ifdef __cplusplus
}
#endif

#endif // SOLOADER_TROPHIES_H
