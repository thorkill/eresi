##
## Makefile for elfsh testsuite/etrel_inject
## 
## Made by mayhem
## Login   <mayhem@epita.fr>
## 
## Started on  Sun Dec 15 01:34:42 2002 mayhem
## Last update Sat May 24 17:45:46 2003 mayhem
##

all	:
	@cd et_rel2static      && $(MAKE) 
	@cd etrel_memory       && $(MAKE)  
	@cd etrel_original     && $(MAKE) 
	@cd etrel_pie-ssp      && $(MAKE) 
	@cd etrel_static       && $(MAKE) 

clean	:
	@cd et_rel2static      && $(MAKE) clean
	@cd etrel_memory       && $(MAKE) clean
	@cd etrel_original     && $(MAKE) clean
	@cd etrel_pie-ssp      && $(MAKE) clean
	@cd etrel_static       && $(MAKE) clean

fclean	: clean
	@cd et_rel2static      && $(MAKE) fclean
	@cd etrel_memory       && $(MAKE) fclean
	@cd etrel_original     && $(MAKE) fclean
	@cd etrel_pie-ssp      && $(MAKE) fclean
	@cd etrel_static       && $(MAKE) fclean
