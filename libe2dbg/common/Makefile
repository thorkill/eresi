##
## Makefile for common (user/kernel code) parts of libe2dbg in the ERESI framework
##
## Started on  Tue Aug  16 06:32:29 2005 jfv
## $Id$
##

include ../../config.h

LD			?= ld
CC			?= gcc
RM			= rm -f
SRC			= backtrace.c breakpoints.c display.c mode.c stack.c \
			dumpregs.c dbghooks.c mutex.c common.c delete.c linkmap.c
OBJ32			= $(SRC:.c=.32.o)
OBJ64			= $(SRC:.c=.64.o)
OBJNAME32		= libe2dbg-common32
OBJNAME64		= libe2dbg-common64

CFLAGS32		+= -fPIC -g3 -Wall -I../include $(DYNOPT) $(BUILDOP) 	 \
			-I ../../libgdbwrap/include/ \
			-I../../libelfsh/include/ -I../../libdump/include/       \
			-I../../libasm/include/ -I../../libstderesi/include/	 \
			-I../../libui/include/ -I../../libmjollnir/include/ 	 \
			-I../../libaspect/include -I../../librevm/include/	 \
			-I../../libedfmt/include/ -I../../liballocproxy/include/ \
			-I../../libetrace/include/			         \
			-DE2DBG_PRESENT  -DE2DBG_INTERN $(ASMOPT) $(EXTRACFLAGS) \
			-DERESI32

CFLAGS64		+= -fPIC -g3 -Wall -I../include $(DYNOPT) $(BUILDOP) 	 \
			-I ../../libgdbwrap/include/ \
			-I../../libelfsh/include/ -I../../libdump/include/       \
			-I../../libasm/include/ -I../../libstderesi/include/	 \
			-I../../libui/include/ -I../../libmjollnir/include/ 	 \
			-I../../libaspect/include -I../../librevm/include/	 \
			-I../../libedfmt/include/ -I../../liballocproxy/include/ \
			-I../../libetrace/include				 \
			-DE2DBG_PRESENT  -DE2DBG_INTERN $(ASMOPT) $(EXTRACFLAGS) \
			-DERESI64

all			: all32 all64

all32			: ${OBJ32}
			  $(LD) -r ${OBJ32} -o $(OBJNAME32).o

all64			: ${OBJ64}
			  $(LD) -r ${OBJ64} -o $(OBJNAME64).o

clean		       :
		       @$(RM) $(OBJ32) $(OBJ64) $(OBJNAME64).o $(OBJNAME32).o	\
			*~ \#* *\# .\#*

fclean		       : clean
			@$(RM) *.o

%.32.o : %.c
			$(CC) $(CFLAGS32) -c -o $@ $<
%.64.o : %.c
			$(CC) $(CFLAGS64) -c -o $@ $<


