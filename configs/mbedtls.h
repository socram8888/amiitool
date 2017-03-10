/*
 * (c) 2017      Marcos Del Sol Vives
 *
 * SPDX-License-Identifier: MIT
 */

#ifndef MBEDTLS_CONFIG_H
#define MBEDTLS_CONFIG_H

// AES-CTR for encryption, with AES-NI if possible.
#define MBEDTLS_CIPHER_C
#define MBEDTLS_AES_C
#define MBEDTLS_AESNI_C
#define MBEDTLS_CIPHER_MODE_CTR

// SHA256 for HMACing
#define MBEDTLS_MD_C
#define MBEDTLS_SHA256_C

// Every modern processor has SSE2. Fuck backwards compatibility.
#define MBEDTLS_HAVE_SSE2

// Random stuff.
#define MBEDTLS_HAVE_ASM
#define MBEDTLS_PLATFORM_C

#include "mbedtls/check_config.h"

#endif
