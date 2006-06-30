##
## 
## Makefile in 
## 
## Author  : sk at devhell dot org
## Started : Thu May 29 21:50:28 2003
## Updated : Thu Nov 27 22:06:16 2003
##
include ../../config.h

SRCS 	= modflow.c control.c blocks.c inspect.c graph.c flowjack.c

OBJS 	= $(SRCS:.c=.o)
NAME 	= modflow.so
CFLAGS 	= -Wall -g3 -fPIC -rdynamic -DELFSH_INTERN $(BITS) \
	-I../../vm/include -I../../libdump/include/ \
	-I../../libelfsh/include -I../../libhash/include \
	-I../../libdump/include -I../../e2dbg/include \
	-I../../libasm/include/ -I../../libc/include/
RM 	= rm -f
CC 	?= gcc

all:	$(OBJS)
	$(CC) -shared -o ../$(NAME) $(OBJS)

clean	:
	$(RM) $(OBJS) *~

fclean	: clean
	$(RM) ../$(NAME)
