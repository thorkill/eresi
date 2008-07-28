##
## Makefile for  in 
## 
## Made by jfv
## Login   <jfv@epita.fr>
## 
## Started on  Sun Mar 10 01:19:24 2002 jfv
## Last update Sun Dec 15 01:55:25 2002 jfv
##

include ../../../config.h

SRC	= dtors_hijack.c
OBJ	= $(SRC:.c=.o)
CC	= gcc
LIB	= -L../../../libelfsh/ -lelfsh32 -L../../../libasm -lasm32 \
	-L../../../libaspect/ -laspect32
NAM	= dtors_hijacker
RM	= rm -f 
VULN	= hijacked.c
OPT	= -O2 -I../../../libelfsh/include/ -I../../../libaspect/include $(BITS) -DERESI32

CFLAGS	= $(OPT)

all32: all
all	: $(OBJ)
	@$(CC) $(OPT) $(OBJ) $(LIB) -o $(NAM)
	@$(CC) $(OPT) $(VULN)

clean	:
	$(RM) $(OBJ) *~ \#* *\#


fclean	: clean
	$(RM) $(NAM) a.out fake_aout