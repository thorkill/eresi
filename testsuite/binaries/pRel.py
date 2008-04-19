#!/usr/bin/python

import re
from StringIO import StringIO

logElfsh = open("tRel.elfsh.log")
logObjdump = open("tRel.objdump.log")

reElfsh = re.compile("[^\]]+ ([^ ]+)")
reObjdump = re.compile('(?P<address>[^ ]+) (?P<type>R_[^ ]+)[ ]+(?P<symbol>[^ ]+)')

ioElfsh = StringIO(
"""
 [*] Object bin has been loaded (O_RDONLY) 
 [RELOCATION TABLES]
 [000] R_386_GLOB_DAT  0x0804969C sym[002] : __gmon_start__                  => Create GOT entry
 [000] R_386_JMP_SLOT  0x080496AC sym[001] : myfunc2                         => Create PLT entry
 [001] R_386_JMP_SLOT  0x080496B0 sym[002] : __gmon_start__                  => Create PLT entry
 [002] R_386_JMP_SLOT  0x080496B4 sym[004] : __libc_start_main               => Create PLT entry
 [003] R_386_JMP_SLOT  0x080496B8 sym[005] : myfunc3                         => Create PLT entry
 [004] R_386_JMP_SLOT  0x080496BC sym[006] : myfunc1                         => Create PLT entry
""")
ioObjdump = StringIO("""
DYNAMIC RELOCATION RECORDS
0804969c R_386_GLOB_DAT    __gmon_start__
080496ac R_386_JUMP_SLOT   myfunc2
080496b0 R_386_JUMP_SLOT   __gmon_start__
080496b4 R_386_JUMP_SLOT   __libc_start_main
080496b8 R_386_JUMP_SLOT   myfunc3
080496bc R_386_JUMP_SLOT   myfunc1
""")

def parseRelObjdump(lines):
    to_ret = []
    for line in lines:
        #print line.strip()
        m = reObjdump.match(line.strip())
        if m != None:
            dir(m)
            to_ret.append(m.groups())
    return to_ret

def parseRelElfsh(lines):
    to_ret = []
        
    return to_ret


lines  = ioObjdump.readlines()
print parseRelObjdump(lines)
