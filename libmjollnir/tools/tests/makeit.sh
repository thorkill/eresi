#!/bin/sh

# ERESI (c) 2008

for i in *.asm; do
 ofile=`echo $i | sed 's/.asm//g'`
 nasm -f elf $i
 ld -s -o $ofile $ofile.o
done
