#!/bin/sh

DIR=`dirname $0`
OPTS="$@"
. $DIR/process_opts.sh

$DIR/configure $EXTRA_FLAGS -embedded -reduce-exports -no-nis -no-3dnow -webkit -openssl-linked -DNFLX_NO_CLOEXEC -R "/usr/local/i686-netflix-linux-gnu-3.2/netflix/lib" -platform linux-g++ -xplatform qws/linux-partner-x86-g++ -DNETFLIX_ALTGR_SUPPORT_HACK $OPTS
