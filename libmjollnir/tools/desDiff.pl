#!/usr/bin/perl

# (C) 2006-2007 Asgard Labs, thorolf
# BSD License
# $Id: desDiff.pl,v 1.7 2007-05-29 12:01:56 thor Exp $

# the objects should be striped libraries
# this script was 'designed' to search for differences in
# disassembly between objdump and mydisasm/libasm

$sparcObjDump = "sparc-rtems-objdump";
$intelObjDump = "objdump";

# overwrite default objdump
if ($ENV{'ERESI_OBJDUMP'}) {
    $sparcObjDump = $ENV{'ERESI_OBJDUMP'};
    $intelObjDump = $ENV{'ERESI_OBJDUMP'};
}

if (!$ARGV[0]) {print "usage: ./desDiff.pl test.so\n";exit();}

$b = $ARGV[0];

open(SI,"file $b |");
$tmp = <SI>;
@fileInfo = split(',',$tmp);
close(SI);

$arch = $fileInfo[1];
$arch =~ s/^ //;
$arch =~ s/,//;

if ($arch =~ /^Intel/) {
 $objDump = $intelObjDump;
} elsif ($arch =~ /SPARC$/) {
 $objDump = $sparcObjDump;
} else {
 print "[E] Architecture [$arch] is not supported\n";
 exit(1);
}

$tmpFile = $b;
$tmpFile =~ s/\//\###/g;
$tmpFile = $$.".$tmpFile";

print STDERR "[i] Checking: $b [$arch] - ... ";
print STDERR "Objdump, ";
system("$objDump -d -j .text $b > $tmpFile.objdump");
print STDERR "Mydisasm, ";
system("./mydisasm $b .text > $tmpFile.mydisasm");

open(X1, "$tmpFile.mydisasm");
open(X2, "$tmpFile.objdump");

print STDERR "Analysis started...\n";

$waitFlag = 0;

while($myDisasmInput = <X1>) {
 chomp($myDisasmInput);
 @myDisasmLine = split("\t",$myDisasmInput);
 $mdAddr = $myDisasmLine[0];
 $mdAddr =~ s/ //g;
 $mdAddr =~ s/://;
 $mdInstr = $myDisasmLine[1];
 $mdInstr =~ s/ //g;
 $mdOpcode = $myDisasmLine[2];
 $mdOpcode =~ s/ //g;

 if (!$waitFlag) {
  # objdump opcodes
  @objDumpLine = x2();
 }

 $obAddr   = $objDumpLine[0];
 $obAddr   =~ s/ //g;
 $obAddr   =~ s/://g;
 $obAddr   = "0x".$obAddr;
 $obOpcode = $objDumpLine[1];
 $obOpcode =~ s/ //g;
 $obInstr  = $objDumpLine[2];

## debug 
# print "MD: ". $mdAddr . " " .$mdInstr . " " . $mdOpcode . "\n";
# print "OD: ". $obAddr . " " .$obInstr . " " . $obOpcode . "\n";

 if (oct($mdAddr) < oct($obAddr))
   {
    $waitFlag = 1;
    next;
   }

 if ($mdOpcode ne $obOpcode) {
 
 if (($obOpcode ne "...") && ($mdInstr ne "illtrap"))
  {
  $tmp = "[!] opcodes: ".  $mdOpcode . " :: " . $obOpcode ." in $b\n";
  $tmp .= "[X] LIBASM: ". $myDisasmInput . "\n";
  $tmp .= "[X] OBJDUMP: ". $objDumpInput . "\n[o] ======\n";
  $x = $mdOpcode;
  $x =~ s/$obOpcode//g;
  x2();
  if ($x ne $obOpcode) {
    print $tmp;
    print STDERR "[o] diffs found\n";
    exit(1);
  }
 }
 }

 $waitFlag = 0;

}

close(X1);
close(X2);

unlink("$tmpFile.objdump");
unlink("$tmpFile.mydisasm");


### Read objdump output
sub x2 {

 $objDumpInput = <X2>;
 if (!$objDumpInput) {exit();}
 chomp($objDumpInput);
 if ($objDumpInput !~ /^\s+/) {x2();}
 @objDumpLine = split("\t",$objDumpInput);

 return (@objDumpLine);
}
