##
## Makefile for elfsh
##
## Started on  Sun Mar 10 01:19:24 2002 jfv
##
##

include ../../../config.h

CC	= gcc
RM	= rm -f

HOSTSRC	= host.c
RELSRC	= rel.c
RELOBJ	= rel.o
RTSRC	= myputs.c
RTOBJ	= myputs.o
PIEBIN	= pie-test

LIB	= -L../../../libelfsh/ -lelfsh32 -L../../../libasm -lasm -L../../../elibc \
	-lcelfsh32 -L../../../liballocproxy -lallocproxy -L../../../libaspect/    \
	-laspect32 -lpthread
OPT	= -g3 -O2 -I../../../libelfsh/include/ -I../../../libmjollnir/include/ \
	-I../../../libaspect/include/ $(BITS)
CFLAGS	= $(OPT) 

all	: $(OBJ)
	$(CC) $(OPT) $(HOSTSRC)
	#@strip a.out
	$(CC) $(OPT) $(HOSTSRC) -pie -o $(PIEBIN)
	#@strip $(PIEBIN)
	$(CC) $(OPT) $(RTSRC) -o $(RTOBJ) -c
	@echo Now run etrelmem.esh inside e2dbg

clean	:
	$(RM) $(RTOBJ) $(OBJ) *~ \#* *\# $(RELOBJ)

fclean	: clean
	$(RM) $(NAM) a.out $(PIEBIN)
