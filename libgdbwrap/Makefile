SRC       = gdbwrapper.c 
OBJ       = $(SRC:.c=.o)
CFLAGS    = -Iinclude -Wall -g3 -std=c89 -ansi -Werror
NAME      = libgdbwrap

CC      ?= gcc
LD      ?= ld

all     : all32 

all32:  $(OBJ)
	$(LD) -r $(OBJ) -o $(NAME).o $(LDFLAGS)

clean   :
	@rm -f $(OBJ32) $(OBJ64) *~ *\# \#* .\#* 
	@rm -f include/*~ include/*\# include/\#* include/.\#* 

fclean  : clean
	@rm -f $(NAME).a $(NAME).o 

%.o     : %.c
	$(CC) $(CFLAGS) -c -o $@ $<

