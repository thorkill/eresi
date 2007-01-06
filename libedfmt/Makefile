##
## Makefile for libedfmt
##
## Started on  Tue Dec  26 11:05:39 2006 mxatone
##
include ../config.h

CC			?= gcc
LD			= gcc
RM			= rm -f 
AR			= ar rc

SRC			= edfmt.c dwarf2.c

OBJ			= $(SRC:.c=.o)
OBJ32	 		= ${SRC:.c=.32.o}
OBJ64	 		= ${SRC:.c=.64.o}
NAME32			= libedfmt32
NAME64			= libedfmt64

CFLAGS32		+= -Wall -fPIC -g3 -O2 -Iinclude -I ../libaspect/include/ \
			-I../libelfsh/include/ -I../libc/include/ -DELFSH_INTERN -DELFSH32

CFLAGS64		+= -Wall -fPIC -g3 -O2 -Iinclude -I ../libaspect/include/ \
			-I../libelfsh/include/ -I../libc/include/ -DELFSH_INTERN -DELFSH64

LDFLAGS32		+= -L../libaspect/ -laspect32 -L../libelfsh/ -lelfsh32 \
			-L../libc/ -lcelfsh32 -L../libmalloc/ -lmalloc-e2dbg

LDFLAGS64		+= -L../libaspect/ -laspect64 -L../libelfsh/ -lelfsh64 \
			-L../libc/ -lcelfsh64 -L../libmalloc/ -lmalloc-e2dbg

all			: all32 all64

all32			: $(OBJ32)
			@$(LD) $(LDFLAGS32) -shared $(OBJ32) -o $(NAME32).so
			@$(AR) $(NAME32).a $(OBJ32)
			@$(RANLIB) $(NAME32).a

all64			: $(OBJ64)
			@$(LD) $(LDFLAGS64) -shared $(OBJ64) -o $(NAME64).so
			@$(AR) $(NAME64).a $(OBJ64)
			@$(RANLIB) $(NAME64).a

clean			: 
			@$(RM) \#* *\# *~ *.o .\#* include/\#* include/*\#  \
			include/*~ include/.\#*

fclean			: clean
			@$(RM) $(NAME).so $(NAME).a \
			$(NAME32).so $(NAME32).a \
			$(NAME64).so $(NAME64).a

%.32.o : %.c
			$(CC) $(CFLAGS32) -c -o $@ $<
%.64.o : %.c
			$(CC) $(CFLAGS64) -c -o $@ $<
