## 
## Makefile for static instrumentation flowjack entry in the ERESI testsuite
##
## Made by vanegue at eresi-project dot org
## June 21st 2009
##

CC	= gcc
RM	= rm -fr
TNAM32	= target32
RNAM32	= recur32
TNAM64	= target64
RNAM64	= recur64

all: all32 all64

all32: infected.32.o target.32.o recursivite.32.o
	$(CC) -o $(TNAM32) target.32.o
	$(CC) -o $(RNAM32) recursivite.32.o

all64: infected.64.o target.64.o recursivite.64.o
	$(CC) -o $(TNAM64) target.64.o
	$(CC) -o $(RNAM64) recursivite.64.o

fclean: clean
	$(RM) $(TNAM32) $(RNAM32) $(TNAM64) $(RNAM64) *.hijacked

clean:
	$(RM) *.o *~ 

%.32.o:	%.c
	$(CC) -c $< -o $@ $(CFLAGS32) -g

%.64.o:	%.c
	$(CC) -c $< -o $@ $(CFLAGS64) -g

