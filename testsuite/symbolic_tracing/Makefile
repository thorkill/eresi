##
## Makefile for elfsh
## 
## Started on  Sun Mar 10 01:19:24 2002 mayhem
## Last update Sun March 21 23:15:15 2007 mxatone
##
include ../../config.h

SRC 	= tracer.c
OBJ	= $(SRC:.c=.o)
CC	= gcc
LIB	= -L../../librevm/ -lrevm32 -L../../libelfsh/ -lelfsh32
NAM	= tracer
RM	= rm -f 
OPT	= -gdwarf-2 -g3 -O2 -I../../librevm/include/ -I../../libelfsh/include/ \
	-I../../libasm/include/ -I../../elibc/include/ -I../../libaspect/include/ \
	-I../../libmjollnir/include/ -I../../libedfmt/include/ $(BITS)
CFLAGS	= $(OPT)

HOSTSRC	= host.c
HOSTOBJ	= host.o
HOSTLIB = -lcrypt

all	: $(OBJ)
	@$(CC) $(OPT) $(HOSTLIB) $(HOSTSRC)
	@$(CC) $(OPT) $(OBJ) $(LIB) -o $(NAM)

clean	:
	$(RM) $(OBJ) $(HOSTOBJ) *~ \#* *\#


fclean	: clean
	$(RM) $(NAM) a.out fake_aout