##
## Makefile for elfsh
## 
## Started on  Sun Mar 10 01:19:24 2002 jfv
## Last update Sun Dec 15 01:54:50 2002 jfv
##

include	../../../config.h

SRC	= ctors_hijack.c
OBJ	= $(SRC:.c=.o)
CC	= gcc
LIB	= -L../../../libelfsh/ -lelfsh32 -L../../../libasm -lasm \
	-L../../../libaspect/ -laspect32 \
	-L../../../libetrace/ -letrace32
NAM	= ctors_hijacker
RM	= rm -f 
VULN	= hijacked.c
OPT	= -O2 -I../../../libelfsh/include/ -I../../../libaspect/include $(BITS)
CFLAGS	= $(OPT) 

all	: $(OBJ)
	@$(CC) $(OPT) $(OBJ) -o $(NAM) $(LIB)
	@$(CC) $(OPT) $(VULN)

clean	:
	$(RM) $(OBJ) *~ \#* *\#


fclean	: clean
	$(RM) $(NAM) a.out fake_aout