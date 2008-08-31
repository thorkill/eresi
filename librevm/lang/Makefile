##
## Makefile for lang in librevm for the ELF shell project
##
## Started on  Fri Nov  2 16:32:29 2001 jfv
##
include ../../config.h

SRC		= access.c convert.c grammar.c lookup.c objects.c \
		options.c parser.c scanner.c variables.c expressions.c
OBJ32		= $(SRC:.c=.32.o)
OBJ64		= $(SRC:.c=.64.o)
CC		?= gcc
LD		?= ld
RM		= rm -f
NAME32		= ../revm-lang.32.o
NAME64		= ../revm-lang.64.o

CFLAGS32	+= -fPIC -g3 -Wall -I../include $(DYNOPT)        \
		 -I../../libelfsh/include/ \
		 -I../../libdump/include/ -I../../libasm/include/         \
		 -I/usr/pkg/include/  -I../../libui/include/ $(BUILDOP)   \
		 -I../../libmjollnir/include/ -I../../libaspect/include/  \
		 $(KERNSHINCINC) $(KERNSH)			          \
		 -I../../libedfmt/include/				  \
		 $(STATOPT2) $(ASMOPT) -DERESI32 $(EXTRACFLAGS)		  \
		-I../../e2dbg/include/ $(NCURSESLNOPT) $(READLNOPT)

CFLAGS64	+= -fPIC -g3 -Wall -I../include $(DYNOPT) 	  \
		 -I../../libelfsh/include/ \
		 -I../../libdump/include/ -I../../libasm/include/         \
		 -I/usr/pkg/include/  -I../../libaspect/include/          \
		 -I../../libui/include/ -I../../libmjollnir/include/      \
		 $(KERNSHINCINC) $(KERNSH) $(EXTRACFLAGS)		  \
		 -I../../libedfmt/include/				  \
		 $(STATOPT2) $(ASMOPT) -DERESI64 $(BUILDOP)		  \
		-I../../e2dbg/include/ $(NCURSESLNOPT) $(READLNOPT)

all		: all32 all64

$(NAME32)		: $(OBJ32)
		$(LD) -r $(OBJ32) -o $(NAME32)

all32: $(NAME32)

$(NAME64)		: $(OBJ64)
		$(LD) -r $(OBJ64) -o $(NAME64)

all64:	$(NAME64)

clean		:
		@$(RM) $(OBJ) $(OBJ32) $(OBJ64) *~ \#* *\# .\#*

%.32.o 		: %.c
		$(CC) $(CFLAGS32) -c -o $@ $<

%.64.o 		: %.c
		$(CC) $(CFLAGS64) -c -o $@ $<
