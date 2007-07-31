##
## Makefile for elfsh testsuite/etrel_inject
## 
## Made by jfv
## Login   <jfv@epita.fr>
## 
## Started on  Sun Dec 15 01:34:42 2002 jfv
## Last update Sat May 24 17:45:46 2003 jfv
##

all	:
	@cd et_rel2static      && $(MAKE) 
	@cd etrel_memory       && $(MAKE)  
	@cd etrel_original     && $(MAKE) 
	@cd etrel_pie-ssp      && $(MAKE) 
	@cd etrel_static       && $(MAKE) 
	@cd etrel_extplt       && $(MAKE)

clean	:
	@cd et_rel2static      && $(MAKE) clean
	@cd etrel_memory       && $(MAKE) clean
	@cd etrel_original     && $(MAKE) clean
	@cd etrel_pie-ssp      && $(MAKE) clean
	@cd etrel_static       && $(MAKE) clean
	@cd etrel_extplt       && $(MAKE) clean

fclean	: clean
	@cd et_rel2static      && $(MAKE) fclean
	@cd etrel_memory       && $(MAKE) fclean
	@cd etrel_original     && $(MAKE) fclean
	@cd etrel_pie-ssp      && $(MAKE) fclean
	@cd etrel_static       && $(MAKE) fclean
	@cd etrel_extplt       && $(MAKE) fclean
