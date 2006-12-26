##
## Makefile.am for libaspect in elfsh
## 
## Started on  Fri Dec 22 16:32:29 2006 mayhem
##
include ../config.h

CC			?= gcc
LD			= gcc
RM			= rm -f 
AR			= ar rc
CFLAGS			+= -Iinclude -Wall -fPIC -g3 -O2 -DASPECT_ELFSH_INTERN
SRC			= vectors.c libhash.c
OBJ			= $(SRC:.c=.o)
NAME			= libaspect

all			: $(OBJ)
			@$(LD) -L../libc/ -lcelfsh -L../libmalloc/ -lmalloc-e2dbg \
			-shared $(OBJ) -o $(NAME).so
			@$(AR) $(NAME).a $(OBJ)
			@$(RANLIB) $(NAME).a

clean			: 
			@$(RM) \#* *\# *~ *.o .\#* include/\#* include/*\#  \
			include/*~ include/.\#*

fclean			: clean
			@$(RM) $(NAME).so $(NAME).a

