#!/bin/sh

mkdir -p /tmp/autoconf

setup_default_route() {

	[ -e "/var/dhcp/wan-ipaddr" ] && {
		echo "Setup DHCP/FTTH infra"
		ln -sf /var/dhcp/wan-resolv.conf /etc/resolv.conf
		ip route replace default via "`cat /var/dhcp/wan-gateway`"
		ip route flush cache
		IPADDR="`cat /var/dhcp/wan-ipaddr`"
		NET_INFRA="thd"
		export IPADDR
		export NET_INFRA
		return 0
	}
	[ -e "/var/dhcp/voip-ipaddr" ] && {
		echo "Setup DHCP/VOIP infra"
		ln -sf /var/dhcp/voip-resolv.conf /etc/resolv.conf
		ip route replace default via "`cat /var/dhcp/voip-gateway`"
		ip route flush cache
		IPADDR="`cat /var/dhcp/voip-ipaddr`"
		NET_INFRA="dsl"
		export IPADDR
		export NET_INFRA
		return 0
	}
	[ -e "/var/ppp/adsl-ipaddr" ] && {
		echo "Setup PPP/ADSL infra"
		ln -sf /var/ppp/adsl-resolv.conf /etc/resolv.conf
		ip route replace default via "`cat /var/ppp/adsl-gateway`"
		ip route flush cache
		IPADDR="`cat /var/ppp/adsl-ipaddr`"
		NET_INFRA="dsl"
		OPTION3=1
		export IPADDR
		export NET_INFRA
		return 0
	}

	NET_INFRA=''
	export NET_INFRA
	return 1

}

autoconf_firmware() {

	eval `/usr/lib/autoconf.awk /tmp/autoconf/${GENERAL_FILE}`

	echo "wget -q -T 60 -O /tmp/autoconf/${FIRMWARE_FILE} ${FIRMWARE_PROTO}://${FIRMWARE_URL}/${FIRMWARE_FILE}?${NETWOK_ARGS}${PPP_ARGS}${FIRMWARE_ARGS}${HOTSPOT_ARGS}"

	wget -q -T 60 -O /tmp/autoconf/${FIRMWARE_FILE} "${FIRMWARE_PROTO}://${FIRMWARE_URL}/${FIRMWARE_FILE}?${NETWOK_ARGS}${PPP_ARGS}${FIRMWARE_ARGS}${HOTSPOT_ARGS}"

	[ -e "/tmp/autoconf/${FIRMWARE_FILE}" ] && /usr/sbin/upgrade.sh /tmp/autoconf/${FIRMWARE_FILE}
}

autoconf_general() {

	GENERAL_PROTO="http"
	GENERAL_URL="general.$(hostname)${NET_INFRA}.$OPERATOR_DOMAIN"
	GENERAL_FILE="cfg$(hostname)${NET_INFRA}general.xml"
	MAC_ADDR_BASE="`cat /proc/mac_address_base`"
	MAC_ADDR_PPP="`mac_addr -p 3`"
	PRODUCTID="`cat /proc/productid`"
	FIRMWARE=$E
	[ -z "${OPTION3}" ] && {
		NETWOK_ARGS="ip_data=${IPADDR}&ip_voip=${IPADDR}&ip_tv=${IPADDR}&mac=${MAC_ADDR_BASE}"
	} || {
		NETWOK_ARGS="ip_data=${IPADDR}&mac=${MAC_ADDR_BASE}"
	}
	[ "${NET_INFRA}" == "dsl" ] && PPP_ARGS="&mac_ppp=${MAC_ADDR_PPP}&login_ppp=rescue@neufpnp"

	if [ -z "${OPTION3}" ]
	then
		if [ "${NET_INFRA}" = "dsl" ]
		then
			if [ -e "/var/ppp/adsl-ipaddr" ]
			then
				IP_ADDRESS="03=${IPADDR};29=`cat /var/ppp/adsl-ipaddr`"
			else
				IP_ADDRESS="03=${IPADDR}"
			fi
		else
			IP_ADDRESS="29=${IPADDR}"
		fi
	else
		IP_ADDRESS="29=${IPADDR}"
	fi

	FIRMWARE_ARGS="&genrel=0&hw=${PRODUCTID}&sw=${FIRMWARE}"
	HOTSPOT_ARGS="&hotspot_mode=3&hotspot_status=off"

	export NETWOK_ARGS
	export PPP_ARGS
	export FIRMWARE_ARGS
	export HOTSPOT_ARGS

	export PRODUCTID
	export MAC_ADDR_BASE
	export FIRMWARE
	export IP_ADDRESS

	spy-send "01=8;04=${PRODUCTID};05=${MAC_ADDR_BASE};2A=${FIRMWARE};06=$(cat /proc/uptime|awk -F "[ \.]" '{print $1}');${IP_ADDRESS};"

	echo "wget -q -T 60 -O /tmp/autoconf/${GENERAL_FILE} ${GENERAL_PROTO}://${GENERAL_URL}/${GENERAL_FILE}?${NETWOK_ARGS}${PPP_ARGS}${FIRMWARE_ARGS}${HOTSPOT_ARGS}"
	wget -q -T 60 -O /tmp/autoconf/${GENERAL_FILE} "${GENERAL_PROTO}://${GENERAL_URL}/${GENERAL_FILE}?${NETWOK_ARGS}${PPP_ARGS}${FIRMWARE_ARGS}${HOTSPOT_ARGS}"

	[ -e "/tmp/autoconf/${GENERAL_FILE}" ] && autoconf_firmware

	sleep 60
}

while true; do
	setup_default_route
	if [ -z $NET_INFRA ]
	then
		echo "Network not ready..."
		sleep 3
		continue
	fi

	autoconf_general
done

