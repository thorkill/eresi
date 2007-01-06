##
## Makefile for elfsh
##
## Started on  Fri Nov  2 16:32:29 2001 mayhem
##

include ../config.h

OBJ32			= lang/*.32.o elf/*.32.o vm/*.32.o cmd/*.32.o
OBJ64			= lang/*.64.o elf/*.64.o vm/*.64.o cmd/*.64.o
CC			?= gcc
RM			= rm -f

NAME32			= librevm32
NAME64			= librevm64

LDFLAGS32		+= -L../libelfsh/ -lelfsh32 -L../libdump/ -L/usr/pkg/lib/ \
			-L../libmjollnir/ -lmjollnir32 $(LPTHREAD) -L../elibc 	  \
			-lcelfsh32 -L../libasm/ -lasm -L../libui/ -lui32 -g3 	  \
			$(LDASMOPT) -L../liballocproxy -L../libmjollnir/          \
			-L../libaspect/ -laspect32 -lallocproxy $(LDMJOLLNIR)     \
			-L/usr/freeware/lib32/ $(DYNOPT) $(BITS) $(EXTRAOPT) 	  \
			$(LDDUMPOPT) $(LDNCURSESLNOPT) $(LDREADLNOPT) $(DLOPT)

LDFLAGS64		+= -L../libelfsh/ -lelfsh64 -L../libdump/ -L/usr/pkg/lib/ \
			-L../libmjollnir/ -lmjollnir64 $(LPTHREAD) -L../elibc 	  \
			-lcelfsh64 -L../libasm/ -lasm -L../libui/ -lui64 -g3 	  \
			$(LDASMOPT) -L../liballocproxy -L../libmjollnir/          \
			-L../libaspect/ -laspect64 -lallocproxy $(LDMJOLLNIR)     \
			-L/usr/freeware/lib32/ $(DYNOPT) $(BITS) $(EXTRAOPT) 	  \
			$(LDDUMPOPT) $(LDNCURSESLNOPT) $(LDREADLNOPT) $(DLOPT)

all			: all32 all64

all32			:
			gmake -C lang all32
			gmake -C elf  all32
			gmake -C vm   all32
			gmake -C cmd  all32
			$(CC) -shared $(OBJ32) -o $(NAME32).so $(LDFLAGS32) $(STATOPT)
			@ar rc ${NAME32}.a ${OBJ32} 
			@ranlib ${NAME32}.a

all64			:
			gmake -C lang all64
			gmake -C elf  all64
			gmake -C vm   all64
			gmake -C cmd  all64
			$(CC) -shared $(OBJ64) -o $(NAME64).so $(LDFLAGS64) $(STATOPT)
			@ar rc ${NAME64}.a ${OBJ64} 
			@ranlib ${NAME64}.a

clean		       :
			gmake -C lang clean
			gmake -C elf  clean
			gmake -C vm   clean
			gmake -C cmd  clean
		       @$(RM) *~ \#* *\# .\#*
		       @$(RM) include/*~ include/\#* include/*\# include/.\#*

fclean		       : clean
		       @$(RM) *.so *.a $(OBJ_DYN)
