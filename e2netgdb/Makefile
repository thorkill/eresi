##
## Makefile for e2dbg in ERESI
##
## Started on  Tue Aug  16 06:32:29 2005 jfv
## $Id
##

include ../config.h

SRC			= main.c
OBJ			= $(SRC:.c=.o)
OBJ32			= $(SRC:.c=.32.o)
OBJ64			= $(SRC:.c=.64.o)
CC			?= gcc
RM			= rm -f
DBGNAME			= e2dbg
DBGNAME32		= e2dbg32
DBGNAME64		= e2dbg64

CFLAGS32		+= -Wpointer-arith -Wextra -fPIC -g3 -Wall -Iinclude $(DYNOPT) $(BUILDOP) 	 \
			-I../libgdbwrap/include/			      	 \
			-I../libaspect/include/				      	 \
			-I../librevm/include/				      	 \
			-I../libelfsh/include/				      	 \
			-I../libedfmt/include/				      	 \
			-I../libasm/include/				      	 \
			-I../libmjollnir/include/			      	 \
			-DERESI32

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
			$(LDMJOLLNIR) $(KERNSHLIB32) $(NCURSESLNOPT) $(READLNOPT)

all			: all32 

all32			: ${OBJ32}
			$(CC) $(OBJ32) $(CFLAGS32) $(LDFLAGS32) -o $(DBGNAME32)

all64			: ${OBJ64}
			$(CC) $(OBJ64) $(LDFLAGS64) -o $(DBGNAME64)

clean		       :
		       $(RM) $(OBJ) $(OBJ32) $(OBJ64) *~ \#* *\# .\#*
		       $(RM) include/*~ include/\#* include/*\# include/.\#*
		       $(RM) *.??.o

fclean		       : clean
		       $(RM) $(NAME) $(OBJ_DYN) $(DBGNAME)
		       $(RM) $(NAME32) $(OBJ_DYN32) $(DBGNAME32)
		       $(RM) $(NAME64) $(OBJ_DYN64) $(DBGNAME64)

%.32.o : %.c
			$(CC) $(CFLAGS32) -c -o $@ $<
%.64.o : %.c
			$(CC) $(CFLAGS64) -c -o $@ $<


