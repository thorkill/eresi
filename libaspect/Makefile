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
CFLAGS32		+= -Iinclude -Wall -fPIC -g3 -O2 -DASPECT_ELFSH_INTERN -DELFSH32
CFLAGS64		+= -Iinclude -Wall -fPIC -g3 -O2 -DASPECT_ELFSH_INTERN -DELFSH64
SRC			= vectors.c libhash.c
OBJ			= $(SRC:.c=.o)
OBJ32			= $(SRC:.c=.32.o)
OBJ64			= $(SRC:.c=.64.o)
NAME			= libaspect
NAME32			= libaspect32
NAME64			= libaspect64

all			: all32 all64
#all			: $(OBJ)
#			@$(LD) -L../libc/ -lcelfsh -L../libmalloc/ -lmalloc-e2dbg \
#			-shared $(OBJ) -o $(NAME).so
#			@$(AR) $(NAME).a $(OBJ)
#			@$(RANLIB) $(NAME).a

all32			: $(OBJ32)
			@$(LD) -L../libc/ -lcelfsh32 -L../libmalloc/ -lmalloc-e2dbg \
			-shared $(OBJ32) -o $(NAME32).so
			@$(AR) $(NAME32).a $(OBJ32)
			@$(RANLIB) $(NAME32).a

all64			: $(OBJ64)
			@$(LD) -L../libc/ -lcelfsh64 -L../libmalloc/ -lmalloc-e2dbg \
			-shared $(OBJ64) -o $(NAME64).so
			@$(AR) $(NAME64).a $(OBJ64)
			@$(RANLIB) $(NAME64).a

clean			: 
			@$(RM) \#* *\# *~ *.o .\#* include/\#* include/*\#  \
			include/*~ include/.\#*

fclean			: clean
			@$(RM) $(NAME).so $(NAME).a

%.32.o : %.c
			$(CC) $(CFLAGS32) -c -o $@ $<
%.64.o : %.c
			$(CC) $(CFLAGS64) -c -o $@ $<

