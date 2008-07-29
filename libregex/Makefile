##
## Makefile.am for libaspect in elfsh
## 
## Started on  Fri Dec 22 16:32:29 2006 jfv
##

include ../config.h

CC		?= gcc
LD		= ld
RM		= rm -f 
AR		= ar rc
CFLAGS	        += -Iinclude -fPIC -g3 -O2 -DERESI32
SRC		= regex.c
OBJ		= $(SRC:.c=.o)
NAME		= libregex

all		: $(OBJ)
		@$(AR) $(NAME).a $(OBJ)
		@$(RANLIB) $(NAME).a

clean		: 
		@$(RM) \#* *\# *~ $(OBJ) .\#* include/\#* \
		include/*\# include/*~ include/.\#*

fclean		: clean
		@$(RM) *.a
