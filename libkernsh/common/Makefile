include ../../config.h
#
# Architecture source files
#
SRCS 	= autotypes.c dump.c hash.c memory.c vectors.c vectors_register.c gdt.c idt.c sct.c vectors_default.c

OBJS32	=	${SRCS:.c=.32.o} 
OBJS64	=	${SRCS:.c=.64.o}

CFLAGS32	= 	-I../include -Wall -g3 -DERESI32 \
			-I../../libaspect/include -I../../libasm/include  \
			-I../../libelfsh/include -I../../libedfmt/include \
			-I../../libmjollnir/include -O2
CFLAGS64	=	-I../include -Wall -g3 -fPIC -DERESI64 \
			-I../../libaspect/include -I../../libasm/include \
			-I../../libelfsh/include -I../../libedfmt/include \
			-I../../libmjollnir/include -O2
RM 		= 	rm -f
ETAGS 		= 	etags
CC 		?= 	gcc
LD		?=	ld
CP 		= 	cp

all:	all32 all64 

symbs :
	@cd symbols && $(MAKE)

all32:	$(OBJS32)

all64:	$(OBJS64)

clean:
	@$(RM) $(OBJS32) $(OBJS64) *~ \#* *\# .\#*

fclean: clean

#tags:
#	@$(ETAGS) -a arch/i386/*.c include/*.h engine/*.c sample/*.c
#	@echo TAGS generated succesfully

%.32.o: %.c
	@$(CC) $(CFLAGS32) ${DBGFLAGS} -c $*.c -o $*.32.o
	@echo "[CC] $*.32.o"

%.64.o: %.c
	@$(CC) $(CFLAGS64) ${DBGFLAGS} -c $*.c -o $*.64.o
	@echo "[CC] $*.64.o"
