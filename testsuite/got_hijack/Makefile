##
## Makefile for elfsh
## 
## Started on  Sun Mar 10 01:19:24 2002 jfv
## Last update Sun Dec 15 01:55:43 2002 jfv
##

include ../../config.h

SRC	= got_hijack.c
OBJ	= $(SRC:.c=.o)
CC	?= gcc
LIB	= -L../../libelfsh/ -lelfsh32 -L../../libasm -lasm \
	-L../../libaspect/ -laspect32
NAM	= got_hijacker
RM	= rm -f 
VULN	= hijacked.c
OPT	= -O2 -I../../libelfsh/include/ \
	-I../../libaspect/include $(BITS)
CFLAGS	= $(OPT)

all	: $(OBJ)
	@$(CC) $(OPT) $(OBJ) $(LIB) -o $(NAM)
	@$(CC) $(OPT) $(VULN)

clean	:
	$(RM) $(OBJ) *~ \#* *\#


fclean	: clean
	$(RM) $(NAM) a.out fake_aout