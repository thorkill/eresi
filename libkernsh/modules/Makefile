EXTRA_CFLAGS	:= 	-g3 -Wall -I/usr/include -I$(src)/include \
			-I$(src)/../include -I$(src)/../kernel/include \
			-DERESI32 -Wno-strict-prototypes

EXTRAOBJ	:= ../kernel/libkernsh-kernel.o

kernsh-virtm-linux-y	:= kernsh-virtm.o $(EXTRAOBJ)

obj-m	:= kernsh-virtm-linux.o 

all:
	$(MAKE) -C /lib/modules/`uname -r`/build M=`pwd`/

clean:
	@$(RM) *.o *~ \#* *\# .\#* *.symvers *.mod.c
	@$(RM) include/*~ include/\#* include/*\# include/.\#*

fclean: clean
	@$(RM) *.ko
