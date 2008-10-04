/**
 * @file hijack.c
 * @ingroup libelfsh
** hijack.c in elfsh
** 
** Started on  Tue Feb  4 14:41:34 2003 emsi
** 
**
** $Id: hijack.c,v 1.12 2008-02-16 12:32:27 thor Exp $
**
*/
#include "libelfsh.h"


/**
 * Perform custom function hijack, return 0 on success -1 on error 
 *
 * @param file
 * @param type
 * @param name
 * @param addr
 * @param hooked
 * @return
 */
int		elfsh_hijack_function_by_name(elfshobj_t *file, 
					      uint32_t	 type,
					      char	 *name, 
					      eresi_Addr addr,
					      eresi_Addr *hooked)
{
  elfsh_Sym	*symbol;
  elfsh_Sym	*symbol2;
  int		ret;
  int		ispltent;
  elfshsect_t	*hooks;
  uint32_t	pgsize;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);
  
  elfsh_setup_hooks();

  /* Sanity checks */
  if (file == NULL || name == NULL || addr == 0) 
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Invalid NULL parameter", -1);
  if (elfsh_copy_plt(file, elfsh_get_pagesize(file)) < 0)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
		      "Unable to copy PLT", -1);

  /* Guess the hijack type */
  switch (type) 
    {
      
      /* static function hijacking */
      /* This type is general enough for redirecting on the good hijack technique
      ** if it finds that the current file architecture is not suited to the 
      ** requested hijack type.
      */
    case ELFSH_HIJACK_TYPE_FLOW:
      
      /* If the hook section does not exist, create it */
      hooks = elfsh_get_section_by_name(file, ELFSH_SECTION_NAME_HOOKS, 0, 0, 0); 
      if (!hooks)
	{
	  pgsize = elfsh_get_pagesize(file);
	  hooks = elfsh_insert_section(file, 
				       ELFSH_SECTION_NAME_HOOKS, 
				       NULL,
				       ELFSH_CODE_INJECTION, 
				       pgsize - 1, pgsize);
	  if (!hooks)
	    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
			      "Cannot get and inject .hooks", -1);
	  hooks->curend = 0;
	}

      /* Resolve hooked function symbol */
      symbol = elfsh_get_symbol_by_name(file, name);
      if (!symbol || symbol->st_value == 0)
	symbol = elfsh_get_dynsymbol_by_name(file, name);
      if (!symbol && elfsh_debugger_present())
	{
	  elfsh_toggle_mode();
	  symbol2 = elfsh_get_dynsymbol_by_name(file, name);
	  elfsh_toggle_mode();
	}
      else
	symbol2 = NULL;
      if (!symbol && !symbol2)
	PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		     "Unknown function (no symbol)", -1);
      if (!symbol || (!symbol->st_value && symbol2))
	symbol = symbol2;
      ispltent = elfsh_is_pltentry(file, symbol);
	  
#if __DEBUG_REDIR__
      fprintf(stderr, "[DEBUG_REDIR] Symbol %s [file %s] resolved at %08X isplt(%u)\n",
	      name, file->name, symbol->st_value, ispltent);
#endif

      /* Perform PLT redirection if we deal with a PLT symbol */
      if (ispltent == 1)
	{

	  /* MIPS handling */
	  if (FILE_IS_MIPS(file))
	    {
	      if (hooked)
		*hooked = * (unsigned long *) elfsh_get_got_entry_by_name(file, name);
	      ret = elfsh_set_got_entry_by_name(file, name, addr);
	      if (ret < 0) 
		PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
				  "Unable to patch GOT entry", -1);
	      PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
	    }
	
	  /* Hook using ALTPLT technique */
	  if (hooked)
	    *hooked = symbol->st_value;
	  ret = elfsh_plt(file, symbol, addr);
	  if (ret < 0)
	    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
			      "Unable to perform ALTPLT", -1);

	  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, ELFSH_REDIR_ALTPLT);
	}
      
      /* If not a PLT entry, we do a control flow hijack */
      if (hooked)
	*hooked = symbol->st_value;
      ret = elfsh_cflow(file, name, symbol, addr);
      if (ret < 0)
	PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
			  "Unable to perform CFLOW redir", -1);

      /* Perform a second stage runtime relocation after injecting this new old symbol */
      if (elfsh_is_debug_mode() && elfsh_save_relocate(file) < 0)
	PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		     "Failed second stage runtime relocation", -1);

      PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, ELFSH_REDIR_CFLOW);

      /* GOT entry hijacking */
    case ELFSH_HIJACK_TYPE_GOT:
      if (hooked)
	*hooked = *(unsigned long *) elfsh_get_got_entry_by_name(file, name);
      ret = elfsh_set_got_entry_by_name(file, name, addr);
      if (ret < 0) 
	PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
			  "Unable to patch GOT entry", -1);
      PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, ELFSH_REDIR_ALTGOT);
      
      /* PLT hijack */
    case ELFSH_HIJACK_TYPE_PLT:
      if (FILE_IS_MIPS(file))
	PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
			  "There is not PLT to hijack on MIPS", -1);

      symbol = elfsh_get_dynsymbol_by_name(file, name);
      if (NULL == symbol)
	PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
			  "Unknown dynamic symbol", -1);

      if (!elfsh_is_pltentry(file, symbol))
	PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
			  "Symbol is not a PLT entry", -1);
      
      /* Now use ELFsh 0.6 hooks model */
      if (hooked)
	*hooked = symbol->st_value;
      ret = elfsh_plt(file, symbol, addr);
      if (ret < 0)
	PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
                          "Unable to do ALTPLT", -1);
      PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, ELFSH_REDIR_ALTPLT);
    }
  
  PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
		    "Unknown redirection type", -1);
}
