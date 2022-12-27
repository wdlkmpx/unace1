#!/bin/sh
# Public domain

MWD=$(pwd)
TESTDIR="$HOME/.cache/unace1tests"
#KEEP_TESTS=1
mkdir -p "${TESTDIR}"

app="$(pwd)/src/unace1"
appbn=$(basename $app)
export CFLAGS="-D${appbn}_TRACE -ggdb3"
test_ace_dir=${MWD}/tests

# ===========================================================================

ERROR_FILES=''
add_error_file()
{
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
		MD5SUM='md5sum'
	elif command -v gmd5sum 2>/dev/null ; then
		MD5SUM='gmd5sum'
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
				ret=1
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
		ret=1
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
expected_ret=8
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

echo
printf "* tests/dirtraversal1.ace: "
cmdecho ${app} x -y ${test_ace_dir}/dirtraversal1.ace >${TESTDIR}/dirtraversal1.log 2>&1
if [ $? -eq 7 ] ; then
	echo "OK"
else
	ret=1
	echo "ERROR"
	add_error_file ${TESTDIR}/dirtraversal1.log
fi

printf "* tests/dirtraversal2.ace: "
cmdecho ${app} x -y ${test_ace_dir}/dirtraversal2.ace >${TESTDIR}/dirtraversal2.log 2>&1
if [ $? -eq 7 ] ; then
	echo "OK"
else
	ret=1
	echo "ERROR"
	add_error_file ${TESTDIR}/dirtraversal2.log
fi

printf "* tests/out_of_bounds.ace: "
cmdecho ${app} t ${test_ace_dir}/out_of_bounds.ace >${TESTDIR}/out_of_bounds.log 2>&1
if grep 'fault' ${TESTDIR}/out_of_bounds.log ; then
	ret=1
	echo "ERROR"
	add_error_file ${TESTDIR}/out_of_bounds.log
else
	echo "OK"
fi

printf "* tests/onefile.ace: "
rm -f CHANGES.LOG
cmdecho ${app} x -y ${test_ace_dir}/onefile.ace >${TESTDIR}/onefile.log 2>&1
check_md5 ${test_ace_dir}/onefile.md5 ${TESTDIR}/onefile.log

## unace1 doesn't support password protected archives
#printf "* tests/passwd.ace: "
#rm -f passwd.m4
#cmdecho ${app} x -y -p1234 ${test_ace_dir}/passwd.ace >${TESTDIR}/passwd.log 2>&1
#check_md5 ${test_ace_dir}/passwd.md5 ${TESTDIR}/passwd.log

printf "* tests/ZGFX2.ace: "
rm -rf ZGFX2
cmdecho ${app} x -y ${test_ace_dir}/ZGFX2.ace >${TESTDIR}/ZGFX2.log 2>&1
if [ $? -eq 0 ] && [ $(find ZGFX2 -type f | wc -l) -eq 223 ] ; then
	check_md5 ${test_ace_dir}/ZGFX2.md5 ${TESTDIR}/ZGFX2.log
else
	ret=1
	echo "ERROR"
	add_error_file ${TESTDIR}/ZGFX2.log
fi

printf "* tests/zdir.ace: "
rm -rf zman
cmdecho ${app} x -y ${test_ace_dir}/zdir.ace >${TESTDIR}/zdir.log 2>&1
if [ $? -eq 0 ] && [ $(find zman -type f | wc -l) -eq 20 ] ; then
	check_md5 ${test_ace_dir}/zdir.md5 ${TESTDIR}/zdir.log
else
	ret=1
	echo "ERROR"
	add_error_file ${TESTDIR}/zdir.log
fi

printf "* tests/multivolume.ace: "
rm -rf aclocal
cmdecho ${app} x -y ${test_ace_dir}/multivolume.ace >${TESTDIR}/multivolume.log 2>&1
if [ $? -eq 0 ] && [ $(find aclocal -type f | wc -l) -eq 77 ] ; then
	check_md5 ${test_ace_dir}/multivolume.md5 ${TESTDIR}/multivolume.log
else
	ret=1
	echo "ERROR"
	add_error_file ${TESTDIR}/multivolume.log
fi

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
