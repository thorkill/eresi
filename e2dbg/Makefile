##
## Makefile for elfsh
##
## Started on  Tue Aug  16 06:32:29 2005 mayhem
##

include ../config.h

SRC			= e2dbg.c backtrace.c breakpoints.c delete.c display.c \
			signal.c mode.c stack.c step.c linkmap.c dumpregs.c    \
			vmhooks.c mutex.c continue.c xmalloc.c resolv.c        \
			rlheapdbg.c threads.c entry.c e2dbg-misc.c
OBJ			= $(SRC:.c=.o)
OBJ32			= $(SRC:.c=.32.o)
OBJ64			= $(SRC:.c=.64.o)
CC			?= gcc
RM			= rm -f
DBGNAME			= e2dbg
DBGNAME32		= e2dbg32
DBGNAME64		= e2dbg64

CFLAGS32		+= -fPIC -g3 -Wall -Iinclude $(DYNOPT) $(BUILDOP) 	 \
			-I../libelfsh/include/ -I../libdump/include/      	 \
			-I../libasm/include/ -I../vm/include/ -I../elibc/include/ \
			-I../libui/include/ -I../libmjollnir/include/ 		 \
			-I../libaspect/include -I../librevm/include/		 \
			-DELFSH_INTERN -DE2DBG_INTERN $(ASMOPT) $(READLNOPT)     \
			-DELFSH32

CFLAGS64		+= -fPIC -g3 -Wall -Iinclude $(DYNOPT) $(BUILDOP) 	 \
			-I../libelfsh/include/ -I../libdump/include/      	 \
			-I../libasm/include/ -I../vm/include/ -I../elibc/include/ \
			-I../libui/include/ -I../libmjollnir/include/ 		 \
			-I../libaspect/include -I../librevm/include/		 \
			-DELFSH_INTERN -DE2DBG_INTERN $(ASMOPT) $(READLNOPT)     \
			-DELFSH64

LDFLAGS32		+= -L../liballocproxy -lallocproxy -L../libelfsh/ -lelfsh32 \
			-L../libdump/ -L/usr/pkg/lib/ -L../libasm/ -lasm            \
			-L../libaspect/ -laspect32 -L../librevm/ -lrevm32           \
			-L../libmjollnir -lmjollnir32 -g3 -L../elibc/ -lcelfsh32    \
			-L../libui/ -lui32 $(LDASMOPT) $(LDREADLNOPT) $(DLOPT)      \
			-L/usr/freeware/lib32/ $(DYNOPT) -DELFSH32 $(EXTRAOPT)      \
			$(LDDUMPOPT) $(LPTHREAD) $(LDMJOLLNIR)

LDFLAGS64		+= -L../liballocproxy -lallocproxy -L../libelfsh/ -lelfsh64 \
			-L../libdump/ -L/usr/pkg/lib/ -L../libasm/ -lasm            \
			-L../libaspect/ -laspect64 -L../librevm/ -lrevm64           \
			-L../libmjollnir -lmjollnir64 -g3 -L../elibc/ -lcelfsh64    \
			-L../libui/ -lui64 $(LDASMOPT) $(LDREADLNOPT) $(DLOPT)      \
			-L/usr/freeware/lib32/ $(DYNOPT) -DELFSH64 $(EXTRAOPT)      \
			$(LDDUMPOPT) $(LPTHREAD) $(LDMJOLLNIR)

all			: all32 all64

all32			: ${OBJ32}
			$(CC) -shared $(OBJ32) $(LDFLAGS32) -o $(DBGNAME32).so

all64			: ${OBJ64}
			$(CC) -shared $(OBJ64) $(LDFLAGS64) -o $(DBGNAME64).so

# This target is work in progress (static files debugger)
et_rel			: ${OBJ}
			$(CC) -c $(CFLAGS) $(SRC_DYN) -o $(OBJ_DYN)
			$(LD) ${OBJ_DYN} ${OBJ} ../libasm/libasm.a	\
			../libelfsh/libelfsh.a ../libmalloc-e2dbg.a	\
			/usr/lib/libreadline.a /usr/lib/libdl.a 	\
			/usr/lib/libtermcap.a /usr/lib/libhistory.a   	\
			/usr/lib/libc.a -r -o $(DBGNAME)_rel.o	  

clean		       :
		       @$(RM) $(OBJ) $(OBJ32) $(OBJ64) *~ \#* *\# .\#*
		       @$(RM) include/*~ include/\#* include/*\# include/.\#*

fclean		       : clean
		       @$(RM) $(NAME) $(OBJ_DYN) $(DBGNAME).so
		       @$(RM) $(NAME32) $(OBJ_DYN32) $(DBGNAME32).so
		       @$(RM) $(NAME64) $(OBJ_DYN64) $(DBGNAME64).so

%.32.o : %.c
			$(CC) $(CFLAGS32) -c -o $@ $<
%.64.o : %.c
			$(CC) $(CFLAGS64) -c -o $@ $<


