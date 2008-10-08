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
DBGNAME			= e2netgdb
DBGNAME32		= e2netgdb32
DBGNAME64		= e2netgdb64

CFLAGS32 		+= -fPIC -g3 -Wall -Iinclude $(DYNOPT) $(BUILDOP) 	 \
			-I../libgdbwrap/include/			      	 \
			-I../libaspect/include/				      	 \
			-I../librevm/include/				      	 \
			-I../libelfsh/include/				      	 \
			-I../libedfmt/include/				      	 \
			-I../libasm/include/				      	 \
			-I../libui/include/				      	 \
			-I../libmjollnir/include/			      	 \
			-I../libe2dbg/include/ 					 \
			-I../libstderesi/include				 \
			-I../libetrace/include/ 				 \
			-DERESI32

LDFLAGS32		+= -L../libstderesi -lstderesi32    			 \
			-L../libgdbwrap/ -lgdbwrap				 \
			../libe2dbg/common/libe2dbg-common32.o 			 \
			../libe2dbg/user/libe2dbg-user-noinit32.o 		 \


all			: all32 

all32			: ${OBJ32}
			$(CC) $(OBJ32) $(LDFLAGS32) -o $(DBGNAME32)

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


