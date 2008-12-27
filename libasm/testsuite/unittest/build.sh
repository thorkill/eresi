#!/bin/sh


cc $(eresi-config libasm --cflags) test_libasm_support.c -o test_libasm_support $(eresi-config libasm --libs) -ggdb
cc $(eresi-config libasm --cflags) test_op_mov_reg_iv.c -o test_op_mov_reg_iv $(eresi-config libasm --libs) -ggdb
cc $(eresi-config libasm --cflags) test_call16.c -o test_call16 $(eresi-config libasm --libs) -ggdb
cc $(eresi-config libasm --cflags) test_groups.c -o test_groups $(eresi-config libasm --libs) -ggdb
