##
## Makefile for libdump in ERESI
## 
## Started on  Sat Jan 25 05:41:19 2003 jfv
## $Id
##

include ../config.h

SRC	  = dump.c recv.c send.c
OBJ32	  = $(SRC:.c=.32.o)
OBJ64	  = $(SRC:.c=.64.o)
CFLAGS32  = -Wall -g3 -Iinclude -I../libaspect/include/  -fPIC -DERESI32 -libdump32
CFLAGS64  = -Wall -g3 -Iinclude -I../libaspect/include/  -fPIC -DERESI64 -libdump64
LDFLAGS32 = $(EXTRAOPT)
LDFLAGS64 = $(EXTRAOPT)
NAME32	  = libdump32
NAME64	  = libdump64

CC	?= gcc
LD	?= ld
AR	= ar rc

all	: all32 all64

all32:	$(OBJ32)
	$(AR) $(NAME32).a $(OBJ32)
	$(RANLIB) $(NAME32).a
	$(LD) -r $(OBJ32) -o $(NAME32).o $(LDFLAGS32)

all64:	$(OBJ64)
	$(AR) $(NAME64).a $(OBJ64)
	$(RANLIB) $(NAME64).a
	$(LD) -r $(OBJ64) -o $(NAME64).o $(LDFLAGS64)

clean	:
	@rm -f $(OBJ32) $(OBJ64) *~ *\# \#* .\#* dump_main.o
	@rm -f include/*~ include/*\# include/\#* include/.\#* 

fclean	: clean
	@rm -f $(NAME32).a $(NAME32).o dump_main* $(NAME64).a $(NAME64).o

%.32.o 	: %.c
	$(CC) $(CFLAGS32) -c -o $@ $<

%.64.o 	: %.c
	$(CC) $(CFLAGS64) -c -o $@ $<
