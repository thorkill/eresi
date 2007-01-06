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
			  reladd.c job.c flow.c inspect.c flowjack.c graph.c \
			  configure.c
OBJ			= $(SRC:.c=.o)
OBJ32			= $(SRC:.c=.32.o)
OBJ64			= $(SRC:.c=.64.o)
CC			?= gcc
RM			= rm -f
NAME			= elfsh
NAME32			= elfsh32
NAME64			= elfsh64

CFLAGS32		+= -fPIC -g3 -gstabs+ -Wall -Iinclude $(DYNOPT) $(BUILDOP)\
			 -I../libelfsh/include/ -I../libc/include/ \
			 -I../libdump/include/ -I../libasm/include/ \
			 -I../e2dbg/include/ -I/usr/pkg/include/  \
			-I../libui/include/ -I../libmjollnir/include/ \
			-I../libaspect/include/ -I../librevm/include/ \
			-I../libedfmt/include/ -DELFSH_INTERN $(STATOPT2) \
			$(ASMOPT) -DELFSH32 $(NCURSESLNOPT) $(READLNOPT)

CFLAGS64		+= -fPIC -g3 -gstabs+ -Wall -Iinclude $(DYNOPT) $(BUILDOP)\
			 -I../libelfsh/include/ -I../libc/include/ \
			 -I../libdump/include/ -I../libasm/include/ \
			 -I../e2dbg/include/ -I/usr/pkg/include/  \
			-I../libui/include/ -I../libmjollnir/include/ \
			-I../libaspect/include/ -I../librevm/include/ \
			-I../libedfmt/include/ -DELFSH_INTERN $(STATOPT2) \
			$(ASMOPT) -DELFSH64 $(NCURSESLNOPT) $(READLNOPT)

LDFLAGS32		+= -L../libedfmt/ -L../libelfsh/ -ledfmt32 -lelfsh32 -L../libdump/ -L/usr/pkg/lib/ \
			-L../libmjollnir/ -lmjollnir32 $(LPTHREAD) -L../libc 	  \
			-lcelfsh32 -L../libasm/ -lasm -L../libui/ -lui32 -g3 	  \
			$(LDASMOPT) -L../libmalloc -L../libmjollnir/ 		  \
			-L../libaspect/ -laspect32 -L../librevm/        \
		        -lmalloc-e2dbg  $(DLOPT)       \
			-L/usr/freeware/lib32/ $(DYNOPT) $(BITS) $(EXTRAOPT) 	  \
			$(LDDUMPOPT) $(LDNCURSESLNOPT) $(LDREADLNOPT)

LDFLAGS64		+= -L../libelfsh/ -lelfsh64 -L../libdump/ -L/usr/pkg/lib/ \
			-L../libmjollnir/ -lmjollnir64 $(LPTHREAD) -L../libc 	  \
			-lcelfsh64 -L../libasm/ -lasm -L../libui/ -lui64 -g3 	  \
			$(LDASMOPT) -L../libmalloc -L../libmjollnir/ 		  \
			-L../libaspect/ -laspect64 -L../librevm/ -ledfmt64	  \
			-lmalloc-e2dbg -L../libedfmt/ -ledfmt32 $(DLOPT)       \
			-L/usr/freeware/lib32/ $(DYNOPT) $(BITS) $(EXTRAOPT) 	  \
			$(LDDUMPOPT) $(LDNCURSESLNOPT) $(LDREADLNOPT)

all			: all32 all64

all32			: $(OBJ32)
			$(CC) $(OBJ32) -o $(NAME32) $(LDFLAGS32) $(STATOPT)
			@$(RM) rlheap.32.o # make sure we dont link this file into e2dbg

all64			: $(OBJ64)
			$(CC) $(OBJ64) -o $(NAME64) $(LDFLAGS64) $(STATOPT)
			@$(RM) rlheap.64.o # make sure we dont link this file into e2dbg

clean		       :
		       @$(RM) $(OBJ) *~ \#* *\# .\#* *.32.o *.64.o
		       @$(RM) include/*~ include/\#* include/*\# include/.\#*

fclean		       : clean
		       @$(RM) $(NAME) $(NAME32) $(NAME64) $(OBJ_DYN)

%.32.o : %.c
			$(CC) $(CFLAGS32) -c -o $@ $<
%.64.o : %.c
			$(CC) $(CFLAGS64) -c -o $@ $<
