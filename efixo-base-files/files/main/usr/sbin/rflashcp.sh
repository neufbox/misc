#!/bin/sh

usage()
{
    echo "Usage: $1 <url img> <partition>"
    exit 1
}

if [ ! $# -eq 2 ]
then
    usage `basename $0`
fi

scheme=`echo $1 | awk -F "://" '{print $1}'`
url=`echo $1 | awk -F "://" '{print $2}'`
host=`echo $url | awk -F "/" '{print $1}'`
filename=`echo $url | awk -F "/" '{print $2}'`
basefilename=`basename $filename`

mtdpart=$2

case "$scheme" in
    "tftp")
        cd /tmp/ && tftp -g -r $filename $host
        ;;
    "http" | "ftp")
        cd /tmp/ && wget $1
        ;;
    *)
        echo "Invalid scheme '$scheme' (available: tftp, http, ftp)"
        exit 1
esac

if [ ! $? ]
then
    echo "Unable to download file '$filename' from '$host' by '$scheme' method"
    exit 1
fi

flashcp -v $basefilename $mtdpart
if [ $? ]
then
    echo "Flash done !"
fi

exit 0
