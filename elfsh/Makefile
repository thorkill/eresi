##
## Makefile for elfsh
##
## Started on  Fri Nov  2 16:32:29 2001 mayhem
## Last update Thu Aug 21 03:23:17 2003 mayhem
##



# Change here to link in static
#STATOPT		= -static
#DLOPT			= -DUSE_STATIC
#DLOPT_linux-gnu	= -ldl
#DLOPT_solaris		= -ldl
#EXTRAOPT		= -lefence

# Uncomment that to use libasm
#ASMOPT			= -DUSE_LIBASM
#LDASMOPT		= -lasm

# Uncommant that to use libreadline
#READLNOPT		= -DUSE_READLN
#LDREADLNOPT		= -lreadline -ltermcap


# End of configurable Makefile


SRC			= grammar.c disasm.c dyn.c hdr.c main.c got.c interp.c \
			  notes.c pht.c rel.c sht.c stab.c sym.c quit.c data.c \
			  ctors.c dtors.c fileops.c comment.c save.c load.c \
			  unload.c list.c doswitch.c tables.c help.c info.c \
			  readln.c print.c lexer.c misc.c atomic.c opt.c \
			  implicit.c exec.c modules.c findrel.c objects.c \
			  cmdapi.c filter.c reladd.c fcthijack.c
OBJ			= $(SRC:.c=.o)
SRC_DYN			= elfsh_etdyn.c
OBJ_DYN			= $(SRC_DYN:.c=.o)
CC			= gcc
RM			= rm -f
NAME			= elfsh

CFLAGS			= -rdynamic -fPIC -g3 -O2 -Wall -Iinclude \
			-I../libhash/include/ -I../libelfsh/include/ \
			-DELFSH_INTERN $(DLOPT) $(ASMOPT) $(READLNOPT)

LDFLAGS			= -L../libelfsh/ -lelfsh -L../libhash/ -lhashelfsh \
			-rdynamic -g3 $(LDASMOPT) $(LDREADLNOPT) $(DLOPT)\
			$(EXTRAOPT)


all		       	: $(OBJ)
			$(CC) $(OBJ) -o $(NAME) $(LDFLAGS) $(STATOPT)

et_dyn			: $(OBJ) $(OBJ_DYN)
			$(CC) -shared $(OBJ) $(OBJ_DYN) -o $(NAME) $(LDFLAGS)

clean		       :
		       @$(RM) $(OBJ) *~ \#* *\# .\#*
		       @$(RM) include/*~ include/\#* include/*\# include/.\#*

fclean		       : clean
		       @$(RM) $(NAME)


