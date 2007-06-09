#!/usr/bin/env bash
#
# $Id: launch_test.sh,v 1.2 2007-06-09 22:35:16 thor Exp $
# 
# This script launch lbmjollnir script to make diffs
# between an objdump dump and a mydisasm dump of each
# .asm file in the current directory.
#
MYDISASM=./mydisasm

function init_test
{

    for i in *.asm; do 
      as $i -o ${i/asm/o}
      ld ${i/asm/o} -o ${i/.asm/}
    done
    ln -sf ../../mydisasm mydisasm
}

function diff_test
{
    for i in *.asm ; do
	perl ../../../libmjollnir/tools/desDiff.pl ${i/.asm/}
    done
}


function purge_test
{
    for i in *.asm; do
	rm ${i/asm/o} ${i/.asm/}
    done
    rm -f *.mydisasm
    rm -f *.objdump
    rm -f ./mydisasm 
}

if [ ! -z "$1" ] ; then
    ARG=$1
else
    ARG="*.asm"
fi

init_test $ARG
diff_test $ARG
purge_test $ARG