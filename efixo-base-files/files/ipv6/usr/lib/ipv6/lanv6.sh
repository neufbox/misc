#!/bin/sh
#
# lan ipv6 addr configuration
#
# usage : lanv6.sh config|deconfig
#


reason=${REASON}
prefix=${prefix_iapd}

ip6calc=/usr/bin/ip6calc
ip=/usr/sbin/ip
lan=`nvram get lan_ifname`

lanv6_ifaddr()
{
	addr=$1
	prefix6=$2

	# set the new addr
	$ip -6 addr add $addr dev $lan

	status set net_ipv6_ifname "$lan"
	status set net_ipv6_ipaddr "$addr"
	status set net_ipv6_prefix "${prefix6}/56"
}

lanv6_config()
{
	addr2=`$ip -6 addr show $lan|grep inet6|grep global|awk '{print $2}' `

	if [ -z "$addr2" ]; then
		logger -s -t "[ipv6] $0" -p daemon.debug "$lan has no global ipv6 addr..."
		exit 1
	fi

	prefix6=`$ip6calc -n $addr2|sed 's/::0$//'`
	lan0_std_addr="${prefix}::1/64"

	lanv6_ifaddr $lan0_std_addr "${prefix6}::"
}

lanv6_deconfig()
{

	addr=`status get net_ipv6_ipaddr`

	if [ -z "$addr" ]; then
		logger -s -t "[ipv6] $0" -p daemon.debug "$lan has no global ipv6 addr..."
		exit 1
	fi


	# remove ipv6 addr
	$ip -6 addr del $addr dev $lan

}

dhcp6_setup()
{
	if [ -n "$prefix" ]; then
		logger -s -t "lanv6" -p daemon.debug "Called by dhcp6c to setup the prefix $prefix"
	else
		logger -s -t "lanv6" -p daemon.debug "Error: No prefix ..."
		exit 1;
	fi

	lan0_eui64addr=`$ip -6 addr show $lan|grep inet6|grep global|awk '{print $2}' `
	if [ -n "$lan0_eui64addr" ]; then
		status set net_ipv6_eui64addr "$lan0_eui64addr"
	fi

	lan0_addr="${prefix}1/64"

	lanv6_ifaddr $lan0_addr $prefix

	logger -s -t "[ipv6] $0" -p daemon.debug "Now we are launching radvd..."
	/etc/init.d/ipv6 radvd_config
	nbctl async_run	/etc/init.d/ipv6 radvd_start
}

dhcp6_deconfig()
{
	lanv6_deconfig
}

dhcp6_entry()
{
	logger -s -t "lanv6" -p daemon.debug "Called by dhcp6c on a $reason"

	case "$REASON" in
		'REQUEST') 
			dhcp6_setup
			;;
		'RENEW')
			dhcp6_setup
			;;
		'REBIND')
			dhcp6_deconfig
			;;
		'RELEASE')
			dhcp6_deconfig
			;;
	esac

	exit 0;
}

			
action="$1"

# if dhcp6 launch this script, it is usually on REQUEST 
# and prefix is automatically provided in env var prefix_iapd
if [ -n "$REASON" ]; then
	dhcp6_entry
fi

# we launch this script by hand
case "$action" in 
	'config')
		lanv6_config
		;;
	'deconfig')
		lanv6_deconfig
		;;
esac

exit 0;
