include ../../config.h
#
# Architecture source files
#
SRCS 	= kernsh.c kernimage.c alloc.c kvirtm.c misc.c symbs.c kcore.c kmem.c mem.c module.c

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

all32:	symbs $(OBJS32)

all64:	symbs $(OBJS64) 

clean:
	@$(RM) $(OBJS32) $(OBJS64) *~ \#* *\# .\#*
	@cd symbols && $(MAKE) clean

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
