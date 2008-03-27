ARCH	:= $(shell uname -m | sed -e s/i.86/x86/)
OS	:= $(shell uname -s | sed -e s/Linux*/Linux/)
RELEASE := $(shell uname -r | sed -e s/2.6.*/2.6/ -e s/2.4.*/2.4/)

ifeq ($(ARCH),x86)
all:
	$(MAKE) -C arch/x86

clean:
	@$(RM) *.o *.ko
	$(MAKE) -C arch/x86 clean

fclean:
	@$(RM) *.o *.ko
	$(MAKE) -C arch/x86 fclean

endif
