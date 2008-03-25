#!/usr/bin/python
# $Id$
#
# This script compare mydisasm and objdump outputs to
# find bugs in libasm.
#

import os
import re
import sys
PATH_ELFSH32 = '../../../elfsh/elfsh32'
path_mydisasm = '../../mydisasm'
path_objdump = 'objdump'
dump_mydisasm = "dump.mydisasm"
dump_objdump = "dump.objdump"

line_mydisasm = ''
line_objdump = ''
nline = ''
nline = ''


def extract_textinfo(binary):
    fd = os.popen("%s -f %s -quiet -sym | egrep .text" % (PATH_ELFSH32, binary))
    line = fd.readline()
    #line = " [012] 0x08049AE0 SECTION  .text           sz:052692 foff:006880 scop:Local"
    regexp = re.compile(".*(?P<address>0x[0-9,A-F]{8}).*sz:(?P<size>[^ ]+)")
    m = regexp.match(line)
    if m != None:
        print m.groups()
        return m.groups()
    else:
        return None
        

def next_line_mydisasm(hdl_mydisasm):
    """Get next valid line from mydisasm output
    return an array
    
    @param hdl_mydisasm Handle to mydisasm output
    """
    rejected = 1
    while rejected:
        nline_mydisasm = hdl_mydisasm.next()
        if nline_mydisasm == '':
            return None
        
        nline_mydisasm = nline_mydisasm.rstrip() #([a-z,\%,\,]*)
        regexp = re.compile("0x(?P<address>[0-9,a-z]{8}):\s+(?P<instr>[a-z]*)\s+(?P<operands>\S+).*")
        m = regexp.match(nline_mydisasm)
        if m:
            #print 'mydisasm -> ' + str(m.groups())
            to_ret = m.groups()
            to_ret = {
                "address": m.group('address'),
                "instr": m.group('instr'),
                "operands": m.group('operands')
                }
            return to_ret
        else:
            pass
            #print "mydisasm '%s' rejected"%(nline_mydisasm)
            


def next_line_objdump(hdl_objdump):
    """Get next valid line from objdump output

    @param hdl_objdump file handle to objdump output.
    """
    rejected = 1
    while rejected:
        nline_objdump = hdl_objdump.next()
        if nline_objdump == '':
            return None
        nline_objdump = nline_objdump.rstrip()
        regexp = re.compile(".*(?P<address>[0-9,a-f]{7}):\s+(?P<opcodes>[a-f,0-9, ]*)\s+" +
                            "(?P<instr>[a-z]+)\s+(?P<operands>.*)")
        m = regexp.match(nline_objdump)
        if m:
            to_ret = m.groups()
            #print 'objdump -> ' + str(to_ret)
            if len(to_ret) < 2:
                print 'fuck this : ' + nline_objdump
            to_ret = {
                "address": m.group('address'),
                "instr": m.group("instr"),
                "operands": m.group('operands'),
                "opcodes": m.group('opcodes')
                }
            return to_ret
        else:
            #print "objdump '%s' rejected"%(nline_objdump)
            pass


def compare_instruction(mydisasm, objdump):
    """Compare two instruction
    @param mydisasm
    @param objdump
    @return True if both instruction are the sames.
    """
    vaddr = int(objdump['address'], 16)
    imydisasm = mydisasm['instr']
    omydisasm = mydisasm['operands']
    iobjdump = objdump['instr']
    oobjdump = objdump['operands']
    if ((imydisasm[0:len(iobjdump)] == iobjdump) and
        (omydisasm and oobjdump)):
        return True
    else:
        return False

