#!/bin/sh
#
# Environment script for using ERESI without install
#

export LD_LIBRARY_PATH=`pwd`/libelfsh
export LD_LIBRARY_PATH=`pwd`/libui/:$LD_LIBRARY_PATH
export LD_LIBRARY_PATH=`pwd`/librevm/:$LD_LIBRARY_PATH
export LD_LIBRARY_PATH=`pwd`/e2dbg:$LD_LIBRARY_PATH
export LD_LIBRARY_PATH=`pwd`/libmjollnir:$LD_LIBRARY_PATH
export LD_LIBRARY_PATH=`pwd`/testsuite/e2dbg-multithread:$LD_LIBRARY_PATH
export LD_LIBRARY_PATH=`pwd`/libaspect:$LD_LIBRARY_PATH
export LD_LIBRARY_PATH=`pwd`/libedfmt/:$LD_LIBRARY_PATH
export LD_LIBRARY_PATH=`pwd`/testsuite/e2dbg_multithreads/:$LD_LIBRARY_PATH

echo Environment now ready for testing ERESI without install
