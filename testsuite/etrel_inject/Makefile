##
## Makefile for elfsh
##
## Started on  Sun Mar 10 01:19:24 2002 mayhem
## Last update Thu Aug 21 02:24:39 2003 jv
##

SRC	= etrel_inject.c
OBJ	= $(SRC:.c=.o)
CC	= gcc
LIB	= -L../../libelfsh/ -lelfsh
NAM	= etrel_injecter

HOSTSRC	= host.c
RELSRC	= rel.c
RELOBJ	= rel.o

OPT	= -g3 -O2 -I../../libelfsh/include/
RM	= rm -f
CFLAGS	= $(OPT)

all	: $(OBJ)
	@$(CC) $(OPT) $(OBJ) $(LIB) -o $(NAM) -static
	@$(CC) $(OPT) $(HOSTSRC)
	strip a.out

	@$(CC) $(OPT) $(RELSRC) -o $(RELOBJ) -c

clean	:
	$(RM) $(OBJ) *~ \#* *\# $(RELOBJ)


fclean	: clean
	$(RM) $(NAM) a.out fake_aout
