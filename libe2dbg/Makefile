##
## Makefile for libe2dbg in the ERESI framework
##
## Started on  Tue Aug  16 06:32:29 2005 jfv
## $Id: Makefile,v 1.17 2008-02-16 12:32:27 thor Exp $
##

include ../config.h

LD			?= ld
RM			= rm -f
DBGNAME32		= libe2dbg32
DBGNAME64		= libe2dbg64
DBGNAME32O		= e2dbg-rel32.o
DBGNAME64O		= e2dbg-rel64.o
KDBGNAME32O		= ke2dbg-rel32.o
KDBGNAME64O		= ke2dbg-rel64.o
AR			= ar rc
STRIP			= strip

LDFLAGS32		+= $(DLOPT) $(DYNOPT) $(EXTRAOPT)
LDFLAGS64		+= $(DLOPT) $(DYNOPT) $(EXTRAOPT)

EXTRAOBJ32		= ../libasm/libasm32.o ../libmjollnir/libmjollnir32.o     \
			../libelfsh/libelfsh32.o ../libedfmt/libedfmt32.o         \
			../liballocproxy/liballocproxy.o ../librevm/revm-*.32.o 	  \
			 ../libaspect/libaspect32.o ../libstderesi/libstderesi32.o \
			../libetrace/libetrace32.o $(LDDUMPOBJ32)

EXTRAOBJ64		= ../libasm/libasm64.o ../libmjollnir/libmjollnir64.o     \
			../libelfsh/libelfsh64.o ../libedfmt/libedfmt64.o         \
			../liballocproxy/liballocproxy.o ../librevm/revm-*.64.o	  \
			../libaspect/libaspect64.o ../libstderesi/libstderesi64.o \
			../libetrace/libetrace64.o $(LDDUMPOBJ64)

COBJ32			= common/libe2dbg-common32.o
COBJ64			= common/*.64.o
KOBJ32			= kernel/*.32.o
KOBJ64			= kernel/*.64.o
UOBJ32			= user/libe2dbg-user32.o
UOBJ64			= user/*.64.o

all			: all32 all64

all32			: mod32 lib32
all32			: lib32

mod32			:
			$(MAKE) -C common all32
			$(MAKE) -C user   all32
ifdef USE_KE2DBG
			$(MAKE) -C kernel 
endif

$(DBGNAME32O):
			@$(CC) -shared $(COBJ32) $(EXTRAOBJ32) $(UOBJ32) $(LDFLAGS32) \
			$(KERNSHLIB32) -o $(DBGNAME32).so
			@$(AR) $(DBGNAME32).a $(COBJ32) $(UOBJ32) $(EXTRAOBJ32)
			@$(RANLIB) $(DBGNAME32).a
			@$(LD) -r $(COBJ32) $(EXTRAOBJ32) $(UOBJ32) -o $(DBGNAME32O)
			#$(STRIP) $(DBGNAME32O)

lib32:  $(DBGNAME32O)
lib64:  $(DBGNAME64O)


all64			: mod64 lib64

mod64			:
			$(MAKE) -C common all64
			$(MAKE) -C user   all64

$(DBGNAME64O)			:
			@$(CC) -shared $(COBJ64) $(EXTRAOBJ64) $(UOBJ64) $(LDFLAGS64) \
			$(KERNSHLIB64) -o $(DBGNAME64).so
			@$(AR) $(DBGNAME64).a $(OBJ64)
			@$(RANLIB) $(DBGNAME64).a
			@$(LD) -r $(OBJ64) $(EXTRAOBJ64) -o $(DBGNAME64O)
			#$(STRIP) $(DBGNAME64O)

# This target is work in progress (static files debugger)
et_rel			: ${OBJ}
			$(CC) -c $(CFLAGS) $(SRC_DYN) -o $(OBJ_DYN)
			$(LD) ${OBJ_DYN} ${OBJ} ../libasm/libasm.a	\
			../libelfsh/libelfsh.a ../liballocproxy.a	\
			/usr/lib/libdl.a /usr/lib/libc.a -r             \
			-o $(DBGNAME)_rel.o	  

clean		       :
		       @$(MAKE) -C common clean
		       @$(MAKE) -C kernel clean
		       @$(MAKE) -C user   clean
		       @$(RM) *~ \#* *\# .\#*
		       @$(RM) include/*~ include/\#* include/*\# include/.\#*

fclean		       : clean
		       @$(RM) $(KDBGNAME64O) $(KDBGNAME32O) $(DBGNAME32O) $(DBGNAME64O)
		       @$(RM) $(DBGNAME32).so $(DBGNAME64).so $(DBGNAME32).a $(DBGNAME64).a

%.32.o : %.c
			$(CC) $(CFLAGS32) -c -o $@ $<
%.64.o : %.c
			$(CC) $(CFLAGS64) -c -o $@ $<


