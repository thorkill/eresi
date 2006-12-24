##
## Makefile for modules in elfsh
## 
## Started on  Wed Feb 19 08:20:54 2003 mayhem
## Last update Wed Dec 10 14:47:58 2006 mayhem
##

include ../config.h

SRC	= modtest.c
OBJ	= $(SRC:.c=.o)
NAM	= modtest.so

SRC3	= modremap.c
OBJ3	= $(SRC3:.c=.o)
NAM3	= modremap.so

CFLAGS	= -I../vm/include/ -I../libhash/include/ -I../libui/include/           \
	  -I../libdump/include/ -I../libelfsh/include/  -I../libaspect/include \
	  -I../libasm/include/ -I../e2dbg/include/ -I../libc/include/          \
	  -I../libmjollnir/include/ -Wall -g3 -O2 -fPIC 		       \
	  -DELFSH_INTERN $(DYNOPT) $(BITS) $(NCURSESLNOPT) $(READLNOPT) $(BUILDOP)

CC	= gcc
LD	= gcc

all	: test remap

test	: $(OBJ)
	@$(LD) $(BITS) -shared $(OBJ) -o $(NAM)

remap	: $(OBJ3)
	@$(LD) $(BITS) -shared $(OBJ3) -o $(NAM3)

clean	:
	@rm -f $(OBJ) $(OBJ3) *~ *\# \#* .\#*

fclean	: clean
	@rm -f *.so
