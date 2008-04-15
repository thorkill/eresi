##
## Makefile 
## 
## Started on  Fri Dec  7 06:54:35 2001 jfv
## Automatically generated from the configure script
## 

CONFIG = $(shell test -f config.h && ls config.h)
ifneq ($(CONFIG), config.h)
$(info [ ============= ERROR ===========])
$(info [ config.h file does not exist   ])
$(info [ Run the configure script first ])
$(error Build interrupted.)
endif

include ./config.h 

CC      = gcc 
RM      = rm -f 
ETAGS   = etags 
CTAGS   = ctags 
DESTDIR = 
BUILD_TARGET = all32
MOD_TARGET = mod32
BASEPATH = $(DESTDIR)/usr/local
MANPATH = $(BASEPATH)/share/man
MODPATH = $(BASEPATH)/share/elfsh/
BINPATH = $(BASEPATH)/bin/
LIBPATH = $(BASEPATH)/lib/
INCPATH = $(BASEPATH)/include/
RM      = rm -f 

all     : world modules tags

ifdef NAME
include  build/defs/dep-rules.def
component:	dep-$(NAME)
else
component:
	@echo Precise which component by specifying a NAME option.
	@echo gmake component NAME=component-name
	@echo component names are mostly subdirectories
endif

world   : 
	@echo 'Building liballocproxy ..' 
	@cd liballocproxy && $(MAKE) 
	@echo 'Liballoc-proxy has been built successfully.' 
	@echo 'Building libaspect..' 
	@cd libaspect && $(MAKE) $(BUILD_TARGET)
	@echo 'Libaspect has been built successfully.' 
	@echo 'Building libasm...' 
	@cd libasm && sh configure 
	@cd libasm && $(MAKE) 
	@echo 'Libasm has been built successfully.' 
ifneq ($(KERNSH),)
	@echo 'Building libkernsh...'
	@cd libkernsh && $(MAKE) $(BUILD_TARGET)
	@echo 'Libkernsh has been built successfully.'
endif
	@echo 'Building libelfsh...' 
	@cd libelfsh && $(MAKE) $(BUILD_TARGET)
	@echo 'Libelfsh has been built successfully.' 
	@echo 'Building libetrace...' 
	@cd libetrace && $(MAKE) $(BUILD_TARGET)
	@echo 'Libetrace has been built successfully.' 
	@echo 'Building libedfmt...'                     
	@cd libedfmt && $(MAKE) $(BUILD_TARGET)        
	@echo 'Libedfmt has been built successfully.'    
	@echo 'Building libui...'			
	@cd libui && $(MAKE) $(BUILD_TARGET)		
	@echo 'Libui has been built successfully.' 	
	@echo 'Building libmjollnir...'			
	@cd libmjollnir && $(MAKE) $(BUILD_TARGET)	
	@echo 'Libmjollnir has been built successfully.'
	@echo 'Building librevm...'			
	@cd librevm && $(MAKE) $(BUILD_TARGET)	
	@echo 'Librevm has been built successfully.'
	@cd librevm/io/ && $(MAKE) $(BUILD_TARGET)  
	@test -f config.back && cp config.back config.h
	@echo 'Librevm has been rebuilt successfully.'
	@echo 'Building libstderesi...'			
	@cd libstderesi && $(MAKE) $(BUILD_TARGET)	
	@echo 'Libstderesi has been built successfully.'
	@echo 'Building libe2dbg...'			
	@cd libe2dbg && $(MAKE) $(BUILD_TARGET)	
	@echo 'Libe2dbg has been built successfully.'
	@echo 'Building ELFsh ET_EXEC'			
	@cd elfsh && $(MAKE)	$(BUILD_TARGET)     	
	@echo 'ELFsh ET_EXEC has been built successfully.' 

	@echo 'Building E2dbg' 
	@cd e2dbg && $(MAKE) $(BUILD_TARGET)	     	
	@echo 'E2dbg has been built successfully.' 

	@echo 'Building Etrace'				
	@cd etrace && $(MAKE)	$(BUILD_TARGET)     	
	@echo 'ETrace has been built successfully.' 	

	@echo 'Building Evarista'			
	@cd evarista && $(MAKE) $(BUILD_TARGET)     	
	@echo 'Evarista has been built successfully.' 	

