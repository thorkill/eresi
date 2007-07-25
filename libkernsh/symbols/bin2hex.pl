#!/usr/bin/perl


use strict;



my $code = "";
my $hex = "";
my $i = 0;
my $op = "";
my $function_name = "";



if ($#ARGV != 0) {
	die("usage : ./bin2hex.pl <file.d>\n");
}



if (! -e $ARGV[0]) { 
  die("[-] $!.");
}
open(FP , $ARGV[0]);
# skip the 6 lines of information and get the function name
while (<FP>) {
  $i++;
  if ($i >= 6) {
    ($function_name) = ($_ =~ /<([a-zA-Z0-9_]+)>/);
    last;
  }
}

# get the opcodes
while (<FP>) {
  ($op) = ($_ =~ /.*:\t([^\t]+)/);
  $op =~ s/[^a-z0-9]+$//g;
  $op =~ s/ /\\x/g;
  $op = "\\x".$op;
  $hex .= $op;
}

# build the string
$i = 0;
$code = "#define ".uc($function_name)." \\\n";
$code .= "  \"";
while ($hex) {
  ($op) = ($hex =~ /^(\\x[a-z0-9]+)/);
  $code .= $op;
  $hex =~ s/^(\\x[a-z0-9]+)//g;

  $i++;
  if ($i == 16) {
    $i = 0;
    $code .= "\" \\\n  \"";
  }
}
$code .= "\"\n\n";
print($code);



