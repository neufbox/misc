#!/bin/bash

web_login="admin"
web_password="admin"

function initializeANSI {
  esc=""

  blackf="${esc}[30m";   redf="${esc}[31m";    greenf="${esc}[32m"
  yellowf="${esc}[33m"   bluef="${esc}[34m";   purplef="${esc}[35m"
  cyanf="${esc}[36m";    whitef="${esc}[37m"
  
  blackb="${esc}[40m";   redb="${esc}[41m";    greenb="${esc}[42m"
  yellowb="${esc}[43m"   blueb="${esc}[44m";   purpleb="${esc}[45m"
  cyanb="${esc}[46m";    whiteb="${esc}[47m"

  boldon="${esc}[1m";    boldoff="${esc}[22m"
  italicson="${esc}[3m"; italicsoff="${esc}[23m"
  ulon="${esc}[4m";      uloff="${esc}[24m"
  invon="${esc}[7m";     invoff="${esc}[27m"

  reset="${esc}[0m"
}

initializeANSI

boxip="192.168.1.1"

auth="all"
auth_button="fail"
auth_passwd="fail"

module=
method=

fail_count=0
ok_count=0
warning_count=0

i=0

while getopts 'M:m:a:' OPTION
do
    case $OPTION in
	M)	module="$OPTARG"

	    ;;
	m)	method="$OPTARG"
	    ;;
        a)      auth="$OPTARG"
            ;;
	?)	printf "Usage: %s: [-M <module>] [-m <method>] <boxip>\n" $(basename $0) >&2
	    exit 2
	    ;;
    esac
done
shift $(($OPTIND - 1))

#printf "Remaining arguments are: %s\n" "$*"

