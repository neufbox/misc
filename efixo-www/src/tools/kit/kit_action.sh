#! /bin/sh

if [ $# -lt 2 ]; then
	echo "Usage: `basename $0` script action [boxip]"
	exit 1
fi

BOXIP="neufbox"
if [ ! -z $3 ]; then
	BOXIP=$3
fi

curl -d "script=$1&action=$2" $BOXIP/kit/action
