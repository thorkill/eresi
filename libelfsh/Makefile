##
## Makefile.am
## 
## Started on  Fri Nov  2 16:32:29 2001 mayhem
## Last update Mon Feb 23 17:22:18 2004 mayhem
##

include ../config.h

CC			?= gcc
LD			= gcc
RM			= rm -f 
AR			= ar rc
CFLAGS			+= -Iinclude -Wall -fPIC -g3 -O2 -DELFSH_INTERN      \
			-I../libasm/include/ -I../libc/include $(BITS) $(BUILDOP) 
SRC			= dynamic.c dynsym.c elf.c fixup.c got.c hash.c libbtree.c \
			interp.c pht.c plt.c section.c sht.c error.c stab.c \
			symbol.c notes.c reloc.c blocks.c ctors.c dtors.c   \
			raw.c map.c strtab.c sht_rebuild.c comment.c        \
			sym_common.c hijack.c obj.c pax.c save.c copy.c     \
			strip.c remap.c relinject.c sanitize.c debug.c      \
			bss.c sort.c hooks.c ia32.c sparc32.c sparc64.c     \
			mips32.c alpha64.c ia64.c mips64.c reginfo.c        \
			altplt.c altgot.c extplt.c runtime.c state.c        \
			inject.c linkmap.c bp.c libhash.c rpht.c version.c
OBJ			= $(SRC:.c=.o)
NAME			= libelfsh

all			: $(OBJ)
			@$(LD) --trace-symbol=malloc -L../libc/ -lcelfsh -L../libmalloc/ -lmalloc-e2dbg \
			$(BITS) -shared $(OBJ) -o $(NAME).so
			@$(AR) $(NAME).a $(OBJ)
			@$(RANLIB) $(NAME).a


dumpregs.o:		dumpregs.c
			$(CC) $(CFLAGS) -fomit-frame-pointer $< -o $@ -c


clean			: 
			@$(RM) \#* *\# *~ *.o .\#* include/\#* include/*\#  \
			include/*~ include/.\#*

fclean			: clean
			@$(RM) $(NAME).so $(NAME).a

