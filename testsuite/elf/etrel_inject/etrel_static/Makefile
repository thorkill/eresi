#
# Simple Makefile for generating ET_REL list from archive files.
#
# The ELFsh project 2005
#

.PHONY: libc libcnonshared libpthread libpthreadnonshared libnsl


all: 	libc libcnonshared libpthread libpthreadnonshared libnsl
	rm -f load_lib.esh
	echo "#!../../../vm/elfsh" > load_lib.esh
	find libc libnsl libcnonshared libpthread libpthreadnonshared -name "*.o*" | xargs \
	-ibou echo load bou >> load_lib.esh

libnsl:
	mkdir -p libnsl
	cd libnsl && ar x /usr/lib/libnsl.a

libc:
	mkdir -p libc
	cd libc && ar x /usr/lib/libc.a

libcnonshared:
	mkdir -p libcnonshared
	cd libcnonshared && ar x /usr/lib/libc_nonshared.a

libpthread:
	mkdir -p libpthread
	cd libpthread && ar x /usr/lib/libpthread.a

libpthreadnonshared:
	mkdir -p libpthreadnonshared
	cd libpthreadnonshared && ar x /usr/lib/libpthread_nonshared.a

clean:
	rm -f core a.out fake_aout *.o *~ *\#*	
fclean: clean
	rm -f *~ \#* load_lib.esh libc/*.o* libcnonshared/*.o* \
	libpthread/*.o* libpthreadnonshared/*.o* libnsl/*.o*
	rm -fr libc libnsl libpthread libcnonshared libpthreadnonshared
