##
## Makefile for libui
##
## Started on  Sun Jan  22 18:12:29 2006 mxatone
##

include ../config.h

SRC 		= io.c color.c filter.c job.c lexer.c log.c network.c readln.c
OBJ	 	= ${SRC:.c=.o}
OBJ32	 	= ${SRC:.c=.32.o}
OBJ64	 	= ${SRC:.c=.64.o}
NAME	 	= libui
NAME32	 	= libui32
NAME64	 	= libui64

CFLAGS		+= -fPIC -g3 -gstabs+ -Wall -Iinclude $(DYNOPT) $(BUILDOP) 	\
		-I../libelfsh/include/ -I../libc/include/ -I../libdump/include/ \
		-I../libasm/include/  -I../e2dbg/include/ -I /usr/pkg/include/  \
		-I../vm/include/ -I../libmjollnir/include/ -DELFSH_INTERN 	\
		-I../libaspect/include/ $(STATOPT2) $(ASMOPT) $(BITS) 		\
		$(NCURSESLNOPT) $(READLNOPT)

CFLAGS32	+= -fPIC -g3 -gstabs+ -Wall -Iinclude $(DYNOPT) $(BUILDOP) 	\
		-I../libelfsh/include/ -I../libc/include/ -I../libdump/include/ \
		-I../libasm/include/  -I../e2dbg/include/ -I /usr/pkg/include/  \
		-I../vm/include/ -I../libmjollnir/include/ -DELFSH_INTERN 	\
		-I../libaspect/include/ $(STATOPT2) $(ASMOPT) -DELFSH32		\
		$(NCURSESLNOPT) $(READLNOPT)

CFLAGS64	+= -fPIC -g3 -gstabs+ -Wall -Iinclude $(DYNOPT) $(BUILDOP) 	\
		-I../libelfsh/include/ -I../libc/include/ -I../libdump/include/ \
		-I../libasm/include/  -I../e2dbg/include/ -I /usr/pkg/include/  \
		-I../vm/include/ -I../libmjollnir/include/ -DELFSH_INTERN 	\
		-I../libaspect/include/ $(STATOPT2) $(ASMOPT) -DELFSH64		\
		$(NCURSESLNOPT) $(READLNOPT)

LDFLAGS		+= -L../libelfsh/ -lelfsh -L../libdump/ -L/usr/pkg/lib/ 	\
		$(LPTHREAD) -L../libc -lcelfsh -L../libasm/ -lasm 		\
		-L../libmalloc -lmalloc-e2dbg -g3 $(LDASMOPT) 			\
		$(DLOPT)  -L/usr/freeware/lib32/ $(DYNOPT) $(BITS) $(EXTRAOPT)  \
		$(LDDUMPOPT) $(LDNCURSESLNOPT) $(LDREADLNOPT)

LDFLAGS32	+= -L../libelfsh/ -lelfsh32 -L../libdump/ -L/usr/pkg/lib/ 	\
		$(LPTHREAD) -L../libc -lcelfsh32 -L../libasm/ -lasm 		\
		-L../libmalloc -lmalloc-e2dbg -g3 $(LDASMOPT) 			\
		$(DLOPT)  -L/usr/freeware/lib32/ $(DYNOPT) $(BITS) $(EXTRAOPT)  \
		$(LDDUMPOPT) $(LDNCURSESLNOPT) $(LDREADLNOPT)

LDFLAGS64	+= -L../libelfsh/ -lelfsh64 -L../libdump/ -L/usr/pkg/lib/ 	\
		$(LPTHREAD) -L../libc -lcelfsh64 -L../libasm/ -lasm 		\
		-L../libmalloc -lmalloc-e2dbg -g3 $(LDASMOPT) 			\
		$(DLOPT)  -L/usr/freeware/lib32/ $(DYNOPT) $(BITS) $(EXTRAOPT)  \
		$(LDDUMPOPT) $(LDNCURSESLNOPT) $(LDREADLNOPT)

RM 		= rm -f
ETAGS 		= etags
AR		= ar rc
LD		= gcc

all2		: all32 all64

all		: $(OBJ)
		@$(LD) $(LDFLAGS) $(BITS) -shared $(OBJ) -o $(NAME).so
		@$(AR) $(NAME).a $(OBJ)
		@$(RANLIB) $(NAME).a

all32		: $(OBJ32)
		@$(LD) $(LDFLAGS32) $(BITS) -shared $(OBJ32) -o $(NAME32).so
		@$(AR) $(NAME32).a $(OBJ32)
		@$(RANLIB) $(NAME32).a

all64		: $(OBJ64)
		@$(LD) $(LDFLAGS64) $(BITS) -shared $(OBJ64) -o $(NAME64).so
		@$(AR) $(NAME64).a $(OBJ64)
		@$(RANLIB) $(NAME64).a

clean		: 
		@$(RM) \#* *\# *~ *.o .\#* include/\#* include/*\#  \
		include/*~ include/.\#*

tags		:
		@$(ETAGS) -a *.c include/*.h *.c
		@echo TAGS generated succesfully

fclean		: clean
		@$(RM) $(NAME).so $(NAME).a
		@$(RM) $(NAME32).so $(NAME32).a
		@$(RM) $(NAME64).so $(NAME64).a

%.32.o : %.c
			$(CC) $(CFLAGS32) -c -o $@ $<
%.64.o : %.c
			$(CC) $(CFLAGS64) -c -o $@ $<
