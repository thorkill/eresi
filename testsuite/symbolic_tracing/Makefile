##
## Makefile for elfsh
## 
## Started on  Sun Mar 10 01:19:24 2002 mayhem
## Last update Sun May 25 22:26:01 2003 mayhem
##

SRC	= tracing.c
OBJ	= $(SRC:.c=.o)
CC	= gcc
LIB	= -L../../libelfsh/ -lelfsh
NAM	= tracer
RM	= rm -f 
VULN	= tracing.c
OPT	= -g3 -O2 -I../../libelfsh/include/
CFLAGS	= $(OPT)

all	: $(OBJ)
	@$(CC) -shared $(OPT) $(OBJ) -o $(NAM) $(LIB)
	@$(CC) $(OPT) $(VULN)

clean	:
	$(RM) $(OBJ) *~ \#* *\#


fclean	: clean
	$(RM) $(NAM) a.out fake_aout