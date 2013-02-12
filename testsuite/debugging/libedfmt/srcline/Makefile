##
## Makefile for srclinetest in ERESI testsuite
## 
## $Id 
##
include ../../../../config.h

SRC	= test_usage.c
CC	= gcc
RM	= rm -f 

OBJ32	= $(SRC:.c=.32.o)
OBJ64	= $(SRC:.c=.64.o)
CFLAGS32  =$(shell eresi-config32 libedfmt --cflags)
LDFLAGS32 =$(shell eresi-config32 libedfmt --libs)
CFLAGS64  =$(shell eresi-config64 libedfmt --cflags)
LDFLAGS64 =$(shell eresi-config64 libedfmt --libs)
NAME32	= srclinetest32
NAME64	= srclinetest64

all: all32 all64

all32: $(OBJ32)
	$(CC) $(OBJ32) -o $(NAME32) $(LDFLAGS32)

all64: $(OBJ64)
	$(CC) $(OBJ64) -o $(NAME64) $(LDFLAGS64)

fclean: clean
	$(RM) $(NAME32) $(NAME64) srclinetest*

clean:
	$(RM) $(OBJ32) $(OBJ64) *~

%.32.o:	%.c
	$(CC) -c $< -o $@ $(CFLAGS32) -g

%.64.o:	%.c
	$(CC) -c $< -o $@ $(CFLAGS64) -g