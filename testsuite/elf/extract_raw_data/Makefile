SRCS	= load.c
RM	= rm -fr 

OBJ32	= $(SRCS:.c=.32.o)
OBJ64	= $(SRCS:.c=.64.o)
OBJ6464	= $(SRCS:.c=.64.o.64)
CFLAGS32  =$(shell eresi-config32 libelfsh --cflags)
LDFLAGS32 =$(shell eresi-config32 libelfsh --libs)
CFLAGS64  =$(shell eresi-config64 libelfsh --cflags)
LDFLAGS64 =$(shell eresi-config64 libelfsh --libs)
NAME32	= load32
NAME64	= load64

all: all32 all64

all32: $(OBJ32)
	$(CC) -o $(NAME32) $(OBJ32) $(LDFLAGS32)

all64: $(OBJ64) $(OBJ6464)
	$(CC) -o $(NAME64) $(OBJ64) $(LDFLAGS64)

fclean: clean
	$(RM) $(NAME32) $(NAME64)
clean:
	$(RM) $(OBJ32) $(OBJ64) $(OBJ6464) *~

%.32.o:	%.c
	$(CC) -c $< -o $@ $(CFLAGS32) -g

%.64.o:	%.c
	$(CC) -c $< -o $@ $(CFLAGS64) -g

%.64.o.64: %.c
	$(CC) -m64 -c $< -o $@ $(CFLAGS64) -g
