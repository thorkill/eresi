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
