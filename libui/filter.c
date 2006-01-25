/*
** filter.c for elfsh
** 
** Started on  Thu Mar 13 23:27:58 2003 mayhem
** Last update Sat Mar 15 07:00:38 2003 mayhem
*/
#include "libui.h"


/* Filter \x00 in the OBJ_TYPESTR object */
void			vm_filter_zero(char *buf)
{
  char			*ptr;
  u_int			size;

  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);

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
  ELFSH_PROFILE_OUT(__FILE__, __FUNCTION__, __LINE__);
}




/* Replace \xNUM taking care of the \x00 in a string */
char		*vm_filter_param(char *buf, char *ptr)
{
  u_int		nbr;
  char		c;
  char		d;
  
  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);

  /* if string ends with '\x', its over for this entry */
  if (*(ptr + 2) == 0x00)
    ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, (buf));
  
  /* Do exception for \x00 which must not be resolved */
  c = *(ptr + 2);
  d = *(ptr + 3);
  if (c == '0' && !((d >= 'A' && d <= 'F') || (d >= '1' && d <= '9')))
    ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, 
		       (buf + (d == '0' ? 4 : 3)));
  
  /* else read the hexadecimal value */
  else
    {
      sscanf(ptr + 2, "%X", &nbr);
      *ptr = (char) nbr;
      buf = ptr + 4;
      
      /* and copy the data, strcpy put NUL at the end */
      strcpy(ptr + 1, buf);
      ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, (ptr + 1));
    }

  assert(1);
}
