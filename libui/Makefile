##
## Makefile for libui
##
## Started on  Sun Jan  22 18:12:29 2006 mxatone
##

include ../config.h

SRC 		=	io.c color.c filter.c job.c lexer.c log.c network.c readln.c

OBJ	 	=	${SRC:.c=.o}
NAME	 	= 	libui

CFLAGS		+= 	-fPIC -g3 -gstabs+ -Wall -Iinclude $(DYNOPT) $(BUILDOP) \
			 -I../libelfsh/include/ -I../libc/include/ -I../libdump/include/ \
			 -I../libasm/include/  -I../e2dbg/include/ -I /usr/pkg/include/  \
			 -I../vm/include/ \
			-DELFSH_INTERN $(STATOPT2) $(ASMOPT) $(BITS) $(NCURSESLNOPT) $(READLNOPT)

LDFLAGS		+= 	-L../libelfsh/ -lelfsh -L../libdump/ -L/usr/pkg/lib/ \
			$(LPTHREAD) -L../libc -lcelfsh -L../libasm/ -lasm \
			-L../libmalloc -lmalloc-e2dbg -g3 $(LDASMOPT) \
			$(DLOPT)  -L/usr/freeware/lib32/ $(DYNOPT) $(BITS) $(EXTRAOPT) \
			$(LDDUMPOPT) $(LDNCURSESLNOPT) $(LDREADLNOPT)


RM 		= 	rm -f
ETAGS 		= 	etags
AR		=	ar rc
LD		=	gcc

all			: $(OBJ)
			@$(LD) $(LDFLAGS) $(BITS) -shared $(OBJ) -o $(NAME).so
			@$(AR) $(NAME).a $(OBJ)
			@$(RANLIB) $(NAME).a

clean			: 
			@$(RM) \#* *\# *~ *.o .\#* include/\#* include/*\#  \
			include/*~ include/.\#*

tags			:
			@$(ETAGS) -a *.c include/*.h *.c
			@echo TAGS generated succesfully

fclean			: clean
			@$(RM) $(NAME).so $(NAME).a