ifneq ($(KERNSH),)
	@echo 'Building kernsh...'
	@cd kernsh && $(MAKE) $(BUILD_TARGET)
	@echo 'Kernsh has been built successfully.'
endif

.PHONY: modules 
modules: 
#	@echo 'Building ELFsh modules suite...' 
#	@cd modules && $(MAKE) 
#	@echo 'ELFsh modules suite built successfully.' 

install64 :
	@ln -sf $(BINPATH)elfsh64         $(BINPATH)elfsh
	@ln -sf $(BINPATH)e2dbg64         $(BINPATH)e2dbg
	@ln -sf $(BINPATH)etrace64        $(BINPATH)etrace
	@ln -sf $(BINPATH)evarista64      $(BINPATH)evarista
	@ln -sf $(LIBPATH)libaspect64.a   $(LIBPATH)libaspect.a
	@ln -sf $(LIBPATH)libelfsh64.a    $(LIBPATH)libelfsh.a
	@ln -sf $(LIBPATH)libe2dbg64.a    $(LIBPATH)libe2dbg.a
	@ln -sf $(LIBPATH)libmjollnir64.a $(LIBPATH)libmjollnir.a
	@ln -sf $(LIBPATH)librevm64.a     $(LIBPATH)librevm.a
	@ln -sf $(LIBPATH)libstderesi64.a $(LIBPATH)libstderesi.a
	@ln -sf $(LIBPATH)libedfmt64.a    $(LIBPATH)libedfmt.a
	@ln -sf $(LIBPATH)libui64.a       $(LIBPATH)libui.a
	@ln -sf $(LIBPATH)libaspect64.so   $(LIBPATH)libaspect.so
	@ln -sf $(LIBPATH)libelfsh64.so    $(LIBPATH)libelfsh.so
	@ln -sf $(LIBPATH)libetrace64.so    $(LIBPATH)libetrace.so
	@ln -sf $(LIBPATH)libe2dbg64.so    $(LIBPATH)libe2dbg.so
	@ln -sf $(LIBPATH)libmjollnir64.so $(LIBPATH)libmjollnir.so
	@ln -sf $(LIBPATH)librevm64.so     $(LIBPATH)librevm.so
	@ln -sf $(LIBPATH)libstderesi64.so $(LIBPATH)libstderesi.so
	@ln -sf $(LIBPATH)libedfmt64.so    $(LIBPATH)libedfmt.so
	@ln -sf $(LIBPATH)libui64.so       $(LIBPATH)libui.so
