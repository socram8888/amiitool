/*
 * Copyright (C) 2015 Marcos Vives Del Sol
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

#include "nfc3d/keygen.h"
#include "nfc3d/ninkeys.h"
#include "util.h"

const uint8_t SEED[NFC3D_KEYGEN_SEED_SIZE] = {
	0x00, 0x13, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x04, 0x62, 0xA1, 0x4F, 0x5A, 0x17, 0x3C, 0x81, 0x04, 0x62, 0xA1, 0x4F, 0x5A, 0x17, 0x3C, 0x81,
	0xE3, 0x60, 0x76, 0x44, 0x5E, 0xED, 0x5A, 0x25, 0xB2, 0x83, 0x66, 0x6E, 0xEA, 0x47, 0x1A, 0xAA,
	0x65, 0xB7, 0x00, 0x3A, 0xEA, 0x90, 0x29, 0xFF, 0xB8, 0xEE, 0x57, 0xE0, 0xE4, 0xD1, 0x5B, 0x15
};

const nfc3d_keygen_derivedkeys CORRECT = {
	{ 0xFE, 0xE0, 0x46, 0x80, 0x9D, 0x52, 0x2D, 0xC2, 0x4F, 0x8A, 0x65, 0x8C, 0xDB, 0xC2, 0xB2, 0x73 },
	{ 0x3F, 0x09, 0x20, 0x0E, 0xE7, 0x2D, 0xA3, 0x4D, 0x74, 0xA3, 0x7A, 0x42, 0xED, 0xD2, 0x33, 0x59 },
	{ 0xF3, 0x2A, 0xF8, 0xD4, 0x37, 0x78, 0xB8, 0xC6, 0xDE, 0x95, 0x68, 0x8F, 0x0F, 0xEA, 0xDC, 0x4A }
};

int main() {
	nfc3d_keygen_derivedkeys outkeys;
	nfc3d_keygen(&NFC3D_NINKEYS_RETAIL_UNFIXED_INFOS, SEED, &outkeys);

	printf("Calculated:\n");
	printhex(&outkeys, sizeof(outkeys));

	printf("Matches console: ");
	if (memcmp(&CORRECT, &outkeys, sizeof(outkeys)) == 0) {
		printf("YES\n");
	} else {
		printf("NO\n");
	}

	return 0;
}
