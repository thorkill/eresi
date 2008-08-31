
SRC	= test-ptrfn1.c test-ptrfn2.c test-ptrfn3.c test-recur.c test-moredepth.c
OBJ	= $(SRC:.c=.o)
CC	?= gcc
RM	= rm -fr

all32: all
all: $(OBJ)

test-ptrfn1.o: 
	$(CC) test-ptrfn1.c -o test-ptrfn1.elf

test-ptrfn2.o: 
	$(CC) test-ptrfn2.c -o test-ptrfn2.elf

test-ptrfn3.o: 
	$(CC) test-ptrfn3.c -o test-ptrfn3.elf

test-recur.o:
	$(CC) test-recur.c -o test-recur.elf

test-moredepth.o:
	$(CC) test-moredepth.c -o test-moredepth.elf

fclean: clean
clean:
	$(RM) $(OBJ) test*.o test*elf *~