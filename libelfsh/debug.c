/*
** debug.c for elfsh
**
** Started on  Thu Apr  3 21:36:55 2003 mayhem
** Last update Sat Aug 16 22:10:07 2003 jv
*/
#include "libelfsh.h"



/* Debug : print the section list */
int		elfsh_print_sectlist(elfshobj_t *obj, char *label)
{
  elfshsect_t	*actual;
  u_int		index;
  u_char	*data;
  char		*sctname;

  printf(" [SCTLIST][%s]\n", label);
  for (index = 0, actual = obj->sectlist;
       actual != NULL;
       index++, actual = actual->next)
    {
      sctname = elfsh_get_section_name(obj, actual);
      if (sctname == NULL)
	sctname = "UNK";
      data = (actual->data == NULL ? "\xFF\xFF\xFF" : actual->data);
      printf(" [%03u] %-15s HDRNAM: %-15s BYTES[%02X %02X %02X] P(%08X) "
	     "A(%08X) N(%08X) SCTIDX(%03u) HDRFOFF:%08x SZ:%08x VADDR:%08X \n",
	     index, (actual->name != NULL ? actual->name : "UNK"),
	     sctname,
	     (u_int) data[0],
	     (u_int) data[1],
	     (u_int) data[2],
	     (u_int) actual->prev,
	     (u_int) actual,
	     (u_int) actual->next,
	     actual->index,
	     (u_int) actual->shdr->sh_offset,
	     (u_int) actual->shdr->sh_size,
	     (u_int) actual->shdr->sh_addr);
    }
  puts(" [EOL]\n");
  fflush(stdout);
  return (0);
}
