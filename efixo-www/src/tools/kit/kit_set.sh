#! /bin/sh

if [ $# -lt 2 ]; then
	echo "Usage: `basename $0` name value [boxip]"
	exit 1
fi

BOXIP="neufbox"
if [ ! -z $3 ]; then
	BOXIP=$3
fi

curl -d "name=$1&value=$2" $BOXIP/kit/set
