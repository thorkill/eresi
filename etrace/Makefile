##
## Makefile for etrace
##
## Started on  Fri Nov  2 16:32:29 2001 mxatone
##

include ../config.h

SRC			= main.c tables.c rlheap.c cleanup.c run.c misc.c
OBJ			= $(SRC:.c=.o)
OBJ32			= $(SRC:.c=.32.o)
OBJ64			= $(SRC:.c=.64.o)
CC			?= gcc
RM			= rm -f
NAME			= etrace
NAME32			= etrace32
NAME64			= etrace64

CFLAGS32		+= -fPIC -g3 $(GSTABS) -Wall -Iinclude $(DYNOPT) $(BUILDOP)\
			-I../libelfsh/include/ 		\
			-I../libgdbwrap/include/ 	\
			-I../libetrace/include/ 	\
			-I../libstderesi/include/    	\
			-I../libdump/include/ 		\
			-I../libasm/include/ 		\
			-I../e2dbg/include/ 		\
			-I/usr/pkg/include/  		\
			-I../libui/include/ 		\
			-I../libmjollnir/include/ 	\
			-I../libaspect/include/ 	\
			-I../librevm/include/ 		\
			-I../libedfmt/include/  $(STATOPT2) \
			$(ASMOPT) -DERESI32 $(NCURSESLNOPT) $(READLNOPT)

CFLAGS64		+= -fPIC -g3 $(GSTABS) -Wall -Iinclude $(DYNOPT) $(BUILDOP)\
			 -I../libelfsh/include/ \
			-I../libgdbwrap/include/ 	\
			 -I../libetrace/include/ -I../libstderesi/include/ \
			 -I../libdump/include/ -I../libasm/include/ \
			 -I../e2dbg/include/ -I/usr/pkg/include/  \
			-I../libui/include/ -I../libmjollnir/include/ \
			-I../libaspect/include/ -I../librevm/include/ \
			-I../libedfmt/include/  $(STATOPT2) \
			$(ASMOPT) -DERESI64 $(NCURSESLNOPT) $(READLNOPT)

LDFLAGS32		+= -L../libedfmt/ -L../libelfsh/ -ledfmt32 -lelfsh32      \
			-L../libgdbwrap/					  \
			-L../libetrace/ -letrace32 -L../libstderesi -lstderesi32  \
			-L../libdump/ -L/usr/pkg/lib/ -L../libmjollnir/		  \
			-lmjollnir32 $(LPTHREAD)				  \
			-L../libasm/ -lasm32 -L../libui/ -lui32 -g3		  \
			-L../liballocproxy -L../libmjollnir/          		  \
			-L../libaspect/ -laspect32 -L../librevm/ -lrevm32         \
			-lallocproxy $(LDMJOLLNIR) $(KERNSHLIB32) $(DLOPT)  	  \
			-L/usr/freeware/lib32/ $(DYNOPT) $(BITS) $(EXTRAOPT) 	  \
			$(LDDUMPOPT32) $(LDNCURSESLNOPT) $(LDREADLNOPT)

LDFLAGS64		+= -L../libelfsh/ -lelfsh64 -L../libdump/ -L/usr/pkg/lib/ \
			-L../libgdbwrap/					  \
			-L../libetrace/ -letrace64 -L../libstderesi -lstderesi64  \
			-L../libmjollnir/ -lmjollnir64 $(LPTHREAD) 		  \
                        -L../libasm/ -lasm64 -L../libui/ -lui64 -g3   		  \
			-L../liballocproxy -L../libmjollnir/          		  \
			-L../libaspect/ -laspect64 -L../librevm/ -lrevm64         \
			-lallocproxy -L../libedfmt/ -ledfmt64 $(LDMJOLLNIR)       \
			$(KERNSHLIB64)						  \
			$(DLOPT)  -L/usr/freeware/lib32/ $(DYNOPT) $(BITS)        \
			$(EXTRAOPT) $(LDDUMPOPT64) $(LDNCURSESLNOPT) $(LDREADLNOPT)

all			: all32 all64

$(NAME32)			: $(OBJ32)
			$(CC) $(OBJ32) -o $(NAME32) $(LDFLAGS32) $(STATOPT)

$(NAME64)			: $(OBJ64)
			@$(CC) $(OBJ64) -o $(NAME64) $(LDFLAGS64) $(STATOPT)

all32: $(NAME32)
all64: $(NAME64)

clean		       :
		       @$(RM) $(OBJ) *~ \#* *\# .\#* *.32.o *.64.o
		       @$(RM) include/*~ include/\#* include/*\# include/.\#*

fclean		       : clean
		       @$(RM) $(NAME) $(NAME32) $(NAME64) $(OBJ_DYN)

%.32.o : %.c
			$(CC) $(CFLAGS32) -c -o $@ $<
%.64.o : %.c
			$(CC) $(CFLAGS64) -c -o $@ $<
