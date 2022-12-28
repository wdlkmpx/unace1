#!/bin/sh
# Public domain

# command to generate md5 checksums from <dirx>
# md5sum $(find dirx -type f | sort | sed 's%\./%%')

MWD=$(pwd)
TESTDIR="$HOME/.cache/unace1tests"
#KEEP_TESTS=1
mkdir -p "${TESTDIR}"

app="$(pwd)/src/unace1"
appbn=$(basename $app)
export CFLAGS="-D${appbn}_TRACE -g -O0 -ggdb3 -Wextra -Wno-unused-parameter -Wno-missing-field-initializers"
test_acev1_dir=${MWD}/tests
help_ret_code=8

# ===========================================================================

ERROR_FILES=''
add_error_file()
{
    ret=1
    ERROR_FILES="$ERROR_FILES $1"
}


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


if test -z "$MD5SUM" ; then
	if command -v md5sum 2>/dev/null ; then
		if [ -n "$(md5sum --help 2>&1 | grep BusyBox)" ] ; then
			MD5SUM='md5sum -s' #busybox
		else
			MD5SUM='md5sum --quiet'
		fi
	elif command -v gmd5sum 2>/dev/null ; then
		MD5SUM='gmd5sum --quiet'
	elif command -v md5 2>/dev/null ; then
		MD5SUM='md5'
	fi
fi


check_md5()
{
	if test -z "$MD5SUM" ; then
		return
	fi
	md5file="$1"
	logfile="$2"

	if [ "$MD5SUM" = "md5" ] ; then
		# BSD
		while read md5 file
		do
			if ! md5 -q -s "$md5" "$file" >/dev/null 2>&1 ; then
				echo "ERROR"
				add_error_file ${logfile}
				return
			fi
		done < ${md5file}
		echo "OK"
		return
	fi

	if ${MD5SUM} -c ${md5file} >>${logfile} 2>&1 ; then
		echo "OK"
	else
		echo "ERROR"
		add_error_file ${logfile}
	fi
}

echo

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
expected_ret=${help_ret_code}
${app} -h >/dev/null
iret=$?
if [ ${iret} -ne ${expected_ret} ] ; then
    echo "Something is wrong with $app"
    echo "Got code $iret (expected ${expected_ret})"
    echo "Aborted"
    exit 1
fi

# ===========================================================================

cmdecho()
{
    gdbargs=''
    if command -v gdb >/dev/null ; then
        gdbargs='gdb --args'
    fi
    echo "------------------------------------"
    echo "${gdbargs} $@"
    echo "------------------------------------"
    "$@"
}

cd ${TESTDIR}

ret=0

ACEFILE=${test_acev1_dir}/dirtraversal1.ace
MD5FILE=
LOGFILE=${TESTDIR}/dirtraversal1.log
echo
printf "* tests/${ACEFILE##*/}: "
cmdecho ${app} x -y ${ACEFILE} >${LOGFILE} 2>&1
if [ $? -eq 7 ] ; then
	echo "OK"
else
	echo "ERROR"
	add_error_file ${LOGFILE}
fi

ACEFILE=${test_acev1_dir}/dirtraversal2.ace
MD5FILE=
LOGFILE=${TESTDIR}/dirtraversal2.log
printf "* tests/${ACEFILE##*/}: "
cmdecho ${app} x -y ${ACEFILE} >${LOGFILE} 2>&1
if [ $? -eq 7 ] ; then
	echo "OK"
else
	echo "ERROR"
	add_error_file ${LOGFILE}
fi

ACEFILE=${test_acev1_dir}/out_of_bounds.ace
MD5FILE=
LOGFILE=${TESTDIR}/out_of_bounds.log
printf "* tests/out_of_bounds.ace: "
cmdecho ${app} t ${ACEFILE} >${LOGFILE} 2>&1
if grep 'fault' ${LOGFILE} ; then
	echo "ERROR"
	add_error_file ${LOGFILE}
else
	echo "OK"
fi

ACEFILE=${test_acev1_dir}/onefile.ace
MD5FILE=${test_acev1_dir}/onefile.md5
LOGFILE=${TESTDIR}/onefile.log
printf "* tests/${ACEFILE##*/}: "
rm -f CHANGES.LOG
cmdecho ${app} x -y ${ACEFILE} >${LOGFILE} 2>&1
check_md5 ${MD5FILE} ${LOGFILE}

## unace1 doesn't support password protected archives
#ACEFILE=${test_acev1_dir}/passwd.ace
#MD5FILE=${test_acev1_dir}/passwd.md5
#LOGFILE=${TESTDIR}/passwd.log
#printf "* tests/${ACEFILE##*/}: "
#rm -f passwd.m4
#cmdecho ${app} x -y -p1234 ${ACEFILE} >${LOGFILE} 2>&1
#check_md5 ${MD5FILE} ${LOGFILE}

ACEFILE=${test_acev1_dir}/ZGFX2.ace
MD5FILE=${test_acev1_dir}/ZGFX2.md5
LOGFILE=${TESTDIR}/ZGFX2.log
printf "* tests/${ACEFILE##*/}: "
rm -rf ZGFX2
cmdecho ${app} x -y ${ACEFILE} >${LOGFILE} 2>&1
check_md5 ${MD5FILE} ${LOGFILE}

ACEFILE=${test_acev1_dir}/zdir.ace
MD5FILE=${test_acev1_dir}/zdir.md5
LOGFILE=${TESTDIR}/zdir.log
printf "* tests/${ACEFILE##*/}: "
rm -rf zman
cmdecho ${app} x -y ${ACEFILE} >${LOGFILE} 2>&1
check_md5 ${MD5FILE} ${LOGFILE}

ACEFILE=${test_acev1_dir}/multivolume.ace
MD5FILE=${test_acev1_dir}/multivolume.md5
LOGFILE=${TESTDIR}/multivolume.log
printf "* tests/${ACEFILE##*/}: "
rm -rf aclocal
cmdecho ${app} x -y ${ACEFILE} >${LOGFILE} 2>&1
check_md5 ${MD5FILE} ${LOGFILE}

# ===========================================================================

if [ $ret != 0 ] ; then
    KEEP_TESTS=1 # errors, need logs
    cat $ERROR_FILES
fi

if test -z "$KEEP_TESTS" ; then
	rm -rf ${TESTDIR}
else
	printf "\n\n.......................................\n"
	printf "- Logs are in $TESTDIR\n"
	printf ".......................................\n\n"
fi

exit $ret
