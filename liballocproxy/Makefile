#
# Makefile for libmalloc-e2dbg
#
# The ELF shell project 
#
# Started Tue Oct 04 15:13:19 2005 mm
# 

include ../config.h

CC	?= gcc
LD	?= ld
SRC	= aproxy_malloc.c aproxy_strdup.c
OBJ	= $(SRC:.c=.o)
CFLAGS	+= -D__STD_C -DMALLOC_DEBUG -DUSE_ARENAS -DSHARED -DUSE_TLS \
	-fPIC -g3 -Iinclude -I../libelfsh/include/
NAME	= allocproxy

all	: heap

heap	: $(OBJ)
	$(AR) rc lib$(NAME).a $(OBJ)
	$(RANLIB) lib$(NAME).a
	$(LD) -r $(OBJ) -o lib$(NAME).o

clean	:
	rm -f $(OBJ) *~ \#* *\#

fclean	: clean
	rm -f lib$(NAME).a lib$(NAME).o
