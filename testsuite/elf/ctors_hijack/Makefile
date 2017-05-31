##
## Makefile for ctors_hijack in ERESI testsuite
## 
## $Id
##

include ../../../config.h

SRC	= ctors_hijack.c
CC	= gcc
RM	= rm -f 
VULN	= hijacked.c

OBJ32	= $(SRC:.c=.32.o)
OBJ64	= $(SRC:.c=.64.o)
CFLAGS32  =$(shell eresi-config32 libelfsh --cflags)
LDFLAGS32 =$(shell eresi-config32 libelfsh --libs)
CFLAGS64  =$(shell eresi-config64 libelfsh --cflags)
LDFLAGS64 =$(shell eresi-config64 libelfsh --libs)
NAME32	= ctors_hijacker32
NAME64	= ctors_hijacker64

all: all32 all64

all32: $(OBJ32)
	$(CC) $(OBJ32) -o $(NAME32) $(LDFLAGS32)
	$(CC) $(VULN) -o hijackme32

all64: $(OBJ64)
	$(CC) $(OBJ64) -o $(NAME64) $(LDFLAGS64)
	$(CC) -m64 $(VULN) -o hijackme64

fclean: clean
	$(RM) $(NAME32) $(NAME64) hijackme* fake_aout*

clean:
	$(RM) $(OBJ32) $(OBJ64) *~

%.32.o:	%.c
	$(CC) -c $< -o $@ $(CFLAGS32) -g

%.64.o:	%.c
	$(CC) -c $< -o $@ $(CFLAGS64) -g
