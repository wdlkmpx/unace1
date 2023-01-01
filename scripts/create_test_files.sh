#!/bin/sh
# Public domain

MWD=$(pwd)
CHKSUM_TYPE='md5'
WGET_IS_REQUIRED='no'
CMD="unace x %s" # %s = inputfile


scriptdir=$(dirname "$0")
. ${scriptdir}/functions.sh

set_checksum_app
set_wget

create_dirlist_file_from_dir() # $1:<from_dir> $2:<dirlist_file>
{
    fromdir="$1"
    dirlist_file="$2"
    find ${fromdir} -type d | sort | sed -e 's%\./%%' -e '/^\.$/d' > ${dirlist_file}
    if [ "$(cat "${dirlist_file}")" = "" ] ; then
        # empty dir, remove dirlist_file
        rm -f "${dirlist_file}"
    fi
}

create_chksum_file_from_dir() # $1:<from_dir> $2:<chkfile>
{
    fromdir="$1"
    chkfile="$2"
    if test -z "$CHKSUM_APP" ; then
        return 0 # ok
    fi
    if [ "$CHKSUM_APP" = "$CHKSUM_BSD" ] ; then
        # --BSD--
        find ${fromdir} -type f | sort | sed 's%\./%%' | \
            while read xfilex ; do
                filesum=$($CHKSUM_APP -q "$xfilex")
                echo "$filesum  $xfilex"
            done > ${chkfile}
            
    else
        # --GNU--
        find ${fromdir} -type f | sort | sed 's%\./%%' | \
            while read xfilex ; do
                ${CHKSUM_APP} "$xfilex" # handle files with spaces
            done > ${chkfile}
    fi
    if [ $? -ne 0 ] ; then
        return 1 #error
    fi
    return 0 # ok
}

process_file()
{
    inputfile=${1}
    file=$(basename "$inputfile")
    base=${file%.*} # remove extension
    case $inputfile in
      http*|ftp*)
        download_file "$inputfile"
        if [ $? -ne 0 ] ; then
            exit 1
        fi
        realpath=$(readlink -f "$file")
        TESTFILE_DIR=$(dirname "$realpath")
        ;;
      *)
        TESTFILE_DIR=$(dirname "$inputfile")
        ;;
    esac
    #--
    TESTFILE_NAME=${base}
    TESTFILE=${TESTFILE_DIR}/${file}
    CHKFILE=${TESTFILE_DIR}/${TESTFILE_NAME}.${CHKSUM_TYPE}
    DIRFILE=${TESTFILE_DIR}/${TESTFILE_NAME}.dirs
    if [ -f "$CHKFILE" ] ; then
        echo "** ${base} has already been processed. Delete generated files to process again"
        return
    fi
    #--
    rm -rf ${base}_test
    mkdir -p ${base}_test
    cd  ${base}_test
    #--
    case $CMD in
        *"%s"*) RCMD=$(printf "$CMD" "$TESTFILE") ;;
        *) RCMD=${CMD} ;;
    esac
    echo "# ${RCMD}"
    ${RCMD}
    create_chksum_file_from_dir . ${CHKFILE}
    create_dirlist_file_from_dir . ${DIRFILE}
    cd ..
}

#===================================================================

if [ -z "$1" ] ; then
    echo "Nothing to process..."
    exit 0
fi

rpath=$(readlink -f "$1") # readlink -f = realpath()

mkdir -p create_test_tmp
cd create_test_tmp


case "$rpath" in *create_test.list)
    echo "create_test.list: should contain files to process..."
    while read line
    do
        if [ -n "$line" ] ; then
            echo "Processing $line"
            process_file "$line"
        fi
    done < "$rpath"
    exit
    ;;
esac

if [ -f "$rpath" ] ; then
    process_file "$rpath"
else
    echo "$rpath doesn't exist"
    exit 1
fi
