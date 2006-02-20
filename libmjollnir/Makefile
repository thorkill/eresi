#
# LibMjollnir Makefile
# (C) 2006 Asgard Labs, thorolf a grid.einherjar.de
# BSD License 
# $Id: Makefile,v 1.1.1.1 2006-02-20 23:25:27 thor Exp $
#

SRC= src/fngdb.c \
     src/fngengine.c \
     src/hunter.c \
     src/libmjollnir.c

OBJ=${SRC:.c=.o}

SRCT = tools/thor.c
OBJT = thor

SRCM = tools/mydisasm.c
OBJM = mydisasm

CFLAGS= -I./include/ \
        -I../libc/include/ \
        -I../libasm/include/ \
        -I../libelfsh/include/ \
        -I../libhash/include/ \
        -Wall -DELFSH32

LDFLAGS= -L. -L../libasm/ \
         -L../libelfsh/ \
         -L../libc/ \
         -L../libmalloc/ \
         -L/usr/lib/ \
         -L/usr/local/lib/ \
         -lelfsh -lasm -lmd -lcelfsh -lmalloc-e2dbg -lmjollnir

CC=gcc -ggdb -g3

NAME=libmjollnir.a

all: $(OBJ)
	@ar rc ${NAME} ${OBJ} 
	@ranlib ${NAME}
	$(CC) $(CFLAGS) $(SRCT) -o $(OBJT) $(LDFLAGS)
	$(CC) $(CFLAGS) $(SRCM) -o $(OBJM) $(LDFLAGS)


clean:
	rm -f src/*.o $(NAME) $(OBJT) $(OBJM)

.c.o:
	$(CC) $(CFLAGS) -c $*.c -o $*.o

