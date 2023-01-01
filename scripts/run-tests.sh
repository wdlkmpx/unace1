#!/bin/sh
# Public domain

MWD=$(pwd)
CHKSUM_TYPE='md5'
WGET_IS_REQUIRED='no'

TESTDIR="$HOME/.cache/unace1tests"
#KEEP_TESTS=1
#VERBOSE_ERRORS=1 #this is for CI
mkdir -p "${TESTDIR}"

app="$(pwd)/src/unace1"
appbn=$(basename $app)
export CFLAGS="-D${appbn}_TRACE -g -O0 -ggdb3 -Wextra -Wno-unused-parameter -Wno-missing-field-initializers"
help_param="-h"
help_ret_code=0

test_acev1_dir=${MWD}/tests

# ===========================================================================
# Generic functions

scriptdir=$(dirname "$0")
. ${scriptdir}/functions.sh

set_checksum_app
set_wget

set_wine()
{
    wine=''
    if [ "$(uname)" = "Linux" ] ; then
        wine='wine'
        if command -v wine >/dev/null ; then
            echo "WINE is not installed, cannot test .exe binaries..."
            exit 1
        fi
    fi
}

check_sums_from_file()
{
	chksum_file="$1"
	logfile="$2"
	if test -z "$CHKSUM_APP" || test -z "$chksum_file" ; then
		return 0 # ok
	fi
    if [ -n "$logfile" ] ; then
        echo "------------------------------" >>${logfile}
    fi
	if [ "$CHKSUM_APP" = "$CHKSUM_BSD" ] ; then
		# --BSD--
		while read sum file
		do
			filesum=$($CHKSUM_APP -q "$file")
			if [ "$sum" != "$filesum" ] ; then
				echo "${file}: FAILED"  >>${logfile}
				echo "*** checksum failed. stopped" >>${logfile}
				return 1 #error
			fi
		done < ${chksum_file}
	else
		# --GNU--
		if ! ${CHKSUM_APP} -c ${chksum_file} >>${logfile} 2>&1 ; then
			return 1 #error
		fi
	fi
    if [ -n "$logfile" ] ; then
        echo "------------------------------" >>${logfile}
    fi
	return 0 # ok
}


check_dirs_from_dirlist_file()
{
	dirlist_file="$1"
	logfile="$2"
	if [ -z "$dirlist_file" ] ; then
		return 0
	fi
	wret=0
	echo "checking dirs..." >>${logfile}
	while read dir ; do
		if [ ! -d "$dir" ] ; then
			wret=1
			echo "[ERROR] $dir is missing" >>${logfile}
		fi
	done < ${dirlist_file}
	return ${wret}
}

# ===========================================================================
# Functions specific to this script

set_test_variables() # $1: <test_name> [exit_code]
{
    # variables to set before calling this function:
    # - TESTFILE_DIR
    # - TESTFILE_EXT
    # - TESTDIR
    # - TEST_USE_SUBDIR [yes/no]
    TESTFILE_NAME="$1"
    if [ -z "$TESTFILE_NAME" ] ; then
        echo "run_test: no test name"
        exit 1
    fi
    TEST_EXIT_CODE="$2" # !XX = error code
    #--
    TESTFILE1=${TESTFILE_DIR}/${TESTFILE_NAME}${TESTFILE_EXT}
    TESTFILE2=${TESTFILE_DIR}/${TESTFILE_NAME}${TESTFILE_EXT2}
    TESTFILE=''
    if [ -f "$TESTFILE1" ] ; then
        TESTFILE=${TESTFILE1}
    elif [ -f "$TESTFILE2" ] ; then
        TESTFILE=${TESTFILE2}
    fi
    CHKFILE=${TESTFILE_DIR}/${TESTFILE_NAME}.${CHKSUM_TYPE}
    DIRFILE=${TESTFILE_DIR}/${TESTFILE_NAME}.dirs
    LOGFILE=${TESTDIR}/${TESTFILE_NAME}.log
    #--
    if [ -z "$TESTFILE" ] ; then
        echo "ERROR: $TESTFILE1 doesn't exist "
        echo "TEST = ${TESTFILE_NAME}"
        echo "Fix the script!"
        exit 1
    fi
    if [ ! -f "${CHKFILE}" ] ; then
        unset CHKFILE
    fi
    if [ ! -f "${DIRFILE}" ] ; then
        unset DIRFILE
    fi
}

