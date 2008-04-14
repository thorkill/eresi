##
## $Id: Makefile,v 1.31 2008/01/01 17:07:26 heroine Exp $
## Author  : <sk at devhell dot org>
## Started : Xxx Xxx xx xx:xx:xx 2002
## Updated : Sun Mar 21 00:03:07 2004
##

#DBGFLAGS	=	-ggdb -DDEBUG_SPARC
#include Makefile.inc
include ../config.h
#
# Architecture source files
#



SRCS_libasm 	=				\
		src/generic.c			\
		src/output.c			\
		src/error.c			\
		src/operand.c			\
		src/instruction.c		\
		src/register.c			\
		src/build.c			\
		src/vectors.c

ifeq ($(LIBASM_ENABLE_IA32), 1)
SRCS_libasm +=	\
		src/arch/ia32/init_i386.c	\
		src/arch/ia32/register.c	\
		src/arch/ia32/tables_i386.c	\
		src/arch/ia32/output_ia32.c	\
		src/arch/ia32/operand_ia32.c
SRCS_libasm    += $(foreach dir,src/arch/ia32/handlers/,\
	$(wildcard $(dir)/*.c))
SRCS_libasm    += $(foreach dir,src/arch/ia32/operand_handlers/,\
	$(wildcard $(dir)/*.c))
CFLAGS += -DLIBASM_ENABLE_IA32
endif

ifeq ($(LIBASM_ENABLE_SPARC), 1)
SRCS_libasm += 	\
		src/arch/sparc/init_sparc.c	\
		src/arch/sparc/register.c	\
		src/arch/sparc/tables_sparc.c	\
		src/arch/sparc/output_sparc.c	\
		src/arch/sparc/sparc_convert.c
SRCS_libasm    += $(foreach dir,src/arch/sparc/handlers/,\
	$(wildcard $(dir)/*.c))
SRCS_libasm    += $(foreach dir,src/arch/sparc/operand_handlers/,\
	$(wildcard $(dir)/*.c))
CFLAGS += -DLIBASM_ENABLE_SPARC
endif

ifeq ($(LIBASM_ENABLE_MIPS), 1)
SRCS_libasm +=	\
		src/arch/mips/init_mips.c	\
		src/arch/mips/output_mips.c	\
		src/arch/mips/tables_mips.c	\
		src/arch/mips/mips_convert.c	\
		src/arch/mips/register_mips.c
SRCS_libasm    += $(foreach dir,src/arch/mips/handlers/,\
	$(wildcard $(dir)/*.c))
SRCS_libasm    += $(foreach dir,src/arch/mips/operand_handlers/,\
	$(wildcard $(dir)/*.c))
CFLAGS += -DLIBASM_ENABLE_MIPS
endif

OBJS_libasm 	=	${SRCS_libasm:.c=.o} 
OBJS_libasm	+=	${SRCS_hdl:.c=.o}
NAME_libasm 	= 	libasm.a
NAME_libasm_o	=	libasm.o

SRCS_sparc	=	tools/sparc_mydisasm.c
OBJS_sparc	=	${SRCS_sparc:.c=.o}
NAME_sparc	=	test_sparc

CFLAGS 		= 	-Iinclude -Isrc/include -Wall -g3 -fPIC -DERESI32 \
			-I../libaspect/include
#CFLAGS		+=	-O2
RM 		= 	rm -f
ETAGS 		= 	etags
CC 		?= 	gcc -E
LD		?=	ld
CP 		= 	cp

all:	libasm.so


install:
	${CP} ${NAME_libasm} /usr/lib
	${CP} include/libasm.h /usr/include
	${CP} include/libasm-i386.h /usr/include
	${CP} include/libasm-sparc.h /usr/include

dep:
	@sh configure
	@rm -f src/build.o

libasm.so:	$(OBJS_libasm)
	@ar rc ${NAME_libasm} ${OBJS_libasm}
	@echo "[AR] ${NAME_libasm}"
	@${RANLIB} ${NAME_libasm}
	@echo "[RANLIB] ${NAME_libasm}"
	@$(LD) -r $(OBJS_libasm) -o ${NAME_libasm_o}
	@echo "[CC -shared] libasm.so"
ifeq ($(IRIX),1)
	@$(LD) ${OBJS_libasm} -o libasm.so -shared
else
	@$(CC) ${OBJS_libasm} -o libasm.so -shared
endif

clean:
	@$(RM) ${OBJS_libasm} ${OBJS_mydisasm} ${OBJS_cmpdump} Makefile.inc
	@find .   -name '*~' -exec rm -f {} \;
	@find src -name '*.o' -exec rm -rf {} \;

fclean: clean
	@$(RM) ${NAME_libasm} ${NAME_libasm_o} ${NAME_mydisasm} ${NAME_cmpdump}
	@$(RM) *.so *.o *.a
	@$(RM) mydisasm

tags:
	@$(ETAGS) -a arch/i386/*.c include/*.h engine/*.c sample/*.c
	@echo TAGS generated succesfully

.c.o:
	@$(CC) $(CFLAGS) ${DBGFLAGS} -c $*.c -o $*.o
	@echo "[CC] $*.o"

docs:
	doxygen doc/doxygen.conf

mydisasm:
	$(CC) -g3 -I../libelfsh/include/ -I../libaspect/include/ -I./include/ \
	tools/mydisasm.c -DERESI32 -o ./mydisasm -L ../libelfsh/ -lelfsh32 \
	-L../libaspect/ -laspect32 -L./ -lasm -L../libetrace/ -letrace32

test:
	(cd testsuite/ia32 && ${SHELL} launch_test.sh)

Makefile.inc:	
	$(SHELL) configure
