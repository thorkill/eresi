KDIR:=/lib/modules/$(shell uname -r)/build

obj-m:=lkm-original.o lkm-inject.o lkm-sys_setdomainname.o

default:
	$(MAKE) -C $(KDIR) SUBDIRS=$(PWD) modules

clean:
	@rm -f Module.symvers
	@rm -f *.o
	@rm -f *.ko
	@rm -f *.mod.c
	@rm -fr .tmp_versions
	@rm -fr .*.cmd
