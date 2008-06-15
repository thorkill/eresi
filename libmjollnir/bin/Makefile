SRCS = elfThor.c
OBJS =$(SRCS:.c=.o)
NAME = elfThor
CFLAGS=`eresi-config libmjollnir --cflags`
LDFLAGS=`eresi-config libmjollnir --libs`

all:	$(OBJS)
	$(CC) -o $(NAME) $(OBJS) $(LDFLAGS)
