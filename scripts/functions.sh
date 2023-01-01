#!/bin/sh
# Public domain
#
# misc functions to be used by misc scripts...
#

set_checksum_app() # $1:[$CHKSUM_TYPE]
{
    # set checksum app to be used later
    if [ -n "$1" ] ; then
        CHKSUM_TYPE="$1"
    fi
    case ${CHKSUM_TYPE} in
        md5)    CHKSUM_GNU='md5sum'    ; CHKSUM_BSD='md5'    ;;
        sha1)   CHKSUM_GNU='sha1sum'   ; CHKSUM_BSD='sha1'   ;;
        sha256) CHKSUM_GNU='sha256sum' ; CHKSUM_BSD='sha256' ;;
        sha512) CHKSUM_GNU='sha512sum' ; CHKSUM_BSD='sha512' ;;
        *)
            echo "*** ${CHKSUM_TYPE}: unknown checksum type"
            echo "*** Valid types: md5, sha1, sha256, sha512"
            exit 1 ;;
    esac

    if test -z "$CHKSUM_APP" ; then
        if command -v ${CHKSUM_GNU} ; then
            export CHKSUM_APP=${CHKSUM_GNU}
        elif command -v ${CHKSUM_BSD} ; then
            export CHKSUM_APP=${CHKSUM_BSD}
        else
            echo "${CHKSUM_GNU}/${CHKSUM_BSD} utility is missing, please install it"
            echo "aborting tests"
            exit 1
        fi
    fi
}


set_wget()
{
    if test -n "$XWGET" ; then
        return # already set
    fi
    if command -v curl ; then
        export XWGET='curl -sSL -o'
    elif command -v wget ; then
        export XWGET='wget -q --no-check-certificate -O'
    elif command -v fetch ; then # FreeBSD
        export XWGET='fetch -q --no-verify-peer -o'
    else
        echo "Cannot find curl/wget/fetch.."
        if [ -n "$WGET_IS_REQUIRED" = "yes" ] ; then
            exit 1
        fi
    fi
}


download_file() # $1:<url> [outfile]
{
    if test -z "$XWGET" ; then
        return
    fi
    dlurl="$1"
    dlfile="$2"
    if [ -z "$dlfile" ] ; then
        dlfile=$(basename "$dlurl")
    fi
    if ! [ -f "${dlfile}" ] ; then
        echo "# ${XWGET} \"${dlfile}\" \"${dlurl}\""
        ${XWGET} "${dlfile}" "${dlurl}"
        if [ $? -ne 0 ] ; then
            echo "Could not download file" >&2
            rm -f "${dlfile}"
            return 1
        fi
    fi
    return 0
}


check_app_help() # $1<ret_code> $2:<app_help_cmd>
{
    expected_ret="$1"
    app_help="$2"
    #echo "# $app_help"
    ${app_help} >/dev/null 2>&1
    appret=$?
    if [ ${appret} -ne ${expected_ret} ] ; then
        echo "Something is wrong with ${app}"
        echo "Got code $appret (expected ${expected_ret})"
        echo "Aborted"
        exit 1
    fi
}