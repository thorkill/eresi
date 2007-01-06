##
## Makefile for lang in librevm for the ELF shell project
##
## Started on  Fri Nov  2 16:32:29 2001 mayhem
##
include ../../config.h

SRC			= grammar.c lookup.c lts.c objects.c variables.c
OBJ			= $(SRC:.c=.o)
OBJ32			= $(SRC:.c=.32.o)
OBJ64			= $(SRC:.c=.64.o)
CC			?= gcc
RM			= rm -f

CFLAGS			+= -fPIC -g3 -gstabs+ -Wall -I../include $(DYNOPT)         \
			 -I../../e2dbg/include/ -I../../libelfsh/include/ 	   \
			 -I../../libdump/include/ -I../../libasm/include/ 	   \
			 -I/usr/pkg/include/ -I../../libui/include/ $(BUILDOP)     \
			-I../../libmjollnir/include/ -I../../libaspect/include/    \
			-DELFSH_INTERN $(STATOPT2) $(ASMOPT) $(BITS) 		   \
			-I../../e2dbg/include/ $(NCURSESLNOPT) $(READLNOPT)

CFLAGS32		+= -fPIC -g3 -gstabs+ -Wall -I../include $(DYNOPT)        \
			 -I../../libelfsh/include/ -I../../libc/include/          \
			 -I../../libdump/include/ -I../../libasm/include/         \
			 -I/usr/pkg/include/  -I../../libui/include/ $(BUILDOP)   \
			 -I../../libmjollnir/include/ -I../../libaspect/include/  \
			-DELFSH_INTERN $(STATOPT2) $(ASMOPT) -DELFSH32 		  \
			-I../../e2dbg/include/ $(NCURSESLNOPT) $(READLNOPT)

CFLAGS64		+= -fPIC -g3 -gstabs+ -Wall -I../include $(DYNOPT) 	  \
			 -I../../libelfsh/include/ -I../../libc/include		  \
			 -I../../libdump/include/ -I../../libasm/include/         \
			 -I/usr/pkg/include/  -I../../libaspect/include/          \
			 -I../../libui/include/ -I../../libmjollnir/include/      \
			-DELFSH_INTERN $(STATOPT2) $(ASMOPT) -DELFSH64 $(BUILDOP) \
			-I../../e2dbg/include/ $(NCURSESLNOPT) $(READLNOPT)

all2			: all32 all64
all		       	: $(OBJ)
all32			: $(OBJ32)
all64			: $(OBJ64)
clean		        :
		       @$(RM) $(OBJ) $(OBJ32) $(OBJ64) *~ \#* *\# .\#*

%.32.o : %.c
			$(CC) $(CFLAGS32) -c -o $@ $<
%.64.o : %.c
			$(CC) $(CFLAGS64) -c -o $@ $<
