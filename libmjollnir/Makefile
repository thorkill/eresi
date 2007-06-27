#
# LibMjollnir Makefile
#
# (C) 2001-2006 - Devhell Labs / Asgard Labs (sk, mayhem, thorolf)
#
# BSD License 
#
include ../config.h

SRC	= src/blocks.c src/core.c src/function.c src/history.c src/init.c \
	src/container.c src/symtab.c src/findentry.c src/cfg.c src/fingerprint.c \
	src/display.c src/links.c

OBJ	= ${SRC:.c=.o}
OBJ32	= ${SRC:.c=.32.o}
OBJ64	= ${SRC:.c=.64.o}
SRCT 	= bin/elfThor.c
OBJT 	= elfThor
SRCUT 	= tools/mjollnirUnitTest.c
OBJUT 	= mjollnirUnitTest

CC	?= gcc
LD	?= ld
NAME32	= libmjollnir32
NAME64	= libmjollnir64

CFLAGS32 = -fPIC -I./include/ -I../elibc/include/ -I../libasm/include/       \
	 -I../libelfsh/include/ -I../libhash/include/ -I../libaspect/include \
	 -Wall -DELFSH32 -DELFSH_INTERN -g3

CFLAGS64 = -fPIC -I./include/ -I../elibc/include/ -I../libasm/include/ 	     \
	 -I../libelfsh/include/ -I../libhash/include/ -I../libaspect/include \
	 -Wall -DELFSH64 -DELFSH_INTERN -g3

LDFLAGS32 = -L. -L../libasm/ -L../libelfsh/ -L../elibc/ -L../liballocproxy   \
	  -L../libaspect/ -lelfsh32 -lasm -lcelfsh32 -laspect32              \
	  -lallocproxy $(LPTHREAD)

LDFLAGS64 = -L. -L../libasm/ -L../libelfsh/ -L../elibc/ -L../liballocproxy   \
	  -L../libaspect/ -lelfsh64 -lasm -lcelfsh64  -laspect64             \
	  -lallocproxy $(LPTHREAD)

all	: all32 all64

all32	: $(OBJ32)
	@$(CC) $(BITS) -shared $(OBJ32) -o $(NAME32).so -lcrypto $(LDFLAGS32)
	@ar rc ${NAME32}.a ${OBJ32} 
	@${RANLIB} ${NAME32}.a
#	$(CC) $(CFLAGS32) $(LDFLAGS32) $(SRCT) -o $(OBJT) $(STATOPT) -lmjollnir32
	@$(LD) -r $(OBJ32) -o $(NAME32).o

all64	: $(OBJ64)
	@$(CC) $(BITS) -shared $(OBJ64) -o $(NAME64).so -lcrypto $(LDFLAGS64)
	@ar rc ${NAME64}.a ${OBJ64} 
	@${RANLIB} ${NAME64}.a
	@$(LD) -r $(OBJ64) -o $(NAME64).o

docs:
	doxygen doc/doxygen.conf

clean:
	rm -f $(OBJ) $(OBJ32) $(OBJ64) src/*~ src/\#* \
	include/\#* include/*~ $(OBJT) $(OBJM)

fclean: clean
	rm -f bin/elfThor mjollnirUnitTest *.a *.so *~ $(NAME64).o $(NAME32).o

.c.o:
	$(CC) $(CFLAGS) -c $*.c -o $*.o

%.32.o : %.c
	$(CC) $(CFLAGS32) -c -o $@ $<
%.64.o : %.c
	$(CC) $(CFLAGS64) -c -o $@ $<

