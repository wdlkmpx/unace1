#!/bin/sh

#RUN_TEST_SCRIPT_DIRECTLY=1

w_system=$(uname -s)

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

cmdecho()
{
    echo "---------------------"
    echo "# $@"
    echo "---------------------"
    "$@"
}


print_config_log()
{
    for i in config.h config.log config.mk
    do
        echo "
===============================
	$i
===============================
"
        cat ${i}
    done
}


exit_error()
{
    print_config_log
    exit 1
}

# ============================================
# print some system info

uname -a
echo
id
echo

case "$w_system" in
    Darwin|FreeBSD)
        sysctl kern.coredump
        sysctl kern.corefile
        ;;
esac

# ============================================

#export CFLAGS="-DDEBUG_W_ENDIAN"
export VERBOSE_ERRORS='yes' # the test script read this
#export PRINT_SUM_ON_ERROR='yes'
#exprt DOWNLOAD_IS_REQUIRED='yes'

if [ "$RUN_TEST_SCRIPT_DIRECTLY" ] ; then
    cmdecho ./scripts/run-tests.sh -debug || exit_error
else
    cmdecho ./configure  || exit_error
    cmdecho make         || exit_error
    cmdecho make check   || exit_error
    #cmdecho make install || exit_error
fi
