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
PIESRC	= host.c
PIEOBJ	= host.o

RM	= rm -f
CFLAGS	= $(OPT) -I../../libhash/include/

all	: 
	$(CC) $(PIESRC) -pie #-ssp
	$(CC) $(RELSRC) -o $(RELOBJ) -c -fPIC

	$(PAXCTL) -permxs ../../../vm/elfsh
	$(PAXCTL) -v ../../../vm/elfsh

clean	:
	$(RM) *~ \#* *\# $(RELOBJ) $(PIEOBJ)

fclean	: clean
	$(RM) a.out fake_aout

