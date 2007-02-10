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
  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);

  if (buf == NULL || size == 0)
    ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Invalid parameters", NULL);

  /* That big but malloc fault on small chunk ! */
  snprintf(buf, size - 1, "%08ld", key);

  ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}

/* Create a string from a file + line (used in hash table) */
char 			*edfmt_cline(char *buf, u_int size, u_int line, char *file)
{
  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);

  if (buf == NULL || size == 0)
    ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Invalid parameters", NULL);

  snprintf(buf, size - 1, "%s:%d", file, line);

  ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}

/* Create a string from an addr (used in hash table) */
char 			*edfmt_caddr(char *buf, u_int size, elfsh_Addr addr)
{
  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);

  if (buf == NULL || size == 0)
    ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Invalid parameters", NULL);

  snprintf(buf, size - 1, XFMT, addr);

  ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}

/* like vm_lookup_addr - Get address value */
elfsh_Addr		edfmt_lookup_addr(elfshobj_t *file, char *param)
{
  elfsh_Sym		*sym;
  char			eol;
  int			ret;
  elfsh_Addr	       	val;

  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);

  /* Lookup .symtab */
  sym = elfsh_get_symbol_by_name(file, param);
  if (sym != NULL && sym->st_value > 0)
    {
      ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, sym->st_value);
    }

  /* Lookup .dynsym */
  sym = elfsh_get_dynsymbol_by_name(file, param);
  if (sym != NULL && sym->st_value > 0)
    {
      ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, sym->st_value);
    }

  /* Lookup hexadecimal numeric value */
  ret = sscanf(param, XFMT "%c", &val, &eol);
  if (ret == 1)
    {
      ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, val);
    }

  /* No match -- returns ERR */
  ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, 
		    "Unable to lookup address object", (elfsh_Addr) 0);
}
