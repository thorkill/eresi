#
#
#
#
#
#

include ../../../config.h

CC	?= gcc
NAME	= allocproxy
BINAME	= debugme-mt

all:
	$(CC) -fPIC libtest.c -Iinclude -c 
	$(CC) -g3 libtest.o -L. -shared -o libtest.so -L../../../liballocproxy/ -l$(NAME)
	$(CC) -g3 -fPIC test-threads.c -Iinclude -L. -ltest -lpthread -L../../../liballocproxy -l$(NAME) -o $(BINAME)

clean: 
	rm -f *.o *~ 

fclean: clean
	rm -f *.so $(BINAME)
