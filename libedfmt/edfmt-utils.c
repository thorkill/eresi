/*
** edfmt-utils.c for libedfmt (The Elf debug format library of ELFsh)
**
** Started Jan 13 2007 18:09:02 mxatone
**
*/

#include "libedfmt.h"

/* Create a string from a key (used in hash table) */
char 			*edfmt_ckey(char *buf, u_int size, long key)
{
  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  if (buf == NULL || size == 0)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Invalid parameters", NULL);

  /* That big but malloc fault on small chunk ! */
  snprintf(buf, size - 1, "%08ld", key);

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}

/* Create a string from a file + line (used in hash table) */
char 			*edfmt_cline(char *buf, u_int size, u_int line, char *file)
{
  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  if (buf == NULL || size == 0)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Invalid parameters", NULL);

  snprintf(buf, size - 1, "%s:%d", file, line);

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}

/* Create a string from an addr (used in hash table) */
char 			*edfmt_caddr(char *buf, u_int size, elfsh_Addr addr)
{
  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  if (buf == NULL || size == 0)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Invalid parameters", NULL);

  snprintf(buf, size - 1, XFMT, addr);

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}

/* like vm_lookup_addr - Get address value */
elfsh_Addr		edfmt_lookup_addr(elfshobj_t *file, char *param)
{
  elfsh_Sym		*sym;
  char			eol;
  int			ret;
  elfsh_Addr	       	val;

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
		    "Unable to lookup address object", (elfsh_Addr) 0);
}

void 			*edfmt_alloc_pool(char **pool, int *apos, int *asize, 
					  int astep, int nsize)
{
  char			*prevpool;
  char			*ret;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  if (!pool || !apos || astep <= 0 || !asize || nsize <= 0)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		 "Invalid parameters", NULL);

  //printf("asize = %d apos = %d diff = %d, size = %d\n", *asize, *apos, (*asize - *apos), nsize);

  if (*pool == NULL || *asize == 0)
    {
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
      *(int *) *pool = prevpool;
    }

  ret = (char *) *pool + *apos;
  *apos += nsize;

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, (void *) ret);
}

/* Clean an allocated pool */
int 			edfmt_clean_pool(char **pool)
{
  char			*prevpool = NULL;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  if (!pool)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		 "Invalid parameters", -1);

  do {
    prevpool = *(int *) *pool;
    XFREE(__FILE__, __FUNCTION__, __LINE__, *pool);
    *pool = prevpool;
  } while (*pool != NULL);

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}
