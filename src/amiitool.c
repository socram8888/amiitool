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

#include "nfc3d/amiibo.h"
#include "util.h"
#include <stdio.h>
#include <string.h>
#include <getopt.h>
#include <errno.h>

#define NTAG215_SIZE 540

void usage() {
	fprintf(stderr,
		"amiitool\n"
		"by Marcos Vives Del Sol <socram@protonmail.ch>\n"
		"\n"
		"Usage: amiitool (-e|-d) -k keyfile [-i input] [-o output]\n"
		"   -e encrypt and sign amiibo\n"
		"   -d decrypt and test amiibo\n"
		"   -k key set file. For retail amiibo, use \"retail unfixed\" key set\n"
		"   -i input file. If not specified, stdin will be used.\n"
		"   -o output file. If not specified, stdout will be used.\n"
		"   -l decrypt files with invalid signatures.\n"
	);
}

int main(int argc, char ** argv) {
	char * infile = NULL;
	char * outfile = NULL;
	char * keyfile = NULL;
	char op = '\0';
	bool lenient = false;

	char c;
	while ((c = getopt(argc, argv, "edi:o:k:l")) != -1) {
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
			case 'k':
				keyfile = optarg;
				break;
			case 'l':
				lenient = true;
				break;
			default:
				usage();
				return 2;
		}
	}

	if (op == '\0' || keyfile == NULL) {
		usage();
		return 1;
	}

	nfc3d_amiibo_keys amiiboKeys;
	if (!nfc3d_amiibo_load_keys(&amiiboKeys, keyfile)) {
		fprintf(stderr, "Could not load keys from \"%s\": %s (%d)\n", keyfile, strerror(errno), errno);
		return 5;
	}

	uint8_t original[NTAG215_SIZE];
	uint8_t modified[NFC3D_AMIIBO_SIZE];

	FILE * f = stdin;
	if (infile) {
		f = fopen(infile, "rb");
		if (!f) {
			fprintf(stderr, "Could not open input file: %s (%d)\n", strerror(errno), errno);
			return 3;
		}
	}
	size_t readPages = fread(original, 4, NTAG215_SIZE / 4, f);
	if (readPages < NFC3D_AMIIBO_SIZE / 4) {
		fprintf(stderr, "Could not read from input: %s (%d)\n", strerror(errno), errno);
		return 3;
	}
	fclose(f);

	if (op == 'e') {
		nfc3d_amiibo_pack(&amiiboKeys, original, modified);
	} else {
		if (!nfc3d_amiibo_unpack(&amiiboKeys, original, modified)) {
			fprintf(stderr, "!!! WARNING !!!: Tag signature was NOT valid\n");
			if (!lenient) {
				return 6;
			}
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
	if (readPages > NFC3D_AMIIBO_SIZE / 4) {
		if (fwrite(original + NFC3D_AMIIBO_SIZE, readPages * 4 - NFC3D_AMIIBO_SIZE, 1, f) != 1) {
			fprintf(stderr, "Could not write to output: %s (%d)\n", strerror(errno), errno);
			return 4;
		}
	}
	fclose(f);

	return 0;
}