/*
 * Copyright (C) 2017 Marcos Vives Del Sol
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
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
