SRCS = test_usage.c
OBJS = test_usage.o
NAME = main

CFLAGS = -I../../../libedfmt/include	\
	-I../../../libaspect/include	\
	-I../../../libelfsh/include	\
	-DERESI32			\
	-gdwarf-2

LDFLAGS = -L../../../libedfmt/ -ledfmt32	\
	-L../../../libelfsh -lelfsh32	\
	-L../../../libaspect -laspect32	\
	-L../../../libasm/ -lasm32		\
	-L../../../libetrace -letrace32 -g-dwarf2

RM	= rm -fr
CC	?= gcc

all32: all
all:	${OBJS}
	${CC} -o $(NAME) ${OBJS} ${LDFLAGS}

clean:
	rm -f ${OBJS}

fclean:	clean
	$(RM) $(NAME)