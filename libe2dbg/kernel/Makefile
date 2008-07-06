##
## Makefile for kernel part of libe2dbg in the ERESI framework
##
## Started on  Tue Aug  16 06:32:29 2005 jfv
## $Id: Makefile,v 1.1 2008-02-16 12:32:27 thor Exp $
##

ifneq ($(KERNELRELEASE),)

EXTRA_CFLAGS :=  -g3 -Wall -I/usr/include -I$(src)/include -I$(src)/../include \
		-I$(src)/../../libelfsh/include/ -I$(src)/../../libasm/include/ \
                -I$(src)/../../libstderesi/include/ -I$(src)/../../libmjollnir/include/  \
		-I$(src)/../../libaspect/include -I$(src)/../../librevm/include/       \
		-I$(src)/../../libedfmt/include/ -DELFSH_INTERN  -DE2DBG_INTERN \
                -DERESI32 -Wno-strict-prototypes

EXTRAOBJ     := ../../libasm/libasm32.o ../../libmjollnir/libmjollnir32.o    \
		../../libelfsh/libelfsh32.o ../../libedfmt/libedfmt32.o      \
		../../librevm/revm-api.32.o ../../libaspect/libaspect32.o    \
		../../librevm/revm-machine.32.o ../../librevm/revm-lang.32.o \
                ../../libstderesi/libstderesi32.o ../../libregex/regex.o     \
		../../libetrace/libetrace32.o ../../librevm/revm-io.32.o     \
		../../libe2dbg/common/*.32.o 

ke2dbg-y	:= kern-bp.o kentry.o video.o buffering.o utils.o pci.o page.o \
		interrupts.o ports.o commands.o keyboard.o idt.o syscalls.o libc-wrapper.o $(EXTRAOBJ)

obj-m		:= ke2dbg.o

else

ke2dbg.ko:
	$(MAKE) -C /lib/modules/`uname -r`/build M=`pwd`/

all:	ke2dbg.ko

clean::
	@$(RM) *.o *~ \#* *\# .\#*
	@$(RM) include/*~ include/\#* include/*\# include/.\#*

fclean:: clean
	@$(RM) *.ko

endif
