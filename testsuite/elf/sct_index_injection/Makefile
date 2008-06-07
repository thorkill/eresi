##
## Makefile for elfsh
## 
## Started on  Sun Mar 10 01:19:24 2002 jfv
## Last update Mon Jun 30 11:01:15 2003 jfv
##

include ../../config.h

SRC	= section_inject.c
OBJ	= $(SRC:.c=.o)
CC	= gcc
LIB	= -L../../libelfsh/ -lelfsh32 -L../../libasm -lasm \
	-L../../libaspect/ -laspect32	\
	-L../../libetrace/ -letrace32
NAM	= sct_injecter
RM	= rm -f 
VULN	= main.c
OPT	= -O2 -I../../libelfsh/include/ \
	-I../../libaspect/include $(BITS)
CFLAGS	= $(OPT)

all	: $(OBJ)
	@$(CC) $(OPT) $(OBJ) $(LIB) -o $(NAM)
	@$(CC) $(OPT) $(VULN)
	@echo WARNING: The output file is not intended to be used in this example

clean	:
	$(RM) $(OBJ) *~ \#* *\#


fclean	: clean
	$(RM) $(NAM) a.out fake_aout