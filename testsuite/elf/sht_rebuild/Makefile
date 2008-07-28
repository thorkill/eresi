##
## Makefile for elfsh
## 
## Started on  Sun Mar 10 01:19:24 2002 jfv
## Last update Sun Dec 15 01:56:29 2002 jfv
##

include ../../../config.h

SRC	= sht_rebuild.c
OBJ	= $(SRC:.c=.o)
CC	= gcc
LIB	= -L../../../libelfsh/ -lelfsh32 -L../../../libasm -lasm32 \
	-L../../../libaspect/ -laspect32
NAM	= sht_rebuilder
RM	= rm -f 
OPT	= -O2 -I../../../libelfsh/include/ \
	-I../../../libaspect/include $(BITS) -DERESI32
CFLAGS	= $(OPT)

all32: all
all	: $(OBJ)
	@$(CC) $(OPT) $(OBJ) $(LIB) -o $(NAM)

clean	:
	$(RM) $(OBJ) *~ \#* *\#


fclean	: clean
	$(RM) $(NAM) sht_rebuilt_file
