#
#
#
#
#
#

NAME	= malloc-e2dbg
CC	?= gcc
BINAME	= test-multithread

all:
	$(CC) -fPIC libtest.c -Iinclude -c 
	$(CC) -g3 libtest.o -L. -shared -o libtest.so -L../ -l$(NAME)
	$(CC) -g3 -fPIC test-threads.c -Iinclude -L. -ltest -lpthread -L../ -l$(NAME) -o $(BINAME)

clean: 
	rm -f *.o *~ 

fclean: clean
	rm -f *.so $(BINAME)
