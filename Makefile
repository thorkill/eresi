##
## Makefile
##
## Started on  Fri Dec  7 06:54:35 2001 mayhem
## Last update Sun May 25 18:12:43 2003 mayhem
##

RM	= rm -f
ETAGS   = etags
MODPATH = /usr/share/elfsh/
BINPATH = /usr/bin
LIBPATH = /usr/lib/
INCPATH = /usr/include/

all	:
	@echo 'Builting libelfsh...'
	@cd libelfsh && $(MAKE)
	@echo 'Libelfsh has been built successfully.'

	@echo 'Builting libhashelfsh...'
	@cd libhash && $(MAKE) -s
	@echo 'Libhash has been built successfully.'

	@echo 'Builting ELFsh...'
	@cd vm && $(MAKE)
	@echo 'ELFsh has been built successfully.'

	@echo 'Building ELFsh modules suite...'
	@cd modules && $(MAKE)
	@echo 'ELFsh modules suite built successfully.'

	#$(MAKE) -s tags

install	: mod_install
	@cp vm/elfsh $(BINPATH)/
	@cp libelfsh/libelfsh.a libelfsh/libelfsh.so $(LIBPATH)
	@cp libelfsh/include/libelfsh*.h $(INCPATH)
	@chmod 755 $(BINPATH)/elfsh $(LIBPATH)libelfsh.so $(LIBPATH)libelfsh.a $(INCPATH)libelfsh*.h
	@echo 'ELFsh and Libelfsh installed successfully .'

mod_install:
	@mkdir $(MODPATH) 2>/dev/null || true
	@cp modules/*.so $(MODPATH)
	@chmod -R 755 $(MODPATH)

uninstall:
	rm -f /usr/bin/elfsh
	rm -f /usr/lib/libelfsh*
	rm -f /usr/include/libelfsh*.h
	rm -fr $(MODPATH)
	@echo 'ELFsh and Libelfsh uninstalled successfully'

clean	: cleandoc
	@echo 'Cleaning tree ...'
	cd vm        && $(MAKE) -s clean
	cd libelfsh  && $(MAKE) -s clean
	cd libhash   && $(MAKE) -s clean
	cd modules   && $(MAKE) -s clean
	@echo 'Tree cleaned .'

fclean	: cleandoc
	@echo 'Fcleaning tree ...'
	cd vm        && $(MAKE) -s fclean
	cd libelfsh  && $(MAKE) -s fclean
	cd libhash   && $(MAKE) -s fclean
	cd modules   && $(MAKE) -s fclean
	@echo 'Tree cleaned .'

cleandoc:
	@rm -f *~ *\# \#* .\#*
	@rm -f doc/*~ doc/*\# doc/\#* doc/.\#* 
	@rm -f doc/research/*~ doc/research/*\# doc/research/\#* doc/research/.\#* 

tags	:
	@echo Doing tags ...
	@$(ETAGS) vm/*.c vm/include/*.h libelfsh/*.c \
	libelfsh/include/*.h libhash/*.c libhash/include/*.h \
	libhash/*.c libhash/include/*.h modules/*.c

info	:
	@echo '################' Total '################'
	wc -l vm/*.c libelfsh/*.c libelfsh/include/*.h vm/include/*.h \
	modules/*.c
	@echo '################' Total w/o headers  '################'
	wc -l vm/*.c libelfsh/*.c modules/*.c
	@echo '################' Libelfsh only '################'
	wc -l libelfsh/*.c
	@echo '################' VM only '################'
	wc -l vm/*.c
	@echo '################' Modules only '################'
	wc -l modules/*.c
