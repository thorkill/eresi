##
## Makefile for elfsh
##
## Started on  Sun Mar 10 01:19:24 2002 mayhem
##
##

include ../../../config.h

CC	= gcc

HOSTSRC	= host.c
RELSRC	= rel.c
RELOBJ	= rel.o

RTSRC	= myputs.c
RTOBJ	= myputs.o

OPT	= -g3 -O2 -I../../../libelfsh/include/ $(BITS)
RM	= rm -f
CFLAGS	= $(OPT) -I../../../libhash/include/

all	: $(OBJ)

	$(CC) $(OPT) $(HOSTSRC) #-m64 -static
	#@strip a.out
#	$(CC) $(OPT) $(RELSRC) -o $(RELOBJ) -c #-m64
#	$(CC) $(OPT) $(RELSRC2) -o $(RELOBJ2) -c -fPIC #-m64

	make -C ../../../ e2dbg
	make -C ../../../

	/sbin/paxctl -m -s -p -r -x ../../../vm/elfsh
	/sbin/paxctl -v ../../../vm/elfsh

	cp ../../../e2dbg/e2dbg.so ibc.so.6

	$(CC) $(OPT) $(RTSRC) -o $(RTOBJ) -c

	@echo Now run ./etrelmem.esh

clean	:
	$(RM) $(RTOBJ) $(OBJ) *~ \#* *\# $(RELOBJ) inject_e2dbg.esh 


fclean	: clean
	$(RM) $(NAM) a.out a.out_e2dbg ibc.so.6 inject_e2dbg.esh inject_e2dbg.out 
