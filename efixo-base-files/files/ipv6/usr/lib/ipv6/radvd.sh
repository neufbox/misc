#!/bin/sh
#
# radvd configuration
#
# usage : radvd.sh <file> <lanif> <prefix> <addr>
#

if [ $# -ne 4 ]
then
	logger -s -t "[ipv6] $0" -p daemon.debug "usage: radvd.sh <file> <lanif> <prefix>"
	exit 1
fi

RADVD_CONF=$1
lanif=$2
prefix=`echo $3|sed 's/\/56$/\/64/'`
addr=`echo $4|sed 's/\/64$//'`



[ -e "$RADVD_CONF" ] && rm -f $RADVD_CONF

echo "interface $lanif
{
 AdvSendAdvert on;
 AdvManagedFlag off;
 AdvOtherConfigFlag off;
 AdvHomeAgentFlag on;
 AdvHomeAgentInfo on;
 HomeAgentPreference 10;
 AdvIntervalOpt on;
 prefix $prefix
 {
    AdvOnLink on;
    AdvAutonomous on;
    AdvRouterAddr on;
    AdvValidLifetime 604800;
    AdvPreferredLifetime 604800;
 };
 RDNSS $addr {  };
};
" > $RADVD_CONF

exit 0
