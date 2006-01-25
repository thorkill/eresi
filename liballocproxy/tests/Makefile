
NAME	= malloc-e2dbg
CC	?= gcc

all:
	$(CC) -fPIC libtest.c -Iinclude -c 
	$(CC) libtest.o -L. -shared -o libtest.so -L../ -l$(NAME)
	$(CC) -fPIC test-threads.c -Iinclude -L. -ltest -lpthread -L../ -l$(NAME)

clean: 
	rm -f *.o *~ 

fclean: clean
	rm -f *.so a.out
