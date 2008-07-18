##
## Makefile for libstderesi in ERESI
##
## Started on  Fri Nov  2 16:32:29 2001 jfv
## $Id
##

include ../config.h

OBJ32			= *.32.o
OBJ64			= *.64.o
CC			?= gcc
RM			= rm -f
NAME32			= libstderesi32
NAME64			= libstderesi64

LDFLAGS32		+= -L../libedfmt/ -ledfmt32 -L../libelfsh/ -lelfsh32 	  \
			-L../libdump/ -L/usr/pkg/lib/ 				  \
			-L../libmjollnir/ -lmjollnir32 $(LPTHREAD)       	  \
			-L../libasm/ -lasm32 $(LDUIOPT32) -g3		          \
			-L../liballocproxy -L../libmjollnir/                      \
			-L../libaspect/ -laspect32 -lallocproxy $(LDMJOLLNIR)     \
			-L../libetrace/ -letrace32 -L../librevm -lrevm32          \
			$(KERNSHLIB32)                                            \
			-L/usr/freeware/lib32/ $(DYNOPT) $(BITS) $(EXTRAOPT) 	  \
			$(LDDUMPOPT) $(LDNCURSESLNOPT) $(LDREADLNOPT) $(DLOPT)

LDFLAGS64		+= -L../libedfmt/ -ledfmt64 -L../libelfsh/ -lelfsh64 	  \
			-L../libdump/ -L/usr/pkg/lib/ -L../libmjollnir/           \
                        -lmjollnir64 $(LPTHREAD) -L../libasm/ -lasm64             \
			-L../liballocproxy -L../libmjollnir/ $(LDUIOPT64)         \
			-L../libaspect/ -laspect64 -lallocproxy $(LDMJOLLNIR)     \
			-L../libetrace/ -letrace64 -L../librevm -lrevm64          \
			$(KERNSHLIB64) -g3					  \
			-L/usr/freeware/lib32/ $(DYNOPT) $(BITS) $(EXTRAOPT) 	  \
			$(LDDUMPOPT) $(LDNCURSESLNOPT) $(LDREADLNOPT) $(DLOPT)

all			: all32 all64
all32			: mod32 lib32
all64			: mod64 lib64

mod32			:
			$(MAKE) -C elf  all32
			$(MAKE) -C cmd  all32
			$(MAKE) -C type all32
			$(MAKE) -C lang all32
			$(MAKE) -C analysis all32
			$(MAKE) -C files all32

$(NAME32).so		:
			$(CC) -shared $(OBJ32) -o $(NAME32).so $(LDFLAGS32) \
			$(STATOPT) $(NCURSESLNOPT) $(READLNOPT)
			ar rc ${NAME32}.a ${OBJ32} 
			@$(RANLIB) ${NAME32}.a
			$(LD) -r $(OBJ32) -o $(NAME32).o
			@echo LINKED LIBSTDERESI32

lib32: $(NAME32).so
lib64: $(NAME64).so

mod64			:
			$(MAKE) -C elf  all64
			$(MAKE) -C cmd  all64
			$(MAKE) -C type all64
			$(MAKE) -C lang all64
			$(MAKE) -C analysis all64
			$(MAKE) -C files all64

$(NAME64).so			:
			$(CC) -shared $(OBJ64) -o $(NAME64).so $(LDFLAGS64) \
			$(STATOPT) $(NCURSESLNOPT) $(READLNOPT)
			ar rc ${NAME64}.a ${OBJ64} 
			ranlib ${NAME64}.a
			$(LD) -r $(OBJ64) -o $(NAME64).o
			@echo LINKED LIBSTDERESI64

clean		       :
			$(MAKE) -C elf  clean
			$(MAKE) -C cmd  clean
			$(MAKE) -C type clean
			$(MAKE) -C lang clean
			$(MAKE) -C files clean
			$(MAKE) -C analysis clean
		        @$(RM) *~ \#* *\# .\#*
		        @$(RM) include/*~ include/\#* include/*\# include/.\#*

fclean		       : clean
		       @$(RM) *.o *.so *.a $(OBJ_DYN)

docs:
			doxygen doc/doxygen.conf
