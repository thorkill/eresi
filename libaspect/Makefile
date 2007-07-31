##
## Makefile.am for libaspect in elfsh
## 
## Started on  Fri Dec 22 16:32:29 2006 jfv
##
include ../config.h

CC		?= gcc
LD		= ld
RM		= rm -f 
AR		= ar rc
CFLAGS32	+= -Iinclude -fPIC -g3 -O2 -DASPECT_ELFSH_INTERN -DERESI32
CFLAGS64	+= -Iinclude -fPIC -g3 -O2 -DASPECT_ELFSH_INTERN -DERESI64
SRC		= vectors.c libhash.c libbtree.c types.c init.c \
		config.c profiler.c liblist.c

OBJ32		= $(SRC:.c=.32.o)
OBJ64		= $(SRC:.c=.64.o)
NAME		= libaspect
NAME32		= libaspect32
NAME64		= libaspect64

all		: all32 all64

all32		: $(OBJ32)
		@$(CC) -L../elibc/ -lcelfsh32 -L../liballocproxy/ -lallocproxy \
		$(DLOPT) -shared $(OBJ32) -o $(NAME32).so
		@$(AR) $(NAME32).a $(OBJ32)
		@$(RANLIB) $(NAME32).a
		$(LD) -r $(OBJ32) -o $(NAME32).o

all64		: $(OBJ64)
		@$(LD) -L../elibc/ -lcelfsh64 -L../liballocproxy/ -lallocproxy \
		$(DLOPT) -shared $(OBJ64) -o $(NAME64).so
		@$(AR) $(NAME64).a $(OBJ64)
		@$(RANLIB) $(NAME64).a
		$(LD) -r $(OBJ64) -o $(NAME64).o

clean		: 
		@$(RM) \#* *\# *~ $(OBJ) $(OBJ32) $(OBJ64) .\#* include/\#* \
		include/*\# include/*~ include/.\#*

fclean		: clean
		@$(RM) *.so *.a

%.32.o 		: %.c
		$(CC) $(CFLAGS32) -c -o $@ $<

%.64.o 		: %.c
		$(CC) $(CFLAGS64) -c -o $@ $<

