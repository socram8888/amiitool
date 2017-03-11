/*
 * (c) 2017      Marcos Del Sol Vives
 *
 * SPDX-License-Identifier: MIT
 */

#ifndef HAVE_NFC3D_VERSION_H
#define HAVE_NFC3D_VERSION_H

#include <stdint.h>

const char * nfc3d_version();
uint32_t nfc3d_version_code();
uint32_t nfc3d_commit_id();
uint32_t nfc3d_commit_count();

#endif
