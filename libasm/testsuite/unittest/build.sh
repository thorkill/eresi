cc -o test_libasm_support test_libasm_support.c -I../../include -I ../../../libaspect/include/ -L../../ -lasm -L../../../libaspect -laspect32
export LD_LIBRARY_PATH=../..:../../../libaspect
./test_libasm_support