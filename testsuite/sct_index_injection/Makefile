##
## Makefile for elfsh
## 
## Started on  Sun Mar 10 01:19:24 2002 mayhem
## Last update Mon Jun 30 11:01:15 2003 mayhem
##

include ../../config.h

SRC	= section_inject.c
OBJ	= $(SRC:.c=.o)
CC	= gcc
LIB	= -L../../libelfsh/ -lelfsh -L../../libasm/ -lasm \
	-L../../libc/ -lcelfsh -L../../libmalloc/ -lmalloc-e2dbg \
	-L../usr/pkg/lib/ -lpthread
NAM	= sct_injecter
RM	= rm -f 
VULN	= main.c
OPT	= -g3 -O2 -I../../libelfsh/include -I../../libasm/include \
	-I../../libc/include $(BITS)
CFLAGS	= $(OPT)

all	: $(OBJ)
	@$(CC) $(OPT) $(OBJ) $(LIB) -o $(NAM)
	@$(CC) $(OPT) $(VULN)
	@echo WARNING: The output file is not intended to be run for this injection type.

clean	:
	$(RM) $(OBJ) *~ \#* *\#


fclean	: clean
	$(RM) $(NAM) a.out fake_aout