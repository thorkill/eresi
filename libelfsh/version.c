/*
** version.c for elfsh
**
** Started     Nov 25 2003 mayhem
** Last update Nov 25 2003 mayhem
** Last update Sep 27 2005 mxatone
**
*/
#include "libelfsh.h"

int 			elfsh_get_verdauxnamelist(elfshobj_t *file, 
						  hashdef_t *hdef, 
						  char **names, int numnames)
{
  elfsh_Verdaux		*table;
  void			*ps;
  u_int			dindex;
  u_int			offset;
  
  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);

  ps = hdef->ps;
  offset = hdef->aux;
  for (dindex = 0; dindex < numnames; dindex++)
    names[dindex] = NULL;

  for (dindex = 0; dindex < hdef->def->vd_cnt; dindex++)
    {
      table = ps + offset;
      if (dindex < numnames)
	  names[dindex] = elfsh_get_verdauxname(file, table);

      if (table->vda_next == NULL)
	break;

      offset += table->vda_next;
    }

  ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}

int			elfsh_load_needtable(hash_t *t, elfshsect_t *sect)
{
  elfsh_Verneed		*table;
  elfsh_Vernaux		*tableaux;
  hashneed_t		*pneed;
  u_int			offset;
  u_int			auxset;
  u_int			size;
  u_int			index;
  u_int			aux;
  void			*ps;
  char			s_temp[8];

  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);

  ps = sect->data;
  size = sect->shdr->sh_size;
  for (index = 0, offset = 0; offset < size; index++)
    {
      table = ps + offset;

      auxset = offset + table->vn_aux;
      for (aux = 0; aux < table->vn_cnt; aux++) 
	{
	  tableaux = ps + auxset;

	  snprintf(s_temp, 8, "%u", tableaux->vna_other);
	  XALLOC(pneed, sizeof(hashneed_t), -1);
	  pneed->need = table;
	  pneed->aux = tableaux;
	  hash_add(t, strdup(s_temp), (void*) pneed);

	  auxset += tableaux->vna_next;
	}
      
      if (table->vn_next == NULL)
	break;

      offset += table->vn_next;
    }

  ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}

int			elfsh_load_deftable(hash_t *t, elfshsect_t *sect)
{
  elfsh_Verdef		*table;
  hashdef_t		*pdef;
  u_int			offset;
  u_int			size;
  u_int			index;
  void			*ps;
  char			s_temp[8];

  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);

  ps = sect->data;
  size = sect->shdr->sh_size;
  for (index = 0, offset = 0; offset < size; index++)
    {
      table = ps + offset;

      snprintf(s_temp, 8, "%u", table->vd_ndx);
      XALLOC(pdef, sizeof(hashdef_t), -1);
      pdef->ps = ps;
      pdef->def = table;
      pdef->aux = (offset + table->vd_aux);
      hash_add(t, strdup(s_temp), (void*) pdef);

      if (table->vd_next == NULL)
	break;

      offset += table->vd_next;
    }

  ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}

elfshsect_t    		*elfsh_get_verstrtable(elfshobj_t *file)
{
  elfshsect_t		*dynstr;
  int	       		strindex, dnum;

  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);

  if (file->secthash[ELFSH_SECTION_DYNSTR] == NULL)
    {

      dynstr = elfsh_get_section_by_name(file, ELFSH_SECTION_NAME_DYNSTR, 
					 NULL, &strindex, &dnum);
      if (dynstr == NULL)
	ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, 
			  "Section not found", NULL); 

      if (dynstr->data == NULL)
	{
	  dynstr->data = elfsh_load_section(file, dynstr->shdr);
	  if (dynstr->data == NULL)
	    ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, 
			      "Data section not found", NULL); 
	}

      file->secthash[ELFSH_SECTION_DYNSTR] = dynstr;
    
    }

  ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, 
		     (file->secthash[ELFSH_SECTION_DYNSTR]->data));
}

