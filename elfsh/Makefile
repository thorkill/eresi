##
## Makefile for elfsh
##
## Started on  Fri Nov  2 16:32:29 2001 mayhem
##

include ../config.h

SRC			= grammar.c disasm.c dyn.c hdr.c main.c got.c interp.c \
			  notes.c pht.c rel.c sht.c stab.c sym.c quit.c data.c \
			  ctors.c dtors.c fileops.c comment.c save.c load.c \
			  unload.c list.c doswitch.c tables.c help.c info.c \
			  readln.c print.c misc.c atomic.c opt.c lookup.c \
			  implicit.c exec.c modules.c findrel.c objects.c \
			  cmdapi.c filter.c reladd.c fcthijack.c loop.c jmp.c \
			  insert.c remove.c flush.c lexer.c io.c network.c \
		 	  vlist.c lts.c source.c cleanup.c e2dbg.c job.c \
			  dumpregs.c trace.c
OBJ			= $(SRC:.c=.o)

SRC_DYN			= elfsh_etdyn.c 
OBJ_DYN			= $(SRC_DYN:.c=.o)

SRC_STEP		= step.c
OBJ_STEP		= $(SRC_STEP:.c=.o)



CC			= gcc
RM			= rm -f
NAME			= elfsh
DBGNAME			= e2dbg

CFLAGS			= -fPIC -g3 -Wall -Iinclude $(DYNOPT) $(BUILDOP) \
			-I../libhash/include/ -I../libelfsh/include/ \
			-I../libdump/include/ -I../libasm/include/ \
			-DELFSH_INTERN $(STATOPT2) $(ASMOPT) $(READLNOPT) \
			$(BITS)

LDFLAGS			= -L../libelfsh/ -lelfsh -L../libhash/ -lhashelfsh \
			  -L../libdump/ -L/usr/pkg/lib/ -L../libasm/ -lasm \
			-g3 $(LDASMOPT) $(LDREADLNOPT) $(DLOPT) $(DYNOPT) \
			$(BITS) $(EXTRAOPT) $(LDDUMPOPT)

all		       	: $(OBJ) $(OBJ_STEP)
			$(CC) $(OBJ) $(OBJ_STEP) -o $(NAME) $(LDFLAGS) $(STATOPT)

et_rel			: ${OBJ} ${OBJ_DYN}
			echo $(OBJ)
			echo $(OBJ_DYN)
			$(LD) ${OBJ_DYN} ${OBJ}		   \
			../libasm/libasm.a				   \
			../libelfsh/libelfsh.a ../libhash/libhashelfsh.a   \
	                /usr/lib/libreadline.a             \
			/usr/lib/libdl.a /usr/lib/libtermcap.a /usr/lib/libhistory.a   \
			/usr/lib/libc.a -r -o $(DBGNAME)_rel.o	  

et_dyn			: ${OBJ} ${OBJ_DYN}
			echo $(OBJ)
			echo $(OBJ_DYN)
			$(CC) ${OBJ_DYN} ${OBJ} -o $(DBGNAME).so $(LDFLAGS) -shared

clean		       :
		       @$(RM) $(OBJ) $(OBJ_STEP) *~ \#* *\# .\#*
		       @$(RM) include/*~ include/\#* include/*\# include/.\#*

fclean		       : clean
		       @$(RM) $(NAME) $(OBJ_DYN) $(DBGNAME).so


