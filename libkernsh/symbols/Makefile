CC = gcc
CFLAGS = -Wall
LDFLAGS = -Wall
SRC_SYMBOLS = $(wildcard *.c)
ASM_SYMBOLS = $(wildcard *.s)
OBJ_SYMBOLS = $(SRC_SYMBOLS:.c=.o) $(ASM_SYMBOLS:.s=.o)
DIS_SYMBOLS = $(OBJ_SYMBOLS:.o=.dis)

INCLUDE_SYMBOLS = "../include/libkernsh-symbols.h"

all: $(DIS_SYMBOLS)
	@echo "" > $(INCLUDE_SYMBOLS)
	@echo "#ifndef __LIBKERNSH_SYMBOLS_H__" >> $(INCLUDE_SYMBOLS)
	@echo "#define __LIBKERNSH_SYMBOLS_H__" >> $(INCLUDE_SYMBOLS)
	@echo "" >> $(INCLUDE_SYMBOLS)
	@for file in $^; do ./bin2hex.pl $$file >> $(INCLUDE_SYMBOLS); done
	@echo "#endif" >> $(INCLUDE_SYMBOLS)

%.dis: %.o
	@objdump -d $< > $@
	@rm $<

%.o: %.s
	@nasm -f elf -g $<
	
%.o: %.c
	@$(CC) -c $< -o $@ $(CFLAGS)

clean:
	rm -rf *.o *.dis