def check_outputs(hdl_mydisasm, hdl_objdump):
    nline_mydisasm = next_line_mydisasm(hdl_mydisasm)
    nline_objdump = next_line_objdump(hdl_objdump)
    skip_mydisasm = 0
    skip_objdump = 0
    line_match = 0
    line_unmatch = 0
    alined = 1
    try:
        while 1:
            #
            # We have finished of readling lines from a file.
            #
            if (nline_mydisasm == None or nline_mydisasm == None):
                print "End"
                break
            vaddr_myd = int(nline_mydisasm['address'],16)
            vaddr_obj = int(nline_objdump['address'],16)
            # mydisasm address is below objdump one
            # Previous instruction was shorter than in objdump
            # Raise alert
            if vaddr_myd < vaddr_obj:
                alined = 0
                print 'skipping mydisasm ' + str(nline_mydisasm)
                skip_mydisasm += 1
                nline_mydisasm = next_line_mydisasm(hdl_mydisasm)
                continue
            #
            #
            #
            if vaddr_myd > vaddr_obj:
                alined = 0
                print 'skipping objdump ' + str(nline_objdump)
                nline_objdump = next_line_objdump(hdl_objdump)
                skip_objdump += 1
                continue
            vaddr = nline_mydisasm['address']
            if alined == 0:
                print "Disassembly realined."
                alined = 1
        
            if (nline_mydisasm == None or nline_mydisasm == None):
                print "End"
                break
            if compare_instruction(nline_mydisasm, nline_objdump) == False:
                try:
                    print ("%s objdump('%s' '%s') "%(vaddr,
                                                     nline_objdump['instr'],
                                                     nline_objdump['operands']) +
                           "!= mydisasm('%s' '%s') %s"%(nline_mydisasm['instr'],
                                                        nline_mydisasm['operands'],
                                                        nline_objdump['opcodes']))
                except TypeError:
                    print "Objdump parsed line"
                    print nline_objdump
                    print "mydisasm parsed line"
                    print nline_mydisasm
                    return
                line_unmatch += 1
            else:
                line_match += 1
            nline_mydisasm = next_line_mydisasm(hdl_mydisasm)
            nline_objdump = next_line_objdump(hdl_objdump)
    except:
        print "Matching   lines: %d"%(line_match)
        print "Unmatching lines: %d"%(line_unmatch)
        print "mydisasm skipped lines: %d"%(skip_mydisasm)
        print "objdump  skipped lines: %d"%(skip_objdump)


def main():
    i = 1
    if len(sys.argv) < 2:
        print "Usage: %s <binary>"%(sys.argv[0])
        return -1
    else:
            binary = sys.argv[1]            

    #
    # Extract binary information with elfsh
    #
    #
    infos = extract_textinfo(binary)
    if infos == None:
        print "Error while parsing elfsh32 output"
    #    cmd_elfsh = ("../../elfsh/elfsh32 -f %s "%(binary) +
    #                "-pht | egrep 'r-x.*Loadable' | cut -d\" \" -f3,5")
    #print cmd_elfsh
    #elfsh = os.popen(cmd_elfsh)
    #output = elfsh.readline().strip()


    #arr = output.split(" ")
    exe_start = int(infos[0], 16)
    exe_size = int(infos[1])
    exe_end = exe_start + exe_size

    print "Size of .text = %i" % (exe_size)
    #
    # Intialization.
    #
    cmd_my = ("%s "%(path_mydisasm) +
              " %s "%(binary) +
              "%s %i > %s "%(hex(exe_start), exe_size, dump_mydisasm))
    cmd_obj = ("%s -d "%(path_objdump) +
               " -z --start-address=%s --stop-address=%s"%(hex(exe_start), hex(exe_end)) +
               " %s > %s "%(binary, dump_objdump))
    print cmd_my
    print cmd_obj
    import time
    time.sleep(3)
    os.system(cmd_my)
    os.system(cmd_obj)
    hdl_mydisasm = open(dump_mydisasm)
    hdl_objdump = open(dump_objdump)
    check_outputs(hdl_mydisasm, hdl_objdump)

#####################################

import StringIO

def test_objdump():
    print "### Running testsuite for objdump parser"
    testbuf = StringIO.StringIO("""
/bin/ls:     file format elf32-i386

Disassembly of section .init:

080494b4 <_init>:
 80494b4:       55                      push   %ebp
 80494b5:       89 e5                   mov    %esp,%ebp
 80494b7:       83 ec 08                sub    $0x8,%esp
 80494ba:       e8 45 06 00 00          call   8049b04 <strcpy@plt+0x38>
""")
    try:
        while True:
            objdump = next_line_objdump(testbuf)
            print objdump
    except:
        pass
            

def test_mydisasm():
    print "### Running testsuite for mydisasm parser"
    testbuf = StringIO.StringIO("""
0x08049b8f:                                nop  90
0x08049b90:                 push          %ebp  55
0x08049b91:            mov           %esp,%ebp  89 e5
0x08049b93:           sub           $0x18,%esp  83 ec 18
0x08049b96:       mov           0x8(%ebp),%eax  8b 45 08
0x08049b99:       mov           0xc(%ebp),%ecx  8b 4d 0c
0x08049b9c:       mov           0x4(%eax),%edx  8b 50 04
0x08049b9f:          mov           (%eax),%eax  8b 00
0x08049ba1:     mov           %ecx,0x8(%esp,1)  89 4c 24 08
    """)
    try:
        while True:
            mydisasm = next_line_mydisasm(testbuf)
            print mydisasm
    except:
        pass

if  (len(sys.argv) > 1) and (sys.argv[1] == "--test"):
    test_objdump()
    test_mydisasm()
    print extract_textinfo("/bin/ls")
else:
    main()
