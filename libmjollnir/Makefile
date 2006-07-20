#
# LibMjollnir Makefile
# (C) 2006 Asgard Labs, thorolf
# BSD License 
# $Id: Makefile,v 1.9 2006-07-20 17:27:02 thor Exp $
#

include ../config.h

SRC= src/blocks.c \
     src/core.c \
     src/destCall.c \
     src/function.c \
     src/history.c \
     src/init.c \
     src/internal.c \
     src/symtab.c

OBJ=${SRC:.c=.o}

SRCT = bin/elfThor.c
OBJT = elfThor

SRCUT = tools/mjollnirUnitTest.c
OBJUT = mjollnirUnitTest

CFLAGS= -I./include/ \
        -I../libc/include/ \
        -I../libasm/include/ \
        -I../libelfsh/include/ \
        -I../libhash/include/ \
        -Wall $(BITS) -DELFSH_INTERN

LDFLAGS= -L. -L../libasm/ \
         -L../libelfsh/ \
         -L../libc/ \
         -L../libmalloc/ \
         -L/usr/lib/ \
         -L/usr/local/lib/ \
         -lelfsh -lasm -lcelfsh -lmalloc-e2dbg $(LDMJOLLNIR) $(LPTHREAD)

CC=gcc -ggdb -g3

NAME=libmjollnir

all: $(OBJ)
	@ar rc ${NAME}.a ${OBJ} 
	$(CC) $(BITS) -shared $(OBJ) -o $(NAME).so
	@ranlib ${NAME}.a
	$(CC) $(CFLAGS) $(SRCT) -o $(OBJT) $(LDFLAGS)
# comment this out if you don't want to make unittests 
#	$(CC) $(CFLAGS) -I/usr/local/include/ $(SRCUT) -o $(OBJUT) $(LDFLAGS) -lcheck


clean:
	rm -f src/*.o $(NAME).a $(NAME).so $(OBJT) $(OBJM)

fclean: clean
	rm -f bin/elfThor mjollnirUnitTest libmjollnir.a libmjollnir.so

.c.o:
	$(CC) $(CFLAGS) -c $*.c -o $*.o

