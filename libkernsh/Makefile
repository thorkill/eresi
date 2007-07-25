include ../config.h
#
# Architecture source files
#
SRCS_libkernsh 	= kernsh.c vectors.c memory.c mem.c kmem.c kcore.c sct.c idt.c symbs.c alloc.c misc.c kernel.c

OBJS32_libkernsh	=	${SRCS_libkernsh:.c=.32.o} 
OBJS64_libkernsh	=	${SRCS_libkernsh:.64.c=.o}

NAME32_libkernsh 	= 	libkernsh32.a
NAME32_libkernsh_o	=	libkernsh32.o

NAME64_libkernsh        =       libkernsh64.a
NAME64_libkernsh_o      =       libkernsh64.o

CFLAGS32	= 	-Iinclude -Isrc/include -Wall -g3 -DELFSH32 \
			-I../libaspect/include -I../librevm/include \
			-I../libasm/include -I../libelfsh/include \
			-I../libedfmt/include -I../libmjollnir/include -O2
CFLAGS64	=	-Iinclude -Isrc/include -Wall -g3 -fPIC -DELFSH64 \
			-I../libaspect/include -I../librevm/include \
			-I../libasm/include -I../libelfsh/include \
			-I../libedfmt/include -I../libmjollnir/include -O2
RM 		= 	rm -f
ETAGS 		= 	etags
CC 		?= 	gcc
LD		?=	ld
CP 		= 	cp

all:	all32 all64

symbs :
	@cd symbols && $(MAKE)

all32: 	symbs $(OBJS32_libkernsh)
	@ar rc ${NAME32_libkernsh} ${OBJS32_libkernsh}
	@echo "[AR] ${NAME32_libkernsh}"
	@${RANLIB} ${NAME32_libkernsh}
	@echo "[RANLIB] ${NAME32_libkernsh}"
	@$(LD) -r $(OBJS32_libkernsh) -o ${NAME32_libkernsh_o}
	@echo "[CC -shared] libkernsh32.so"
	@${CC} ${OBJS32_libkernsh} -o libkernsh32.so -shared

all64:

clean:
	@$(RM) *.o
	@cd symbols && $(MAKE) clean
	#@find .   -name '*~' -exec rm -f {} \;
	#@find src -name '*.o' -exec rm -rf {} \;
	@echo "Cleaning source tree"

fclean: clean
	@$(RM) *.a *.so

#tags:
#	@$(ETAGS) -a arch/i386/*.c include/*.h engine/*.c sample/*.c
#	@echo TAGS generated succesfully

%.32.o: %.c
	@$(CC) $(CFLAGS32) ${DBGFLAGS} -c $*.c -o $*.32.o
	@echo "[CC] $*.32.o"

%.64.c.o:
	@$(CC) $(CFLAGS64) ${DBGFLAGS} -c $*.c -o $*.64.o
	@echo "[CC] $*.64.o"
docs:
	doxygen doc/doxygen.conf

