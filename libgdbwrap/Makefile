include ../config.h

SRC       = gdbwrapper.c 
CLIENT    = client.o
CLIENT    = client.c
OBJ       = $(SRC:.c=.o)
CFLAGS32  += -Wpointer-arith -Wextra -fPIC -g3 -Wall				 \
			-Iinclude/ $(DYNOPT) $(BUILDOP) 			 \
	  		-I../libgdbwrap/include/			      	 \
			-I../libaspect/include/				      	 \
			-I../librevm/include/				      	 \
			-I../libelfsh/include/				      	 \
			-I../libedfmt/include/				      	 \
			-I../libasm/include/				      	 \
			-I../libmjollnir/include/ -DERESI32			      	 

LDFLAGS32		+= 							 \
			-L../liballocproxy -lallocproxy				 \
			-L../libelfsh/ -lelfsh32 				 \
			-L../libetrace/ -letrace32				 \
			-L../libstderesi -lstderesi32    			 \
			-L../libdump/						 \
			-L  /usr/pkg/lib/					 \
			-L../libgdbwrap/					 \
			-L../libasm/ -lasm32          				 \
			-L../libaspect/ -laspect32				 \
			-L../librevm/ -lrevm32           			 \
			-L../libmjollnir -lmjollnir32 -g3 			 \
			-L../libui/ -lui32					 \
			-L../libedfmt -ledfmt32  			    	 \
			$(LDREADLNOPT) $(DLOPT) -L/usr/freeware/lib32/ $(DYNOPT) \
			-DERESI32 $(EXTRAOPT) $(LDDUMPOPT32) $(LPTHREAD)         \
			$(LDMJOLLNIR) $(KERNSHLIB32) $(NCURSESLNOPT) $(READLNOPT) -DERESI32


NAME      = libgdbwrap

CC      ?= gcc
LD      ?= ld
AR	= ar rc
RANLIB  = ranlib

all:	$(OBJ)
	$(AR) $(NAME).a $(OBJ)
	$(RANLIB) $(NAME).a
	$(LD) -r $(OBJ) -o $(NAME).o $(LDFLAGS)

client:
	$(CC) $(CLIENT) $(CFLAGS32) $(LDFLAGS) libgdbwrap.a -o client

clean:
	rm -f $(OBJ) $(OBJ) *~ *\# \#* .\#* 
	rm -f include/*~ include/*\# include/\#* include/.\#* 

fclean:	clean
	rm -f $(NAME).a $(NAME).o *.o client

%.o:	%.c
	$(CC) $(CFLAGS32) -c -o $@ $<
