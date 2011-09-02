#!/bin/sh

WGET_TIMEOUT=120
WGET="wget -c -T $WGET_TIMEOUT"
HASHSUM="sha256sum -c"

FLASHCP="flashcp -v"
#
# provide by autoconf
#
# URL=
# MAIN_FILE=
# MAIN_HASH=
# CONFIG_FILE=
# CONFIG_HASH=
# ADSL_FILE=
# ADSL_HASH=
eval `/usr/lib/firmware.awk $1`

URL="$PROTO://$URL"

#
# NOTE:
#
# active polling for rescue firmware
# since no services are available
# and we want all working as soon as possible
#
safe_wget() {
	for i in 1 2 3 4 5; do
		echo "$WGET $URL/$1... (try=$i)"
		
		spy-send "01=8;04=${PRODUCTID};05=${MAC_ADDR_BASE};2A=${FIRMWARE};06=$(cat /proc/uptime|awk -F "[ \.]" '{print $1}');${IP_ADDRESS};49=$1;37=9;3E=$(($i-1));"
		$WGET $URL/$1
		case "$?" in
		0)
			echo "$WGET $URL/$1 -- succeeded -- (try=$i)"
			break;
			;;
		2)
			echo "$WGET $URL/$1 -- stalled -- (try=$i)"
			echo "retry... "
			spy-send "01=9;04=${PRODUCTID};05=${MAC_ADDR_BASE};2A=${FIRMWARE};06=$(cat /proc/uptime|awk -F "[ \.]" '{print $1}');${IP_ADDRESS};49=$1;38=5;4A=1;3E=$(($i-1));"
			sleep 5
			;;
		3)
			echo "$WGET $URL/$1 -- unexpected close -- (try=$i)"
			echo "retry... "
			spy-send "01=9;04=${PRODUCTID};05=${MAC_ADDR_BASE};2A=${FIRMWARE};06=$(cat /proc/uptime|awk -F "[ \.]" '{print $1}');${IP_ADDRESS};49=$1;38=5;4A=2;3E=$(($i-1));"
			sleep 5
			;;
		1|*)
			echo "$WGET $URL/$1 -- error -- (try=$i)"
			echo "rm -f $1"
			rm -f $1;
			echo "retry... "
			spy-send "01=9;04=${PRODUCTID};05=${MAC_ADDR_BASE};2A=${FIRMWARE};06=$(cat /proc/uptime|awk -F "[ \.]" '{print $1}');${IP_ADDRESS};49=$1;38=5;4A=3;3E=$(($i-1));"
			sleep 5
			;;
		esac
	done

	echo "$HASHSUM $1.sha256sum"
	if ! $HASHSUM $1.sha256sum
	then
		echo "sha256sum failed"
		echo "rm -f $1"
		rm -f $1;
		spy-send "01=9;04=${PRODUCTID};05=${MAC_ADDR_BASE};2A=${FIRMWARE};06=$(cat /proc/uptime|awk -F "[ \.]" '{print $1}');${IP_ADDRESS};49=$1;38=5;4A=4;"
		# restart all
		exit 1
	fi

	echo "sha256sum $1 succeeded"
}

safe_flashcp() {
	for i in 1 2; do
		echo "$FLASHCP $1 $2 (try:$i)"
		spy-send "01=8;04=${PRODUCTID};05=${MAC_ADDR_BASE};2A=${FIRMWARE};06=$(cat /proc/uptime|awk -F "[ \.]" '{print $1}');${IP_ADDRESS};49=$1;37=10;40=$(($i-1));"
		if $FLASHCP $1 $2; then
			break
		fi
		if [ $i = 2 ]; then
			echo "Fatal $FLASHCP $1 $2 failed"
			spy-send "01=9;04=${PRODUCTID};05=${MAC_ADDR_BASE};2A=${FIRMWARE};06=$(cat /proc/uptime|awk -F "[ \.]" '{print $1}');${IP_ADDRESS};49=$1;38=5;40=$(($i-1));"
			exit 1
		fi
		sleep 5
	done
}

# prepare sha256sum
echo "$MAIN_HASH  $MAIN_FILE" > $MAIN_FILE.sha256sum
echo "$CONFIG_HASH  $CONFIG_FILE" > $CONFIG_FILE.sha256sum
echo "$ADSL_HASH  $ADSL_FILE" > $ADSL_FILE.sha256sum

# download firmwares
ledctl mode downloading
safe_wget $MAIN_FILE 
safe_wget $CONFIG_FILE
[ "${NET_INFRA}" == "dsl" ] && safe_wget $ADSL_FILE
ledctl mode control

# burn firmwares
safe_flashcp $MAIN_FILE "/dev/mtd-main"
cp -f $CONFIG_FILE "/etc/config/system.xml"
[ "${NET_INFRA}" == "dsl" ] && safe_flashcp $ADSL_FILE "/dev/mtd-adslphy"

# upgrade succeeded
echo "Upgrade succeeded"

spy-send "01=8;04=${PRODUCTID};05=${MAC_ADDR_BASE};2A=${FIRMWARE};06=$(cat /proc/uptime|awk -F "[ \.]" '{print $1}');${IP_ADDRESS};37=8;"

# reboot to main firmware
echo "reboot in 2 seconds..."
sleep 2
reboot -f

exit 0
