##
## Makefile for modules in ERESI
## 
## Started on  Wed Feb 19 08:20:54 2003 jfv
## $Id
##

include ../config.h

SRC	= modtest.c
OBJ32	= $(SRC:.c=.32.o)
OBJ64	= $(SRC:.c=.64.o)
NAM32	= modtest32.so
NAM64	= modtest64.so

RSRC	= modremap.c
ROBJ32	= $(RSRC:.c=.32.o)
RNAM32	= modremap32.so
ROBJ64	= $(RSRC:.c=.64.o)
RNAM64	= modremap64.so

CFLAGS	= -I../librevm/include/ -I../libhash/include/ -I../libui/include/		\
	  -I../libdump/include/ -I../libelfsh/include/  -I../libaspect/include		\
	  -I../libasm/include/ -I../e2dbg/include/ -I../libc/include/			\
	  -I../libmjollnir/include/ -I../libedfmt/include/ -I../libstderesi/include/	\
	  -I../libetrace/include/ -Wall -g3 -O2 -fPIC  $(DYNOPT)	\
	  $(BITS) $(NCURSESLNOPT) $(READLNOPT) $(BUILDOP)

CFLAGS32=$(CFLAGS) -DERESI32
CFLAGS64=$(CFLAGS) -DERESI64

CC	= gcc
LD	= gcc

all64	: test64 remap64
all32	: test32 remap32
all	: all32 all64

test32	: $(OBJ32)
	@$(LD) $(BITS) -shared $(OBJ32) -o $(NAM32)

remap32	: $(ROBJ32)
	@$(LD) $(BITS) -shared $(ROBJ32) -o $(RNAM32)

test64	: $(OBJ64)
	@$(LD) $(BITS) -shared $(OBJ64) -o $(NAM64)

remap64	: $(ROBJ64)
	@$(LD) $(BITS) -shared $(ROBJ64) -o $(RNAM64)

clean	:
	@rm -f $(OBJ32) $(ROBJ32) $(OBJ64) $(ROBJ64) *~ *\# \#* .\#*

fclean	: clean
	@rm -f *.so

%.32.o	: %.c
	$(CC) $(CFLAGS32) -c -o $@ $<

%.64.o	: %.c
	$(CC) $(CFLAGS64) -c -o $@ $<