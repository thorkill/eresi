##
## Makefile for etrel_static in ERESI
##
## Started on  Sun Mar 10 01:19:24 2002 jfv
## $Id$
##

include ../../../../config.h

CC	= gcc
RM	= rm -f 
VULN	= host.c
RELSRC	= rel.c

RELOBJ32  = $(RELSRC:.c=.32.o)
RELOBJ64  = $(RELSRC:.c=.64.o)
CFLAGS32  =$(shell eresi-config32 libelfsh --cflags)
LDFLAGS32 =$(shell eresi-config32 libelfsh --libs)
CFLAGS64  =$(shell eresi-config64 libelfsh --cflags)
LDFLAGS64 =$(shell eresi-config64 libelfsh --libs)
NAME32	  = relinject32
NAME64	  = relinject64

all: all32 all64

all32: $(RELOBJ32)
	$(CC) $(VULN) -o hijackme32

all64: $(RELOBJ64)
	$(CC) -m64 $(VULN) -o hijackme64

fclean: clean
	$(RM) $(NAME32) $(NAME64) hijackme* fake_aout*

clean:
	$(RM) $(RELOBJ32) $(RELOBJ64) *~

%.32.o:	%.c
	$(CC) -c $< -o $@ $(CFLAGS32) -g

%.64.o:	%.c
	$(CC) -c $< -o $@ $(CFLAGS64) -g
