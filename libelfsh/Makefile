##
## Makefile.am
## 
## Started on  Fri Nov  2 16:32:29 2001 jfv
## Last update Mon Feb 23 17:22:18 2004 jfv
##

include ../config.h

CC			?= gcc
LD			?= ld
RM			= rm -f 
AR			= ar rc
CFLAGS32		+= -Iinclude -Wall -fPIC -g3 -O2 $(EXTRACFLAGS) \
			-I../libasm/include/ -I../libaspect/include/    \
			$(KERNSHINC) $(KERNSH) -DERESI32 $(BUILDOP)

CFLAGS64		+= -Iinclude -Wall -fPIC -g3 -O2 $(EXTRACFLAGS) \
			-I../libasm/include/ -I../libaspect/include/    \
			$(KERNSHINC) $(KERNSH) -DERESI64 $(BUILDOP)

LDFLAGS32		= -L../liballocproxy/ -lallocproxy -L../libaspect/ \
			-laspect32 -L../libasm -lasm32 $(KERNSHLIB32)

LDFLAGS64		= -L../liballocproxy/ -lallocproxy -L../libaspect/ \
			-laspect64 -L../libasm -lasm64

SRC			= dynamic.c dynsym.c elf.c fixup.c got.c hash.c     \
			interp.c pht.c plt.c section.c sht.c stab.c         \
			symbol.c notes.c reloc.c ctors.c dtors.c search.c   \
			raw.c map.c strtab.c sht_rebuild.c comment.c        \
			sym_common.c hijack.c obj.c pax.c save.c copy.c     \
			strip.c remap.c relinject.c sanitize.c debug.c      \
			bss.c sort.c hooks.c ia32.c sparc32.c sparc64.c     \
			mips32.c alpha64.c ia64.c mips64.c reginfo.c        \
			altplt.c altgot.c extplt.c runtime.c state.c        \
			inject.c linkmap.c bp.c rpht.c version.c dwarf.c

OBJ32			= $(SRC:.c=.32.o)
OBJ64			= $(SRC:.c=.64.o)
NAME32			= libelfsh32
NAME64			= libelfsh64

all			: all32 all64


libelfsh32.so		: $(OBJ32)
			@$(CC) $(LDFLAGS32) -shared $(OBJ32) -o $(NAME32).so
			@$(AR) $(NAME32).a $(OBJ32)
			@$(RANLIB) $(NAME32).a
			@$(LD) -r $(OBJ32) -o $(NAME32).o

all32: libelfsh32.so


libelfsh64.so		:  $(OBJ64)
			@$(CC) $(LDFLAGS64) -shared $(OBJ64) -o $(NAME64).so 
			@$(AR) $(NAME64).a $(OBJ64)
			@$(RANLIB) $(NAME64).a
			@$(LD) -r $(OBJ64) -o $(NAME64).o

all64:	libelfsh64.so

clean			: 
			@$(RM) \#* *\# *~ *.o .\#* include/\#* include/*\#  \
			include/*~ include/.\#* include/libelfsh/\#*	    \
			include/libelfsh/*\# include/libelfsh/*~            \
			include/libelfsh/.\#*

fclean			: clean
			@$(RM) *.so *.a


docs:
			doxygen doc/doxygen.conf

%.32.o : %.c
			$(CC) $(CFLAGS32) -c -o $@ $<
%.64.o : %.c
			$(CC) $(CFLAGS64) -c -o $@ $<

