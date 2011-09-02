#!/bin/sh
#
# xl2tpd configuration 
#
# usage : xl2tpd.sh <file> <name> <lns> <pppconffile>
#


if [ $# -ne 5 ]
then
	logger -s -t "[ipv6] $0" -p daemon.debug "usage: xl2tpd.sh <file> <name> <lns> <pppconffile>"
	exit 1
fi

XL2TPD_CONF=$1
name=$2
lns_ip=$3
PPPV6_CONF=$4
ppp_adsl_ipaddr=$5


[ -e "$XL2TPD_CONF" ] && rm -f $XL2TPD_CONF

echo "[global]
;debug avp = yes 
;debug network = yes 
;debug state = yes 
;debug tunnel = yes 
listen-addr = $ppp_adsl_ipaddr
port = 1701 
access control = no 

[lac L2TP] 

hostname = $lns_ip
name = $name
lns = $lns_ip
redial = no
;redial timeout = 15 
hidden bit = no 
;ppp debug = yes 
pppoptfile = $PPPV6_CONF
require authentication = no 
refuse authentication = no
refuse chap = no
flow bit = yes 
length bit = yes
" > $XL2TPD_CONF


exit 0
