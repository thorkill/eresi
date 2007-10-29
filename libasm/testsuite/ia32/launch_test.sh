#!/usr/bin/env bash
#
# $Id: launch_test.sh,v 1.6 2007-10-29 11:26:44 heroine Exp $
# 
# This script launch lbmjollnir script to make diffs
# between an objdump dump and a mydisasm dump of each
# .asm file in the current directory.
#
MYDISASM=./mydisasm

function init_test
{

    for i in *.asm; do 
	as $i -o ${i/asm/o} 2> /dev/null 
	if ! ld ${i/asm/o} -o ${i/.asm/} 2> /dev/null ; then 
	    echo "[ER] Failed compiling $i"
	fi
    done
    ln -sf ../../mydisasm mydisasm
}

function diff_test
{
    for i in *.asm ; do
	echo -n "[Testing] $i:"
	if perl ../../../libmjollnir/tools/desDiff.pl ${i/.asm/} 2> /dev/null
	    then
	    echo "[OK] Test ok"
	else
	    echo Error detected. Check output bellow and press enter when you are ready to launch gdb on mydisasm
	    echo While in gdb, type run_it to launch gdb tracing until asm_read_instr
	    read
	    cat >> tmp.gdb <<EOF
define run_it
echo [01 loading]\n
file ../../mydisasm
b main
run ${i/.asm/} .text
echo [02 putting breakpoint]\n
b asm_read_instr
echo >>> You can now type continue to reach next asm_read_instr\n
end
EOF
	    gdb -x tmp.gdb mydisasm
	    rm tmp.gdb
	fi
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
