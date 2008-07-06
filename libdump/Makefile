##
## Makefile for libdump in elfsh 
## 
## Started on  Sat Jan 25 05:41:19 2003 jfv
## Last update Tue Feb 15 12:58:45 2005 ym
##

include ../config.h

SRC	= dump.c recv.c send.c
OBJ	= dump.o recv.o send.o
CFLAGS	= -Wall $(BITS) -g3 -Iinclude -I../libc/include/ -I../libelfsh/include/ \
	-I../libaspect/include/ -DELFSH_INTERN -fPIC
LDFLAGS = -L./ -L../libelfsh/ -lelfsh -ldump -L/usr/pkg/lib/ -laspect $(BITS) \
	$(EXTRAOPT)
CC	?= gcc
LD	?= ld
AR	= ar rc
NAME	= libdump

all	: $(OBJ)
	$(AR) $(NAME).a $(OBJ)
	$(RANLIB) $(NAME).a
	$(LD) -r $(OBJ) -o $(NAME).o

test:   all dump_main.o
	gcc  -Wall -DERESI32 -g3 -Iinclude -DELFSH_INTERN -L../libelfsh/ -lelfsh \
	$(OBJ) dump_main.c -o dump_main

clean	:
	@rm -f $(OBJ) *~ *\# \#* .\#* dump_main.o
	@rm -f include/*~ include/*\# include/\#* include/.\#* 

fclean	: clean
	@rm -f $(NAME).a $(NAME).o dump_main
