#! /bin/sh

if [ $# -lt 1 ]; then
	echo "Usage: `basename $0` name [boxip]"
	exit 1
fi

BOXIP="neufbox"
if [ ! -z $2 ]; then
	BOXIP=$2
fi

curl -d "name=$1" $BOXIP/kit/get
