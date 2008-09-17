##
## Makefile for lang in libstderesi for the ERESI project
##
## Started on  Fri Nov  2 16:32:29 2001 jfv
## $Id
##
include ../../config.h

SRC			= atomic.c foreach.c jmp.c return.c vlist.c
OBJ32			= $(SRC:.c=.32.o)
OBJ64			= $(SRC:.c=.64.o)
CC			?= gcc
LD			?= ld
RM			= rm -f
NAME32			= ../std-lang.32.o
NAME64			= ../std-lang.64.o

CFLAGS32		+= -fPIC -g3 -Wall -I../include $(DYNOPT)                  \
			 -I../../libelfsh/include/ \
			 -I../../libdump/include/ -I../../libasm/include/          \
			 -I/usr/pkg/include/  -I../../libui/include/ $(BUILDOP)    \
			 -I../../libmjollnir/include/ -I../../libaspect/include/   \
			 -I../../libedfmt/include/  $(STATOPT2) 	   \
			-I../../libgdbwrap/include/	\
			 -I../../libetrace/include -I../../librevm/include/	   \
			$(ASMOPT) -DERESI32 -I../../e2dbg/include/ $(NCURSESLNOPT) \
			$(READLNOPT) $(BITS)

CFLAGS64		+= -fPIC -g3 -Wall -I../include $(DYNOPT) 	          \
			 -I../../libelfsh/include/ -I../../elibc/include/	  \
			 -I../../libdump/include/ -I../../libasm/include/         \
			 -I/usr/pkg/include/  -I../../libaspect/include/          \
			 -I../../libui/include/ -I../../libmjollnir/include/      \
			 -I../../libedfmt/include/  $(STATOPT2) 	  \
			-I../../libgdbwrap/include/ 				     \
			-I../../libetrace/include -I../../librevm/include/       \
			$(ASMOPT) -DERESI64 $(BUILDOP) -I../../e2dbg/include/ 	  \
			$(NCURSESLNOPT) $(READLNOPT) $(BITS)

all			: all32 all64

$(NAME32)		: $(OBJ32)
			$(LD) -r $(OBJ32) -o $(NAME32)

$(NAME64)		: $(OBJ64)
			$(LD) -r $(OBJ64) -o $(NAME64)
all32: $(NAME32)
all64: $(NAME64)

clean		        :
		       @$(RM) $(OBJ) $(OBJ32) $(OBJ64) *~ \#* *\# .\#*

%.32.o : %.c
			$(CC) $(CFLAGS32) -c -o $@ $<
%.64.o : %.c
			$(CC) $(CFLAGS64) -c -o $@ $<
