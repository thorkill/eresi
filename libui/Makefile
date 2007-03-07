##
## Makefile for libui
##
## Started on  Sun Jan  22 18:12:29 2006 mxatone
##

include ../config.h

SRC 		= readln.c
OBJ	 	= ${SRC:.c=.o}
OBJ32	 	= ${SRC:.c=.32.o}
OBJ64	 	= ${SRC:.c=.64.o}
NAME	 	= libui
NAME32	 	= libui32
NAME64	 	= libui64

CFLAGS32	+= -fPIC -g3 -gstabs+ -Wall -Iinclude $(DYNOPT) $(BUILDOP) 	\
		-I../libelfsh/include/ -I../elibc/include/ -I../libdump/include/ \
		-I../libasm/include/  -I../e2dbg/include/ -I /usr/pkg/include/  \
		-I../elfsh/include/ -I../libmjollnir/include/ 			\
		-I../libedfmt/include/ -DELFSH_INTERN 				\
		-I../libaspect/include/ $(STATOPT2) $(ASMOPT) -DELFSH32		\
		-I../librevm/include/ $(NCURSESLNOPT) $(READLNOPT)

CFLAGS64	+= -fPIC -g3 -gstabs+ -Wall -Iinclude $(DYNOPT) $(BUILDOP) 	\
		-I../libelfsh/include/ -I../elibc/include/ -I../libdump/include/ \
		-I../libasm/include/  -I../e2dbg/include/ -I /usr/pkg/include/  \
		-I../elfsh/include/ -I../libmjollnir/include/			\
		-I../libedfmt/include/ -DELFSH_INTERN 				\
		-I../libaspect/include/ $(STATOPT2) $(ASMOPT) -DELFSH64		\
		-I../librevm/include/ $(NCURSESLNOPT) $(READLNOPT)

LDFLAGS32	+= -L../libelfsh/ -lelfsh32 -L../libdump/ -L/usr/pkg/lib/ 	\
		$(LPTHREAD) -L../elibc -lcelfsh32 -L../libasm/ -lasm 		\
		-L../liballocproxy -lallocproxy -g3 $(LDASMOPT) 		\
		$(DLOPT)  -L/usr/freeware/lib32/ $(DYNOPT) $(BITS) $(EXTRAOPT)  \
		$(LDDUMPOPT) $(LDNCURSESLNOPT) $(LDREADLNOPT)

LDFLAGS64	+= -L../libelfsh/ -lelfsh64 -L../libdump/ -L/usr/pkg/lib/ 	\
		$(LPTHREAD) -L../elibc -lcelfsh64 -L../libasm/ -lasm 		\
		-L../liballocproxy -lallocproxy -g3 $(LDASMOPT) 		\
		$(DLOPT)  -L/usr/freeware/lib32/ $(DYNOPT) $(BITS) $(EXTRAOPT)  \
		$(LDDUMPOPT) $(LDNCURSESLNOPT) $(LDREADLNOPT)

RM 		= rm -f
ETAGS 		= etags
AR		= ar rc
LD		= gcc

all		: all32 all64

all32		: $(OBJ32)
		@$(LD) $(LDFLAGS32) $(BITS) -shared $(OBJ32) -o $(NAME32).so
		@$(AR) $(NAME32).a $(OBJ32)
		@$(RANLIB) $(NAME32).a

all64		: $(OBJ64)
		@$(LD) $(LDFLAGS64) $(BITS) -shared $(OBJ64) -o $(NAME64).so
		@$(AR) $(NAME64).a $(OBJ64)
		@$(RANLIB) $(NAME64).a

clean		: 
		@$(RM) \#* *\# *~ .\#* include/\#* include/*\#  \
		$(OBJ) $(OBJ32) $(OBJ64) include/*~ include/.\#*

tags		:
		@$(ETAGS) -a *.c include/*.h *.c
		@echo TAGS generated succesfully

fclean		: clean
		@$(RM) *.so *.a *.o

%.32.o : %.c
			$(CC) $(CFLAGS32) -c -o $@ $<
%.64.o : %.c
			$(CC) $(CFLAGS64) -c -o $@ $<
