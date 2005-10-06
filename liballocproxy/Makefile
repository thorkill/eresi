#
# Makefile for libmalloc-e2dbg
#
# The ELF shell project 
#
# Started Tue Oct 04 15:13:19 2005 mm
# 

SRC	= malloc.c
OBJ	= $(SRC:.c=.o)
CC	?= gcc
CFLAGS	+= -D__STD_C -DUSE_ARENAS -fPIC -g3 -gstabs+ -Iinclude
AR	?= ar
RANLIB	?= ranlib
NAME	= malloc-e2dbg

all	: heap ex

heap	: $(OBJ)
	$(AR) rc lib$(NAME).a $(OBJ)
	ranlib lib$(NAME).a

ex	: 
	make -C tests all

clean	:
	rm -f $(OBJ) *~ \#* *\#
	make -C tests clean

fclean	: clean
	rm -f lib$(NAME).a
	make -C tests fclean
