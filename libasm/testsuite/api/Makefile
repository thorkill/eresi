SRCS = test_instr_name.c

CONFIG_SCRIPT=$(HOME)/devhell/eresi.trunk/eresi-config

CFLAGS = $(shell $(CONFIG_SCRIPT) libasm --defines --cflags)
LDFLAGS = $(shell $(CONFIG_SCRIPT) libasm --libs)

OBJS = $(SRCS:.c=.o)

all:	$(OBJS)
	$(foreach obj, $(OBJS), cc -o $(obj:.o=.out) $(obj) $(LDFLAGS))