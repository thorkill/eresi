#!/bin/sh

# (C) 2006 Asgard Labs, thorolf a grid.einherjar.de
# BSD License
# $Id: mkDb.sh,v 1.1.1.1 2006-02-20 23:25:27 thor Exp $

for x in `ls -1 /usr/lib/*.a`
do
echo $x
 for f in `ar t $x`
  do
  echo "--->" $f
  ar x $x $f
  ./thor -i $f -O FreeBSD -A i386 -R 5.4-RELEASE-p7 -F $f -c >> tmp.db
  rm -f $f
  done
done
