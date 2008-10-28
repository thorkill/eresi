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
DBGNAME			= kedbg
DBGNAME32		= kedbg32
DBGNAME64		= kedbg64

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
			-DERESI32 -DJEDBG -DKEDBG

LDFLAGS32               += -L../libedfmt/ -ledfmt32 -L../libelfsh/ -lelfsh32        \
                        -L../libetrace/ -letrace32 -L../libstderesi/ -lstderesi32   \
                        -L/usr/pkg/lib/ -L../libmjollnir/ -lmjollnir32              \
                        -L../libetrace -letrace32 $(LPTHREAD) -L../libasm/ -lasm32  \
                        $(LDUIOPT32) -g3 $(STATOPT) \
                        -L../libaspect/ -laspect32 -L../librevm/ -lrevm32           \
                        -L/usr/freeware/lib32/ $(DYNOPT) $(BITS) $(EXTRAOPT)        \
                        $(LDDUMPOPT32) $(LDNCURSESLNOPT) $(LDREADLNOPT) $(READLNOPT) \
                        ../libgdbwrap/libgdbwrap.o                               \
                        ../libe2dbg/common/libe2dbg-common32.o



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


