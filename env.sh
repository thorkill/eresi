#!/bin/sh
#
# Environment script for using ERESI without install
#

if [ ! "_$ERESI_ENV" == "_" ] ; 
then
echo Eresi env already set to $ERESI_ENV
else

export ERESI_ENV=`pwd`

export LD_LIBRARY_PATH=`pwd`/libelfsh
export LD_LIBRARY_PATH=`pwd`/libetrace:$LD_LIBRARY_PATH
export LD_LIBRARY_PATH=`pwd`/libstderesi:$LD_LIBRARY_PATH
export LD_LIBRARY_PATH=`pwd`/libasm/:$LD_LIBRARY_PATH
export LD_LIBRARY_PATH=`pwd`/libui/:$LD_LIBRARY_PATH
export LD_LIBRARY_PATH=`pwd`/librevm/:$LD_LIBRARY_PATH
export LD_LIBRARY_PATH=`pwd`/e2dbg:$LD_LIBRARY_PATH
export LD_LIBRARY_PATH=`pwd`/libmjollnir:$LD_LIBRARY_PATH
export LD_LIBRARY_PATH=`pwd`/testsuite/e2dbg-multithread:$LD_LIBRARY_PATH
export LD_LIBRARY_PATH=`pwd`/libaspect:$LD_LIBRARY_PATH
export LD_LIBRARY_PATH=`pwd`/libedfmt/:$LD_LIBRARY_PATH
export LD_LIBRARY_PATH=`pwd`/testsuite/e2dbg_multithreads/:$LD_LIBRARY_PATH
export LD_LIBRARY_PATH=/usr/lib/debug/:$LD_LIBRARY_PATH
export LD_LIBRARY_PATH=`pwd`/libkernsh/:$LD_LIBRARY_PATH

export PATH=`pwd`/elfsh:$PATH
export PATH=`pwd`/etrace:$PATH
export PATH=`pwd`/e2dbg:$PATH
export PATH=`pwd`/kernsh:$PATH
export PATH=`pwd`:$PATH
echo Environment now ready for testing ERESI without install
fi
