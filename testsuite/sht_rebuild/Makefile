##
## Makefile for elfsh
## 
## Started on  Sun Mar 10 01:19:24 2002 mayhem
## Last update Sun Dec 15 01:56:29 2002 mayhem
##

SRC	= sht_rebuild.c
OBJ	= $(SRC:.c=.o)
CC	= gcc
LIB	= -L../../libelfsh/ -lelfsh
NAM	= sht_rebuilder
RM	= rm -f 
OPT	= -g3 -O2 -I../../libelfsh/include/
CFLAGS	= $(OPT)

all	: $(OBJ)
	@$(CC) $(OPT) $(OBJ) $(LIB) -o $(NAM)

clean	:
	$(RM) $(OBJ) *~ \#* *\#


fclean	: clean
	$(RM) $(NAM) sht_rebuilt_file
