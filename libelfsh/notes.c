/*
** notes.c for libelfsh
** 
** Started on  Sun Jun 10 22:57:09 2001 mayhem
** Last update Tue Jun  3 12:37:56 2003 mayhem
*/
#include "libelfsh.h"




/* Return the content of the 'range'th section of type SHT_NOTE */
elfshsect_t		*elfsh_get_notes(elfshobj_t *file, u_int range)
{
  elfshsect_t		*notes;
  elfshnotent_t		*e;
  elfshnotent_t		*tmp;	
  int			offset;
  int			size;

  /* Search and load the section if necessary */
  notes = elfsh_get_section_by_type(file, SHT_NOTE, range, 
				    NULL, NULL, &size);
  if (notes == NULL)
    return (NULL);
  if (notes->data == NULL)
    {
      notes->data = elfsh_load_section(file, notes->shdr);
      if (notes->data == NULL)
	return (NULL);
    }
  
  /* Parse all the note entries of the section */
  for (offset = 0; offset < size; 
       offset += (e->namesz + e->descsz + sizeof(int) * 3))
    {
      XALLOC(e, sizeof(elfshnotent_t), NULL);
      e->namesz    = * (int *) notes->data;
      e->namesz   += e->namesz % 4;
      e->descsz    = * (int *) notes->data + 1;
      e->descsz   += e->descsz % 4;

      /* Sanity check */
      if (offset + sizeof(int) * 3 + e->namesz >= size)
	ELFSH_SETERROR("[libelfsh_get_notes] Corrupted Notes section\n", NULL);

      e->note      = strdup((char *) notes->data + offset + sizeof(int) * 3);
      e->desc      = strdup((char *) notes->data + offset + sizeof(int) * 3 
			    + e->namesz);
      
      /* Put the note entry at the end of the list */
      if (notes->altdata == NULL)
	notes->altdata = e;
      else
	{
	  tmp = notes->altdata;
	  while (tmp->next)
	    tmp = tmp->next;
	  tmp->next = e;
	  e->prev = tmp;
	}
    }
  
  return (notes);
}








/* Free the notes section linked list */
void			elfsh_free_notes(elfshobj_t *file)
{
  elfshnotent_t		*e;
  elfshnotent_t		*etmp;
  elfshsect_t		*sect;

  for (sect = file->sectlist; sect; sect = sect->next)
    if (sect->data && sect->shdr->sh_type == SHT_NOTE)
      {
	free(sect->data);
	for (e = sect->altdata; e; e = etmp)
	  {
	    etmp = e->next;
	    free(e->note);
	    free(e->desc);
	    free(e);
	  }
      }
}









