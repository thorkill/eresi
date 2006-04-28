#
# LibMjollnir Makefile
# (C) 2006 Asgard Labs, thorolf a grid.einherjar.de
# BSD License 
# $Id: Makefile,v 1.2 2006-04-28 06:56:33 thor Exp $
#

SRC= src/fngdb.c \
     src/fngengine.c \
     src/hunter.c \
     src/bifrost.c \
     src/libmjollnir.c

OBJ=${SRC:.c=.o}

SRCT = tools/thor.c
OBJT = thor

SRCM = tools/mydisasm.c
OBJM = mydisasm

SRCUT = tools/mjollnirUnitTest.c
OBJUT = mjollnirUnitTest

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
         -lelfsh -lasm -lssl -lcelfsh -lmalloc-e2dbg -lmjollnir -lpthread

CC=gcc -ggdb -g3

NAME=libmjollnir.a

all: $(OBJ)
	@ar rc ${NAME} ${OBJ} 
	@ranlib ${NAME}
	$(CC) $(CFLAGS) $(SRCT) -o $(OBJT) $(LDFLAGS)
	$(CC) $(CFLAGS) $(SRCM) -o $(OBJM) $(LDFLAGS)
# comment this out if you don't want to make unittests 
#	$(CC) $(CFLAGS) -I/usr/local/include/ $(SRCUT) -o $(OBJUT) $(LDFLAGS) -lcheck


clean:
	rm -f src/*.o $(NAME) $(OBJT) $(OBJM)

fclean:
	rm -f thor mydisasm mjollnirUnitTest libmjollnir.a

.c.o:
	$(CC) $(CFLAGS) -c $*.c -o $*.o

