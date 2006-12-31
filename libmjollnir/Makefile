#
# LibMjollnir Makefile
#
# (C) 2001-2006 - Devhell Labs / Asgard Labs (sk, mayhem, thorolf)
#
# BSD License 
#
include ../config.h


SRC	= src/blocks.c src/core.c src/function.c src/history.c src/init.c \
	src/symtab.c src/control.c src/types.c

OBJ	= ${SRC:.c=.o}
OBJ32	= ${SRC:.c=.32.o}
OBJ64	= ${SRC:.c=.64.o}
SRCT 	= bin/elfThor.c
OBJT 	= elfThor

SRCUT 	= tools/mjollnirUnitTest.c
OBJUT 	= mjollnirUnitTest

CFLAGS	= -fPIC -I./include/ -I../libc/include/ -I../libasm/include/ \
	-I../libelfsh/include/ -I../libhash/include/ -I../libaspect/include \
	-Wall $(BITS) -DELFSH_INTERN

CFLAGS32	= -fPIC -I./include/ -I../libc/include/ -I../libasm/include/ \
		-I../libelfsh/include/ -I../libhash/include/ -I../libaspect/include \
		-Wall -DELFSH32 -DELFSH_INTERN

CFLAGS64	= -fPIC -I./include/ -I../libc/include/ -I../libasm/include/ \
		-I../libelfsh/include/ -I../libhash/include/ -I../libaspect/include \
		-Wall -DELFSH64 -DELFSH_INTERN

LDFLAGS = -L. -L../libasm/ -L../libelfsh/ -L../libc/ -L../libmalloc \
	 -L../libaspect/ -lelfsh -lasm -lcelfsh  \
	  -laspect -lmalloc-e2dbg $(LPTHREAD)

LDFLAGS32 = 	-L. -L../libasm/ -L../libelfsh/ -L../libc/ -L../libmalloc \
		-L../libaspect/ -lelfsh32 -lasm -lcelfsh32  \
		-laspect32 -lmalloc-e2dbg $(LPTHREAD)

LDFLAGS64 = 	-L. -L../libasm/ -L../libelfsh/ -L../libc/ -L../libmalloc \
		-L../libaspect/ -lelfsh64 -lasm -lcelfsh64  \
		-laspect64 -lmalloc-e2dbg $(LPTHREAD)

CC	= gcc -ggdb -g3
NAME	= libmjollnir
NAME32	= libmjollnir32
NAME64	= libmjollnir64

all	: $(OBJ32) $(OBJ64)
#	$(CC) $(BITS) -shared $(OBJ) -o $(NAME).so -lcrypto $(LDFLAGS)
#	@ar rc ${NAME}.a ${OBJ} 
#	@ranlib ${NAME}.a
	$(CC) $(BITS) -shared $(OBJ32) -o $(NAME32).so -lcrypto $(LDFLAGS32)
	@ar rc ${NAME32}.a ${OBJ32} 
	@ranlib ${NAME32}.a
	$(CC) $(BITS) -shared $(OBJ64) -o $(NAME64).so -lcrypto $(LDFLAGS64)
	@ar rc ${NAME64}.a ${OBJ64} 
	@ranlib ${NAME64}.a
#	$(CC) $(CFLAGS) $(LDFLAGS) $(SRCT) -o $(OBJT) $(STATOPT) -lmjollnir
# comment this out if you don't want to make unittests 
#	$(CC) $(CFLAGS) -I/usr/local/include/ $(SRCUT) -o $(OBJUT) $(LDFLAGS) -lcheck

clean:
	rm -f src/*.o src/*~ src/\#* $(NAME).a $(NAME).so $(OBJT) $(OBJM)

fclean: clean
	rm -f bin/elfThor mjollnirUnitTest libmjollnir.a libmjollnir.so

.c.o:
	$(CC) $(CFLAGS) -c $*.c -o $*.o

%.32.o : %.c
			$(CC) $(CFLAGS32) -c -o $@ $<
%.64.o : %.c
			$(CC) $(CFLAGS64) -c -o $@ $<

