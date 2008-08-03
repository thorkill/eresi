$##
## Makefile for libedfmt
##
## Started on  Tue Dec  26 11:05:39 2006 mxatone
##
include ../config.h

CC			?= gcc
LD			?= ld
RM			= rm -f 
AR			= ar rc

SRC			= edfmt.c edfmt-utils.c dwarf2.c dwarf2-utils.c dwarf2-abbrev.c \
			  dwarf2-cfa.c dwarf2-trans.c stabs.c stabs-trans.c stabs-utils.c api.c
OBJ			= $(SRC:.c=.o)
OBJ32	 		= ${SRC:.c=.32.o}
OBJ64	 		= ${SRC:.c=.64.o}
NAME32			= libedfmt32
NAME64			= libedfmt64

CFLAGS32		+= -Wall -fPIC -g3 -Iinclude -I ../libaspect/include/ \
			-I../libelfsh/include/  -DERESI32

CFLAGS64		+= -Wall -fPIC -g3 -Iinclude -I ../libaspect/include/ \
			-I../libelfsh/include/  -DERESI64

LDFLAGS32		+= -L../libaspect/ -laspect32 -L../libelfsh/ -lelfsh32 
#			-L../liballocproxy -lallocproxy

LDFLAGS64		+= -L../libaspect/ -laspect64 -L../libelfsh/ -lelfsh64 \
			-L../liballocproxy/ -lallocproxy

all			: all32 all64

libedfmt32.so			: $(OBJ32)
			@$(CC) $(LDFLAGS32) -shared $(OBJ32) -o $(NAME32).so
			@$(AR) $(NAME32).a $(OBJ32)
			@$(RANLIB) $(NAME32).a
			@$(LD) -r $(OBJ32) -o $(NAME32).o
all32: libedfmt32.so

libedfmt64.so		: $(OBJ64)
			@$(CC) $(LDFLAGS64) -shared $(OBJ64) -o $(NAME64).so
			@$(AR) $(NAME64).a $(OBJ64)
			@$(RANLIB) $(NAME64).a
			@$(LD) -r $(OBJ64) -o $(NAME64).o

all64: libedfmt64.so

clean			: 
			@$(RM) \#* *\# *~ *.o .\#* include/\#* include/*\#  \
			include/*~ include/.\#*

docs:
			doxygen doc/doxygen.conf

fclean			: clean
			@$(RM) $(NAME).so $(NAME).a \
			$(NAME32).so $(NAME32).a \
			$(NAME64).so $(NAME64).a

%.32.o : %.c
			$(CC) $(CFLAGS32) -c -o $@ $<
%.64.o : %.c
			$(CC) $(CFLAGS64) -c -o $@ $<
