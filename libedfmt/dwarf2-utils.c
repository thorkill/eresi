/*
** dwarf2-utils.c for libedfmt (The Elf debug format library of ELFsh)
**
** Started Dec 26 2006 10:49:45 mxatone
**
*/

#include "libedfmt.h"

/* Create a string from a key (used in hash table) */
char 		*edfmt_dwarf2_ckey(char *buf, u_int size, long key)
{
  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);

  if (buf == NULL || size == 0)
    ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Invalid parameters", NULL);

  snprintf(buf, size, "%ld", key);

  ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}

/* Create a string from a file + line (used in hash table) */
char 		*edfmt_dwarf2_cline(char *buf, u_int size, u_int line, char *file)
{
  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);

  if (buf == NULL || size == 0)
    ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Invalid parameters", NULL);

  snprintf(buf, size, "%s:%d", file, line);

  ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}

/* Create a string from an addr (used in hash table) */
char 		*edfmt_dwarf2_caddr(char *buf, u_int size, elfsh_Addr addr)
{
  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);

  if (buf == NULL || size == 0)
    ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Invalid parameters", NULL);

  snprintf(buf, size, XFMT, addr);

  ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}

/* Read an unsigned leb128 number */
u_long 		edfmt_read_uleb128(void *data, u_int *bread)
{
  u_long	sum = 0;
  u_int		read = 0;
  u_char 	c;
  int		s = 0;

  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);

  do {
    c = *(u_char *) (data + read) & 0xFF; 
    read++;
    sum |= ((u_long)(c & 127) << s);
    
    s += 7;
  } while ((c & 128) != 0);

  if (bread)
    *bread = read;

  ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, sum);
}

/* Read a signed leb128 number */
long 		edfmt_read_leb128(void *data, u_int *bread)
{
  long		sum = 0;
  u_int		read = 0;
  u_char 	c;
  int		s = 0;

  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);

  do {
    c = *(u_char *) (data + read) & 0xFF; 
    read++;
    sum |= ((long)(c & 127) << s);
    
    s += 7;
  } while ((c & 128) != 0);

  if ((s < 8 * sizeof(sum)) && (c & 0x40))
    sum |= -(((long)1) << s);

  if (bread)
    *bread = read;

  ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, sum);
}
