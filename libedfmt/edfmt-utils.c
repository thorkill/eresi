/*
** @file edfmt-utils.c
** @ingroup libedfmt
**
** Started Jan 13 2007 18:09:02 mxatone
**
**
** $Id: edfmt-utils.c,v 1.11 2007-07-17 18:11:24 may Exp $
**
*/

#include "libedfmt.h"

/**
 * Create a string from a key (used in hash table) 
 * @param buf destination buffer
 * @param size size of the buffer
 * @param key key to transform
 * @return the modified pointer or NULL
 */
char 			*edfmt_ckey(char *buf, unsigned int size, long key)
{
  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  if (buf == NULL || size == 0)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Invalid parameters", NULL);

  /* That big but malloc fault on small chunk ! */
  snprintf(buf, size - 1, "%08ld", key);

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, buf);
}

/**
 * Create a string from a file + line (used in hash table) 
 * @param buf destination buffer
 * @param size size of the buffer
 * @param line line number
 * @param file filename
 * @return the modified pointer or NULL
 */
char 			*edfmt_cline(char *buf, unsigned int size, unsigned int line, char *file)
{
  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  if (buf == NULL || size == 0)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Invalid parameters", NULL);

  snprintf(buf, size - 1, "%s:%d", file, line);

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, buf);
}






/**
 * Create a string from an addr (used in hash table) 
 * @param buf destination buffer
 * @param size size of the buffer
 * @param addr address to transform
 * @return the modified pointer or NULL
 */
char 			*edfmt_caddr(char *buf, unsigned int size, eresi_Addr addr)
{
  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  if (buf == NULL || size == 0)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Invalid parameters", NULL);

  snprintf(buf, size - 1, XFMT, addr);

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, buf);
}

/**
 * like revm_lookup_addr - Get address value 
 * @param file host file
 * @param param name to search
 * @return addresse found or 0
 */
eresi_Addr		edfmt_lookup_addr(elfshobj_t *file, char *param)
{
  elfsh_Sym		*sym;
  char			eol;
  int			ret;
  eresi_Addr	       	val;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  /* Lookup .symtab */
  sym = elfsh_get_symbol_by_name(file, param);
  if (sym != NULL && sym->st_value > 0)
    {
      PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, sym->st_value);
    }

  /* Lookup .dynsym */
  sym = elfsh_get_dynsymbol_by_name(file, param);
  if (sym != NULL && sym->st_value > 0)
    {
      PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, sym->st_value);
    }

  /* Lookup hexadecimal numeric value */
  ret = sscanf(param, XFMT "%c", &val, &eol);
  if (ret == 1)
    {
      PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, val);
    }

  /* No match -- returns ERR */
  PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		    "Unable to lookup address object", (eresi_Addr) 0);
}



/** 
 * Create an allocation pool used to store different data and optimize performance
 * This pool didn't realloc the buffer each time it needs more memory, but create a new
 * buffer and store reference of the previous (like a linked list).
 *
 * Reallocation isn't used because I already have some pointer inside in the allocation and
 * if I made a XREALLOC the whole buffer would be recreate on another memory part, and some
 * pointers would be lost.
 * @param pool pool pointer
 * @param apos pool position
 * @param asize pool size
 * @param astep pool step
 * @param nsize needed size
 * @return pointer on the allocated memory
 */
void 			*edfmt_alloc_pool(char **pool, int *apos, int *asize, 
					  int astep, int nsize)
{
  char			*prevpool;
  char			*ret;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  if (!pool || !apos || astep <= 0 || !asize || nsize <= 0)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		 "Invalid parameters", NULL);

  /* First allocation */
  if (*pool == NULL || *asize == 0)
    {
      /* We add a void* blanked that indicate that its the first block*/
      XALLOC(__FILE__, __FUNCTION__, __LINE__, *pool, astep+sizeof(void*), NULL);
      *asize = astep;
      *apos += sizeof(void*);
    }
  else if (*apos + nsize >= *asize)
    {
      *asize = 0;
      *apos = 0;
      do {
	*asize += astep;
      } while (*apos + nsize >= *asize);

      prevpool = *pool;
      
      XALLOC(__FILE__, __FUNCTION__, __LINE__, *pool, *asize+sizeof(void*), NULL);
      *apos += 4;

      /* Store previous block address then we can retrieve it later */
      *(void **) *pool = prevpool;
    }

  /* Save current position and update position pointer */
  ret = (char *) *pool + *apos;
  *apos += nsize;

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, (void *) ret);
}

/**
 * Clean an allocated pool 
 * @param pool pool pointer
 */
int 			edfmt_clean_pool(char **pool)
{
  char			*prevpool = NULL;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  if (!pool)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		 "Invalid parameters", -1);

  /* Iterate and free each block */
  do {
    prevpool = *(void **) *pool;
    XFREE(__FILE__, __FUNCTION__, __LINE__, *pool);
    *pool = prevpool;
  } while (*pool != NULL);

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}
