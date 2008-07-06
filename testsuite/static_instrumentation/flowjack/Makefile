## 
## Makefile for ERESI testsuite
##
## -jfv 2008
##

CC	= gcc
RM	= rm -fr
NAM	= target

all32: all
all:
	$(CC) -c infected.c
	$(CC) -o $(NAM) target.c

fclean: clean
clean:
	$(RM) *.o *~ $(NAM)
