ELFSH=../../../elfsh/elfsh32

all: all32
all32: test
test:	bin
	@ echo ===================================
	@ echo ========== OBJDUMP ================
	@objdump -R bin | tee tRel.objdump.log
	@ echo ===================================
	@ echo ========== ELFSH ==================
	@${ELFSH} -f bin -rel | tee tRel.elfsh.log

bin:
	cc -c bin.c
	cc -fPIC -shared -o libfunc.so libfunc.c
	cc -o bin bin.o -L. -lfunc

fclean:
clean:
	rm -f libfunc.so bin.o bin *~
