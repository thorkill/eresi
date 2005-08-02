##
## Makefile for elfsh
##
## Started on  Sun Mar 10 01:19:24 2002 mayhem
##
##

include ../../../config.h

PAXCTL	= /sbin/paxctl
CC	= gcc
RELSRC	= simple.c
RELOBJ	= simple.o

RM	= rm -f
CFLAGS	= $(OPT) -I../../libhash/include/

all	: 
	$(CC) $(RELSRC) -o $(RELOBJ) -c -fPIC

	$(PAXCTL) -s -p -m -r -x ../../../vm/elfsh
	$(PAXCTL) -v ../../../vm/elfsh

clean	:
	$(RM) *~ \#* *\# $(RELOBJ)


fclean	: clean
	$(RM) $(RELOBJ) fake_main
