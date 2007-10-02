#!/bin/sh
#
# $Id: get_assembly.sh,v 1.1 2007-10-02 18:11:20 heroine Exp $
#
if [ ! -z "$1" ]; then
objdump -d $1 -w  | grep -v Disassembly | grep -v '>:' | grep -v elf32 | cut -f3 | cut -f1 -d'<' | egrep -v '^call' | egrep -v '^j' | grep -v 'repz ret' |sort -u 
else
cat <<EOF
  
  This script extract assembly from objdump output and print it to stdout.
  
  Usage: $0 binary > binary.asm

EOF

