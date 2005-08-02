##
## Makefile for  in 
## 
## Made by mayhem
## Login   <mayhem@epita.fr>
## 
## Started on  Sun Mar 10 01:19:24 2002 mayhem
## Last update Sun Dec 15 01:55:25 2002 mayhem
##

include ../../config.h

SRC	= dtors_hijack.c
OBJ	= $(SRC:.c=.o)
CC	= gcc
LIB	= -L../../libelfsh/ -lelfsh -L../../libhash/ -lhashelfsh
NAM	= dtors_hijacker
RM	= rm -f 
VULN	= hijacked.c
OPT	= -g3 -O2 -I../../libelfsh/include -I../../libhash/include/ $(BITS)
CFLAGS	= $(OPT)

all	: $(OBJ)
	@$(CC) $(OPT) $(OBJ) $(LIB) -o $(NAM)
	@$(CC) $(OPT) $(VULN)

clean	:
	$(RM) $(OBJ) *~ \#* *\#


fclean	: clean
	$(RM) $(NAM) a.out fake_aout