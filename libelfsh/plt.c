/*
** plt.c for libelfsh
** 
** Started on  Tue Jul 24 06:40:07 2001 mayhem
**
*/
#include "libelfsh.h"


/* Load the Procedure Linkage Table */
elfshsect_t	*elfsh_get_plt(elfshobj_t *file, int *num)
{
  elfshsect_t	*actual;
  void		*ret;

  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);
  
  if (file->sht == NULL && elfsh_get_sht(file, NULL) == NULL)
    ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Unable to get SHT", NULL);

  for (actual = file->sectlist; actual != NULL; actual = actual->next)
    if (elfsh_is_plt(file, actual))
      {
	ret = elfsh_get_anonymous_section(file, actual);
	if (!ret)
	  ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, 
			    "Unable to get PLT", NULL);
	file->secthash[ELFSH_SECTION_PLT] = actual;
      }
    else if (elfsh_is_altplt(file, actual))
      {
	ret = elfsh_get_anonymous_section(file, actual);
	if (!ret)
	  ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, 
			    "Unable to get ALTPLT", NULL);
	file->secthash[ELFSH_SECTION_ALTPLT] = actual;
      }

  ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, 
	                 (file->secthash[ELFSH_SECTION_PLT]));
}


/* Return 1 if the section is the plt or 0 if its not */
int		elfsh_is_plt(elfshobj_t *file, elfshsect_t *sct)
{
  char		*name;

  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);

  name = elfsh_get_section_name(file, sct);
  if (name != NULL && !strcmp(name, ELFSH_SECTION_NAME_PLT))
    ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, 1);
  ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}

/* Return 1 if the section is the plt or 0 if its not */
int		elfsh_is_altplt(elfshobj_t *file, elfshsect_t *sct)
{
  char		*name;

  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);

  name = elfsh_get_section_name(file, sct);
  if (name != NULL && !strcmp(name, ELFSH_SECTION_NAME_ALTPLT))
    ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, 1);
  ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}

/* Return 1 if the symbol points in the PLT, 0 if not, -1 if the PLT cant be found */
int			elfsh_is_pltentry(elfshobj_t *file, elfsh_Sym *sym)
{
  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);

  if (sym == NULL || elfsh_get_plt(file, NULL) == NULL)
    ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Invalid NULL parameter", -1);

  if (sym->st_size != file->secthash[ELFSH_SECTION_PLT]->shdr->sh_size)
  {
    if (INTERVAL(file->secthash[ELFSH_SECTION_PLT]->shdr->sh_addr, 
                 sym->st_value, 
                 file->secthash[ELFSH_SECTION_PLT]->shdr->sh_addr + 
                 file->secthash[ELFSH_SECTION_PLT]->shdr->sh_size))
      ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, (1));
  }
  ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}


/* Return the size of 1 PLT entry */
int		elfsh_get_pltentsz(elfshobj_t *file)
{
  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);

  if (file == NULL)
    ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Invalid NULL parameter", -1);

  switch (elfsh_get_arch(file->hdr))
    {
    case EM_386:
      /* XXX: case EM_486: */
      ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, (16));
    case EM_SPARC:
    case EM_SPARC32PLUS:
      ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, (12));
    case EM_SPARCV9:
      ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, (32));
    case EM_ALPHA:
#if (EM_ALPHA != EM_ALPHA_EXP)
    case EM_ALPHA_EXP:
#endif
      ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, (12));
    case EM_MIPS:
    case EM_MIPS_RS3_LE:
      ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, (16));
    default:
      ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, 
			"Unsupported architecture", -1);
    }
}


/* Return the size of the FIRST PLT entry on this arch */
/* Its a different size on Alpha and Sparc */
int		 elfsh_get_first_pltentsz(elfshobj_t *file)
{
  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);

  if (file == NULL)
    ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Invalid NULL parameter", -1);

  switch (elfsh_get_arch(file->hdr))
    {
    case EM_386:
      /* XXX: case EM_486: */
      ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, (16));
    case EM_SPARC:
    case EM_SPARC32PLUS:
      ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, (48));
    case EM_SPARCV9:
      ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, (124));
    case EM_ALPHA:
#if (EM_ALPHA != EM_ALPHA_EXP)
    case EM_ALPHA_EXP:
#endif
      ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, (32));
    case EM_MIPS:
    case EM_MIPS_RS3_LE:
      ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, (16));
    default:
      ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, 
			"Unsupported architecture", -1);
    }
}














