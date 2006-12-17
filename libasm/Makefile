##
## this is private - do not distribute
##
## Author  : <sk at devhell dot org>
## Started : Xxx Xxx xx xx:xx:xx 2002
## Updated : Sun Mar 21 00:03:07 2004
##

#DBGFLAGS	=	-ggdb -DDEBUG_SPARC

#
# Architecture source files
#
SRCS_libasm 	=				\
		src/arch/ia32/i386-0.c		\
		src/arch/ia32/i386-1.c		\
		src/arch/ia32/i386-2.c		\
		src/arch/ia32/i386-3.c		\
		src/arch/ia32/i386-4.c		\
		src/arch/ia32/i386-5.c		\
		src/arch/ia32/i386-6.c		\
		src/arch/ia32/i386-7.c		\
		src/arch/ia32/i386-8.c		\
		src/arch/ia32/i386-9.c		\
		src/arch/ia32/i386-a.c		\
		src/arch/ia32/i386-b.c		\
		src/arch/ia32/i386-c.c		\
		src/arch/ia32/i386-d.c		\
		src/arch/ia32/i386-e.c		\
		src/arch/ia32/i386-f.c		\
		src/arch/ia32/ei386-0.c		\
		src/arch/ia32/ei386-1.c		\
		src/arch/ia32/ei386-2.c		\
		src/arch/ia32/ei386-3.c		\
		src/arch/ia32/ei386-4.c		\
		src/arch/ia32/ei386-6.c		\
		src/arch/ia32/ei386-7.c		\
		src/arch/ia32/ei386-8.c		\
		src/arch/ia32/ei386-9.c		\
		src/arch/ia32/ei386-a.c		\
		src/arch/ia32/ei386-b.c		\
		src/arch/ia32/ei386-c.c     \
		src/arch/ia32/ei386-d.c     \
		src/arch/ia32/ei386-e.c     \
		src/arch/ia32/init_i386.c	\
		src/arch/ia32/tables_i386.c	\
		src/arch/ia32/operand_ia32.c	\
		src/arch/sparc/init_sparc.c	\
		src/arch/sparc/tables_sparc.c	\
		src/arch/sparc/sparc_decode.c	\
		src/arch/sparc/sparc_convert.c	\
		src/arch/sparc/sparc_synthetic.c\
		src/arch/ia32/output_ia32.c	\
		src/arch/sparc/output_sparc.c	\
		src/generic.c			\
		src/output.c			\
		src/error.c			\
		src/operand.c			\
		src/instruction.c

OBJS_libasm 	=	${SRCS_libasm:.c=.o}
NAME_libasm 	= 	libasm.a

SRCS_sparc	=	tools/sparc_mydisasm.c
OBJS_sparc	=	${SRCS_sparc:.c=.o}
NAME_sparc	=	test_sparc

CFLAGS 		= 	-Iinclude -Isrc/include -Wall -g3 -fPIC
RM 		= 	rm -f
ETAGS 		= 	etags
CC 		= 	gcc
CP 		= 	cp

all:	lib


install:
	${CP} ${NAME_libasm} /usr/lib
	${CP} include/libasm.h /usr/include
	${CP} include/libasm-i386.h /usr/include
	${CP} include/libasm-sparc.h /usr/include

lib:	$(OBJS_libasm)
	ar rc ${NAME_libasm} ${OBJS_libasm}
	ranlib ${NAME_libasm}

clean:
	$(RM) ${OBJS_libasm} ${OBJS_mydisasm} ${OBJS_cmpdump}
	find . -name '*~' -exec rm -f {} \;

fclean: clean
	$(RM) ${NAME_libasm} ${NAME_mydisasm} ${NAME_cmpdump} 

tags:
	@$(ETAGS) -a arch/i386/*.c include/*.h engine/*.c sample/*.c
	@echo TAGS generated succesfully

.c.o:
	$(CC) $(CFLAGS) ${DBGFLAGS} -c $*.c -o $*.o

mydisasm:
	$(CC) -g3 -I../libelfsh/include/ -I./include/ \
	-I../libc/include/ -L./ -L../libelfsh/ \
	-L../libc/ -L../libmalloc/ -lpthread \
	tools/mydisasm.c -o ./mydisasm -lasm -lelfsh -lcelfsh -lmalloc-e2dbg


