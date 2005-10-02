##
## this is private - do not distribute
##
## Author  : <sk at devhell dot org>
## Started : Xxx Xxx xx xx:xx:xx 2002
## Updated : Sun Mar 21 00:03:07 2004
##


include ../config.h


#
# Architecture source files
#
SRCS_libasm = src/arch/ia32/i386-0.c	\
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
	src/arch/ia32/ei386-4.c		\
	src/arch/ia32/ei386-8.c		\
	src/arch/ia32/ei386-9.c		\
	src/arch/ia32/ei386-a.c		\
	src/arch/ia32/ei386-b.c		\
	src/arch/ia32/init_i386.c	\
	src/generic.c			\
	src/operand_ia32.c		\
	src/instr_table.c		\
	src/output.c			\
	src/error.c			\
	src/operand.c			\
	src/instruction.c


OBJS_libasm =	${SRCS_libasm:.c=.o}

NAME_libasm = libasm.a

#
# debug source files
#

SRCS_mydisasm = tools/mydisasm.c

OBJS_mydisasm = ${SRCS_mydisasm:.c=.o}

CFLAGS = -Iinclude -fPIC

RM = rm -f

LDFLAGS = -L. -lasm

ETAGS = etags

CC ?= gcc

all:	lib

mydisasm: ${OBJS_mydisasm}
	$(CC) -o mydisasm ${OBJS_mydisasm} ${LDFLAGS} -lelfsh

install:
	cp libasm.a /usr/lib
	cp include/libasm.h /usr/include
	cp include/libasm-i386.h /usr/include

lib:	$(OBJS_libasm)
	@echo Building library.
	@ar rc libasm.a ${OBJS_libasm}
	@$(RANLIB) libasm.a

clean:
	@echo Cleaning sources tree.
	@$(RM) ${OBJS_libasm} ${OBJS_mydisasm} 
	@find . -name '*~' -exec rm -f {} \;

fclean: clean
	@echo Removing builds.
	@$(RM) mydisasm libasm.a 
tags:
	@$(ETAGS) -a arch/i386/*.c include/*.h engine/*.c sample/*.c
	@echo TAGS generated succesfully

.c.o:
	$(CC) $(CFLAGS) -c $*.c -o $*.o

