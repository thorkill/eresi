#!/usr/local/bin/kernsh
# Insert ET_REL inside kernel memory

configure libkernsh.use_kernel 1

openmem

load ./lkm-sys_setdomainname.ko
mode dynamic
reladd 1 2

closemem
