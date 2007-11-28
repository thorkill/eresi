/**
 * @file search.c
** search.c for libetrace
** 
** @brief Symbol trace search function 
** 
** Started Jul 2 2005 00:03:44 mxatone
** 
**
** $Id: search.c,v 1.1 2007-11-28 09:32:06 rival Exp $
**
*/

#include "libelfsh.h"
#include "libasm.h"
#include "libetrace.h"

/**
 * Search a symbol on dependences file 
 * @param file parent file
 * @param name symbol name
 * @return file object that containt the symbol (on dynsym)
 */
elfshobj_t   		*elfsh_traces_search_sym(elfshobj_t *file, char *name)
{
  char			**keys;
  int			keynbr;
  u_int			index;
  elfshobj_t		*child, *find = NULL;
  elfsh_Sym		*sym;
  elfshsect_t		*sect;
  char			*sect_name;
  u_int			bindnum;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__); 
  
  keys = hash_get_keys(&(file->child_hash), &keynbr);

  if (keys)
    {
      for (index = 0; index < keynbr; index++)
	{
	  child = (elfshobj_t *) hash_get(&(file->child_hash), keys[index]);

	  if (child)
	    {
	      sym = elfsh_get_dynsymbol_by_name(child, name);

	      if (!sym)
		continue;

	      bindnum = elfsh_get_symbol_bind(sym);
	      
	      /* We need a global or weak symbol */
	      if (bindnum == STB_GLOBAL || bindnum == STB_WEAK)
		{
		  sect = elfsh_get_section_by_index(child, sym->st_shndx, NULL, NULL);

		  /* The symbol must be local and pointing on sect_name */
		  if (sect && (sect_name = elfsh_get_section_name(child, sect)) != NULL
		      && !strncmp(sect_name, ".text", 5))
		    {
		      find = child;
		      break;
		    }
		}
	    }
	  
	  /* Search deeper on this child */
	  find = elfsh_traces_search_sym(child, name);
	  
	  if (find)
	    break;
	}
    }
  
  hash_free_keys(keys);

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, find);
}
