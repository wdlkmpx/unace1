#!/bin/sh
# This is free and unencumbered software released into the public domain.
#  For more information, please refer to <https://unlicense.org>

MWD=$(pwd)
TESTDIR="$HOME/.cache/unace1tests"
remove_test_dir=1 # comment out
mkdir -p "${TESTDIR}"

app="$(pwd)/unace"

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
	make CFLAGS="-D${appbn}_TRACE -ggdb3"
fi

if ! test -f ${app} ; then
	echo "$app not found"
	exit 1
fi

cd ${TESTDIR}

ret=0

echo
printf "* tests/dirtraversal1.ace: "
${app} x ${MWD}/tests/dirtraversal1.ace >${TESTDIR}/dirtraversal1.log 2>&1
if [ $? -eq 7 ] ; then
	echo "OK"
else
	echo "ERROR"
fi

if test -n "$remove_test_dir" ; then
	rm -rf ${TESTDIR}
fi

exit 0
