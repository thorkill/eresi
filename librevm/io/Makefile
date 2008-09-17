##
## Makefile for vm in librevm for the ELF shell project
##
## Started on  Fri Nov  2 16:32:29 2001 jfv
##
include ../../config.h

SRC		= color.c curjob.c input.c io.c log.c network.c output.c \
		prompt.c select.c readln.c
OBJ32		= $(SRC:.c=.32.o)
OBJ64		= $(SRC:.c=.64.o)
CC		?= gcc
LD		?= ld
RM		= rm -f
NAME32		= ../revm-io.32.o
NAME64		= ../revm-io.64.o

CFLAGS32	+= -fPIC -g3 -Wall -I../include $(DYNOPT)           \
		-I../../libelfsh/include/ -I../../libedfmt/include/          \
		-I../../libui/include/ -I../../libasm/include/               \
		-I../../libmjollnir/include/ -I../../libaspect/include/      \
		-I../../libgdbwrap/include/ 				     \
		 -I../../libdump/include/ $(BUILDOP) $(EXTRACFLAGS)          \
		$(STATOPT2) $(ASMOPT) $(BITS) -DERESI32 $(READLNOPT)			

CFLAGS64	+= -fPIC -g3 -Wall -I../include -Iinclude $(DYNOPT) \
		-I../../libelfsh/include/ -I../../libedfmt/include/          \
		-I../../libui/include/ -I../../libasm/include/               \
		-I../../libgdbwrap/include/ 				     \
		-I../../libmjollnir/include/ -I../../libaspect/include/      \
		 -I../../libdump/include/  $(STATOPT2) $(EXTRACFLAGS)        \
		$(ASMOPT) $(BITS) -DERESI64 $(BUILDOP) $(READLNOPT)

all		: all32 all64

$(NAME32)		: $(OBJ32)
		$(LD) -r $(OBJ32) -o $(NAME32)

$(NAME64)		: $(OBJ64)
		$(LD) -r $(OBJ64) -o $(NAME64)

all32:	$(NAME32)
all64:	$(NAME64)

clean		:
		@$(RM) $(OBJ) $(OBJ32) $(OBJ64) *~ \#* *\# .\#*

%.32.o 		: %.c
		$(CC) $(CFLAGS32) -c -o $@ $<

%.64.o 		: %.c
		$(CC) $(CFLAGS64) -c -o $@ $<
