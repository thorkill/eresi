#
#
#

CC		?= gcc
NAME		= allocproxy
BINAME32	= debugme-mt
BINAME64	= debugme-mt64

all32: all

all:
	$(CC) -fPIC libtest.c -Iinclude -c 
	$(CC) -g3 libtest.o -L. -shared -o libtest.so -L../../../liballocproxy/ -l$(NAME)
	$(CC) -g3 -fPIC test-threads.c -Iinclude -L. -ltest -lpthread -L../../../liballocproxy \
	-l$(NAME) -o $(BINAME32)

all64: 
	$(CC) -fPIC libtest.c -Iinclude -c -m64
	$(CC) -g3 libtest.o -L. -shared -o libtest.so -L../../../liballocproxy/ -l$(NAME) -m64
	$(CC) -g3 -fPIC test-threads.c -Iinclude -L. -ltest -lpthread -L../../../liballocproxy \
	-l$(NAME) -o $(BINAME64) -m64

clean: 
	rm -f *.o *~ 

fclean: clean
	rm -f *.so $(BINAME32) $(BINAME64)
