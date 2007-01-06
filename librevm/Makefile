##
## Makefile for elfsh
##
## Started on  Fri Nov  2 16:32:29 2001 mayhem
##

include ../config.h

OBJ			= lang/*.o elf/*.o vm/*.o cmd/*.o
OBJ32			= lang/*.32.o elf/*.32.o vm/*.32.o cmd/*.32.o
OBJ64			= lang/*.64.o elf/*.64.o vm/*.64.o cmd/*.64.o
CC			?= gcc
RM			= rm -f
NAME			= librevm
NAME32			= librevm32
NAME64			= librevm64

LDFLAGS			+= -L../libelfsh/ -lelfsh -L../libdump/ -L/usr/pkg/lib/   \
			-L../libmjollnir/ -lmjollnir $(LPTHREAD) -L../libc 	  \
			-lcelfsh -L../libasm/ -lasm -L../libui/ -lui -g3 	  \
			$(LDASMOPT) -L../libmalloc -L../libmjollnir/ 		  \
			-L../libaspect/ -laspect -lmalloc-e2dbg $(LDMJOLLNIR)     \
			-L/usr/freeware/lib32/ $(DYNOPT) $(BITS) $(EXTRAOPT) 	  \
			$(LDDUMPOPT) $(LDNCURSESLNOPT) $(LDREADLNOPT) $(DLOPT)

LDFLAGS32		+= -L../libelfsh/ -lelfsh32 -L../libdump/ -L/usr/pkg/lib/ \
			-L../libmjollnir/ -lmjollnir32 $(LPTHREAD) -L../libc 	  \
			-lcelfsh32 -L../libasm/ -lasm -L../libui/ -lui32 -g3 	  \
			$(LDASMOPT) -L../libmalloc -L../libmjollnir/ 		  \
			-L../libaspect/ -laspect32 -lmalloc-e2dbg $(LDMJOLLNIR)   \
			-L/usr/freeware/lib32/ $(DYNOPT) $(BITS) $(EXTRAOPT) 	  \
			$(LDDUMPOPT) $(LDNCURSESLNOPT) $(LDREADLNOPT) $(DLOPT)

LDFLAGS64		+= -L../libelfsh/ -lelfsh64 -L../libdump/ -L/usr/pkg/lib/ \
			-L../libmjollnir/ -lmjollnir64 $(LPTHREAD) -L../libc 	  \
			-lcelfsh64 -L../libasm/ -lasm -L../libui/ -lui64 -g3 	  \
			$(LDASMOPT) -L../libmalloc -L../libmjollnir/ 		  \
			-L../libaspect/ -laspect64 -lmalloc-e2dbg $(LDMJOLLNIR)   \
			-L/usr/freeware/lib32/ $(DYNOPT) $(BITS) $(EXTRAOPT) 	  \
			$(LDDUMPOPT) $(LDNCURSESLNOPT) $(LDREADLNOPT) $(DLOPT)

all2			: all32 all64

all		       	: 
			make -C lang all
			make -C elf  all
			make -C vm   all
			make -C cmd  all
			$(CC) -shared $(OBJ) -o $(NAME).so $(LDFLAGS) $(STATOPT)
			@ar rc ${NAME}.a ${OBJ} 
			@ranlib ${NAME}.a

all32			:
			make -C lang all32
			make -C elf  all32
			make -C vm   all32
			make -C cmd  all32
			$(CC) -shared $(OBJ32) -o $(NAME32).so $(LDFLAGS32) $(STATOPT)
			@ar rc ${NAME32}.a ${OBJ32} 
			@ranlib ${NAME32}.a

all64			:
			make -C lang all64
			make -C elf  all64
			make -C vm   all64
			make -C cmd  all64
			$(CC) -shared $(OBJ64) -o $(NAME64).so $(LDFLAGS64) $(STATOPT)
			@ar rc ${NAME64}.a ${OBJ64} 
			@ranlib ${NAME64}.a

clean		       :
			make -C lang clean
			make -C elf  clean
			make -C vm   clean
			make -C cmd  clean
		       @$(RM) *~ \#* *\# .\#*
		       @$(RM) include/*~ include/\#* include/*\# include/.\#*

fclean		       : clean
		       @$(RM) *.so *.a $(OBJ_DYN)
