
## Makefile for elfsh
##
## Started on  Sun Mar 10 01:19:24 2002 mayhem
##
##

include ../../../config.h

CC	= gcc
LIB	= -L../../../libelfsh/ -lelfsh -L../../../libhash/ -lhashelfsh -L../../../libasm -lasm

HOSTSRC	= host.c
RELSRC	= rel.c
RELOBJ	= rel.o

OPT	= -g3 -O2 -I../../../libelfsh/include/ $(BITS)
RM	= rm -f
CFLAGS	= $(OPT) -I../../../libhash/include/

all	: 
	#@~/paxctl-0.3/paxctl -c etrel_injecter 
#	@/sbin/paxctl -c ../../../vm/elfsh
	#@~/paxctl-0.3/paxctl -m etrel_injecter 
	@/sbin/paxctl -m ../../../vm/elfsh
	$(CC) $(OPT) $(HOSTSRC) -static
	#@strip a.out

	$(CC) $(OPT) $(RELSRC) -o $(RELOBJ) -c  #-m64

clean	:
	$(RM) $(OBJ) *~ \#* *\# $(RELOBJ)


fclean	: clean
	$(RM) $(NAM) a.out fake_aout core
