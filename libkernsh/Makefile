include ../config.h

NAME32_libkernsh 	= 	libkernsh32
NAME64_libkernsh        =       libkernsh64

LDFLAGS32		+= $(DLOPT) $(DYNOPT) $(EXTRAOPT)
LDFLAGS64		+= $(DLOPT) $(DYNOPT) $(EXTRAOPT)

EXTRAOBJ32 		= ../libasm/libasm.o ../libmjollnir/libmjollnir32.o \
			  ../libelfsh/libelfsh32.o ../libedfmt/libedfmt32.o \
			  ../libaspect/libaspect32.o

EXTRAOBJ64		= ../libasm/libasm.o ../libmjollnir/libmjollnir64.o \
                          ../libelfsh/libelfsh64.o ../libedfmt/libedfmt64.o \
			  ../libaspect/libaspect64.o

RM 		= 	rm -f
ETAGS 		= 	etags
CC 		?= 	gcc
LD		?=	ld
CP 		= 	cp
AR		=	ar rc

COBJ32                  = common/*.32.o
COBJ64                  = common/*.64.o
KOBJ32                  = kernel/*.32.o
KOBJ64                  = kernel/*.64.o
UOBJ32                  = user/*.32.o
UOBJ64                  = user/*.64.o

all:	all32 all64 

all32: 	mod32 lib32

mod32:	
	$(MAKE) -C common all32
	$(MAKE) -C user   all32
	$(MAKE) -C kernel

lib32:
	@$(CC) -shared -lcrypto $(COBJ32) $(EXTRAOBJ32) $(UOBJ32)  $(LDFLAGS32) -o $(NAME32_libkernsh).so
	@${AR} ${NAME32_libkernsh}.a ${COBJ32} $(UOBJ32)
	@${RANLIB} ${NAME32_libkernsh}.a

all64:	mod64 lib64

mod64:  
	$(MAKE) -C common all64
	$(MAKE) -C user   all64
	$(MAKE) -C kernel

lib64:
	$(CC) -shared -lcrypto $(COBJ64) $(EXTRAOBJ64) $(UOBJ64)  $(LDFLAGS64) -o $(NAME64_libkernsh).so
	@${AR} ${NAME64_libkernsh}.a ${COBJ4} $(UOBJ64)
	@${RANLIB} ${NAME64_libkernsh}.a

clean:
	@$(MAKE) -C common clean
	@$(MAKE) -C kernel clean
	@$(MAKE) -C user   clean
	@$(RM) *~ \#* *\# .\#*	

fclean: clean
	@$(MAKE) -C common fclean
	@$(MAKE) -C kernel fclean
	@$(MAKE) -C user   fclean
	@$(RM) *.a *.so

#tags:
#	@$(ETAGS) -a arch/i386/*.c include/*.h engine/*.c sample/*.c
#	@echo TAGS generated succesfully

%.32.o: %.c
	@$(CC) $(CFLAGS32) ${DBGFLAGS} -c $*.c -o $*.32.o
	@echo "[CC] $*.32.o"

%.64.o: %.c
	@$(CC) $(CFLAGS64) ${DBGFLAGS} -c $*.c -o $*.64.o
	@echo "[CC] $*.64.o"
docs:
	doxygen doc/doxygen.conf

