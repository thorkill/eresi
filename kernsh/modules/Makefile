
obj-m	:= kernsh-virtm.o

all:
	$(MAKE) -C /lib/modules/`uname -r`/build M=`pwd`/

clean:
	@$(RM) *.o *~ \#* *\# .\#* *.symvers *.mod.c
	@$(RM) include/*~ include/\#* include/*\# include/.\#*

fclean: clean
	@$(RM) *.ko
