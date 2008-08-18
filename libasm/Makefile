##
## $Id: Makefile,v 1.31 2008/01/01 17:07:26 heroine Exp $
## Author  : <sk at devhell dot org>
## Started : Xxx Xxx xx xx:xx:xx 2002
## Updated : Sun Mar 21 00:03:07 2004
##

#DBGFLAGS	=	-ggdb -DDEBUG_SPARC
include ../config.h
#
# Architecture source files
#

#CFLAGS += -ggdb
CFLAGS += $(CFLAGS_libasm) $(EXTRACFLAGS)

SRCS_libasm 	=				\
		src/generic.c			\
		src/output.c			\
		src/error.c			\
		src/operand.c			\
		src/instruction.c		\
		src/register.c			\
		src/build.c			\
		src/vectors.c

#
# IA32 Support
#

ifeq ($(LIBASM_ENABLE_IA32), 1)
SRCS_libasm +=	src/arch/ia32/init_i386.c	\
		src/arch/ia32/register.c	\
		src/arch/ia32/tables_i386.c	\
		src/arch/ia32/output_ia32.c	\
		src/arch/ia32/operand_ia32.c
ifeq ($(LIBASM_PACKED_HANDLERS), 1)
$(info IA32 Packed)
PACKED_ARCH += packed_ia32
SRCS_libasm += 	src/arch/ia32/packed_handlers.c
else
$(info IA32 Unpacked)
SRCS_libasm    += $(foreach dir,src/arch/ia32/handlers/,\
	$(wildcard $(dir)/*.c))
SRCS_libasm    += $(foreach dir,src/arch/ia32/operand_handlers/,\
	$(wildcard $(dir)/*.c))
endif
CFLAGS += -DLIBASM_ENABLE_IA32
endif

#
# Sparc Support
#

ifeq ($(LIBASM_ENABLE_SPARC), 1)
PACKED_ARCH += packed_sparc
SRCS_libasm += 	src/arch/sparc/init_sparc.c	\
		src/arch/sparc/register.c	\
		src/arch/sparc/tables_sparc.c	\
		src/arch/sparc/output_sparc.c	\
		src/arch/sparc/sparc_convert.c
ifeq ($(LIBASM_PACKED_HANDLERS),1)
$(info SPARC Packed)
PACKED_ARCH += packed_sparc
SRCS_libasm += src/arch/sparc/packed_handlers.c
else
$(info SPARC Unpacked)
SRCS_libasm    += $(foreach dir,src/arch/sparc/handlers/,\
	$(wildcard $(dir)/*.c))
SRCS_libasm    += $(foreach dir,src/arch/sparc/operand_handlers/,\
	$(wildcard $(dir)/*.c))
endif
CFLAGS += -DLIBASM_ENABLE_SPARC
endif

#
# Mips Support
#

ifeq ($(LIBASM_ENABLE_MIPS), 1)
PACKED_ARCH += packed_mips
SRCS_libasm +=	src/arch/mips/init_mips.c	\
		src/arch/mips/output_mips.c	\
		src/arch/mips/tables_mips.c	\
		src/arch/mips/mips_convert.c	\
		src/arch/mips/register_mips.c
ifeq ($(LIBASM_PACKED_HANDLERS), 1)
$(info MIPS Packed)
PACKED_ARCH += packed_mips
SRCS_libasm += src/arch/mips/packed_handlers.c
else
$(info MIPS Unpacked)
SRCS_libasm    += $(foreach dir,src/arch/mips/handlers/,\
	$(wildcard $(dir)/*.c))
SRCS_libasm    += $(foreach dir,src/arch/mips/operand_handlers/,\
	$(wildcard $(dir)/*.c))
endif
CFLAGS += -DLIBASM_ENABLE_MIPS
endif

OBJS32_libasm 	=	${SRCS_libasm:.c=.32.o} 
OBJS32_libasm	+=	${SRCS_hdl:.c=.32.o}		
OBJS64_libasm 	=	${SRCS_libasm:.c=.64.o} 
OBJS64_libasm	+=	${SRCS_hdl:.c=.64.o}		

NAME32_libasm 	= 	libasm32.a
NAME32_libasm_o	=	libasm32.o
NAME64_libasm 	= 	libasm64.a
NAME64_libasm_o	=	libasm64.o

SRCS_sparc	=	tools/sparc_mydisasm.c
OBJS_sparc	=	${SRCS_sparc:.c=.o}
NAME_sparc	=	test_sparc

CFLAGS32 	= $(CFLAGS) -Iinclude -Isrc/include -Wall -g3 -fPIC -I../libaspect/include -DERESI32
CFLAGS64 	= $(CFLAGS) -Iinclude -Isrc/include -Wall -g3 -fPIC -I../libaspect/include -DERESI64

RM 		= 	rm -f
ETAGS 		= 	etags
CC 		?= 	gcc -E
LD		?=	ld
CP 		= 	cp


all:	all32 all64
all32: 	libasm32.so
all64: 	libasm64.so

#src/arch/ia32/packed_handlers.c:
#	cat ${SRCS_IA32} > src/arch/ia32/packed_handlers.c

install:
	${CP} ${NAME_libasm} /usr/lib
	${CP} include/libasm.h /usr/include
	${CP} include/libasm-i386.h /usr/include
	${CP} include/libasm-sparc.h /usr/include

dep:
	@sh configure
	@rm -f src/build.o

libasm32.so: $(PACKED_ARCH) $(OBJS32_libasm)
	ar rc ${NAME32_libasm} ${OBJS32_libasm}
#echo "[AR] ${NAME32_libasm}"
	${RANLIB} ${NAME32_libasm}
#echo "[RANLIB] ${NAME_libasm}"
	$(LD) -r $(OBJS32_libasm) -o ${NAME32_libasm_o}
#echo "[CC -shared] libasm32.so"
ifeq ($(IRIX),1)
	$(LD) -L../libaspect/ -laspect32 ${OBJS32_libasm} -o libasm32.so -shared
else
	$(CC) -L../libaspect/ -laspect32 ${OBJS32_libasm} -o libasm32.so -shared
endif

libasm64.so: $(PACKED_ARCH) $(OBJS64_libasm)
	ar rc ${NAME64_libasm} ${OBJS64_libasm}
#echo "[AR] ${NAME64_libasm}"
	${RANLIB} ${NAME64_libasm}
#echo "[RANLIB] ${NAME64_libasm}"
	$(LD) -r $(OBJS64_libasm) -o ${NAME64_libasm_o}
#echo "[CC -shared] libasm64.so"
ifeq ($(IRIX),1)
	$(LD) -L../libaspect/ -laspect64 ${OBJS64_libasm} -o libasm64.so -shared
else
	$(CC) -L../libaspect/ -laspect64 ${OBJS64_libasm} -o libasm64.so -shared
endif

clean:
	$(RM) ${OBJS32_libasm} ${OBJ64_libasm} ${OBJS_mydisasm} ${OBJS_cmpdump} Makefile.inc
	find .   -name '*~' -exec rm -f {} \;
	find src -name '*.o' -exec rm -rf {} \;
	find src -name 'packed_handlers.c' -exec rm -rf {} \;

fclean: clean
	$(RM) ${NAME32_libasm} ${NAME64_libasm} ${NAME_mydisasm} ${NAME_cmpdump}
	$(RM) *.so *.o *.a
	$(RM) mydisasm

tags:
	$(ETAGS) -a arch/i386/*.c include/*.h engine/*.c sample/*.c
	echo TAGS generated succesfully

#.c.o:
#	$(CC) $(CFLAGS) ${DBGFLAGS} -c $*.c -o $*.o
#echo "[CC] $*.o"

docs:
	doxygen doc/doxygen.conf

mydisasm32:
	$(CC) -g3 -I../libelfsh/include/ -I../libaspect/include/ -I./include/ \
	tools/mydisasm.c -DERESI32 -o ./mydisasm -L ../libelfsh/ -lelfsh32 \
	-L../libaspect/ -laspect32 -L./ -lasm32

mydisasm64:
	$(CC) -g3 -I../libelfsh/include/ -I../libaspect/include/ -I./include/ \
	tools/mydisasm.c -DERESI64 -o ./mydisasm -L ../libelfsh/ -lelfsh64 \
	-L../libaspect/ -laspect64 -L./ -lasm64

test:
	(cd testsuite/ia32 && ${SHELL} launch_test.sh)

Makefile.inc:	
	$(SHELL) configure

packed_ia32:
	cat src/arch/ia32/handlers/*.c src/arch/ia32/operand_handlers/*.c > src/arch/ia32/packed_handlers.c
packed_sparc:
	cat src/arch/sparc/handlers/*.c src/arch/sparc/operand_handlers/*.c > src/arch/sparc/packed_handlers.c
packed_mips:
	cat src/arch/mips/handlers/*.c src/arch/mips/operand_handlers/*.c > src/arch/mips/packed_handlers.c

%.32.o 		: %.c
		$(CC) $(CFLAGS32) -c -o $@ $<

%.64.o 		: %.c
		$(CC) $(CFLAGS64) -c -o $@ $<
