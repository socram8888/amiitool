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

#include "nfc3d/ninkeys.h"
#include "nfc3d/amiibo.h"
#include "util.h"
#include <stdio.h>
#include <string.h>
#include <getopt.h>
#include <errno.h>

const nfc3d_keygen_masterkeys * KEYS[4] = {
	&NFC3D_NINKEYS_RETAIL_UNFIXED_INFOS,
	&NFC3D_NINKEYS_RETAIL_LOCKED_SECRET,
	&NFC3D_NINKEYS_DEBUG_UNFIXED_INFOS,
	&NFC3D_NINKEYS_DEBUG_LOCKED_SECRET
};

void usage() {
	fprintf(stderr,
		"amiitool\n"
		"by Marcos Vives Del Sol <socram@protonmail.ch>\n"
		"\n"
		"Usage: amiitool [-edtl] -i input -o output\n"
		"   -e encrypt amiibo\n"
		"   -d decrypt amiibo\n"
		"   -i input file\n"
		"   -o output file\n"
		"   -t use test (debug) keys instead of retail\n"
		"   -l use \"locked secret\" keys\n"
	);
}

int main(int argc, char ** argv) {
	char * infile = NULL;
	char * outfile = NULL;
	char op = '\0';
	bool debug = false;
	bool locked = false;

	char c;
	while ((c = getopt(argc, argv, "edi:o:tl")) != -1) {
		switch (c) {
			case 'e':
			case 'd':
				op = c;
				break;
			case 'i':
				infile = optarg;
				break;
			case 'o':
				outfile = optarg;
				break;
			case 't':
				debug = true;
				break;
			case 'l':
				locked = true;
				break;
			default:
				usage();
				return 2;
		}
	}

	if (op == '\0') {
		usage();
		return 1;
	}

	uint8_t original[NFC3D_AMIIBO_SIZE];
	uint8_t modified[NFC3D_AMIIBO_SIZE];

	FILE * f = stdin;
	if (infile) {
		f = fopen(infile, "rb");
		if (!f) {
			fprintf(stderr, "Could not open input file: %s (%d)\n", strerror(errno), errno);
			return 3;
		}
	}
	if (fread(original, NFC3D_AMIIBO_SIZE, 1, f) != 1) {
		fprintf(stderr, "Could not read from input: %s (%d)\n", strerror(errno), errno);
		return 3;
	}
	fclose(f);

	const nfc3d_keygen_masterkeys * masterKeys = KEYS[(debug ? 2 : 0) + (locked ? 1 : 0)];
	if (op == 'e') {
		nfc3d_amiibo_pack(masterKeys, original, modified);
	} else {
		if (!nfc3d_amiibo_unpack(masterKeys, original, modified)) {
			fprintf(stderr, "!!! WARNING !!!: Tag signature was NOT valid\n");
		}
	}

	f = stdout;
	if (outfile) {
		f = fopen(outfile, "wb");
		if (!f) {
			fprintf(stderr, "Could not open output file: %s (%d)\n", strerror(errno), errno);
			return 4;
		}
	}
	if (fwrite(modified, NFC3D_AMIIBO_SIZE, 1, f) != 1) {
		fprintf(stderr, "Could not write to output: %s (%d)\n", strerror(errno), errno);
		return 4;
	}
	fclose(f);

	return 0;
}