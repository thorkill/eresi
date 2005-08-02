##
## Makefile for libdump in elfsh 
## 
## Started on  Sat Jan 25 05:41:19 2003 mayhem
## Last update Tue Feb 15 12:58:45 2005 ym
##

include ../config.h

SRC	= dump.c recv.c send.c
OBJ	= dump.o recv.o send.o
CFLAGS	= -Wall $(BITS) -g3 -Iinclude -I../libhash/include/ -DDUMP_INTERN -DELFSH_INTERN
LDFLAGS = -L./ -L../libhash/ -lhashelfsh  -ldumpelfsh -L/usr/pkg/lib/ -ltermcap -lreadline $(BITS) $(EXTRAOPT)
CC	= gcc
AR	= ar rc
RANLIB	= ranlib
NAME	= libdumpelfsh


all	: $(OBJ)
	$(AR) $(NAME).a $(OBJ)
	$(RANLIB) $(NAME).a

test:   all dump_main.o
#	$(CC) $(CFLAGS) $(LDFLAGS) dump_main.o -o dump_main 
	gcc  -Wall -DELFSH32 -g3 -Iinclude -I../libhash/include/ \
	-DELFSH_INTERN -L./ -L../libhash/ -lhashelfsh -L/usr/pkg/lib/ \
	-ltermcap -lreadline -DELFSH32  ../libhash/hash.o dump.o recv.o\
	send.o dump_main.c -o dump_main

clean	:
	@rm -f $(OBJ) *~ *\# \#* .\#* dump_main.o
	@rm -f include/*~ include/*\# include/\#* include/.\#* 

fclean	: clean
	@rm -f $(NAME).a dump_main
