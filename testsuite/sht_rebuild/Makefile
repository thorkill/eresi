##
## Makefile for elfsh
## 
## Started on  Sun Mar 10 01:19:24 2002 mayhem
## Last update Sun Dec 15 01:56:29 2002 mayhem
##

include ../../config.h

SRC	= sht_rebuild.c
OBJ	= $(SRC:.c=.o)
CC	= gcc
LIB	= -L../../libelfsh/ -lelfsh -L../../libasm/ -lasm \
	-L../../libc/ -lcelfsh -L../../libmalloc/ -lmalloc-e2dbg \
	-L../usr/pkg/lib/ -lpthread
NAM	= sht_rebuilder
RM	= rm -f 
OPT	= -g3 -O2 -I../../libelfsh/include/ -I../../libasm/include/ \
	-I../../libc/include $(BITS)
CFLAGS	= $(OPT)

all	: $(OBJ)
	@$(CC) $(OPT) $(OBJ) $(LIB) -o $(NAM)

clean	:
	$(RM) $(OBJ) *~ \#* *\#


fclean	: clean
	$(RM) $(NAM) sht_rebuilt_file
