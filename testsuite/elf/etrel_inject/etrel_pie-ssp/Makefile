##
## Makefile for elfsh
##
## Started on  Sun Mar 10 01:19:24 2002 jfv
##
##

include ../../../../config.h

PAXCTL	= /sbin/paxctl
CC	= gcc
RELSRC	= simple.c
RELOBJ	= simple.o
PIESRC	= host.c
PIEOBJ	= host.o

RM	= rm -f
CFLAGS	= -fPIC

all	: $(PIEOBJ) $(RELOBJ)
	$(CC) -g3 $(PIEOBJ) -pie

clean	:
	$(RM) *~ \#* *\# $(RELOBJ) $(PIEOBJ)

fclean	: clean
	$(RM) a.out fake_aout

