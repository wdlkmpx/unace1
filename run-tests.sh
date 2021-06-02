#!/bin/sh
# This is free and unencumbered software released into the public domain.
#  For more information, please refer to <https://unlicense.org>

MWD=$(pwd)
TESTDIR="$HOME/.cache/unace1tests"
#KEEP_TESTS=1
mkdir -p "${TESTDIR}"

app="$(pwd)/unace"
appbn=$(basename $app)

# ===========================================================================

if test -z "$MD5SUM" ; then
	if command -v md5sum 2>/dev/null ; then
		MD5SUM='md5sum'
	elif command -v gmd5sum 2>/dev/null ; then
		MD5SUM='gmd5sum'
	elif command -v md5 2>/dev/null ; then
		MD5SUM='md5'
	fi
fi

compiler=
for i in gcc clang cc
do
	if command -v $i 2>/dev/null ; then
		compiler="${i}"
		CC="CC=${i}"
		break
	fi
done
echo $CC

if [ "${compiler}" = "clang" ] ; then
	sh --version # it might not be bash, need to know
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
				return
			fi
		done < ${md5file}
		echo "OK"
		return
	fi

	if ${MD5SUM} -c ${md5file} >${logfile} 2>&1 ; then
		echo "OK"
	else
		ret=1
		echo "ERROR"
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

if test -f Makefile ; then
	${make_clean}
	make $CC CFLAGS="-D${appbn}_TRACE -ggdb3"
fi

if ! test -f ${app} ; then
	echo "$app not found"
	exit 1
fi

# ===========================================================================

cd ${TESTDIR}

ret=0

echo
printf "* tests/dirtraversal1.ace: "
${app} x -y ${MWD}/tests/dirtraversal1.ace >${TESTDIR}/dirtraversal1.log 2>&1
if [ $? -eq 7 ] ; then
	echo "OK"
else
	ret=1
	echo "ERROR"
fi

printf "* tests/dirtraversal2.ace: "
${app} x -y ${MWD}/tests/dirtraversal2.ace >${TESTDIR}/dirtraversal2.log 2>&1
if [ $? -eq 7 ] ; then
	echo "OK"
else
	ret=1
	echo "ERROR"
fi

printf "* tests/out_of_bounds.ace: "
${app} t ${MWD}/tests/out_of_bounds.ace >${TESTDIR}/out_of_bounds.log 2>&1
if grep 'fault' ${TESTDIR}/out_of_bounds.log ; then
	echo "ERROR"
else
	echo "OK"
fi

printf "* tests/onefile.ace: "
rm -f CHANGES.LOG
${app} x -y ${MWD}/tests/onefile.ace >${TESTDIR}/onefile.log 2>&1
check_md5 ${MWD}/tests/onefile.md5 onefile.log

printf "* tests/ZGFX2.ace: "
rm -rf ZGFX2
${app} x -y ${MWD}/tests/ZGFX2.ace >${TESTDIR}/ZGFX2.log 2>&1
if [ $? -eq 0 ] && [ $(find ZGFX2 -type f | wc -l) -eq 223 ] ; then
	check_md5 ${MWD}/tests/ZGFX2.md5 ZGFX2.log
else
	ret=1
	echo "ERROR"
fi

printf "* tests/zman.ace: "
rm -rf zman
${app} x -y ${MWD}/tests/zman.ace >${TESTDIR}/zman.log 2>&1
if [ $? -eq 0 ] && [ $(find zman -type f | wc -l) -eq 20 ] ; then
	check_md5 ${MWD}/tests/zman.md5 zman.log
else
	ret=1
	echo "ERROR"
fi

printf "* tests/aclocal.ace: "
rm -rf aclocal
${app} x -y ${MWD}/tests/aclocal.ace >${TESTDIR}/aclocal.log 2>&1
if [ $? -eq 0 ] && [ $(find aclocal -type f | wc -l) -eq 77 ] ; then
	check_md5 ${MWD}/tests/aclocal.md5 aclocal.log
else
	ret=1
	echo "ERROR"
fi

# ===========================================================================

if test -z "$KEEP_TESTS" ; then
	rm -rf ${TESTDIR}
fi

exit $ret
