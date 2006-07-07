##
## Makefile for elfsh
##
## Started on  Tue Aug  16 06:32:29 2005 mayhem
##

include ../config.h

SRC			= e2dbg.c backtrace.c breakpoints.c delete.c display.c      \
			signal.c mode.c stack.c step.c linkmap.c dumpregs.c         \
			vmhooks.c mutex.c continue.c xmalloc.c resolv.c rlheapdbg.c \
			threads.c
OBJ			= $(SRC:.c=.o)
VMOBJ			= ../vm/*.o
CC			?= gcc
RM			= rm -f
DBGNAME			= e2dbg

CFLAGS			+= -fPIC -g3 -Wall -Iinclude $(DYNOPT) $(BUILDOP) \
			-I../libelfsh/include/ \
			-I../libdump/include/  \
			-I../libasm/include/   \
			-I../vm/include/       \
			-I../libc/include/     \
			-I../libui/include/    \
			-DELFSH_INTERN -DE2DBG_INTERN $(ASMOPT) $(READLNOPT) $(BITS)

LDFLAGS			+= -L../libmalloc -lmalloc-e2dbg -L../libelfsh/ -lelfsh                       \
			-L../libdump/ -L/usr/pkg/lib/ -L../libasm/ -lasm $(LPTHREAD)                  \
			-g3 -L../libc/ -lcelfsh -L../libui/ -lui $(LDASMOPT) $(LDREADLNOPT) $(DLOPT)  \
			-L/usr/freeware/lib32/ $(DYNOPT) $(BITS) $(EXTRAOPT) $(LDDUMPOPT)

all                     : ${OBJ}
			$(CC) $(OBJ) $(VMOBJ) $(LDFLAGS) -o $(DBGNAME).so -shared 

# This target is work in progress (static files debugger)
et_rel			: ${OBJ}
			$(CC) -c $(CFLAGS) $(SRC_DYN) -o $(OBJ_DYN)
			$(LD) ${OBJ_DYN} ${OBJ}		   		               \
			../libasm/libasm.a				               \
			../libelfsh/libelfsh.a                                         \
			../libmalloc-e2dbg.a					       \
			/usr/lib/libreadline.a                                         \
			/usr/lib/libdl.a /usr/lib/libtermcap.a /usr/lib/libhistory.a   \
			/usr/lib/libc.a -r -o $(DBGNAME)_rel.o	  

clean		       :
		       @$(RM) $(OBJ) *~ \#* *\# .\#*
		       @$(RM) include/*~ include/\#* include/*\# include/.\#*

fclean		       : clean
		       @$(RM) $(NAME) $(OBJ_DYN) $(DBGNAME).so


