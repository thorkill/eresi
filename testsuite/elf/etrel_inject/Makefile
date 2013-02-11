##
## Makefile for ELF testsuite in ERESI
## 
## Started on  Sun Dec 15 01:34:42 2002 jfv
## $Id$
##

all: all32 all64

all32	:
	@cd etrel_original     && $(MAKE) all32
	@cd etrel_pie-ssp      && $(MAKE) all32
	@cd etrel_static       && $(MAKE) all32
	@cd etrel_extplt       && $(MAKE) all32
#	@cd etrel_extstatic    && $(MAKE) all32

all64	:
	@cd etrel_original     && $(MAKE) all64
	@cd etrel_pie-ssp      && $(MAKE) all64
	@cd etrel_static       && $(MAKE) all64
	@cd etrel_extplt       && $(MAKE) all64
#	@cd etrel_extstatic    && $(MAKE) all64

clean	:
	@cd etrel_original     && $(MAKE) clean
	@cd etrel_pie-ssp      && $(MAKE) clean
	@cd etrel_static       && $(MAKE) clean
	@cd etrel_extplt       && $(MAKE) clean
#	@cd etrel_extstatic    && $(MAKE) clean

fclean	: clean
	@cd etrel_original     && $(MAKE) fclean
	@cd etrel_pie-ssp      && $(MAKE) fclean
	@cd etrel_static       && $(MAKE) fclean
	@cd etrel_extplt       && $(MAKE) fclean
#	@cd etrel_extstatic    && $(MAKE) fclean

