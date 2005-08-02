#!/bin/sh

test $# -ne 1 &&  echo "usage : "$0" file" && exit 0

DT=`../../../vm/elfsh -f $1 -d | grep DT_DEBUG | awk -F"[][]" '{ print $2;}'`

PRG=$1

echo "#!../../../vm/elfsh"				 > inject_e2dbg.esh
echo "load "$1					>> inject_e2dbg.esh
echo "set 1.dynamic["$DT"].val 0x2"	        >> inject_e2dbg.esh
echo "set 1.dynamic["$DT"].tag DT_NEEDED"	>> inject_e2dbg.esh
echo "redir main e2dbg_run"			>> inject_e2dbg.esh
echo "save "$1"_e2dbg"				>> inject_e2dbg.esh
echo ""						>> inject_e2dbg.esh

echo  "#!../../../vm/elfsh"			> etrelmem.esh
echo "load myputs.o"				>> etrelmem.esh
echo "switch 1"					>> etrelmem.esh
echo "mode dynamic"				>> etrelmem.esh
echo "reladd 1 10"				>> etrelmem.esh
#echo "quiet s"					>> etrelmem.esh
echo "profile"					>> etrelmem.esh
echo "redir puts myputs"			>> etrelmem.esh
#echo "redir printf e2dbg_sigint_handler"	>> etrelmem.esh
echo "profile"					>> etrelmem.esh
echo ""						>> etrelmem.esh

chmod ugo+x inject_e2dbg.esh

./inject_e2dbg.esh > inject_e2dbg.out

/sbin/paxctl -p -m -s -r -x $PRG"_e2dbg"

./$PRG"_e2dbg"



