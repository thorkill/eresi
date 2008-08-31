##
## Makefile for userland part of libe2dbg in the ERESI framework
##
## Started on Wed Dec 19 06:32:29 2005 jfv
## $Id
##

include ../../config.h

SRC			= e2dbg.c delete.c signal.c linkmap.c continue.c \
			alloc-proxy.c resolv.c threads.c entry.c e2dbg-misc.c
OBJ32			= $(SRC:.c=.32.o)
OBJ64			= $(SRC:.c=.64.o)
CC			?= gcc
RM			= rm -f

CFLAGS32		+= -fPIC -g3 -Wall -I../include $(DYNOPT) $(BUILDOP) 	 \
			-I../../libelfsh/include/ -I../../libdump/include/     	 \
			-I../../libasm/include/ -I../../libstderesi/include/	 \
			-I../../libui/include/ -I../../libmjollnir/include/ 	 \
			-I../../libaspect/include -I../../librevm/include/	 \
			-I../../libedfmt/include/ -I../../liballocproxy/include/ \
			-I../../libetrace/include/ $(KERNSHINCINC) $(KERNSH)     \
			-DE2DBG_PRESENT	 -DE2DBG_INTERN $(ASMOPT) $(EXTRACFLAGS) \
			-DERESI32	 

CFLAGS64		+= -fPIC -g3 -Wall -I../include $(DYNOPT) $(BUILDOP) 	 \
			-I../../libelfsh/include/ -I../../libdump/include/       \
			-I../../libasm/include/ -I../../libstderesi/include/	 \
			-I../../libui/include/ -I../../libmjollnir/include/ 	 \
			-I../../libaspect/include -I../../librevm/include/	 \
			-I../../libedfmt/include/ -I../../liballocproxy/include/ \
			-I../../libetrace/include/ $(KERNSHINC) $(KERNSH)        \
			-DE2DBG_PRESENT	 -DE2DBG_INTERN $(ASMOPT) $(EXTRACFLAGS) \
			-DERESI64

all			: all32 all64

all32			: ${OBJ32}

all64			: ${OBJ64}

clean		       :
		       @$(RM) $(OBJ) $(OBJ32) $(OBJ64) *~ \#* *\# .\#*
		       @$(RM) include/*~ include/\#* include/*\# include/.\#*

fclean		       : clean

%.32.o : %.c
			$(CC) $(CFLAGS32) -c -o $@ $<
%.64.o : %.c
			$(CC) $(CFLAGS64) -c -o $@ $<


