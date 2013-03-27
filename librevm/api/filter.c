/**
 * @file librevm/api/filter.c
 * @ingroup api
 * @brief Filter 0x00 in the OBJ_TYPESTR object.
 * Started on  Thu Mar 13 23:27:58 2003 jfv
 * Last update Sat Mar 15 07:00:38 2003 jfv
 *
 * $Id$
 *
 */
#include "libui.h"

/* Filter \x00 in the OBJ_TYPESTR object */
void			revm_filter_zero(char *buf)
{
  char			*ptr;
  u_int			size;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  size = strlen(buf);
  do
    {
      ptr = strstr(buf, "\\x00");
      if (ptr != NULL)
        {
          *ptr = 0x00;
          memmove(ptr + 1, ptr + 4, (u_int) buf + size - (u_int) (ptr + 4));
          buf = ptr + 1;
          size -= 3;
        }
      else
        {
          ptr = strstr(buf, "\\x0");
          if (ptr == NULL)
            break;
          *ptr = 0x00;
          memmove(ptr + 1, ptr + 3, (u_int) buf + size - (u_int) (ptr + 3));
          buf = ptr + 1;
          size -= 2;
        }
    }
  while (ptr != NULL);
  PROFILER_OUT(__FILE__, __FUNCTION__, __LINE__);
}




/* Replace \xNUM taking care of the \x00 in a string */
/*
char		*revm_filter_param(char *buf, char *ptr)
{
  u_int		nbr;
  char		c;
  char		d;
  u_int		len;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  //if string ends with '\x', its over for this entry
  if (*(ptr + 2) == 0x00)
    PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, (buf));

  //Do exception for \x00 which must not be resolved
  c = *(ptr + 2);
  d = *(ptr + 3);
  if (c == '0' && !((d >= 'A' && d <= 'F') || (d >= '1' && d <= '9')))
    PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, ptr + 4);

  //else read the hexadecimal value
  sscanf(ptr + 2, "%X", &nbr);
  *ptr = (char) nbr;
  buf = ptr + 4;

  //If we are ending the search buffer
  if (*buf == 0x00)
    {
      *(ptr + 1) = 0x00;
      PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, (ptr + 1));
    }

  //Copy the remaining data
  len = strlen(buf);
  memmove(ptr + 1, buf, len);
  *(ptr + 1 + len) = 0x00;
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, (ptr + 1));
}
*/

char		*revm_filter_param(char *buf, char *ptr)
{
  u_int		nbr;
  char		c;
  char		d;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  //if string ends with '\x', its over for this entry
  if (*(ptr + 2) == 0x00)
    PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, (buf));

  //Do exception for \x00 which must not be resolved
  c = *(ptr + 2);
  d = *(ptr + 3);
  if (c == '0' && !((d >= 'A' && d <= 'F') || (d >= '1' && d <= '9')))
    {
      u_int off = (*(ptr + 3) == 0x00 ? 3 : 4);
      PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, ptr + off);
    }
  
  sscanf(ptr + 2, "%X", &nbr);
  *ptr = (char) nbr;
  buf = ptr + 4;

  /* and copy the data, strcpy put NUL at the end */
  strcpy(ptr + 1, buf);
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, (ptr + 1));
}
