##
## Makefile for modules in elfsh
## 
## Started on  Wed Feb 19 08:20:54 2003 mayhem
## Last update Wed Apr 20 11:47:58 2005 kahmm
##

include ../config.h

SRC	= modtest.c
OBJ	= $(SRC:.c=.o)
NAM	= modtest.so

SRC2	= modgraph.c
OBJ2	= $(SRC2:.c=.o)
NAM2	= modgraph.so

SRC3	= modremap.c
OBJ3	= $(SRC3:.c=.o)
NAM3	= modremap.so

CFLAGS	= -I../vm/include/ -I../libhash/include/        \
	  -I../libdump/include/ -I../libelfsh/include/  \
	  -I../libasm/include/ -I../e2dbg/include/      \
	  -Wall -g3 -O2 -fPIC -DELFSH_INTERN            \
	  $(DYNOPT) $(BITS)

CC	= gcc
LD	= gcc

all	: test remap flow

test	: $(OBJ)
	@$(LD) $(BITS) -shared $(OBJ) -o $(NAM)

remap	: $(OBJ3)
	@$(LD) $(BITS) -shared $(OBJ3) -o $(NAM3)

flow 	: 
	cd modflow && $(MAKE)

clean	:
	@rm -f $(OBJ) $(OBJ2) $(OBJ3) *~ *\# \#* .\#*
	cd modflow && $(MAKE) clean

fclean	: clean
	@rm -f *.so
	cd modflow && $(MAKE) fclean
