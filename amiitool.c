/*
 * (c) 2015-2017 Marcos Del Sol Vives
 * (c) 2016      javiMaD
 *
 * SPDX-License-Identifier: MIT
 */

#include <nfc3d/amiibo.h>
#include <nfc3d/version.h>
#include "util.h"
#include <stdio.h>
#include <string.h>
#include <getopt.h>
#include <errno.h>

#define NTAG215_SIZE 540

static char * self;

void usage() {
	fprintf(stderr,
		"amiitool v%s (commit %08x)\n"
		"by Marcos Del Sol Vives <marcos@dracon.es>\n"
		"\n"
		"Usage: %s (-e|-d) -k keyfile [-i input] [-o output]\n"
		"   -e encrypt and sign amiibo\n"
		"   -d decrypt and test amiibo\n"
		"   -k key set file. For retail amiibo, use \"retail unfixed\" key set\n"
		"   -i input file. If not specified, stdin will be used.\n"
		"   -o output file. If not specified, stdout will be used.\n"
		"   -l decrypt files with invalid signatures.\n",
		nfc3d_version(), nfc3d_commit_id(), self
	);
}

int main(int argc, char ** argv) {
	self = argv[0];

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