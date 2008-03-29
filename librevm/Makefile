##
## Makefile for librevm in ERESI
##
## Started on  Fri Nov  2 16:32:29 2001 jfv
## Updated on  Wed Nov 28 12:57:38 2001 jfv
##

include ../config.h

OBJ32			= *.32.o
OBJ64			= *.64.o
CC			?= gcc
RM			= rm -f
NAME32			= librevm32
NAME64			= librevm64

LDFLAGS32		+= -L../libedfmt/ -ledfmt32 -L../libelfsh/ -lelfsh32 	  \
			-L../libdump/ -L/usr/pkg/lib/ 				  \
			-L../libmjollnir/ -lmjollnir32 $(LPTHREAD)       	  \
			-L../libasm/ -lasm $(LDUIOPT32) -g3		          \
			$(LDASMOPT) -L../liballocproxy -L../libmjollnir/          \
			-L../libaspect/ -laspect32 -lallocproxy $(LDMJOLLNIR)     \
			-L../libetrace/ -letrace32				  \
			$(KERNSHLIB32)                                            \
			-L/usr/freeware/lib32/ $(DYNOPT) $(BITS) $(EXTRAOPT) 	  \
			$(LDDUMPOPT) $(LDNCURSESLNOPT) $(LDREADLNOPT) $(DLOPT)

LDFLAGS64		+= -L../libedfmt/ -ledfmt64 -L../libelfsh/ -lelfsh64 	  \
			-L../libdump/ -L/usr/pkg/lib/ 				  \
			-L../libmjollnir/ -lmjollnir64 $(LPTHREAD)      	  \
			-L../libasm/ -lasm $(LDUIOPT64) -g3		          \
			$(LDASMOPT) -L../liballocproxy -L../libmjollnir/          \
			-L../libaspect/ -laspect64 -lallocproxy $(LDMJOLLNIR)     \
			-L../libetrace/ -letrace64				  \
			$(KERNSHLIB64)						  \
			-L/usr/freeware/lib32/ $(DYNOPT) $(BITS) $(EXTRAOPT) 	  \
			$(LDDUMPOPT) $(LDNCURSESLNOPT) $(LDREADLNOPT) $(DLOPT)

all			: all32 all64
all32			: mod32 lib32
all64			: mod64 lib64

mod32			:
			$(MAKE) -C lang all32
			$(MAKE) -C vm   all32
			$(MAKE) -C io   all32
			$(MAKE) -C api  all32

$(NAME32).so			:
			$(CC) -shared $(OBJ32) -o $(NAME32).so $(LDFLAGS32) \
			$(STATOPT) $(NCURSESLNOPT) $(READLNOPT)
			ar rc ${NAME32}.a ${OBJ32} 
			@$(RANLIB) ${NAME32}.a
			@echo LINKED LIBREVM32

lib32: $(NAME32).so

mod64			:
			$(MAKE) -C lang all64
			$(MAKE) -C vm   all64
			$(MAKE) -C io   all64
			$(MAKE) -C api  all64

lib64			:
			$(CC) -shared $(OBJ64) -o $(NAME64).so $(LDFLAGS64) \
			$(STATOPT) $(NCURSESLNOPT) $(READLNOPT)
			ar rc ${NAME64}.a ${OBJ64} 
			ranlib ${NAME64}.a
			@echo LINKED LIBREVM64

clean		       :
			$(MAKE) -C lang clean
			$(MAKE) -C vm   clean
			$(MAKE) -C io   clean
			$(MAKE) -C api  clean
		        @$(RM) *~ \#* *\# .\#*
		        @$(RM) include/*~ include/\#* include/*\# include/.\#*

fclean		       : clean
		       @$(RM) *.o *.so *.a $(OBJ_DYN)

docs:
			doxygen doc/doxygen.conf
