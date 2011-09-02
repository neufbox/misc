#!/usr/bin/env sh

VERSION=0.0

usage () {
    echo "Usage: $0 <project name>"
}

baseenv () {
    mkdir -p docroot/js
    mkdir -p docroot/css
    mkdir -p docroot/themes
    
    mkdir -p fcgiroot/lng/en
    mkdir -p fcgiroot/rgx
    mkdir -p fcgiroot/tpl

    mkdir -p src/
}

mkdefaultheme() {
    mkdir -p docroot/themes/default
    mkdir -p docroot/themes/default/css
    mkdir -p docroot/themes/default/img

    touch docroot/themes/default/css/common.css
    cat > docroot/themes/default/theme.xml <<EOF
<theme>
	<name>Default Theme</name>
	<version>0.0</version>
	<target>0.0</target>
	<uuid>3005f100-c33a-4611-8a44-f6431ae51617</uuid>
	<description>This is a default theme</description>
	<creator>John Doe</creator>
	<homepage></homepage>
</theme>
EOF
}

basefile() {
    cat > Makefile <<EOF
# Makefile
# --mkewfenv version: $VERSION -- 

PROJNAME := $PRJNAME

# installation variables
prefix	= /var/www/\$(PROJNAME)
docrootdir = \$(prefix)/docroot
fcgirootdir = \$(prefix)/fcgiroot
htdocsdir = \$(prefix)/htdocs

# target config
TGTNAME	:= fastcgi

# global variables
INSTALL	:= /usr/bin/install

# compilation variables
LIBSDYN := -lm -lz -lpcre -lezxml -lpthread
LIBSSTA := -lfcgi -lneo_cgi -lneo_cs -lneo_utl -ltropicssl -letk

LDLIBS :=  -Wl,-Bstatic -lewf \$(LIBSSTA) -Wl,-Bdynamic \$(LIBSDYN)

############################################################
# targets
tgt     = \$(TGTNAME)

# list C files
sources	= \$(wildcard src/*.c)

# get objects from C files
objects	= \$(sources:.c=.o)

############################################################
# rules
.PHONY: all
all: build

.PHONY: build
build: \$(tgt)

\$(tgt): \$(objects)

.PHONY: clean
clean:
	find -name "*.o" -delete
	rm -f \$(tgt)

.PHONY: mrproper
mrproper: clean

.PHONY: install
install: build

EOF
}

if [ $# -lt 1 ]
then
    usage
    exit 1
fi  

PRJNAME=$1

echo "- Making base environment"
baseenv

echo "- Install default theme"
mkdefaultheme

echo "- Creating base files"
basefile
