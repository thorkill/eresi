#!/bin/sh
cat ./cpuinsns.txt | xargs -t --replace=REEMP awk '{sub(/SUBSTITUTE/,"REEMP");print $0 > "asm_mips_REEMP.c"}' ./template.txt

