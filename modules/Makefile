##
## Makefile for modules in elfsh
## 
## Started on  Wed Feb 19 08:20:54 2003 mayhem
## Last update Wed Jun 25 06:52:49 2003 mayhem
##


SRC	= modtest.c
OBJ	= $(SRC:.c=.o)
NAM	= modtest.so

SRC2	= modgraph.c
OBJ2	= $(SRC2:.c=.o)
NAM2	= modgraph.so

SRC3	= modremap.c
OBJ3	= $(SRC3:.c=.o)
NAM3	= modremap.so

CFLAGS	= -I../vm/include/ -I../libhash/include/ -I../libelfsh/include/ \
	-Wall -g3 -O2 -fPIC -rdynamic -DELFSH_INTERN
CC	= gcc

all	: test remap #flow

test	: $(OBJ)
	@$(CC) -shared $(OBJ) -o $(NAM)

remap	: $(OBJ3)
	@$(CC) -shared $(OBJ3) -o $(NAM3)

flow 	: 
	make -C modflow

clean	:
	@rm -f $(OBJ) $(OBJ2) $(OBJ3) *~ *\# \#* .\#*
	cd modflow && make clean

fclean	: clean
	@rm -f *.so
	cd modflow && make fclean
