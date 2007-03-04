/*
** stabs-utils.c for libedfmt (The Elf debug format library of ELFsh)
**
** Started Jan 01 2007 21:30:13 mxatone
**
*/

#include "libedfmt.h"

/* Read a stabs name, delimited by a special char */
char		*edfmt_stabs_readstr(char *buf, u_int size, char **str, char c_delim)
{
  u_int		csize;
  char		*delim;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  if (NULL == buf || size == 0 || STABS_IVD_STR(str) || c_delim == 0)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		       "Invalid parameters", NULL);

  delim = strchr(*str, c_delim);

  csize = delim ? delim - *str + 1 : size;

  if (csize > size)
    csize = size;

  /* Copy the name into buf string */
  strncpy(buf, *str, csize);
  buf[csize - 1] = 0x00;

  *str += csize;
  
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, buf);
}

/* Read a number from a string 
   We didn't support octal numbers very well, in fact this informations is quite
   uninsteresting, we know the architecture and the type name, it would be far
   more easy to use an hash table to understand perfectly which type we are
   talking about.
 */
int		edfmt_stabs_readnumber(char **str, char c_delim, long *set_num)
{
  int		sign = 1;
  int		base = 10;
  long		num = 0;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  if (STABS_IVD_STR(str) || !set_num)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Invalid parameter", -1);

  if (**str == '-')
    {
      sign = -1;
      (*str)++;
    }

  if (**str == '0')
    {
      base = 8;
      (*str)++;
    }

  for (;**str >= '0' && **str <= ('0' + base); (*str)++)
    {
      num *= base;
      num += **str - '0';
    }

  if (c_delim && **str != c_delim)
    PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, -1);

  *set_num = num * sign;

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}

/* Read a type number (num,num) or num */
int		edfmt_stabs_typenum(edfmtstabstypenum_t *tnum, char **str)
{
  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);
  
  if (STABS_IVD_STR(str))
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Invalid parameter", -1);

  /* With two ids ? */
  if (**str == '(')
    {
      (*str)++;

      /* First element, the file id */
      if (edfmt_stabs_readnumber(str, ',', &tnum->file) != 0)
	PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, -1);

      (*str)++;

      /* Second element the real id */
      if (edfmt_stabs_readnumber(str, ')', &tnum->number) != 0)
	PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, -1);      
       
      (*str)++;
    }
  else
    {
      /* We support basic typenums */
      tnum->file = 0;
      if (edfmt_stabs_readnumber(str, 0, &tnum->number) != 0)
	PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, -1);      
    }

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}

/* Create a string from a typenum
   A typenum represent a type using 1 or 2 ids, on the documentation we see only 1 id
   but now there's two ids, the first describe the file.
 */
int		edfmt_stabs_ctypenum(char *buf, u_int size, edfmtstabstypenum_t *tnum)
{
  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  if (!buf || size == 0 || !tnum)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Invalid parameters", -1);

  snprintf(buf, size - 1, "(%ld,%ld)", tnum->file, tnum->number);

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}
