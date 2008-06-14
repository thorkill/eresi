SRCS = load.c
OBJS = $(SRCS:.c=.o)

CFLAGS=`eresi-config libelfsh --cflags`
LDFLAGS=`eresi-config libelfsh --libs`

$(info Using CFLAGS = $(CFLAGS))
$(info Using LDFLAGS = $(LDFLAGS))



all:	$(OBJS)
	cc -o load load.o $(LDFLAGS)

%.c:%.o
	$(CC) -c $< -o $@ $(CFLAGS) 