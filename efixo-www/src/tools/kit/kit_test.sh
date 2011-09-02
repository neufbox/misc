#! /bin/sh

BOXIP="neufbox"
if [ ! -z $1 ]; then
	BOXIP=$1
fi

curl -d "void=void" $BOXIP/kit/test
