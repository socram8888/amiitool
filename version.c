/*
 * (c) 2015-2017 Marcos Del Sol Vives
 * (c) 2016      javiMaD
 *
 * SPDX-License-Identifier: MIT
 */

#include <nfc3d/version.h>
#include <stdio.h>
#include "gitversion.h"

static char vstr[16] = { 0 };

const char * nfc3d_version() {
	if (vstr[0]) {
		return vstr;
	}

	uint32_t version = nfc3d_version_code();
	uint32_t major = version >> 24;
	uint32_t minor = version >> 12 & 0xFFF;
	uint32_t revision = version & 0xFFF;

	snprintf(vstr, sizeof(vstr) - 1, "%x.%x.%x", major, minor, revision);
	return vstr;
}

uint32_t nfc3d_version_code() {
	// TODO: can we get this from Git tags somehow?
	return 0x01000000;
}

uint32_t nfc3d_commit_id() {
	return GIT_COMMIT_ID;
}

uint32_t nfc3d_commit_count() {
	return GIT_COMMIT_COUNT;
}
