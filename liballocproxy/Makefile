#
# Makefile for libmalloc-e2dbg
#
# The ELF shell project 
#
# Started Tue Oct 04 15:13:19 2005 mm
# 

include ../config.h

CC	?= gcc
SRC	= elfsh_malloc.c
OBJ	= $(SRC:.c=.o)
CFLAGS	+= -D__STD_C -DMALLOC_DEBUG -DUSE_ARENAS -DSHARED -DUSE_TLS \
	-fPIC -g3 -Iinclude 
NAME	= malloc-e2dbg

all	: heap ex

heap	: $(OBJ)
	$(AR) rc lib$(NAME).a $(OBJ)
	$(RANLIB) lib$(NAME).a

ex	: 
	$(MAKE) -C tests all

clean	:
	rm -f $(OBJ) *~ \#* *\#
	$(MAKE) -C tests clean

fclean	: clean
	rm -f lib$(NAME).a
	$(MAKE) -C tests fclean
