include ../config.h

SRC       = gdbwrapper.c interface.c

OBJ32      = $(SRC:.c=.32.o)
CFLAGS32  += -Wpointer-arith -Wextra -fPIC -g3 -Wall -Iinclude/ $(DYNOPT) $(BUILDOP) \
	  -I../libgdbwrap/include/ -I../libaspect/include/ -DERESI32
LDFLAGS32 += -L../libaspect/ -laspect32				 

OBJ64      = $(SRC:.c=.64.o)
CFLAGS64  += -Wpointer-arith -Wextra -fPIC -g3 -Wall -Iinclude/ $(DYNOPT) $(BUILDOP) \
	  -I../libgdbwrap/include/ -I../libaspect/include/ -DERESI64
LDFLAGS64 += -L../libaspect/ -laspect64

NAME32    = libgdbwrap32
NAME64    = libgdbwrap64

CC      ?= gcc
LD      ?= ld
AR	= ar rc

all  :  all32 all64

all32:  $(OBJ32)
	$(AR) $(NAME32).a $(OBJ32)
	$(RANLIB) $(NAME32).a
	$(CC) -shared $(OBJ32) -o $(NAME32).so

all64:  $(OBJ64)
	$(AR) $(NAME64).a $(OBJ64)
	$(RANLIB) $(NAME64).a
	$(CC) -shared $(OBJ64) -o $(NAME64).so

client:
	$(CC) client.c $(CFLAGS32) $(LDFLAGS32) $(NAME32).a -o client32
	$(CC) client.c $(CFLAGS64) $(LDFLAGS64) $(NAME64).a -o client64

clean:
	rm -f $(OBJ32) $(OBJ64) *~ *\# \#* .\#* 
	rm -f include/*~ include/*\# include/\#* include/.\#* 

fclean:	clean
	rm -f $(NAME32).* $(NAME64).* $(OBJ32) $(OBJ64) client* 

%.32.o:	%.c
	$(CC) $(CFLAGS32) -c -o $@ $<

%.64.o:	%.c
	$(CC) $(CFLAGS64) -c -o $@ $<
