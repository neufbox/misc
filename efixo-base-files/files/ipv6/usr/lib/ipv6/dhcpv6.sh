#!/bin/sh
#
# dhcpv6 configuration
#
# usage : dhcpv6.sh <file> <wan6if>
#

if [ $# -ne 2 ]
then
	logger -s -t "[ipv6] $0" -p daemon.debug "usage: dhcpv6.sh <file> <wan6if>"
	exit 1
fi

DHCPV6_CONF=$1
wan6if=$2

[ -e "$DHCPV6_CONF" ] && rm -f $DHCPV6_CONF

echo "interface $wan6if {
        send ia-pd 0;
	script \"/usr/lib/ipv6/lanv6.sh\";
};

id-assoc pd {
        prefix-interface lan0 {
               sla-id 1;
               sla-len 8;
        };
};
" > $DHCPV6_CONF

exit 0
