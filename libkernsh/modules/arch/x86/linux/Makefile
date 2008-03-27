RELEASE	:= $(shell uname -r | sed -e s/2.6.*/2.6/ -e s/2.4.*/2.4/)
CP	:= cp

ifeq ($(RELEASE),2.6)

EXTRA_CFLAGS	:= 	-g3 -Wall -I/usr/include -I$(src)/../../../../include \
			-I$(src)/../include -I$(src)/../../../../kernel/include \
			-DERESI32 -Wno-strict-prototypes

EXTRAOBJ	:= ../../../../kernel/libkernsh-kernel.o

kernsh-virtm-linux-y	:= kernsh-virtm.o $(EXTRAOBJ)

obj-m	:= kernsh-virtm-linux.o 

all:
	$(MAKE) -C /lib/modules/`uname -r`/build M=`pwd`/
	$(CP) kernsh-virtm-linux.ko ../../../

clean:
	@$(RM) *.o *~ \#* *\# .\#* *.symvers *.mod.c Module.* .*.o.cmd .*.ko.cmd
	@$(RM) include/*~ include/\#* include/*\# include/.\#*
	@$(RM) -fr .tmp_versions*

fclean: clean
	@$(RM) *.ko
endif
