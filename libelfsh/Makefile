##
## Makefile.am
## 
## Started on  Fri Nov  2 16:32:29 2001 mayhem
## Last update Tue Jun  3 09:30:50 2003 mayhem
##

CC			= gcc
RM			= rm -f 
AR			= ar rc
CFLAGS			= -rdynamic -Iinclude -Wall -fPIC -g3 -O2 -DELFSH_INTERN
SRC			= dynamic.c dynsym.c elf.c fixup.c got.c hash.c \
                        interp.c pht.c plt.c section.c sht.c error.c stab.c \
			symbol.c notes.c reloc.c blocks.c ctors.c dtors.c \
			raw.c map.c strtab.c sht_rebuild.c comment.c \
			sym_common.c hijack.c obj.c pax.c save.c copy.c \
			strip.c remap.c relinject.c sanitize.c debug.c \
			bss.c sort.c
OBJ			= $(SRC:.c=.o)
NAME			= libelfsh
RANLIB			= ranlib

all			: $(OBJ)
			@$(CC) -shared $(OBJ) -o $(NAME).so #-lasm
			@$(AR) $(NAME).a $(OBJ)
			@$(RANLIB) $(NAME).a

clean			: 
			@$(RM) \#* *\# *~ *.o .\#* include/\#* include/*\# \
			include/*~ include/.\#*

fclean			: clean
			@$(RM) $(NAME).so $(NAME).a

