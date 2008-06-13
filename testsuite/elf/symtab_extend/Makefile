##
## Makefile for elfsh
## 
## Started on  Sun Mar 10 01:19:24 2002 jfv
## Last update Sun May 25 18:04:33 2003 jfv
##

include ../../../config.h

SRC	= symtab_extend.c
OBJ	= $(SRC:.c=.o)
CC	= gcc
LIB	= -L../../../libelfsh/ -lelfsh32 -L../../../libasm -lasm \
	-L../../../libaspect/ -laspect32 \
	-L../../../libetrace/ -letrace32
NAM	= symtab_extend
RM	= rm -f 
VULN	= main.c
OPT	= -O2 -I../../../libelfsh/include/ \
	-I../../../libaspect/include $(BITS)
CFLAGS	= $(OPT)

all	: $(OBJ)
	@$(CC) $(OPT) $(OBJ) $(LIB) -o $(NAM) -static
	@$(CC) $(OPT) $(VULN) -static

clean	:
	$(RM) $(OBJ) *~ \#* *\#


fclean	: clean
	$(RM) $(NAM) a.out fake_aout
