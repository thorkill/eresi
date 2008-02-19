#!/bin/sh

nasm -f elf test-1.asm
ld -s -o test-1 test-1.o

