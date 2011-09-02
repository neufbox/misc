#!/bin/sh
#
# pppv6 configuration
#
# usage : pppv6.sh <file> <name> <password> 
#


if [ $# -ne 3 ]
then
	logger -s -t "[ipv6] $0" -p daemon.debug "usage: pppv6.sh <file> <name> <password>"
	exit 1
fi

PPPV6_CONF=$1
name=$2
password=$3

[ -e "$PPPV6_CONF" ] && rm -f $PPPV6_CONF

echo "user  $name
password $password
unit 1
noip
noipdefault 
noauth 
defaultroute 
noaccomp 
ipv6 ,
+ipv6
ipv6cp-use-persistent
ip-up-script /etc/ppp/ipv6-up
ip-down-script /etc/ppp/ipv6-down
lock
child-timeout 20
lcp-echo-failure 3
lcp-echo-interval 20
" > $PPPV6_CONF

exit 0
