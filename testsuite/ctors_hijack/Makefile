##
## Makefile for elfsh
## 
## Started on  Sun Mar 10 01:19:24 2002 mayhem
## Last update Sun Dec 15 01:54:50 2002 mayhem
##

include	../../config.h

SRC	= ctors_hijack.c
OBJ	= $(SRC:.c=.o)
CC	= gcc
LIB	= -L../../libelfsh/ -lelfsh -L../../libasm/ -lasm
NAM	= ctors_hijacker
RM	= rm -f 
VULN	= hijacked.c
OPT	= -g3 -O2 -I../../libelfsh/include/ -I../../libasm/include/ $(BITS)
CFLAGS	= $(OPT) 

all	: $(OBJ)
	@$(CC) $(OPT) $(OBJ) -o $(NAM) $(LIB)
	@$(CC) $(OPT) $(VULN)

clean	:
	$(RM) $(OBJ) *~ \#* *\#


fclean	: clean
	$(RM) $(NAM) a.out fake_aout