##
## 
## Makefile in 
## 
## Author  : sk at devhell dot org
## Started : Thu May 29 21:50:28 2003
## Updated : Tue Jun 17 23:30:19 2003
##

SRCS = modflow.c \
	control.c \
	blocks.c \
	inspect.c \
	graph.c

OBJS = $(SRCS:.c=.o)

NAME = modflow.so

CFLAGS = -Wall -Werror -ggdb -I../../vm/include \
	-I../../libelfsh/include -I../../libhash/include \
	-fPIC -rdynamic -DELFSH_INTERN

RM = rm -f

all:	$(OBJS)
	$(CC) -shared -o ../$(NAME) $(OBJS)

clean	:
	$(RM) $(OBJS) *~

fclean	: clean
	$(RM) ../$(NAME)
