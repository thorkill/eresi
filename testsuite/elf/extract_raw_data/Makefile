SRCS	= load.c
OBJS	= $(SRCS:.c=.o)

CFLAGS	=$(shell eresi-config libelfsh --cflags)
LDFLAGS	=$(shell eresi-config libelfsh --libs)

NAME	= load
RM	= rm -fr 

$(info Using CFLAGS = $(CFLAGS))
$(info Using LDFLAGS = $(LDFLAGS))


all32: all
all:	$(OBJS)
	$(CC) -o $(NAME) $(OBJS) $(LDFLAGS)

fclean: clean
	$(RM) $(NAME)
clean:
	$(RM) $(OBJS) *~

%.o:	%.c
	$(CC) -c $< -o $@ $(CFLAGS) -g
