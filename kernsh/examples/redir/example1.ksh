#!/usr/local/bin/kernsh

openmem

load lkm-sys_setdomainname.ko

D sys_setdomainname

mode dynamic 

reladd 1 2

redir sys_setdomainname new_sys_setdomainname

D sys_setdomainname
D old_sys_setdomainname

redir

closemem
quit

