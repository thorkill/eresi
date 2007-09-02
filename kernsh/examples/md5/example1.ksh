#!/usr/local/bin/kernsh
# Hijack a syscall and check it

type example1 = sys:int[320]
print "Sys_call_table " $sct
inform example1 toto $sct

mode dynamic

set $save_addr 1.example1[toto].sys[17]
print $save_addr

kmd5 $sct%1280
set $md5save $$_

print "Hijack syscall 17"
set 1.example1[toto].sys[17] 0x44444444
kcmd5 $md5save

print "CMP MD5"
cmp $_ 0
je good
print "MD5 MISMATCH !!!"
jmp end

good:
print "MD5 MATCH!!!"
end:

#RESTORE
set 1.example1[toto].sys[17] $save_addr
print 1.example1[toto].sys[17]
