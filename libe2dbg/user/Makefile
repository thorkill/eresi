##
## Makefile for userland part of libe2dbg in the ERESI framework
##
## Started on Wed Dec 19 06:32:29 2005 jfv
## $Id
##

include ../../config.h

LD			?= ld
SRC			= init.c e2dbg.c signal.c linkmap.c continue.c \
			alloc-proxy.c resolv.c threads.c entry.c e2dbg-misc.c \
			dbg-ia32.c  dbg-sparc32.c dbg-amd64.c delbreak.c \
			register.c step.c 
OBJ32			= $(SRC:.c=.32.o)
OBJ64			= $(SRC:.c=.64.o)
OBJNAME32		= libe2dbg-user32
CC			?= gcc
RM			= rm -f

CFLAGS32		+= -fPIC -g3 -Wall -I../include $(DYNOPT) $(BUILDOP) 	 \
			-I ../../libgdbwrap/include/ 				 \
			-I../../libelfsh/include/ -I../../libdump/include/     	 \
			-I../../libasm/include/ -I../../libstderesi/include/	 \
			-I../../libui/include/ -I../../libmjollnir/include/ 	 \
			-I../../libaspect/include -I../../librevm/include/	 \
			-I../../libedfmt/include/ -I../../liballocproxy/include/ \
			-I../../libetrace/include/ $(KERNSHINCINC) $(KERNSH)     \
			-DE2DBG_PRESENT	 -DE2DBG_INTERN $(ASMOPT) $(EXTRACFLAGS) \
			-DERESI32	 

CFLAGS64		+= -fPIC -g3 -Wall -I../include $(DYNOPT) $(BUILDOP) 	 \
			-I ../../libgdbwrap/include/ 				 \
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
			$(LD) -r $(OBJ32) -o $(OBJNAME32).o

all64			: ${OBJ64}
			$(LD) -r $(OBJ64) -o $(OBJNAME64).o

#$(OBJNAME32)		:$(CC) $(SRC) $(OBJ32) -o $(OBJNAME).so
#			$(LD) -r $(OBJ32) -o $(OBJNAME32).so

clean		       :
		       @$(RM) $(OBJ) $(OBJ32) $(OBJ64) *.o *~ \#* *\# .\#*
		       @$(RM) include/*~ include/\#* include/*\# include/.\#*

fclean		       : clean

%.32.o : %.c
			$(CC) $(CFLAGS32) -c -o $@ $<
%.64.o : %.c
			$(CC) $(CFLAGS64) -c -o $@ $<


