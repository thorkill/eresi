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
		 	  vlist.c lts.c source.c cleanup.c job.c run.c trace.c \
			  color.c log.c variables.c version.c

OBJ			= $(SRC:.c=.o)
CC			?= gcc
RM			= rm -f
NAME			= elfsh

CFLAGS			+= -fPIC -g3 -gstabs+ -Wall -Iinclude $(DYNOPT) $(BUILDOP) \
			 -I../libelfsh/include/ -I../libdump/include/   \
			 -I../libasm/include/  -I../e2dbg/include/         \
			-DELFSH_INTERN $(STATOPT2) $(ASMOPT) $(READLNOPT) \
			$(BITS)

LDFLAGS			+= -L../libelfsh/ -lelfsh -L../libdump/ -L/usr/pkg/lib/ \
			-L../libasm/ -lasm -g3 $(LDASMOPT) $(LDREADLNOPT) $(DLOPT)  \
			-L/usr/freeware/lib32/ $(DYNOPT) $(BITS) $(EXTRAOPT) \
			$(LDDUMPOPT)

all		       	: $(OBJ)
			$(CC) $(OBJ) -o $(NAME) $(LDFLAGS) $(STATOPT)

clean		       :
		       @$(RM) $(OBJ) *~ \#* *\# .\#*
		       @$(RM) include/*~ include/\#* include/*\# include/.\#*

fclean		       : clean
		       @$(RM) $(NAME) $(OBJ_DYN)
