##
## Makefile for elfsh
## 
## Started on  Sun Mar 10 01:19:24 2002 mayhem
## Last update Sun May 25 18:04:33 2003 mayhem
##

include ../../config.h

SRC	= symtab_extend.c
OBJ	= $(SRC:.c=.o)
CC	= gcc
LIB	= -L../../libelfsh/ -lelfsh -L../../libasm/ -lasm \
	-L../../libc/ -lcelfsh -L../../libmalloc/ -lmalloc-e2dbg \
	-L../usr/pkg/lib/ -lpthread
NAM	= symtab_extend
RM	= rm -f 
VULN	= main.c
OPT	= -g3 -O2 -I../../libelfsh/include/ -I../../libhash/include \
	-I../../libasm/include/ -I../../libc/include/ -I../../libmalloc/include/ \
	$(BITS)
CFLAGS	= $(OPT)

all	: $(OBJ)
	@$(CC) $(OPT) $(OBJ) $(LIB) -o $(NAM) -static
	@$(CC) $(OPT) $(VULN) -static

clean	:
	$(RM) $(OBJ) *~ \#* *\#


fclean	: clean
	$(RM) $(NAM) a.out fake_aout