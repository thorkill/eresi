##
## Makefile for vm in librevm for the ELF shell project
##
## Started on  Fri Nov  2 16:32:29 2001 jfv
##
include ../../config.h

SRC			= cat.c flow.c graph.c help.c insert.c list.c prefixes.c \
			  profile.c reladd.c save.c source.c unload.c vlist.c	 \
			  fileops.c flowjack.c hammer.c info.c inspect.c load.c  \
			  net.c print.c quit.c remove.c script.c trace.c utils.c \
			  vectors.c types.c hashes.c inform.c debug.c foreach.c  \
			  atomic.c fcthijack.c jmp.c configure.c color.c log.c   \
			  modules.c exec.c switch.c job.c match.c declare.c      \
			  lists.c reflect.c define.c init.c
OBJ32			= $(SRC:.c=.32.o)
OBJ64			= $(SRC:.c=.64.o)
CC			?= gcc
LD			?= ld
RM			= rm -f
NAME32			= revm-commands.32.o
NAME64			= revm-commands.64.o

CFLAGS32		+= -fPIC -g3 -Wall -I../include $(DYNOPT)                  \
			 -I../../libelfsh/include/ -I../../elibc/include/          \
			 -I../../libdump/include/ -I../../libasm/include/          \
			 -I/usr/pkg/include/  -I../../libui/include/ $(BUILDOP)    \
			 -I../../libmjollnir/include/ -I../../libaspect/include/   \
			 -I../../libedfmt/include/ -DELFSH_INTERN $(STATOPT2) 	   \
			 -I../../libetrace/include -I../../librevm/include/	   \
			$(ASMOPT) -DERESI32 -I../../e2dbg/include/ $(NCURSESLNOPT) \
			$(READLNOPT)

CFLAGS64		+= -fPIC -g3 -Wall -I../include $(DYNOPT) 	          \
			 -I../../libelfsh/include/ -I../../elibc/include/	  \
			 -I../../libdump/include/ -I../../libasm/include/         \
			 -I/usr/pkg/include/  -I../../libaspect/include/          \
			 -I../../libui/include/ -I../../libmjollnir/include/      \
			 -I../../libedfmt/include/ -DELFSH_INTERN $(STATOPT2) 	  \
			 -I../../libetrace/include -I../../librevm/include/       \
			$(ASMOPT) -DERESI64 $(BUILDOP) -I../../e2dbg/include/ 	  \
			$(NCURSESLNOPT) $(READLNOPT)

all			: all32 all64

all32			: $(OBJ32)
			$(LD) -r $(OBJ32) -o ../$(NAME32)

all64			: $(OBJ64)
			$(LD) -r $(OBJ64) -o ../$(NAME64)

clean		        :
		       @$(RM) $(OBJ) $(OBJ32) $(OBJ64) *~ \#* *\# .\#*

%.32.o : %.c
			$(CC) $(CFLAGS32) -c -o $@ $<
%.64.o : %.c
			$(CC) $(CFLAGS64) -c -o $@ $<
