##
## Makefile for elfsh testsuite in 
## 
## Made by mayhem
## Login   <mayhem@epita.fr>
## 
## Started on  Sun Dec 15 01:34:42 2002 mayhem
## Last update Sat May 24 17:45:46 2003 mayhem
##

all	:
	@cd ctors_hijack       && $(MAKE)
	@cd dtors_hijack       && $(MAKE)
	@cd got_hijack         && $(MAKE)
	@cd hash_retreive      && $(MAKE)
	@cd sct_fillbss_injection  && $(MAKE)
	@cd sct_top_injection      && $(MAKE)
	@cd sct_unmapped_injection && $(MAKE)
	@cd sht_rebuild	       && $(MAKE)
	@cd sht_stripping      && $(MAKE)
	@cd symtab_extend      && $(MAKE)
	@cd etrel_inject       && $(MAKE)

clean	:
	@echo -n 'Cleaning testsuite ...'
	@cd ctors_hijack       && $(MAKE) clean
	@cd dtors_hijack       && $(MAKE) clean
	@cd got_hijack         && $(MAKE) clean
	@cd hash_retreive      && $(MAKE) clean
	@cd sct_fillbss_injection  && $(MAKE) clean
	@cd sct_top_injection      && $(MAKE) clean
	@cd sct_unmapped_injection && $(MAKE) clean
	@cd sht_rebuild	       && $(MAKE) clean
	@cd sht_stripping      && $(MAKE) clean
	@cd symtab_extend      && $(MAKE) clean
	@cd etrel_inject       && $(MAKE) clean
	@echo 'Testsuite cleaned .'

fclean	: clean
	@cd ctors_hijack       && $(MAKE) fclean
	@cd dtors_hijack       && $(MAKE) fclean
	@cd got_hijack         && $(MAKE) fclean
	@cd hash_retreive      && $(MAKE) fclean
	@cd sct_fillbss_injection  && $(MAKE) fclean
	@cd sct_top_injection      && $(MAKE) fclean
	@cd sct_unmapped_injection && $(MAKE) fclean
	@cd sht_rebuild	       && $(MAKE) fclean
	@cd sht_stripping      && $(MAKE) fclean
	@cd symtab_extend      && $(MAKE) fclean
	@cd etrel_inject       && $(MAKE) fclean
