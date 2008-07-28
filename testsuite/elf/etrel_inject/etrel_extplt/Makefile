##
## Makefile for elfsh
##
## Started on  Sun Mar 10 01:19:24 2002 jfv
##
##

include ../../../../config.h

SRC	= etrel_inject.c
OBJ	= $(SRC:.c=.o)
CC	= gcc

LIB	= -L../../../../libelfsh/ -lelfsh32 -L../../../../libasm -lasm32    \
	-L../../../../libaspect/ -laspect32 -lpthread
OPT	= -O2 -I../../../../libelfsh/include/ -I../../../../libmjollnir/include/ \
	-I../../../../libaspect/include/ $(BITS) -DERESI32 -g3

NAM	= etrel_injecter
HOSTSRC	= host.c
RELSRC	= rel.c
RELOBJ	= rel.o

RM	= rm -f
CFLAGS	= $(OPT)
PAXCTL	= /sbin/paxctl

all32: all
all	: $(OBJ)
	$(CC) $(OPT) $(OBJ) $(LIB) -o $(NAM)
	$(CC) $(OPT) $(HOSTSRC)
	$(CC) $(OPT) $(RELSRC) -o $(RELOBJ) -c

	@$(PAXCTL) -m -s -p -r -x ../../../../vm/elfsh 2>/dev/null || \
	echo Test compiled succesfully. You can also install paxctl for pax tests.
	@$(PAXCTL) -v ../../../../vm/elfsh 2>/dev/null || true

clean	:
	$(RM) $(OBJ) *~ \#* *\# $(RELOBJ)


fclean	: clean
	$(RM) $(NAM) a.out fake_aout
