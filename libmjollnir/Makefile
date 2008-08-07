#
# LibMjollnir Makefile
#
# (C) 2001-2006 - Devhell Labs / Asgard Labs (sk, jfv, thorolf)
#

include ../config.h

SRC	= src/blocks.c src/core.c src/history.c src/init.c src/symtab.c \
	src/findentry.c src/cfg.c src/fingerprint.c src/display.c src/links.c \
	src/ondisk.c src/function.c src/container.c

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

CFLAGS32 = -fPIC -I./include/ -I../libasm/include/ -I../libelfsh/include/ \
	   -I../libaspect/include -Wall -DERESI32 -g3 $(EXTRACFLAGS)

CFLAGS64 = -fPIC -I./include/ -I../libasm/include/ -I../libelfsh/include/ \
	 -I../libaspect/include  -Wall -DERESI64  -g3 $(EXTRACFLAGS)

LDFLAGS32 = -L. -L../libasm/ -L../libelfsh/ -L../liballocproxy   \
	  -L../libaspect/ -lelfsh32 -laspect32 -lasm32 -lallocproxy $(LPTHREAD)

LDFLAGS64 = -L. -L../libasm/ -L../libelfsh/ -L../liballocproxy   \
	  -L../libaspect/ -lelfsh64 -laspect64 -lasm64 -lallocproxy $(LPTHREAD)

all	: all32 all64

$(NAME32).so	: $(OBJ32)
	@$(CC) $(BITS) -shared $(OBJ32) -o $(NAME32).so -lcrypto $(LDFLAGS32)
	@ar rc ${NAME32}.a ${OBJ32} 
	@${RANLIB} ${NAME32}.a
#	$(CC) $(CFLAGS32) $(LDFLAGS32) $(SRCT) -o $(OBJT) $(STATOPT) -lmjollnir32
	@$(LD) -r $(OBJ32) -o $(NAME32).o

all32: $(NAME32).so

$(NAME64).so	: $(OBJ64)
	@$(CC) $(BITS) -shared $(OBJ64) -o $(NAME64).so -lcrypto $(LDFLAGS64)
	@ar rc ${NAME64}.a ${OBJ64} 
	@${RANLIB} ${NAME64}.a
	@$(LD) -r $(OBJ64) -o $(NAME64).o

all64: $(NAME64).so

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

