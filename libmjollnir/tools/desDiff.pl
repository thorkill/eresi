#!/usr/bin/perl

# (C) 2006-2007 Asgard Labs, thorolf
# BSD License
# $Id: desDiff.pl,v 1.6 2007-05-29 08:57:42 thor Exp $

# the objects should be striped libraries
# this script was 'designed' to search for differences in
# disassembly between objdump and mydisasm/libasm


if (!$ARGV[0]) {print "usage: ./desDiff.pl test.so\n";exit();}

$b = $ARGV[0];

$tmpFile = $b;
$tmpFile =~ s/\//\###/g;
$tmpFile = $$.".$tmpFile";

print STDERR "[i] Checking: $b - ... ";
print STDERR "Objdump, ";
system("objdump -d -j .text $b > $tmpFile.objdump");
print STDERR "Mydisasm, ";
system("./mydisasm $b .text > $tmpFile.mydisasm");

open(X1, "$tmpFile.mydisasm");
open(X2, "$tmpFile.objdump");

print STDERR "Analysis started...\n";
while($l1 = <X1>) {
 chomp($l1);
 @d1 = split("\t",$l1);
 
 $a1 = $d1[2];
 $a1 =~ s/ //g;
 x2();
 
 if ($a1 ne $a2) { 
  $tmp = "[!] opcodes: ".  $a1 . " :: " . $a2 ." in $b\n";
  $tmp .= "[X] LIBASM: ". $l1 . "\n";
  $tmp .= "[X] OBJDUMP: ". $l2 . "\n[o] ======\n";
  $x = $a1;
  $x =~ s/$a2//g;
  x2();
  if ($x ne $a2) {
    print $tmp;
    print STDERR "[o] diffs found\n";
    exit(1);
  }
 }

}

close(X1);
close(X2);

unlink("$tmpFile.objdump");
unlink("$tmpFile.mydisasm");

sub x2 {

 $l2 = <X2>;
 if (!$l2) {exit();}
 chomp($l2);
 if ($l2 !~ /^\s+/) {x2();}
 @d2 = split("\t",$l2);
 $a2 = $d2[1];
 $a2 =~ s/ //g;

}

