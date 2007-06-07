##
## Makefile for libe2dbg in the ERESI framework
##
## Started on  Tue Aug  16 06:32:29 2005 mayhem
## $id$
##

include ../config.h

SRC			= e2dbg.c backtrace.c breakpoints.c delete.c display.c \
			signal.c mode.c stack.c step.c linkmap.c dumpregs.c    \
			dbghooks.c mutex.c continue.c alloc-proxy.c resolv.c   \
			threads.c entry.c e2dbg-misc.c dbg-ia32.c
OBJ			= $(SRC:.c=.o)
OBJ32			= $(SRC:.c=.32.o)
OBJ64			= $(SRC:.c=.64.o)
CC			?= gcc
LD			?= ld
RM			= rm -f
DBGNAME32		= libe2dbg32
DBGNAME64		= libe2dbg64
DBGNAME32O		= e2dbg-rel32.o
DBGNAME64O		= e2dbg-rel64.o
AR			= ar rc


CFLAGS32		+= -fPIC -g3 -Wall -Iinclude $(DYNOPT) $(BUILDOP) 	 \
			-I../libelfsh/include/ -I../libdump/include/      	 \
			-I../libasm/include/ -I../vm/include/ -I../elibc/include/ \
			-I../libui/include/ -I../libmjollnir/include/ 		 \
			-I../libaspect/include -I../librevm/include/		 \
			-I../libedfmt/include/ -I../liballocproxy/include/	 \
			-DELFSH_INTERN -DE2DBG_INTERN $(ASMOPT) -DELFSH32	 \
			-DE2DBG_PRESENT

CFLAGS64		+= -fPIC -g3 -Wall -Iinclude $(DYNOPT) $(BUILDOP) 	 \
			-I../libelfsh/include/ -I../libdump/include/      	 \
			-I../libasm/include/ -I../vm/include/ -I../elibc/include/ \
			-I../libui/include/ -I../libmjollnir/include/ 		 \
			-I../libaspect/include -I../librevm/include/		 \
			-I../libedfmt/include/ -I../liballocproxy/include/       \
			-DELFSH_INTERN -DE2DBG_INTERN $(ASMOPT) -DELFSH64	 \
			-DE2DBG_PRESENT

LDFLAGS32		+= $(DLOPT) $(DYNOPT) $(EXTRAOPT)
LDFLAGS64		+= $(DLOPT) $(DYNOPT) $(EXTRAOPT)

EXTRAOBJ32		= ../libasm/libasm.o ../libmjollnir/libmjollnir32.o     \
			../libelfsh/libelfsh32.o ../libedfmt/libedfmt32.o       \
			../liballocproxy/liballocproxy.o ../librevm/revm-*.32.o	\
			../libaspect/libaspect32.o $(LDDUMPOBJ)

EXTRAOBJ64		= ../libasm/libasm.o ../libmjollnir/libmjollnir64.o     \
			../libelfsh/libelfsh64.o ../libedfmt/libedfmt64.o       \
			../liballocproxy/liballocproxy.o ../librevm/revm-*.64.o	\
			../libaspect/libaspect64.o $(LDDUMPOBJ)

all			: all32 all64

all32			: ${OBJ32}
			@$(CC) -shared $(OBJ32) $(EXTRAOBJ32) $(LDFLAGS32) \
			-o $(DBGNAME32).so
			@$(AR) $(DBGNAME32).a $(OBJ32)
			@$(RANLIB) $(DBGNAME32).a
			@$(LD) -r $(OBJ32) $(EXTRAOBJ32) -o $(DBGNAME32O)


all64			: ${OBJ64}
			@$(CC) -shared $(OBJ64) $(EXTRAOBJ64) $(LDFLAGS64) \
			-o $(DBGNAME64).so
			@$(LD) -r $(OBJ64) $(EXTRAOBJ64) -o $(DBGNAME64O)
			@$(AR) $(DBGNAME64).a $(OBJ64)
			@$(RANLIB) $(DBGNAME64).a

# This target is work in progress (static files debugger)
et_rel			: ${OBJ}
			$(CC) -c $(CFLAGS) $(SRC_DYN) -o $(OBJ_DYN)
			$(LD) ${OBJ_DYN} ${OBJ} ../libasm/libasm.a	\
			../libelfsh/libelfsh.a ../liballocproxy.a	\
			/usr/lib/libdl.a /usr/lib/libc.a -r             \
			-o $(DBGNAME)_rel.o	  

clean		       :
		       @$(RM) $(OBJ) $(OBJ32) $(OBJ64) *~ \#* *\# .\#*
		       @$(RM) include/*~ include/\#* include/*\# include/.\#*

fclean		       : clean
		       @$(RM) $(NAME32) $(OBJ_DYN32) $(DBGNAME32).so
		       @$(RM) $(NAME64) $(OBJ_DYN64) $(DBGNAME64).so
		       @$(RM) $(DBGNAME32).a $(DBGNAME64).a

%.32.o : %.c
			$(CC) $(CFLAGS32) -c -o $@ $<
%.64.o : %.c
			$(CC) $(CFLAGS64) -c -o $@ $<


