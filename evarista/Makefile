##
## Makefile for elfsh
##
## Started on  Fri Nov  2 16:32:29 2001 jfv
##

include ../config.h

SRC			= main.c tables.c
OBJ			= $(SRC:.c=.o)
OBJ32			= $(SRC:.c=.32.o)
OBJ64			= $(SRC:.c=.64.o)
CC			?= gcc
RM			= rm -f
NAME			= evarista
NAME32			= evarista32
NAME64			= evarista64

CFLAGS32		+= -fPIC -g3 $(GSTABS) -Wall -Iinclude $(DYNOPT) $(BUILDOP)\
			-I../libgdbwrap/include/ 	\
			-I../libelfsh/include/ -I../libstderesi/include/ \
			-I../libetrace/include/ 			\
			-I../libdump/include/ -I../libasm/include/ \
			-I../e2dbg/include/ -I/usr/pkg/include/  \
			-I../libaspect/include/ -I../librevm/include/ \
			-I../../libgdbwrap/include/ 			\
			-I../libui/include/ -I../libmjollnir/include/ \
			-I../libedfmt/include/  $(STATOPT2) \
			$(ASMOPT) -DERESI32 $(NCURSESLNOPT) $(READLNOPT)

CFLAGS64		+= -fPIC -g3 $(GSTABS) -Wall -Iinclude $(DYNOPT) $(BUILDOP)\
			-I../libelfsh/include/ -I../libstderesi/include/ \
			-I../libetrace/include/ \
			-I../libgdbwrap/include/ 	\
			-I../libdump/include/ -I../libasm/include/ \
			-I../e2dbg/include/ -I/usr/pkg/include/  \
			-I../libaspect/include/ -I../librevm/include/ \
			-I../libui/include/ -I../libmjollnir/include/ \
			-I../libedfmt/include/  $(STATOPT2) \
			$(ASMOPT) -DERESI64 $(NCURSESLNOPT) $(READLNOPT)

LDFLAGS32		+= -L../libedfmt/ -L../libelfsh/ -ledfmt32 -lelfsh32      \
			-L../libetrace/ -letrace32				  \
			-L../libdump/ -L/usr/pkg/lib/ -L../libmjollnir/		  \
			-lmjollnir32 $(LPTHREAD) -L../libstderesi -lstderesi32    \
			-L../libasm/ -lasm32 $(LDUIOPT32) -g3		          \
			-L../liballocproxy -L../libmjollnir/          		  \
			-L../libaspect/ -laspect32 -L../librevm/ -lrevm32         \
			-lallocproxy $(LDMJOLLNIR) $(DLOPT)  			  \
			$(KERNSHLIB32)						  \
			-L/usr/freeware/lib32/ $(DYNOPT) $(BITS) $(EXTRAOPT) 	  \
			$(LDDUMPOPT32) $(LDNCURSESLNOPT) $(LDREADLNOPT) $(READLNOPT)

LDFLAGS64		+= -L../libelfsh/ -lelfsh64 -L../libdump/ -L/usr/pkg/lib/ \
			-L../libetrace/ -letrace64 -L../libstderesi -lstderesi64  \
			-L../libmjollnir/ -lmjollnir64 $(LPTHREAD) 	          \
			-L../libasm/ -lasm64 $(LDUIOPT64) -g3			  \
			-L../liballocproxy -L../libmjollnir/          		  \
			-L../libaspect/ -laspect64 -L../librevm/ -lrevm64         \
			-lallocproxy -L../libedfmt/ -ledfmt64 $(LDMJOLLNIR)       \
			$(KERNSHLIB64)						  \
			$(DLOPT)  -L/usr/freeware/lib32/ $(DYNOPT) $(BITS)        \
			$(EXTRAOPT) $(LDDUMPOPT64) $(LDNCURSESLNOPT) $(LDREADLNOPT) \
			$(READLNOPT)

all			: all32 all64

all32			: $(OBJ32)
			@$(CC) $(OBJ32) -o $(NAME32) $(LDFLAGS32) $(STATOPT)

all64			: $(OBJ64)
			@$(CC) $(OBJ64) -o $(NAME64) $(LDFLAGS64) $(STATOPT)

clean		       :
		       @$(RM) $(OBJ) *~ \#* *\# .\#* *.32.o *.64.o
		       @$(RM) include/*~ include/\#* include/*\# include/.\#*

fclean		       : clean
		       @$(RM) $(NAME) $(NAME32) $(NAME64) $(OBJ_DYN)

%.32.o : %.c
			$(CC) $(CFLAGS32) -c -o $@ $<
%.64.o : %.c
			$(CC) $(CFLAGS64) -c -o $@ $<
