##
## Makefile for elfsh
## 
## Started on  Sun Mar 10 01:19:24 2002 jfv
## Last update Sun March 21 23:15:15 2007 mxatone
##
include ../../config.h

SRC 	= tracer.c
OBJ	= $(SRC:.c=.o)
CC	= gcc
LIB	= -L/usr/pkg/lib -L../../librevm/ -lrevm32 -L../../libelfsh/ -lelfsh32 \
	-L../../libetrace/ -letrace32 -L../../libaspect/ -laspect32 \
	-L../../libstderesi -lstderesi32 $(LDDUMPOPT32)
NAM	= tracer
RM	= rm -f 
OPT	= -gdwarf-2 -g3 -O2 -I../../librevm/include/ -I../../libelfsh/include/ 	\
	$(BITS) -DERESI32 -I../../libstderesi/include/ -I../../libdump/include/ \
	-I../../libasm/include/ -I../../libaspect/include/ -I../../libmjollnir/include/ \
	-I../../libedfmt/include/ $(BITS) -DERESI32 -I../../libetrace/include/ 
CFLAGS	= $(OPT)

HOSTSRC	= host.c
HOSTOBJ	= host.o
HOSTLIB = -lcrypt

all32: all

all	: $(OBJ)
	$(CC) $(HOSTLIB) $(HOSTSRC)
	$(CC) $(OBJ) $(LIB) -o $(NAM)

clean	:
	$(RM) $(OBJ) $(HOSTOBJ) *~ \#* *\#


fclean	: clean
	$(RM) $(NAM) a.out fake_aout
