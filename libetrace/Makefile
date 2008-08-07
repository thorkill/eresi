##
## Makefile.am
## 
## Started on  Fri Nov  2 16:32:29 2001 jfv
## Last update Mon Feb 23 17:22:18 2004 jfv
##

include ../config.h

CC			?= gcc
LD			?= ld
RM			= rm -f 
AR			= ar rc
CFLAGS32		+= -Iinclude -Wall -fPIC -g3 -O2 $(EXTRACFLAGS) \
			-I../libasm/include/ -I../libaspect/include/        \
                        -I../libelfsh/include/ -DERESI32 $(BUILDOP)         \
			-I../libedfmt/include/
CFLAGS64		+= -Iinclude -Wall -fPIC -g3 -O2 $(EXTRACFLAGS) \
			-I../libasm/include/ -I../libaspect/include/        \
                        -I../libelfsh/include/ -DERESI64 $(BUILDOP)	    \
			-I../libedfmt/include/
SRC			= errfunc.c save.c search_addr.c delete.c  \
			check.c check_untracable.c func_add.c func_remove.c \
			func_exclude.c func_enable.c func_disable.c	    \
			func_status.c trace_create.c func_search.c          \
			trace_list.c trace_flush.c trace_run.c func_match.c
OBJ32			= $(SRC:.c=.32.o)
OBJ64			= $(SRC:.c=.64.o)
NAME32			= libetrace32
NAME64			= libetrace64

LDFLAGS			= 

all			: all32 all64

all32			: $(OBJ32)
			@$(CC) -L../liballocproxy/ \
			-lallocproxy -L../libaspect/ -laspect32 -DERESI32 \
			-L../libelfsh/ -lelfsh32 -shared $(OBJ32) -o $(NAME32).so
			@$(AR) $(NAME32).a $(OBJ32)
			@$(RANLIB) $(NAME32).a
			@$(LD) -r $(OBJ32) -o $(NAME32).o

all64			:  $(OBJ64)
			@$(CC) -L../liballocproxy/ \
			-lallocproxy -L../libaspect/ -laspect64 -DERESI64 \
			-L../libelfsh/ -lelfsh64 -shared $(OBJ64) -o $(NAME64).so 
			@$(AR) $(NAME64).a $(OBJ64)
			@$(RANLIB) $(NAME64).a
			@$(LD) -r $(OBJ64) -o $(NAME64).o

clean			: 
			@$(RM) \#* *\# *~ *.o .\#* include/\#* include/*\#  \
			include/*~ include/.\#* include/libelfsh/\#*	    \
			include/libelfsh/*\# include/libelfsh/*~            \
			include/libelfsh/.\#*

fclean			: clean
			@$(RM) *.so *.a


docs:
			doxygen doc/doxygen.conf

%.32.o : %.c
			$(CC) $(CFLAGS32) -c -o $@ $<
%.64.o : %.c
			$(CC) $(CFLAGS64) -c -o $@ $<

