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
CFLAGS			+= -Iinclude -Wall -fPIC -g3 -O2 -DELFSH_INTERN     \
			-I../libasm/include/ -I../libc/include 		    \
			-I../libaspect/include/ $(BITS) $(BUILDOP) 
CFLAGS32		+= -Iinclude -Wall -fPIC -g3 -O2 -DELFSH_INTERN     \
			-I../libasm/include/ -I../libc/include 		    \
			-I../libaspect/include/ -DELFSH32 $(BUILDOP) 
CFLAGS64		+= -Iinclude -Wall -fPIC -g3 -O2 -DELFSH_INTERN     \
			-I../libasm/include/ -I../libc/include 		    \
			-I../libaspect/include/ -DELFSH64 $(BUILDOP) 
SRC			= dynamic.c dynsym.c elf.c fixup.c got.c hash.c     \
			interp.c pht.c plt.c section.c sht.c error.c stab.c \
			symbol.c notes.c reloc.c ctors.c dtors.c search.c   \
			raw.c map.c strtab.c sht_rebuild.c comment.c        \
			sym_common.c hijack.c obj.c pax.c save.c copy.c     \
			strip.c remap.c relinject.c sanitize.c debug.c      \
			bss.c sort.c hooks.c ia32.c sparc32.c sparc64.c     \
			mips32.c alpha64.c ia64.c mips64.c reginfo.c        \
			altplt.c altgot.c extplt.c runtime.c state.c        \
			inject.c linkmap.c bp.c rpht.c version.c            \
			config.c
OBJ			= $(SRC:.c=.o)
OBJ32			= $(SRC:.c=.32.o)
OBJ64			= $(SRC:.c=.64.o)
NAME			= libelfsh
NAME32			= libelfsh32
NAME64			= libelfsh64

all			: all32 all64

all64			:  $(OBJ64)
			$(LD) -L../libc/ -lcelfsh64 -L../libmalloc/ -lmalloc-e2dbg \
			-L../libaspect/ -laspect64 -DELFSH64 -shared $(OBJ64) 	 \
			-o $(NAME64).so
			@$(AR) $(NAME64).a $(OBJ64)
			@$(RANLIB) $(NAME64).a

all32			: $(OBJ32)
			$(LD) -L../libc/ -lcelfsh32 -L../libmalloc/ -lmalloc-e2dbg \
			-L../libaspect/ -laspect32 -DELFSH32 -shared $(OBJ32) 	 \
			-o $(NAME32).so
			@$(AR) $(NAME32).a $(OBJ32)
			@$(RANLIB) $(NAME32).a



dumpregs.o:		dumpregs.c
			$(CC) $(CFLAGS) -fomit-frame-pointer $< -o $@ -c


clean			: 
			@$(RM) \#* *\# *~ *.o .\#* include/\#* include/*\#  \
			include/*~ include/.\#*

fclean			: clean
			@$(RM) $(NAME).so $(NAME).a $(NAME32).so $(NAME32).a \
			$(NAME64).so $(NAME64).a

%.32.o : %.c
			$(CC) $(CFLAGS32) -c -o $@ $<
%.64.o : %.c
			$(CC) $(CFLAGS64) -c -o $@ $<

