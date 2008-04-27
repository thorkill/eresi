#!/bin/sh
rm test_libasm_support
#cc -DERESI32 -ggdb -o test_libasm_support test_libasm_support.c -I../../include -I ../../../libaspect/include/ -L../../ -L../../../libaspect -lasm -laspect32 -static

cc -g3 -I../../../libelfsh/include/ -I../../../libaspect/include/ -I../../include/ \
        test_libasm_support.c -DERESI32 -o test_libasm_support -L ../../../libelfsh/ -lelfsh32 \
        -L../../../libaspect/ -L../../ -L../../ -laspect32 -lasm -L../../../libetrace/ -letrace32 #-static


export LD_LIBRARY_PATH=../..:../../../libaspect:../../../libelfsh:../../../libetrace
./test_libasm_support
