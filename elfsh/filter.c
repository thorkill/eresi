/*
** filter.c for elfsh
** 
** Started on  Thu Mar 13 23:27:58 2003 mayhem
** Last update Sat Mar 15 07:00:38 2003 mayhem
*/
#include "elfsh.h"


/* Filter \x00 in the OBJ_TYPESTR object */
void			vm_filter_zero(elfshpath_t *obj)
{
  char			*ptr;
  char			*buf;
  
  buf = (obj->immed ? obj->immed_val.str : 
	 obj->get_name(obj->root, obj->parent));

  do
    {
      ptr = strstr(buf, "\\x00");
      if (ptr != NULL)
	{
	  *ptr = 0x00;
	  memmove(ptr + 1, ptr + 4, (u_int) obj->immed_val.str + obj->size - (u_int) (ptr + 4));
	  buf = ptr + 1;
	  obj->size -= 3;
	}
      else 
	{
	  ptr = strstr(buf, "\\x0");
	  if (ptr == NULL)
	    break;
	  *ptr = 0x00;
	  memmove(ptr + 1, ptr + 3, (u_int) obj->immed_val.str + obj->size - (u_int) (ptr + 3));
	  buf = ptr + 1;
	  obj->size -= 2;
	}
    }
  while (ptr != NULL);
}




/* Replace \xNUM taking care of the \x00 in a string */
char		*vm_filter_param(char *buf, char *ptr)
{
  u_int		nbr;
  char		c;
  char		d;
  
  /* if string ends with '\x', its over for this entry */
  if (*(ptr + 2) == 0x00)
    return (buf);
  
  /* Do exception for \x00 which must not be resolved */
  c = *(ptr + 2);
  d = *(ptr + 3);
  if (c == '0' && !((d >= 'A' && d <= 'F') || (d >= '1' && d <= '9')))
    return (buf + (d == '0' ? 4 : 3));
  
  /* else read the hexadecimal value */
  else
    {
      sscanf(ptr + 2, "%X", &nbr);
      *ptr = (char) nbr;
      buf = ptr + 4;
      
      /* and copy the data, strcpy put NUL at the end */
      strcpy(ptr + 1, buf);
      return (ptr + 1);
    }
}
