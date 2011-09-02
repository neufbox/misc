#!/bin/sh

WGET="/usr/bin/wget"
BOXIP="neufbox"
TYPE="xml"

if [ -z $1 ]
then
  echo [-] ERROR: please specify a widget name.
  exit 1
fi

if [ ! -z $2 ]
then
  TYPE=$2
fi

$WGET -q -O - http://$BOXIP/wdg/$1?type=$TYPE