char	       	*elfsh_get_verneedfile(elfshobj_t *file, elfsh_Verneed *n)
{
  void 	    	*str;

  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);  

  str = elfsh_get_verstrtable(file);
  if (str == NULL)
       ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Unable to find STR table for verneedfile", NULL);    

  ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, (str + n->vn_file));
}

char		*elfsh_get_vernauxname(elfshobj_t *file, elfsh_Vernaux *a)
{
  void 		*str;

  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);  

  str = elfsh_get_verstrtable(file);
  if (str == NULL)
       ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Unable to find STR table for verneedfile", NULL);    

  ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, (str + a->vna_name));
}

char		*elfsh_get_verdauxname(elfshobj_t *file, elfsh_Verdaux *a)
{
  void 		*str;

  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);  

  str = elfsh_get_verstrtable(file);
  if (str == NULL)
       ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Unable to find STR table for verdeffile", NULL);    

  ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, (str + a->vda_name));
}

void		*elfsh_get_versymtab(elfshobj_t *file, int *num)
{
  elfshsect_t	*sect;
  int		strindex, nbr;

  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);  


  if (file->secthash[ELFSH_SECTION_GNUVERSYM] == NULL)
    {
      sect = elfsh_get_section_by_type(file, SHT_GNU_versym, 0, 
				       NULL, &strindex, &nbr);

      if (sect == NULL)
        ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, 
			  "Unable to find Symbol Version table", NULL);;

      if (sect->data == NULL)
	{
	  sect->data = elfsh_load_section(file, sect->shdr);
	  if (sect->data == NULL)
	    ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, 
			      "Unable to find data for Symbol Version table", 
			      NULL); 
	}

      file->secthash[ELFSH_SECTION_GNUVERSYM] = sect;
    }

  nbr = file->secthash[ELFSH_SECTION_GNUVERSYM]->shdr->sh_size;
  nbr /= sizeof(elfsh_Half);

  *num = nbr;

  ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, 
		     (file->secthash[ELFSH_SECTION_GNUVERSYM]));
}

void		*elfsh_get_verneedtab(elfshobj_t *file, int *num)
{
  elfshsect_t	*sect;
  int		strindex, nbr;

  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);  


  if (file->secthash[ELFSH_SECTION_GNUVERNEED] == NULL)
    {
      sect = elfsh_get_section_by_type(file, SHT_GNU_verneed, 0, 
				       NULL, &strindex, &nbr);

      if (sect == NULL)
        ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, 
			  "Unable to find Needed Version table", NULL);;

      if (sect->data == NULL)
	{
	  sect->data = elfsh_load_section(file, sect->shdr);
	  if (sect->data == NULL)
	    ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, 
			      "Unable to find data for Needed Version table", 
			      NULL); 
	}

      file->secthash[ELFSH_SECTION_GNUVERNEED] = sect;
    }

  nbr = file->secthash[ELFSH_SECTION_GNUVERNEED]->shdr->sh_size;
  nbr /= sizeof(elfsh_Verneed);

  *num = nbr;

  ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, 
		     (file->secthash[ELFSH_SECTION_GNUVERNEED]));
}

void		*elfsh_get_verdeftab(elfshobj_t *file, int *num)
{
  elfshsect_t	*sect;
  int		strindex, nbr;

  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);  


  if (file->secthash[ELFSH_SECTION_GNUVERDEF] == NULL)
    {
      sect = elfsh_get_section_by_type(file, SHT_GNU_verdef, 0, 
				       NULL, &strindex, &nbr);

      if (sect == NULL)
        ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, 
			  "Unable to find Definition Version table", NULL);

      if (sect->data == NULL)
	{
	  sect->data = elfsh_load_section(file, sect->shdr);
	  if (sect->data == NULL)
	    ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, 
			      "Unable to find data for Definition "
			      "Version table", NULL); 
	}

      file->secthash[ELFSH_SECTION_GNUVERDEF] = sect;
    }

  nbr = file->secthash[ELFSH_SECTION_GNUVERDEF]->shdr->sh_size;
  nbr /= sizeof(elfsh_Verdef);

  *num = nbr;

  ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, 
		     (file->secthash[ELFSH_SECTION_GNUVERDEF]));
}

