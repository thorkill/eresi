RELEASE := $(shell uname -r | sed -e s/2.6.*/2.6/ -e s/2.4.*/2.4/)
CP	= cp

ifeq ($(RELEASE),2.6)

EXTRA_CFLAGS	:=	-g3 -Wall -I/usr/include -I$(src)/../../../include \
			-I$(src)/../../../../include \
			-DERESI32 -Wno-strict-prototypes


libkernsh-kernel-y	:= page.o virtm.o mem.o misc.o

obj-m		:= libkernsh-kernel.o

all:
	$(MAKE) -C /lib/modules/`uname -r`/build M=`pwd`/
	$(CP) libkernsh-kernel.o ../../../

clean:
	@$(RM) *.o *~ \#* *\# .\#* *.mod.c Module.* .*.o.cmd .*.ko.cmd
	@$(RM) include/*~ include/\#* include/*\# include/.\#*
	@$(RM) -fr .tmp_versions*

fclean: clean
	@$(RM) *.ko
endif
