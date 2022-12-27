#!/bin/sh

if [ "$1" = "freebsd" ] ; then
    #export REPO_AUTOUPDATE=NO
    #pkg install -y ncurses
    printf ""
    exit $?
fi

if [ "$1" = "macos" ] ; then
    export HOMEBREW_NO_AUTO_UPDATE=1
    ## macOS already provides ncurses by default
    #brew install ncurses
    exit $?
fi

# ============================================

uname -a

export CFLAGS="-DDEBUG_W_ENDIAN"

./configure  || exit 1
make         || exit 1
make check   || exit 1
make install || exit 1

exit 0

for i in config.h config.log config.mk
do
    echo "
===============================
	$i
===============================
"
    cat ${i}
done

exit ${exit_code}
