#!/bin/bash

cd $(dirname "$0")

log_err() {
	echo "[!!!] $1" >>/var/log/amiibo.log
}

log_debug() {
	echo "[---] $1" >>/var/log/amiibo.log
}

checkauth() {
	VERSION=$(head -c1)

	# Check version
	if [ "$VERSION" != "1" ]; then
		log_err "Invalid version: " $(echo $VERSION | od -An -tx1)
		return 1
	fi

	# Now read first 16 bytes which are hexadecimal characters. Those are the fixed
	USER=$(head -c16 | tr [a-f] [A-F])
	if ! [[ "$USER" =~ [A-F0-9]{16} ]]; then
		log_err "Illegal user: " $(echo $USER | od -An -tx1)
		return 1
	fi
	log_debug "User: $USER"

	# Check username and load it so we can use its AMIIBO_API_KEY
	if [ ! -f auth/$USER.dat ]; then
		log_err "Unknown user: $USER"
		return 1
	fi
	. auth/$USER.dat

	# Read next 16 hex chars which are the time-based token
	CLITOKEN=$(head -c16 | tr [A-F] [a-f])
	if ! [[ "$CLITOKEN" =~ [a-f0-9]{16} ]]; then
		log_err "Illegal token: " $(echo $CLITOKEN | od -An -tx1)
		return 1
	fi

	# Tick is as in TOTP: unix time / 30
	SRVTICK=$(($(date -u +%s)/30))
	log_debug "Local tick: $SRVTICK"

	# Now attempt to match time within a certain offset. Computer's time should be NTP-updated and therefore very precise, so no need to check anything outside +-1.
	for OFFSET in 0 -1 1; do
		TICK=$(($SRVTICK+$OFFSET))
		SRVTOKEN=$(echo -n "$TICK-${AMIIBO_API_KEY:16:16}" | sha256sum | head -c16)

		if [ "$SRVTOKEN" == "$CLITOKEN" ]; then
			log_debug "Token for $USER valid"
			return 0
		fi
	done

	log_err "Out-of-date token: $CLITOKEN"
	return 1
}

OP=$(head -c1 | od -tx1 -An)

case $OP in
	\ 04)
		log_debug "Requested decoding"
		(echo -ne "\x04" && cat) | ./amiitool -d -k retail_unfixed.bin 2>/dev/null
		RET=$?
		;;

	\ 45)
		log_debug "Requested encoding"
		if checkauth; then
			./amiitool -e -k retail_unfixed.bin 2>/dev/null
			RET=$?
		else
			RET=81
		fi
		;;

	\ 54)
		log_debug "Requested ToS"
		cat tos.txt
		RET=0
		;;

	*)
		log_err "Unknown command: $OP"
		RET=80
		;;
esac

if [ $RET -ne 0 ]; then
	printf %02d $RET
fi
