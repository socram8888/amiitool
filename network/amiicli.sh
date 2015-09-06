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
	echo "Usage: $0 [e|d] (input)? (output)?" >&2
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

SERVER="151.80.162.73 22403"

if ! which nc &>/dev/null; then
	echo "Netcat not detected. Please install or configure netcat and retry." >&2
	exit 1
fi

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
		{
			# Operation is encryption. Note this can't overlap with decryption as dumps begin with 0x04 (NXP manufacturer code)
			echo -n "E"

			# Now write authentication token
			gentoken "${APIKEY}" 

			cat "$INPUT"
		} | nc $SERVER > "$OUTPUT"
		;;

	d)
		cat "$INPUT" | nc $SERVER > "$OUTPUT"
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