install : prepareDir #mod_install 
	@cp elfsh/elfsh?? $(BINPATH)/ 
	@cp e2dbg/e2dbg?? $(BINPATH)/ 
	@cp etrace/etrace?? $(BINPATH)/ 
	@cp evarista/evarista?? $(BINPATH)/ 
	@cp libaspect/libaspect*.a libaspect/libaspect*.so $(LIBPATH) 
	@cp liballocproxy/liballocproxy.a $(LIBPATH) 
	@cp libelfsh/libelfsh*.a libelfsh/libelfsh*.so $(LIBPATH) 
	@cp libetrace/libetrace*.a libetrace/libetrace*.so $(LIBPATH) 
	@cp libe2dbg/libe2dbg*.a libe2dbg/libe2dbg*.so $(LIBPATH) 
	@cp libasm/libasm.a libasm/libasm.so $(LIBPATH) 
	@cp libmjollnir/libmjollnir*.a $(LIBPATH) 
	@cp libmjollnir/libmjollnir*.so $(LIBPATH) 
	@cp librevm/librevm*.a librevm/librevm*.so $(LIBPATH)
	@cp libstderesi/libstderesi*.a libstderesi/libstderesi*.so $(LIBPATH)
	@cp libedfmt/libedfmt*.a libedfmt/libedfmt*.so $(LIBPATH) 
	@cp libui/libui*.a libui/libui*.so $(LIBPATH) 
	@ln -sf $(BINPATH)elfsh32         $(BINPATH)elfsh
	@ln -sf $(BINPATH)e2dbg32         $(BINPATH)e2dbg
	@ln -sf $(BINPATH)etrace32        $(BINPATH)etrace
	@ln -sf $(BINPATH)evarista32      $(BINPATH)evarista
	@ln -sf $(BINPATH)kernsh32        $(BINPATH)kernsh
	@ln -sf $(LIBPATH)libaspect32.a   $(LIBPATH)libaspect.a
	@ln -sf $(LIBPATH)libelfsh32.a    $(LIBPATH)libelfsh.a
	@ln -sf $(LIBPATH)libetrace32.a    $(LIBPATH)libetrace.a
	@ln -sf $(LIBPATH)libe2dbg32.a    $(LIBPATH)libe2dbg.a
	@ln -sf $(LIBPATH)libmjollnir32.a $(LIBPATH)libmjollnir.a
	@ln -sf $(LIBPATH)librevm32.a     $(LIBPATH)librevm.a
	@ln -sf $(LIBPATH)libstderesi32.a $(LIBPATH)libstderesi.a
	@ln -sf $(LIBPATH)libedfmt32.a    $(LIBPATH)libedfmt.a
	@ln -sf $(LIBPATH)libui32.a       $(LIBPATH)libui.a
	@ln -sf $(LIBPATH)libaspect32.so   $(LIBPATH)libaspect.so
	@ln -sf $(LIBPATH)libelfsh32.so    $(LIBPATH)libelfsh.so
	@ln -sf $(LIBPATH)libetrace32.so    $(LIBPATH)libetrace.so
	@ln -sf $(LIBPATH)libe2dbg32.so    $(LIBPATH)libe2dbg.so
	@ln -sf $(LIBPATH)libmjollnir32.so $(LIBPATH)libmjollnir.so
	@ln -sf $(LIBPATH)librevm32.so     $(LIBPATH)librevm.so
	@ln -sf $(LIBPATH)libstderesi32.so $(LIBPATH)libstderesi.so
	@ln -sf $(LIBPATH)libedfmt32.so    $(LIBPATH)libedfmt.so
	@ln -sf $(LIBPATH)libui32.so       $(LIBPATH)libui.so
	@ln -sf $(LIBPATH)libkernsh32.a   $(LIBPATH)libkernsh.a
	@ln -sf $(LIBPATH)libkernsh32.so   $(LIBPATH)libkernsh.so
	@cp -R libelfsh/include/* $(INCPATH) 
	@cp -R libetrace/include/* $(INCPATH) 
	@cp libasm/include/*.h $(INCPATH)
	@cp libaspect/include/*.h $(INCPATH)
	@cp librevm/include/*.h $(INCPATH)
	@cp libstderesi/include/*.h $(INCPATH)
	@cp libedfmt/include/*.h $(INCPATH) 
	@cp libui/include/libui.h $(INCPATH) 
	@cp libmjollnir/include/libmjollnir*.h $(INCPATH) 
	@cp doc/eresi.1 $(MANPATH)/man1/ 
	@chmod 644 $(MANPATH)/man1/eresi.1 
	@chmod 755 $(BINPATH)elfsh?? 
	@chmod 755 $(BINPATH)e2dbg??
	@chmod 755 $(BINPATH)etrace?? 
	@chmod 755 $(BINPATH)evarista?? 
	@chmod 644 $(LIBPATH)libelfsh*.so 
	@chmod 644 $(LIBPATH)libetrace*.so 
	@chmod 644 $(LIBPATH)libe2dbg*.so 
	@chmod 644 $(LIBPATH)libaspect*.so 
	@chmod 644 $(LIBPATH)libaspect*.a 
	@chmod 644 $(LIBPATH)librevm*.so 
	@chmod 644 $(LIBPATH)librevm*.a 
	@chmod 644 $(LIBPATH)libstderesi*.so 
	@chmod 644 $(LIBPATH)libstderesi*.a 
	@chmod 644 $(LIBPATH)libedfmt*.so 
	@chmod 644 $(LIBPATH)libedfmt*.a 
	@chmod 644 $(LIBPATH)libui*.so 
	@chmod 644 $(LIBPATH)libui*.a 
	@chmod 644 $(INCPATH)libui.h 
	@chmod 644 $(INCPATH)libelfsh*.h 
	@chmod 644 $(LIBPATH)libelfsh*.a 
	@chmod 644 $(LIBPATH)libasm.a 
	@chmod 644 $(INCPATH)libedfmt*.h 
	@chmod 644 $(INCPATH)libasm*.h 
	@chmod 644 $(INCPATH)libmjollnir.h 
	@chmod 644 $(INCPATH)libaspect*.h 
	@chmod 644 $(INCPATH)librevm*.h 
	@chmod 644 $(INCPATH)libstderesi*.h 

prepareDir: 
	@mkdir -p $(BINPATH) || true
	@mkdir -p $(LIBPATH) || true
	@mkdir -p $(INCPATH) || true
	@mkdir -p $(MANPATH)/man1/ || true
	@mkdir -p $(MODPATH) || true
	@echo 'ELFsh, E2dbg, Etrace, Evarista, Libelfsh, Libe2dbg, \
Libmjollnir, Libasm, Libedfmt, Libui, Librevm, Libstderesi \
and Libaspect installed successfully .'
	@echo Default installation is 32bits : make install64 if requested
mod_install: 
	@mkdir $(MODPATH) 2>/dev/null || true 
	@cp modules/*.so $(MODPATH) 
	@chmod -R 755 $(MODPATH) 

uninstall: 
	rm -f  $(BINPATH)/elfsh*
	rm -f  $(BINPATH)/e2dbg*
	rm -f  $(BINPATH)/etrace*
	rm -f  $(BINPATH)/evarista*
	rm -f  $(LIBPATH)/libelfsh*
	rm -f  $(LIBPATH)/libetrace*
	rm -f  $(LIBPATH)/libe2dbg*
	rm -f  $(LIBPATH)/libui*
	rm -f  $(LIBPATH)/liballocproxy*
	rm -f  $(LIBPATH)/libmjollnir*
	rm -f  $(LIBPATH)/libaspect*
	rm -f  $(LIBPATH)/librevm*
	rm -f  $(LIBPATH)/libstderesi*
	rm -f  $(LIBPATH)/libedfmt*
	rm -f  $(LIBPATH)/libasm*
	rm -f  $(LIBPATH)/libmalloc-e2dbg*
	rm -fr $(MODPATH) 
	rm -f  $(MANPATH)/man1/elfsh.1 
	rm -f  $(INCPATH)/libasm*.h 
	rm -f  $(INCPATH)/libmjollnir*.h 
	rm -f  $(INCPATH)/libui.h 
	rm -fr $(INCPATH)/libelfsh*
	rm -fr $(INCPATH)/libaspect*
	rm -fr $(INCPATH)/librevm*
	rm -fr $(INCPATH)/libstderesi*
	rm -fr $(INCPATH)/libedfmt*
	@echo 'ELFsh, E2dbg, Etrace, Evarista, Libelfsh, \
Libmjollnir, Libaspect, Libedfmt, Libe2dbg, Libui, \
Libstderesi and Libasm uninstalled successfully' 

clean   : cleandoc 
	@echo 'Cleaning tree ...' 
	cd elfsh     && $(MAKE) -s clean 
	cd e2dbg     && $(MAKE) -s clean 
	cd etrace    && $(MAKE) -s clean 
	cd evarista  && $(MAKE) -s clean 
	cd libelfsh  && $(MAKE) -s clean 
	cd libetrace && $(MAKE) -s clean 
	cd libe2dbg  && $(MAKE) -s clean 
	cd librevm   && $(MAKE) -s clean 
	cd libstderesi && $(MAKE) -s clean 
	cd libedfmt  && $(MAKE) -s clean 
	cd libaspect && $(MAKE) -s clean 
	cd libui     && $(MAKE) -s clean 
	cd libmjollnir && $(MAKE) -s clean 
	cd libasm    && sh configure 
	cd libasm    && $(MAKE) -s clean 
	cd liballocproxy && $(MAKE) -s clean 
	cd libdump   && $(MAKE) -s clean 
	cd modules   && $(MAKE) -s clean 
	@echo 'Tree cleaned .' 

fclean  : cleandoc 
	@echo 'Fcleaning tree ...' 
	cd elfsh     && $(MAKE) -s fclean 
	cd e2dbg     && $(MAKE) -s fclean 
	cd etrace    && $(MAKE) -s fclean 
	cd evarista  && $(MAKE) -s fclean 
	cd libelfsh  && $(MAKE) -s fclean 
	cd libetrace && $(MAKE) -s fclean 
	cd libe2dbg  && $(MAKE) -s fclean 
	cd libasm    && sh configure 
	cd libasm    && $(MAKE) -s fclean 
	cd librevm   && $(MAKE) -s fclean 
	cd libstderesi && $(MAKE) -s fclean 
	cd libedfmt  && $(MAKE) -s fclean 
	cd libaspect && $(MAKE) -s fclean 
	cd liballocproxy && $(MAKE) -s fclean 
	cd libmjollnir && $(MAKE) -s fclean 
	cd libdump   && $(MAKE) -s fclean 
	cd libui     && $(MAKE) -s fclean 
	cd modules   && $(MAKE) -s fclean 
	cd testsuite && $(MAKE) -s fclean 
	@echo 'Tree cleaned .' 

cleandoc: 
	@rm -f *~ *\# \#* .\#* 
	@rm -f doc/*~ doc/*\# doc/\#* doc/.\#* 
	@rm -f doc/research/*~ doc/research/*\# 
	@rm -f doc/research/\#* doc/research/.\#* 

tags    : 
	@echo Doing tags ... 
info    : 
	@echo '###############' TOTAL '###############'
	@wc -l elfsh/*.c libelfsh/*.c libelfsh/include/*.h \
	libetrace/*.c libetrace/include/*.h                \
	elfsh/include/*.h modules/*.c libasm/src/*.c \
  libasm/src/arch/*/*.c e2dbg/*.c e2dbg/include/*.h \
  libaspect/*.c libmjollnir/src/*.c libaspect/*/*.h \
  librevm/*/*.c librevm/include/*.h libdump/*.c \
  libstderesi/*/*.c libstderesi/include/*.h  \
  libe2dbg/*/*.c libe2dbg/include/*.h \
  libmjollnir/include/*.h libui/include/*.h libui/*.c \
  libedfmt/include/*.h libelfsh/include/*/*.h \
  libasm/src/arch/*/handlers/*.c libedfmt/*.c \
  evarista/*.c evarista/include/*.h
	@echo '#########' TOTAL WITHOUT HEADERS '#######'
	@wc -l elfsh/*.c libelfsh/*.c modules/*.c e2dbg/*.c \
	libaspect/*.c libasm/src/arch/ia32/*.c libui/*.c\
  libasm/src/*.c libasm/src/arch/sparc/*.c libe2dbg/*/*.c \
	librevm/*/*.c libmjollnir/src/*.c libdump/*.c \
  libasm/src/arch/*/handlers/*.c libedfmt/*.c \
  evarista/*.c libstderesi/*/*.c
	@echo '############' LIBASPECT ONLY '############'
	@wc -l libaspect/*.c 
	@echo '#############' LIBELFSH ONLY '#######'
	@wc -l libelfsh/*.c 
	@echo '#############' LIBETRACE ONLY '#######'
	@wc -l libetrace/*.c 
	@echo '#############' LIBE2DBG ONLY '#######'
	@wc -l libe2dbg/*/*.c 
	@echo '###############' LIBASM ONLY '##########'
	@wc -l libasm/src/*.c libasm/src/arch/ia32/*.c \
  libasm/src/arch/sparc/*.c
	@echo '###############' LIBMJOLLNIR ONLY '##########'
	@wc -l libmjollnir/src/*.c 
	@echo '###############' LIBREVM ONLY '##########'
	@wc -l librevm/*/*.c 
	@echo '###############' LIBSTDERESI ONLY '##########'
	@wc -l libstderesi/*/*.c 
	@echo '###############' LIBEDFMT ONLY '##########'
	wc -l libedfmt/*.c 
	@echo '###############' LIBUI ONLY '##########'
	@wc -l libui/*.c 
	@echo '###############' LIBDUMP ONLY '##########'
	@wc -l libdump/*.c 
	@echo '###############' ELFSH only '#############'
	@wc -l elfsh/*.c 
	@echo '###############' E2DBG only '#############'
	@wc -l e2dbg/*.c 
	@echo '###############' ETRACE only '#############'
	@wc -l etrace/*.c 
	@echo '###############' EVARISTA only '#############'
	@wc -l evarista/*.c 
	@echo '###############' MODULES ONLY '########'
	@wc -l modules/*.c 

docs:
	doxygen doc/doxygen.conf
	#(cd libasm && $(MAKE) docs)
	#(cd libelfsh && $(MAKE) docs)
	#(cd librevm && $(MAKE) docs)
	#(cd libstderesi && $(MAKE) docs)
	#(cd libmjollnir && $(MAKE) docs)
	#(cd libui && $(MAKE) docs)
	#(cd libedfmt && $(MAKE) docs)
