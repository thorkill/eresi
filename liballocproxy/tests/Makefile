
NAME	= malloc-e2dbg

all:
	$(CC) -fPIC -g3 -gstabs+ libtest.c -Iinclude -L. -ltest -shared -o libtest.so -L../ -l$(NAME)
	$(CC) -fPIC -g3 -gstabs+ test-threads.c -Iinclude -L. -ltest -lpthread -L../ -l$(NAME)

clean: 
	rm -f *.o *~ 

fclean: clean
	rm -f *.so a.out
