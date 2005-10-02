##
## Makefile for elfsh
## 
## Started on  Sun Mar 10 01:19:24 2002 mayhem
## Last update Sun Dec 15 01:56:52 2002 mayhem
##

include ../../config.h

SRC	= sht_removing.c
OBJ	= $(SRC:.c=.o)
CC	= gcc
LIB	= -L../../libelfsh/ -lelfsh -L../../libasm/ -lasm
NAM	= sht_remover
RM	= rm -f 
VULN	= main.c
OPT	= -g3 -O2 -I../../libelfsh/include/ -I../../libhash/include -I../../libasm/include/  $(BITS)
CFLAGS	= $(OPT)

all	: $(OBJ)
	@$(CC) $(OPT) $(OBJ) $(LIB) -o $(NAM)
	@$(CC) $(OPT) $(VULN)

clean	:
	$(RM) $(OBJ) *~ \#* *\#


fclean	: clean
	$(RM) $(NAM) a.out fake_aout