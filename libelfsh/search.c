/*
** search.c for libelfsh
**
** This function contains the research function inside the ELF structures
** of the address space of the process or program.
**
** Started on Sat Nov 25 21:19:53 2006 mayhem
*/
#include "libelfsh.h"


/* List of loaded objects available for the functions of those files */
static hash_t *hash_workspace = NULL;
static hash_t *hash_shared = NULL;



/* Register list of objects to work with */
int		elfsh_register_working_objects(hash_t *prvhash,
					       hash_t *sharedhash)
{
  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);
  hash_workspace = prvhash;
  hash_shared    = sharedhash;
  ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}



/* Return the strongest symbol in the object */
/* FIXME : we dont support symbol selection when 2 'concurrent' 
   symbols are in the same object, take care */
elfsh_Sym	*elfsh_strongest_symbol(elfsh_Sym *choice, elfsh_Sym *candidate)
{
  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);

  if (elfsh_get_symbol_type(choice) == STT_NOTYPE)
    ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, (candidate));
      
  if (elfsh_get_symbol_type(candidate) == STT_NOTYPE)
    ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, (choice));

  ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__,
		     (elfsh_get_symbol_bind(choice) < 
		      elfsh_get_symbol_bind(candidate) ?
		      candidate : choice));
}





/* Find in the available objects list the most important symbol with this name */
elfshobj_t	*elfsh_find_obj_by_symbol(char *name)
{
  elfshobj_t	*curidx;
  elfshobj_t	*choice;
  elfsh_Sym	*symchoice;
  elfsh_Sym	*sym;
  int		index;
  char		**keys;
  int		keynbr;

  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);

  choice = NULL;
  symchoice = sym = NULL;

#if	__DEBUG_STATIC__	      
  printf("[DEBUG_STATIC] Looking for %s \n", name);
#endif

  /* Look in the user owned files */
  keys = hash_get_keys(hash_workspace, &keynbr);
  for (index = 0; index < keynbr; index++)
    {
      curidx = hash_get(hash_workspace, keys[index]);
      sym = elfsh_get_metasym_by_name(curidx, name);
      if (sym && (!symchoice || (sym == elfsh_strongest_symbol(symchoice, sym))))
	{
	  symchoice = sym;
	  choice = curidx;
#if	__DEBUG_STATIC__	      
	  printf("[DEBUG_STATIC] Found stronger sym %s (%08X) in %s [WSLIST] \n", 
		 name, symchoice->st_value, choice->name);
#endif
	}
    } 
  
  /* If the selected et_rel is beeing injected warn caller */
  if (choice->pending)
    ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, NULL) ;

  /* If we found something in the local list, do not search in the shared one */
  if (symchoice != NULL)
    ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, (choice)) ;

  /* Now look in the shared files */
  if (!hash_shared)
    goto end;

  keys = hash_get_keys(hash_shared, &keynbr);
  for (index = 0; index < keynbr; index++)
    {
      curidx = hash_get(hash_shared, keys[index]);
      sym = elfsh_get_metasym_by_name(curidx, name);
      if (sym && (!symchoice || (sym == elfsh_strongest_symbol(symchoice, sym))))
	{
	  symchoice = sym;
	  choice = curidx;
#if	__DEBUG_STATIC__	      
	  printf("Found stronger symbol %s (%08X) in %s [SHAREDLIST] \n",
		 name, symchoice->st_value, choice->name);
#endif      
	  
	}
    }
  
  end:
  
  /* If the selected et_rel is beeing injected warn caller */
  if (choice->pending)
  ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, NULL);
  
  /* If we found something in the local list, do not search in the shared one */
  if (symchoice != NULL)
  ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, (choice));
  
  ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__,
		    "Object not found", ((void *) -1));
}
