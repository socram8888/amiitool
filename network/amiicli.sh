#!/bin/bash -e

fsize() {
	wc -c "$1" | cut -f1 -d' '
}

gentoken() {
	local LOGIN="$1"

	# Auth version (1)
	echo -n "1"

	# Fixed part
	echo -n "${LOGIN:0:16}"

	# Time-based token
	TICK=$(($(date -u +%s)/30))
	echo -n "$TICK-${LOGIN:16:16}" | sha256sum | head -c16

	echo "Tick: $TICK" >&2
}

usage() {
	echo "Usage: $0 op (input)? (output)?" >&2
	echo >&2
	echo "Operations:" >&2
	echo " - d: decrypt amiibo" >&2
	echo " - e: encrypt amiibo (*)" >&2
	echo " - t: terms of service" >&2
	echo >&2
	echo "If input or output are not specified, stdin and stdout will be used" >&2
	exit 1
}

loadapikey() {
	set +e
	. ~/.amiibokey.sh
	set -e
	if ! [[ "${AMIIBO_API_KEY}" =~ ^[a-fA-F0-9]{32} ]]; then
		echo "API key not found or invalid" >&2
		echo >&2
		echo "To prevent cheating and counterfeiting, this service is not public and you need a key before you may use it. You may request one by contating socram@protonmail.ch along with an explanation on why you need it." >&2
		exit 1
	fi
	echo "${AMIIBO_API_KEY}"
}

tcp_open() {
	local FD="$1"
	local HOST="$2"
	local PORT="$3"

	eval "exec ${FD}<>/dev/tcp/${HOST}/${PORT}"
}

tcp_close() {
	local FD="$1"

	eval "exec ${FD}<&-"
	eval "exec ${FD}>&-"
}

open_conn() {
	tcp_open 3 151.80.162.73 22403
}

close_conn() {
	tcp_close 3
}

if [ $# -ge 1 -a $# -le 3 ]; then
	INPUT=-
	if [ $# -ge 2 ]; then
		INPUT="$2"
	fi

	if [ $# -ge 3 ]; then
		OUTPUT="$3"
		OUTPUTTMP=false
	else
		OUTPUT=$(mktemp)
		OUTPUTTMP=true
	fi
else
	usage
fi

case "$1" in
	e)
		APIKEY=$(loadapikey)

		open_conn

		# Operation is encryption. Note this can't overlap with decryption as dumps begin with 0x04 (NXP manufacturer code)
		echo -n "E" >&3

		# Now write authentication token
		gentoken "${APIKEY}" >&3

		# Send dump
		head -c540 "$INPUT" >&3

		# Read data back
		head -c540 <&3 >"$OUTPUT"

		close_conn
		;;

	d)
		open_conn

		# Ensure it starts with 0x04.
		echo -ne "\x04" | tee -a req >&3
		tail -c +2 "$INPUT" | head -c539 | tee -a req  >&3

		# Read decrypted form
		head -c540 <&3 >"$OUTPUT"

		close_conn
		;;

	t)
		open_conn

		# Send operation code
		echo -n "T" >&3

		# Read ToS
		cat <&3 >"$OUTPUT"

		close_conn
		;;

	*)
		if $OUTPUTTMP; then
			rm "$OUTPUT"
		fi
		usage
		;;
esac

if [ $(fsize "$OUTPUT") == 2 ]; then
	ERR=$(cat "$OUTPUT")

	echo -n "Error $ERR: " >&2
	case "$ERR" in
		03)
			echo "invalid dump size" >&2
			;;
		06)
			echo "invalid dump signature" >&2
			;;
		80)
			echo "unsupported command (server's not up-to-date?)" >&2
			;;
		81)
			echo "invalid authentication token" >&2
			;;
		*)
			echo "unknown error (server on fire?)" >&2
			;;
	esac
	rm "$OUTPUT"
	exit 1
fi

if $OUTPUTTMP; then
	cat "$OUTPUT"
	rm "$OUTPUT"
fi

exit 0