if [ $# == 1 ]
then
    boxip=$1;
fi

# authentificate by button
if [ $auth = "all" -o $auth = "button" ]; then
    echo " -- Authentificate by button -- "
    
    tk1=`curl -s http://$boxip/api/1.0/?method=auth.getToken |grep token|cut -d '"' -f 2|cut -d \n -f 1 2>/dev/null `
    r='fail'
    
    echo "Token: $tk1"
    
    echo "Waiting press authentification button "
    i=0
    while [ "$r" != "ok" -a $i -lt 15 ]; do
        r=`curl -s http://$boxip/api/1.0/?method=auth.checkToken\&token=$tk1 |grep stat|cut -d '"' -f 2 |cut -d \n -f 1 2>/dev/null `
        echo "."
        sleep 1
        let i+=1
    done;

    if [ "$r" = "ok" ]; then
        echo "Authentificate by button OK "
        auth_button="ok"
    else
        let warning_count+=1
        echo "Authentificate by button FAIL !"
    fi
fi

# authentificate by login/password
if [ $auth = "all" -o $auth = "passwd" ]; then
    echo " -- Authentificate by login/password -- "
    
    tk2=`curl -s http://$boxip/api/1.0/?method=auth.getToken |grep token|cut -d '"' -f 2|cut -d \n -f 1 2>/dev/null `
    
    echo "Token: $tk2"

    web_login_sha256hash=`./tests/sha256hash $web_login`
    web_login_hash=`./tests/hmacsha256hash $tk2 $web_login_sha256hash`
    
    web_password_sha256hash=`./tests/sha256hash $web_password`
    web_password_hash=`./tests/hmacsha256hash $tk2 $web_password_sha256hash`
    
    echo "Send login and password"
    r=`curl -s http://$boxip/api/1.0/?method=auth.checkToken\&token=$tk2\&hash=${web_login_hash}${web_password_hash} |grep stat|cut -d '"' -f 2 |cut -d \n -f 1 2>/dev/null `
    
    if [ "$r" != 'ok' ]; then
        let warning_count+=1
        
        echo "Authentificate by login/password FAIL"
        if [ $auth_button = "ok" ]; then
            echo " -> Use token from button auth"
            tk=$tk1
        fi
    else
        echo "Authentificate by login/password OK"
        auth_passwd="ok"
        tk=$tk2
    fi
fi

if [ $auth != "off" -a $auth_button = "fail" -a $auth_passwd = "fail" ]; then
    echo "(All) authentificate method(s) FAILED !"
    exit 1
fi

# $1 -> 'POST' or 'GET'
# $2 -> method name
# $3 -> args
function mexec {
    modn=`echo "$2" | cut -d '.' -f 1`
    metn=`echo "$2" | cut -d '.' -f 2`
    if [ -n "${module}" -a "${module}" != "${modn}" ]; then
	return
    fi
    
    if [ -n "${method}" -a "${method}" != "${metn}" ]; then
	return
    fi
    
    tmp_filename="/tmp/$(basename $0).$RANDOM.xml"
    > $tmp_filename
    
    cmd="";
    args=$3
    if [ -z $args ]; then
	args="void=void";
    fi

    if [ $1 = 'POST' ]; then
	cmd="curl -s -d ${args} http://${boxip}/api/1.0/?method=${2} -o $tmp_filename";
        curl -s -d ${args} http://${boxip}/api/1.0/?method=${2} -o $tmp_filename
        ret=$?
    else
	cmd="curl -s -G -d ${args} http://${boxip}/api/1.0/?method=${2} -o $tmp_filename";
        curl -s -G -d ${args} http://${boxip}/api/1.0/?method=${2} -o $tmp_filename
        ret=$?
    fi
    
    echo "${blueb} -> ${cmd}${reset}"

    if [ $ret != 0 ]; then
        let fail_count+=1
        echo "       ${redf}EXECUTION FAIL !!!${reset}";

        rm "$tmp_filename"
        sleep 1
        return
    fi
    
    cat "$tmp_filename"
    xmllint --noout "$tmp_filename"

    if [ $? != 0 ]; then
        let fail_count+=1
        echo "       ${redf}INVALID XML !!!${reset}";

        rm "$tmp_filename"
        sleep 1
        return
    fi

    r=`cat $tmp_filename |grep "<rsp stat=" | cut -d '"' -f 2|cut -d \n -f 1`
    if [ "$r" = "ok" ]; then
        let ok_count+=1
	echo "       ${greenf}OK${reset}";
    else
        let fail_count+=1
	echo "       ${redf}FAIL !!!${reset}";
    fi

    rm "$tmp_filename"
    sleep 1
}

# $1 -> module name
function mhead {
    if [ -z "${module}" -o "${module}" = "${1}" ]; then
	printf "${yellowb}** %s ******************${reset}\n" $1;
    fi
}

#
#
#

# DSL

mhead "dsl"
mexec GET dsl.getInfo

mhead "firewall"
mexec GET firewall.getInfo "token=${tk}"
mexec POST firewall.disableSmtpFilter "token=${tk}"
mexec POST firewall.enableSmtpFilter "token=${tk}"

mhead "hotspot"
mexec GET hotspot.getInfo "token=${tk}"
mexec GET hotspot.getClientList "token=${tk}"
mexec POST hotspot.disable "token=${tk}"
mexec POST hotspot.enable "token=${tk}"
mexec POST hotspot.stop "token=${tk}"
mexec POST hotspot.start "token=${tk}"
mexec POST hotspot.restart "token=${tk}"
mexec POST hotspot.setMode "token=${tk}&mode=sfr_fon"

mhead "lan"
mexec GET lan.getInfo "token=${tk}"

mhead "ppp"
mexec GET ppp.getInfo
mexec GET ppp.getCredentials "token=${tk}"
mexec POST ppp.setCredentials "token=${tk}&login=0123456789@neufpnp&password=neufpnp"

mhead "system"
mexec GET system.getInfo
mexec GET system.getWpaKey "token=${tk}"
mexec POST system.setNetMode "token=${tk}&mode=router"

mhead "voip"
mexec GET voip.getInfo "token=${tk}"
mexec POST voip.stop "token=${tk}"
mexec POST voip.start "token=${tk}"
mexec POST voip.restart "token=${tk}"

mhead "wan"
mexec GET wan.getInfo "token=${tk}"

mhead "wlan"
mexec GET wlan.getInfo "token=${tk}"
mexec GET wlan.getClientList "token=${tk}"
mexec POST wlan.setChannel "token=${tk}&channel=11"
mexec POST wlan.setWlanMode "token=${tk}&mode=1"
mexec POST wlan.setWl0Ssid "token=${tk}&ssid=SSID_TEST"
mexec POST wlan.setWl0Enc "token=${tk}&enc=WPA2-PSK"
mexec POST wlan.setWl0Keytype "token=${tk}&keytype=ascii"
mexec POST wlan.setWl0Wpakey "token=${tk}&wpakey=EZFFFEZFEZFZE51561FEZFefezFEZFEZG"
mexec POST wlan.setWl0Wepkey "token=${tk}&wepkey=EZFFFEZFEZFZE"
mexec POST wlan.disable "token=${tk}"
mexec POST wlan.enable "token=${tk}"
mexec POST wlan.stop "token=${tk}"
sleep 5
mexec POST wlan.start "token=${tk}"
sleep 5
mexec POST wlan.restart "token=${tk}"

mhead "backup3g"
mexec POST backup3g.forceDataLink "token=${tk}&mode=off"
mexec POST backup3g.forceVoipLink "token=${tk}&mode=off"

echo "------------------------------------------"
echo " RESULT: ${ok_count} ok, ${fail_count} fail(s), ${warning_count} warning(s)"
echo "------------------------------------------"
