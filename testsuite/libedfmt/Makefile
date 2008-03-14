SRCS = test_usage.c

OBJS = test_usage.o

CFLAGS = -I../../libedfmt/include	\
	-I../../libaspect/include	\
	-I../../libelfsh/include	\
	-DERESI32			\
	-ggdb

LDFLAGS = -L../../libedfmt/ -ledfmt32	\
	-L../../libelfsh -lelfsh32	\
	-L../../libaspect -laspect32	\
	-L../../libasm/ -lasm		\
	-L../../libetrace -letrace32

all:	${OBJS}
	${CC} -o main ${OBJS} ${LDFLAGS}

clean:
	rm -f ${OBJS}
