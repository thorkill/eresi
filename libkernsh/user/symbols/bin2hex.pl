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

  #print "BEG Zero loop \n";
    if ($_ =~ /<([a-zA-Z0-9_]+)>/)
    {
	($function_name) = $1;
	last;
    }

  #print "Zero loop \n";

}

# get the opcodes
ITER: while (<FP>) {

  #print "BEG First loop \n";

  ($op) = ($_ =~ /.*:\t([^\t]+)/);
  $op =~ s/[^a-z0-9]+$//g;

  #print "OP before: $op \n";

  if ($op eq "") { next ITER; }

  $op =~ s/ /\\x/g;

  #print "OP middle: $op \n";

  $op = "\\x".$op;

  #print "OP after: $op \n";

  $hex .= $op;

  #print "First loop \n";
}
 
# build the string
$i = 0;
$code = "#define ".uc($function_name)." \\\n";
$code .= "  \"";

while ($hex) {

#  print "BEG Second loop with hex = $hex\n";

  ($op) = ($hex =~ /^(\\x[a-z0-9]+)/);
  $code .= $op;
  $hex =~ s/^(\\x[a-z0-9]+)//g;

  $i++;
  if ($i == 16) {
    $i = 0;
    $code .= "\" \\\n  \"";
  }

 # print "Second loop \n";

}
$code .= "\"\n\n";
print($code);



