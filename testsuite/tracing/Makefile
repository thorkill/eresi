##
## Makefile for elfsh
## 
## Started on  Sun Mar 10 01:19:24 2002 jfv
## Last update Sun March 21 23:15:15 2007 mxatone
##
include ../../config.h

SRC 	= tracer.c
OBJ32	= $(SRC:.c=.32.o)
OBJ64	= $(SRC:.c=.64.o)
HOSTSRC	  = host.c
HOSTOBJ32 = host.32.o
HOSTNAM32 = host32
HOSTOBJ64 = host.64.o
HOSTNAM64 = host64
HOSTLIB = -lcrypt
NAM32	= tracer32
NAM64	= tracer64
RM	= rm -f 
CC	= gcc

LIB32    =$(shell eresi-config32 libetrace libstderesi --libs)
LIB64	 =$(shell eresi-config64 libetrace libstderesi --libs)
CFLAGS32 =$(shell eresi-config32 libetrace libstderesi --cflags) #-gdwarf-2
CFLAGS64 =$(shell eresi-config64 libetrace libstderesi --cflags) #-gdwarf-2

all: all32 all64

all32	: $(OBJ32)
	$(CC) $(HOSTSRC) -o $(HOSTNAM32) $(HOSTLIB) 
	$(CC) $(OBJ32) -o $(NAM32) $(LIB32) 

all64	: $(OBJ64)
	$(CC) $(HOSTSRC) -o $(HOSTNAM64) $(HOSTLIB) 
	$(CC) $(OBJ64) -o $(NAM64) $(LIB64) 

clean	:
	$(RM) $(OBJ32) $(HOSTOBJ32) $(OBJ64) $(HOSTOBJ64) *~ \#* *\#

fclean	: clean
	$(RM) $(NAM32) $(NAM64) $(HOSTNAM32) $(HOSTNAM64)

%.32.o:	%.c
	$(CC) -c $< -o $@ $(CFLAGS32) -g

%.64.o:	%.c
	$(CC) -c $< -o $@ $(CFLAGS64) -g

