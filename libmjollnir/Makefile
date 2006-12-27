#
# LibMjollnir Makefile
#
# (C) 2001-2006 - Devhell Labs / Asgard Labs (sk, thorolf, mayhem)
#
# BSD License 
#

include ../config.h


SRC	= src/blocks.c src/core.c src/destCall.c src/function.c src/history.c \
	src/init.c src/internal.c src/symtab.c src/control.c src/btree.c

OBJ	= ${SRC:.c=.o}
SRCT 	= bin/elfThor.c
OBJT 	= elfThor

SRCUT 	= tools/mjollnirUnitTest.c
OBJUT 	= mjollnirUnitTest

CFLAGS	= -fPIC -I./include/ -I../libc/include/ -I../libasm/include/ \
        -I../libelfsh/include/ -I../libhash/include/ -I../libaspect/include \
	-Wall $(BITS) -DELFSH_INTERN

LDFLAGS = -L. -L../libasm/ -L../libelfsh/ -L../libc/ -L../libmalloc \
	 -L../libaspect/ -lelfsh -lasm -lcelfsh  \
	  -laspect -lmalloc-e2dbg $(LPTHREAD)

CC	= gcc -ggdb -g3
NAME	= libmjollnir

all	: $(OBJ)
	$(CC) $(BITS) -shared $(OBJ) -o $(NAME).so -lcrypto $(LDFLAGS)
	@ar rc ${NAME}.a ${OBJ} 
	@ranlib ${NAME}.a
	$(CC) $(CFLAGS) $(LDFLAGS) $(SRCT) -o $(OBJT) $(STATOPT) -lmjollnir
# comment this out if you don't want to make unittests 
#	$(CC) $(CFLAGS) -I/usr/local/include/ $(SRCUT) -o $(OBJUT) $(LDFLAGS) -lcheck

clean:
	rm -f src/*.o $(NAME).a $(NAME).so $(OBJT) $(OBJM)

fclean: clean
	rm -f bin/elfThor mjollnirUnitTest libmjollnir.a libmjollnir.so

.c.o:
	$(CC) $(CFLAGS) -c $*.c -o $*.o

