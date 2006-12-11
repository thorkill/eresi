##
## Makefile for elfsh
##
## Started on  Fri Nov  2 16:32:29 2001 mayhem
##

include ../config.h

SRC			= grammar.c disasm.c dyn.c hdr.c main.c got.c interp.c \
			  notes.c pht.c rel.c sht.c stab.c sym.c quit.c data.c \
			  ctors.c dtors.c fileops.c comment.c save.c load.c \
			  tables.c help.c info.c print.c atomic.c opt.c lookup.c \
			  log.c exec.c modules.c findrel.c objects.c rlheap.c \
			  cmdapi.c fcthijack.c loop.c jmp.c color.c hammer.c \
			  insert.c remove.c flush.c vlist.c lts.c source.c \
			  cleanup.c run.c trace.c variables.c version.c hash.c \
			  unload.c list.c doswitch.c misc.c implicit.c deps.c \
			  reladd.c job.c flow.c inspect.c flowjack.c graph.c
OBJ			= $(SRC:.c=.o)
CC			?= gcc
RM			= rm -f
NAME			= elfsh

CFLAGS			+= -fPIC -g3 -gstabs+ -Wall -Iinclude $(DYNOPT) $(BUILDOP) \
			 -I../libelfsh/include/ -I../libc/include/ -I../libdump/include/ \
			 -I../libasm/include/  -I../e2dbg/include/ -I /usr/pkg/include/  \
			 -I../libui/include/ -I../libmjollnir/include/ \
			-DELFSH_INTERN $(STATOPT2) $(ASMOPT) $(BITS) $(NCURSESLNOPT) $(READLNOPT)

LDFLAGS			+= -L../libelfsh/ -lelfsh -L../libdump/ -L/usr/pkg/lib/ -L../libmjollnir/\
			-lmjollnir $(LPTHREAD) -L../libc -lcelfsh -L../libasm/ -lasm -L../libui/ -lui \
			-g3 $(LDASMOPT) -L../libmalloc -L../libmjollnir/ -lmalloc-e2dbg $(LDMJOLLNIR)\
			$(DLOPT)  -L/usr/freeware/lib32/ $(DYNOPT) $(BITS) $(EXTRAOPT) \
			$(LDDUMPOPT) $(LDNCURSESLNOPT) $(LDREADLNOPT)


all		       	: $(OBJ)
			$(CC) $(OBJ) -o $(NAME) $(LDFLAGS) $(STATOPT)
			@$(RM) rlheap.o # make sure we dont link this file into e2dbg

clean		       :
		       @$(RM) $(OBJ) *~ \#* *\# .\#*
		       @$(RM) include/*~ include/\#* include/*\# include/.\#*

fclean		       : clean
		       @$(RM) $(NAME) $(OBJ_DYN)
