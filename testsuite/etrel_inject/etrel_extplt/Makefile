##
## Makefile for elfsh
##
## Started on  Sun Mar 10 01:19:24 2002 mayhem
##
##

include ../../../config.h

SRC	= etrel_inject.c
OBJ	= $(SRC:.c=.o)
CC	= gcc
LIB	= -L../../../libelfsh/ -lelfsh -L../../../libasm -lasm -L../../../libc \
	-lcelfsh -L../../../libmalloc -lmalloc-e2dbg -L../../../libaspect/     \
	-laspect -lpthread
NAM	= etrel_injecter

HOSTSRC	= host.c
RELSRC	= rel.c
RELOBJ	= rel.o

OPT	= -g3 -O2 -I../../../libelfsh/include/ -I../../../libmjollnir/include/ \
	$(BITS)
RM	= rm -f
CFLAGS	= $(OPT)
PAXCTL	= /sbin/paxctl

all	: $(OBJ)
	$(CC) $(OPT) $(OBJ) $(LIB) -o $(NAM)
	$(CC) $(OPT) $(HOSTSRC) #-m64
	#@strip a.out
	$(CC) $(OPT) $(RELSRC) -o $(RELOBJ) -c #-m64

	@$(PAXCTL) -m -s -p -r -x ../../../vm/elfsh 2>/dev/null || \
	echo Test compiled succesfully. You can also install paxctl for pax tests.
	@$(PAXCTL) -v ../../../vm/elfsh 2>/dev/null || true

clean	:
	$(RM) $(OBJ) *~ \#* *\# $(RELOBJ)


fclean	: clean
	$(RM) $(NAM) a.out fake_aout
