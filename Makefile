##
## Makefile 
## 
## Started on  Fri Dec  7 06:54:35 2001 mayhem 
## Automatically generated from the configure script
## 


include ./config.h 
 
RM      = rm -f 
ETAGS   = etags 
CTAGS   = ctags 
BASEPATH = $(DESTDIR)/usr
MODPATH = $(BASEPATH)/share/elfsh/
BINPATH = $(BASEPATH)/bin/
LIBPATH = $(BASEPATH)/lib/
INCPATH = $(BASEPATH)/include/
MANPATH = $(BASEPATH)/share/man
 
all     : world modules tags
 
world   : 
	@echo 'Building libasm...' 
	@cd libasm && $(MAKE) 
	@echo 'Libasm has been built successfully.' 
 
	@echo 'Building libelfsh...' 
	@cd libelfsh && $(MAKE) 
	@echo 'Libelfsh has been built successfully.' 
 
	@echo 'Building ELFsh ET_EXEC'			
	@cd vm && $(MAKE)			     	
	@echo 'ELFsh ET_EXEC has been built successfully.' 
 
	@echo 'Building E2dbg' 
	@cd e2dbg && $(MAKE) all		     	
	@echo 'E2dbg has been built successfully.' 
 
.PHONY: modules 
modules: 
	@echo 'Building ELFsh modules suite...' 
	@cd modules && $(MAKE) 
	@echo 'ELFsh modules suite built successfully.' 
 
install : all mod_install 
	@cp vm/elfsh $(BINPATH)/ 
	@cp e2dbg/e2dbg $(BINPATH)/ 
	@cp libelfsh/libelfsh.a libelfsh/libelfsh.so $(LIBPATH) 
	@cp libasm/libasm.a $(LIBPATH) 
	@cp e2dbg/e2dbg.so $(LIBPATH) 
	@cp -R libelfsh/include/* $(INCPATH) 
	@cp libasm/include/libasm.h $(INCPATH)
	@cp libasm/include/libasm-i386.h $(INCPATH) 
	@cp doc/elfsh.1 $(MANPATH)/man1/ 
	@chmod 755 $(MANPATH)/man1/elfsh.1 
	@chmod 755 $(BINPATH)/elfsh 
	@chmod 755 $(BINPATH)/e2dbg 
	@chmod 755 $(LIBPATH)libelfsh.so 
	@chmod 755 $(LIBPATH)e2dbg.so 
	@chmod 755 $(INCPATH)libelfsh*.h 
	@chmod 755 $(LIBPATH)libelfsh.a 
	@chmod 755 $(LIBPATH)libasm.a 
	@chmod 755 $(INCPATH)libelfsh*.h 
	@chmod 755 $(INCPATH)libasm*.h 
	@echo 'ELFsh, E2dbg, Libelfsh, and Libasm installed successfully .'
 
mod_install: 
	@mkdir $(MODPATH) 2>/dev/null || true 
	@cp modules/*.so $(MODPATH) 
	@chmod -R 755 $(MODPATH) 
 
uninstall: 
	rm -f  $(BINPATH)/elfsh 
	rm -f  $(BINPATH)/e2dbg 
	rm -f  $(LIBPATH)/libelfsh* 
	rm -f  $(LIBPATH)/e2dbg* 
	rm -fr $(MODPATH) 
	rm -f  $(MANPATH)/man1/elfsh.1 
	rm -f  $(LIBPATH)/libasm.a 
	rm -f  $(INCPATH)/libasm*.h 
	rm -fr $(INCPATH)/libelfsh*
	@echo 'ELFsh, Libelfsh and Libasm uninstalled successfully' 
 
clean   : cleandoc 
	@echo 'Cleaning tree ...' 
	cd vm        && $(MAKE) -s clean 
	cd e2dbg     && $(MAKE) -s clean 
	cd libelfsh  && $(MAKE) -s clean 
	cd libasm    && $(MAKE) -s clean 
	cd libdump   && $(MAKE) -s clean 
	cd modules   && $(MAKE) -s clean 
	@echo 'Tree cleaned .' 
 
fclean  : cleandoc 
	@echo 'Fcleaning tree ...' 
	cd vm        && $(MAKE) -s fclean 
	cd e2dbg     && $(MAKE) -s fclean 
	cd libelfsh  && $(MAKE) -s fclean 
	cd libasm    && $(MAKE) -s fclean 
	cd libdump   && $(MAKE) -s fclean 
	cd modules   && $(MAKE) -s fclean 
	@echo 'Tree cleaned .' 
 
cleandoc: 
	@rm -f *~ *\# \#* .\#* 
	@rm -f doc/*~ doc/*\# doc/\#* doc/.\#* 
	@rm -f doc/research/*~ doc/research/*\# 
	@rm -f doc/research/\#* doc/research/.\#* 
 
tags    : 
	@echo Doing tags ... 
	@$(ETAGS) vm/*.c vm/include/*.h libelfsh/*.c \
	libelfsh/include/*.h e2dbg/include/*.h \
	modules/*.c libmalloc/*.c libdump/*.c e2dbg/*.c \
	libasm/src/*.c libmalloc/include/*.h \
  libasm/src/arch/ia32/*.c \
	libasm/include/*.h \
	libdump/include/*.h 
 
info    : 
	@echo '###############' Total '###############'
	wc -l vm/*.c libelfsh/*.c libelfsh/include/*.h \
	vm/include/*.h modules/*.c libasm/src/*.c \
  libasm/src/arch/ia32/*.c e2dbg/*.c e2dbg/include/*.h
	@echo '###############' Total w/o headers '###'
	wc -l vm/*.c libelfsh/*.c modules/*.c e2dbg/*.c
	@echo '###############' Libelfsh only '#######'
	wc -l libelfsh/*.c 
	@echo '###############' Libasm only ##########'
	wc -l libasm/src/*.c libasm/src/arch/ia32/*.c 
	@echo '###############' VM only '#############'
	wc -l vm/*.c 
	@echo '###############' E2DBG only '#############'
	wc -l e2dbg/*.c 
	@echo '###############' Modules only '########'
	wc -l modules/*.c 
 
