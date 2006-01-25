##
## Makefile for elfsh
## 
## Started on  Sun Mar 10 01:19:24 2002 mayhem
## Last update Sun Dec 15 01:56:15 2002 mayhem
##

include ../../config.h

SRC	= section_inject.c
OBJ	= $(SRC:.c=.o)
CC	= gcc
LIB	= -L../../libelfsh/ -lelfsh -L../../libasm/ -lasm -L../../libc -lcelfsh \
	-L../../libmalloc -lmalloc-e2dbg -lpthread -L../usr/pkg/lib/ -lpthread
NAM	= sct_injecter
RM	= rm -f 
VULN	= main.c
OPT	= -g3 -O2 -I../../libelfsh/include/ -I../../libasm/include/ $(BITS) \
	 -I../../libmalloc/include/ -I../../libc/include/
CFLAGS	= $(OPT)

all	: $(OBJ)
	@$(CC) $(OPT) $(OBJ) $(LIB) -o $(NAM)
	@$(CC) $(OPT) $(VULN)

clean	:
	$(RM) $(OBJ) *~ \#* *\#


fclean	: clean
	$(RM) $(NAM) a.out fake_aout











