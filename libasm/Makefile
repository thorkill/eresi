##
## this is private - do not distribute
## $Id: Makefile,v 1.22 2007-05-29 00:40:27 heroine Exp $
## Author  : <sk at devhell dot org>
## Started : Xxx Xxx xx xx:xx:xx 2002
## Updated : Sun Mar 21 00:03:07 2004
##

#DBGFLAGS	=	-ggdb -DDEBUG_SPARC
include Makefile.inc
include ../config.h
#
# Architecture source files
#
SRCS_libasm 	=				\
		src/arch/ia32/init_i386.c	\
		src/arch/ia32/tables_i386.c	\
		src/arch/ia32/operand_ia32.c	\
		src/arch/sparc/init_sparc.c	\
		src/arch/sparc/tables_sparc.c	\
		src/arch/sparc/sparc_convert.c	\
		src/arch/ia32/output_ia32.c	\
		src/arch/sparc/output_sparc.c	\
		src/generic.c			\
		src/output.c			\
		src/error.c			\
		src/operand.c			\
		src/instruction.c		\
		src/arch/mips/init_mips.c	\
		src/arch/mips/mips_convert.c	\
		src/arch/mips/mips_decode.c	\
		src/arch/mips/output_mips.c	\
		src/arch/mips/tables_mips.c	\
		src/register.c			\
		src/build.c			\
		src/vectors.c			\
		src/arch/ia32/operand_handlers/asm_operand_fetch.c \
		src/arch/ia32/operand_handlers/asm_operand_fetch_address.c \
		src/arch/ia32/operand_handlers/asm_operand_fetch_control.c \
		src/arch/ia32/operand_handlers/asm_operand_fetch_debug.c \
		src/arch/ia32/operand_handlers/asm_operand_fetch_encoded.c \
		src/arch/ia32/operand_handlers/asm_operand_fetch_encodedbyte.c \
		src/arch/ia32/operand_handlers/asm_operand_fetch_fixed.c \
		src/arch/ia32/operand_handlers/asm_operand_fetch_general.c \
		src/arch/ia32/operand_handlers/asm_operand_fetch_generalbyte.c \
		src/arch/ia32/operand_handlers/asm_operand_fetch_immediate.c \
		src/arch/ia32/operand_handlers/asm_operand_fetch_immediatebyte.c \
		src/arch/ia32/operand_handlers/asm_operand_fetch_immediateword.c \
		src/arch/ia32/operand_handlers/asm_operand_fetch_jump.c \
		src/arch/ia32/operand_handlers/asm_operand_fetch_memory.c \
		src/arch/ia32/operand_handlers/asm_operand_fetch_offset.c \
		src/arch/ia32/operand_handlers/asm_operand_fetch_opmod.c \
		src/arch/ia32/operand_handlers/asm_operand_fetch_pmmx.c \
		src/arch/ia32/operand_handlers/asm_operand_fetch_register.c \
		src/arch/ia32/operand_handlers/asm_operand_fetch_segment.c \
		src/arch/ia32/operand_handlers/asm_operand_fetch_shortjump.c \
		src/arch/ia32/operand_handlers/asm_operand_fetch_xsrc.c \
		src/arch/ia32/operand_handlers/asm_operand_fetch_ydest.c





OBJS_libasm 	=	${SRCS_libasm:.c=.o} 
OBJS_libasm	+=	${SRCS_hdl:.c=.o}
NAME_libasm 	= 	libasm.a
NAME_libasm_o	=	libasm.o

SRCS_sparc	=	tools/sparc_mydisasm.c
OBJS_sparc	=	${SRCS_sparc:.c=.o}
NAME_sparc	=	test_sparc

CFLAGS 		= 	-Iinclude -Isrc/include -Wall -g3 -fPIC -DELFSH32 \
			-I../libaspect/include
RM 		= 	rm -f
ETAGS 		= 	etags
CC 		?= 	gcc
LD		?=	ld
CP 		= 	cp

all:	dep lib


install:
	${CP} ${NAME_libasm} /usr/lib
	${CP} include/libasm.h /usr/include
	${CP} include/libasm-i386.h /usr/include
	${CP} include/libasm-sparc.h /usr/include

dep:
	@rm -f src/build.o

lib:	$(OBJS_libasm)
	@ar rc ${NAME_libasm} ${OBJS_libasm}
	@echo "[AR] ${NAME_libasm}"
	@${RANLIB} ${NAME_libasm}
	@echo "[RANLIB] ${NAME_libasm}"
	@$(LD) -r $(OBJS_libasm) -o ${NAME_libasm_o}
	@echo "[CC -shared] libasm.so"
	@${CC} ${OBJS_libasm} -o libasm.so -shared

clean:
	@$(RM) ${OBJS_libasm} ${OBJS_mydisasm} ${OBJS_cmpdump} Makefile.inc
	@find .   -name '*~' -exec rm -f {} \;
	@find src -name '*.o' -exec rm -rf {} \;
	@echo "Cleaning source tree"

fclean: clean
	$(RM) ${NAME_libasm} ${NAME_libasm_o} ${NAME_mydisasm} ${NAME_cmpdump} 
	$(RM) mydisasm

tags:
	@$(ETAGS) -a arch/i386/*.c include/*.h engine/*.c sample/*.c
	@echo TAGS generated succesfully

.c.o:
	@$(CC) $(CFLAGS) ${DBGFLAGS} -c $*.c -o $*.o
	@echo "[CC] $*.o"

docs:
	doxygen doc/doxygen.conf

mydisasm:
	$(CC) -g3 -I../libelfsh/include/ -I../elibc/include/ -I../libaspect/include/ -I./include/ \
	tools/mydisasm.c -DELFSH32 -o ./mydisasm -L ../libelfsh/ -lelfsh32 -L../elibc/ -lcelfsh32 \
	-L../libaspect/ -laspect32 -L../liballocproxy/ -lallocproxy -L./ -lasm

Makefile.inc:	
	$(SHELL) configure