gdbargs=''
if command -v gdb >/dev/null ; then
    gdbargs='gdb --args'
fi

ERROR_FILES=''

run_test()
{
    test_error=
    printf "* [test] ${TESTFILE_NAME}${TESTFILE_EXT}: "
    echo >${LOGFILE}
    echo "------------------------------------" >>${LOGFILE}
    echo "${gdbargs} $@" >>${LOGFILE}
    echo "------------------------------------" >>${LOGFILE}
    if [ "$TEST_USE_SUBDIR" = "yes" ] ; then
        xcurdirx=$(pwd)
        mkdir -p ${TESTDIR}/${TESTFILE_NAME}_test
        cd ${TESTDIR}/${TESTFILE_NAME}_test
    fi
    "$@" >>${LOGFILE} 2>&1
    exit_code=$?
    if [ -n "$TEST_EXIT_CODE" ] ; then
        case ${TEST_EXIT_CODE} in
        !*)
            if [ "!${exit_code}" = "${TEST_EXIT_CODE}" ] ; then
                echo "[ERROR] Exit code  = ${exit_code}" >>${LOGFILE}
                test_error=1
            fi
            ;;
        *)
            if [ "${exit_code}" = "${TEST_EXIT_CODE}" ] ; then
                echo "[OK] Exit code = ${exit_code}" >>${LOGFILE}
            else
                echo "[ERROR] Exit code  = ${exit_code}" >>${LOGFILE}
                echo "Expected exit code = ${valid_exit_code}" >>${LOGFILE}
                test_error=1
            fi
            ;;
        esac
    fi
    if ! check_sums_from_file "${CHKFILE}" "${LOGFILE}" ; then
        test_error=1
    fi
    if ! check_dirs_from_dirlist_file "${DIRFILE}" "${LOGFILE}" ; then
        test_error=1
    fi
    if [ -z "$test_error" ] ; then
        echo "OK"
    else
        echo "ERROR"
        ERROR_FILES="$ERROR_FILES $1"
    fi
    if [ "$TEST_USE_SUBDIR" = "yes" ] ; then
        cd "${xcurdirx}"
    fi
}

# ===========================================================================

if ! test -f configure ; then
	if test -f autogen.sh ; then
		./autogen.sh
	fi
fi

if test -f configure ; then
	if ! test -f config.h ; then
		./configure ${configure_opts}
	fi
fi

if test -f ${app}.exe ; then
	# .exe binary
	set_wine
	app="${wine} ${app}"
elif test -f ${app} ; then
	app="${app}"
else
	if test -f Makefile ; then
		${make_clean}
		make
	fi

	if ! test -f ${app} ; then
		echo "$app not found"
		exit 1
	fi
fi

# basic check
check_app_help ${help_ret_code} "${app} ${help_param}"

# ===========================================================================

cd ${TESTDIR}

TESTFILE_DIR=${test_acev1_dir}
TESTFILE_EXT='.ace'
TESTFILE_EXT2='.ACE'
TEST_USE_SUBDIR='no'

set_test_variables 'dirtraversal1' 7
run_test ${app} x -y ${TESTFILE}

set_test_variables 'dirtraversal2' 7
run_test ${app} x -y ${TESTFILE}

set_test_variables 'out_of_bounds' "!139" # 139 = segfault
run_test ${app} t ${TESTFILE}

set_test_variables 'onefile'
rm -f CHANGES.LOG
run_test ${app} x -y ${TESTFILE}

#set_test_variables 'passwd'
#rm -f passwd.m4
#run_test ${app} x -y -p1234 ${TESTFILE}

set_test_variables 'ZGFX2'
rm -rf ZGFX2
run_test ${app} x -y ${TESTFILE}

set_test_variables 'zdir'
rm -rf zman
run_test ${app} x -y ${TESTFILE}

set_test_variables 'multivolume'
rm -rf aclocal
run_test ${app} x -y ${TESTFILE}

# ===========================================================================

ret=0
if [ -n "$(echo $ERROR_FILES)" ] ; then
    KEEP_TESTS=1 # errors, need logs
    ret=1
    if [ -n "$VERBOSE_ERRORS" ] ; then
        cat $ERROR_FILES
    fi
fi

if test -z "$KEEP_TESTS" ; then
	rm -rf ${TESTDIR}
else
	printf "\n.......................................\n"
	printf " Logs are in $TESTDIR\n"
	printf ".......................................\n\n"
fi

exit $ret
