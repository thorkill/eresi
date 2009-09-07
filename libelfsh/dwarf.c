/**
 * @file dwarf.c
 * @ingroup libelfsh
** dwarf.c for libelfsh
** 
** Started on  Fri Mao  17 23:11:47 2007 mxatone
** 
**
**
*/
#include "libelfsh.h"

int			elfsh_get_dwarf(elfshobj_t *file)
{
  void			*data;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  data = elfsh_get_dwarf_info(file, NULL);
  //if (data == NULL)
  //fprintf(stderr, "Failed to load section data for INFO\n");
  data = elfsh_get_dwarf_abbrev(file, NULL);
  //if (data == NULL)
  //fprintf(stderr, "Failed to load section data for ABBREV\n");
  data = elfsh_get_dwarf_aranges(file, NULL);
  //if (data == NULL)
  //fprintf(stderr, "Failed to load section data for ARANGES\n");
  data = elfsh_get_dwarf_frame(file, NULL);
  //if (data == NULL)
  //fprintf(stderr, "Failed to load section data for FRAME\n");
  data = elfsh_get_dwarf_line(file, NULL);
  //if (data == NULL)
  //fprintf(stderr, "Failed to load section data for LINE\n");
  data = elfsh_get_dwarf_macinfo(file, NULL);
  //if (data == NULL)
  //fprintf(stderr, "Failed to load section data for MACINFO\n");
  data = elfsh_get_dwarf_pubnames(file, NULL);
  //if (data == NULL)
  //fprintf(stderr, "Failed to load section data for PUBNAMES\n");
  data = elfsh_get_dwarf_str(file, NULL);
  //if (data == NULL)
  //fprintf(stderr, "Failed to load section data for STR\n");
  data = elfsh_get_dwarf_loc(file, NULL);  
  //if (data == NULL)
  //fprintf(stderr, "Failed to load section data for LOC\n");

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}

/**
 * Load dwarf info table
 * @param file file objet
 * @param num object number pointer
 * @return data pointer
 */
void			*elfsh_get_dwarf_info(elfshobj_t *file, int *num)
{
  elfshsect_t		*sect;
  int			strindex, nbr;
  void			*ret;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  if (file->secthash[ELFSH_SECTION_DW2_INFO] == NULL)
    {
      sect = elfsh_get_section_by_name(file, ELFSH_SECTION_NAME_DW2_INFO, 0, 
				       &strindex, &nbr);

      if (sect == NULL)
        PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
			  "Unable to find Dwarf info table", NULL);;

      if (sect->data == NULL)
	{
	  sect->data = elfsh_load_section(file, sect->shdr);
	  if (sect->data == NULL)
	    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
			      "Unable to find data for Dwarf info table", 
			      NULL); 
	}

      file->secthash[ELFSH_SECTION_DW2_INFO] = sect;
    }

  /* Search number of entry do we get here */
  nbr = file->secthash[ELFSH_SECTION_DW2_INFO]->shdr->sh_size;

  if (num)
    *num = nbr;

  ret = elfsh_readmem(file->secthash[ELFSH_SECTION_DW2_INFO]);

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, ret);
}

/**
 * Load dwarf abbrev table
 * @param file file objet
 * @param num object number pointer
 * @return data pointer
 */
void			*elfsh_get_dwarf_abbrev(elfshobj_t *file, int *num)
{
  elfshsect_t		*sect;
  int			strindex, nbr;
  void			*ret;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  if (file->secthash[ELFSH_SECTION_DW2_ABBREV] == NULL)
    {
      sect = elfsh_get_section_by_name(file, ELFSH_SECTION_NAME_DW2_ABBREV, 0, 
				       &strindex, &nbr);

      if (sect == NULL)
        PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
			  "Unable to find Dwarf abbrev table", NULL);;

      if (sect->data == NULL)
	{
	  sect->data = elfsh_load_section(file, sect->shdr);
	  if (sect->data == NULL)
	    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
			      "Unable to find data for Dwarf abbrev table", 
			      NULL); 
	}

      file->secthash[ELFSH_SECTION_DW2_ABBREV] = sect;
    }

  /* Search number of entry do we get here */
  nbr = file->secthash[ELFSH_SECTION_DW2_ABBREV]->shdr->sh_size;

  if (num)
    *num = nbr;

  ret = elfsh_readmem(file->secthash[ELFSH_SECTION_DW2_ABBREV]);

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, ret);
}

/**
 * Load dwarf aranges table
 * @param file file objet
 * @param num object number pointer
 * @return data pointer
 */
void		*elfsh_get_dwarf_aranges(elfshobj_t *file, int *num)
{
  elfshsect_t		*sect;
  int			strindex, nbr;
  void			*ret;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  if (file->secthash[ELFSH_SECTION_DW2_ARANGES] == NULL)
    {
      sect = elfsh_get_section_by_name(file, ELFSH_SECTION_NAME_DW2_ARANGES, 0, 
				       &strindex, &nbr);

      if (sect == NULL)
        PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
			  "Unable to find Dwarf aranges table", NULL);;

      if (sect->data == NULL)
	{
	  sect->data = elfsh_load_section(file, sect->shdr);
	  if (sect->data == NULL)
	    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
			      "Unable to find data for Dwarf aranges table", 
			      NULL); 
	}

      file->secthash[ELFSH_SECTION_DW2_ARANGES] = sect;
    }

  /* Search number of entry do we get here */
  nbr = file->secthash[ELFSH_SECTION_DW2_ARANGES]->shdr->sh_size;

  if (num)
    *num = nbr;

  ret = elfsh_readmem(file->secthash[ELFSH_SECTION_DW2_ARANGES]);

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, ret);
}

/**
 * Load dwarf frame table
 * @param file file objet
 * @param num object number pointer
 * @return data pointer
 */
void		*elfsh_get_dwarf_frame(elfshobj_t *file, int *num)
{
  elfshsect_t		*sect;
  int			strindex, nbr;
  void			*ret;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  if (file->secthash[ELFSH_SECTION_DW2_FRAME] == NULL)
    {
      sect = elfsh_get_section_by_name(file, ELFSH_SECTION_NAME_DW2_FRAME, 0, 
				       &strindex, &nbr);

      if (sect == NULL)
        PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
			  "Unable to find Dwarf frame table", NULL);;

      if (sect->data == NULL)
	{
	  sect->data = elfsh_load_section(file, sect->shdr);
	  if (sect->data == NULL)
	    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
			      "Unable to find data for Dwarf frame table", 
			      NULL); 
	}

      file->secthash[ELFSH_SECTION_DW2_FRAME] = sect;
    }

  /* Search number of entry do we get here */
  nbr = file->secthash[ELFSH_SECTION_DW2_FRAME]->shdr->sh_size;

  if (num)
    *num = nbr;

  ret = elfsh_readmem(file->secthash[ELFSH_SECTION_DW2_FRAME]);

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, ret);
}

/**
 * Load dwarf line table
 * @param file file objet
 * @param num object number pointer
 * @return data pointer
 */
void		*elfsh_get_dwarf_line(elfshobj_t *file, int *num)
{
  elfshsect_t		*sect;
  int			strindex, nbr;
  void			*ret;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  if (file->secthash[ELFSH_SECTION_DW2_LINE] == NULL)
    {
      sect = elfsh_get_section_by_name(file, ELFSH_SECTION_NAME_DW2_LINE, 0, 
				       &strindex, &nbr);

      if (sect == NULL)
        PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
			  "Unable to find Dwarf line table", NULL);;

      if (sect->data == NULL)
	{
	  sect->data = elfsh_load_section(file, sect->shdr);
	  if (sect->data == NULL)
	    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
			      "Unable to find data for Dwarf line table", 
			      NULL); 
	}

      file->secthash[ELFSH_SECTION_DW2_LINE] = sect;
    }

  /* Search number of entry do we get here */
  nbr = file->secthash[ELFSH_SECTION_DW2_LINE]->shdr->sh_size;

  if (num)
    *num = nbr;

  ret = elfsh_readmem(file->secthash[ELFSH_SECTION_DW2_LINE]);

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, ret);
}

/**
 * Load dwarf macinfo table
 * @param file file objet
 * @param num object number pointer
 * @return data pointer
 */
void		*elfsh_get_dwarf_macinfo(elfshobj_t *file, int *num)
{
  elfshsect_t		*sect;
  int			strindex, nbr;
  void			*ret;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  if (file->secthash[ELFSH_SECTION_DW2_MACINFO] == NULL)
    {
      sect = elfsh_get_section_by_name(file, ELFSH_SECTION_NAME_DW2_MACINFO, 0, 
				       &strindex, &nbr);

      if (sect == NULL)
        PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
			  "Unable to find Dwarf abbrev table", NULL);;

      if (sect->data == NULL)
	{
	  sect->data = elfsh_load_section(file, sect->shdr);
	  if (sect->data == NULL)
	    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
			      "Unable to find data for Dwarf abbrev table", 
			      NULL); 
	}

      file->secthash[ELFSH_SECTION_DW2_MACINFO] = sect;
    }

  /* Search number of entry do we get here */
  nbr = file->secthash[ELFSH_SECTION_DW2_MACINFO]->shdr->sh_size;

  if (num)
    *num = nbr;

  ret = elfsh_readmem(file->secthash[ELFSH_SECTION_DW2_MACINFO]);

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, ret);
}

/**
 * Load dwarf pubnames table
 * @param file file objet
 * @param num object number pointer
 * @return data pointer
 */
void		*elfsh_get_dwarf_pubnames(elfshobj_t *file, int *num)
{
  elfshsect_t		*sect;
  int			strindex, nbr;
  void			*ret;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  if (file->secthash[ELFSH_SECTION_DW2_PUBNAMES] == NULL)
    {
      sect = elfsh_get_section_by_name(file, ELFSH_SECTION_NAME_DW2_PUBNAMES, 0, 
				       &strindex, &nbr);

      if (sect == NULL)
        PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
			  "Unable to find Dwarf pubnames table", NULL);;

      if (sect->data == NULL)
	{
	  sect->data = elfsh_load_section(file, sect->shdr);
	  if (sect->data == NULL)
	    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
			      "Unable to find data for Dwarf pubnames table", 
			      NULL); 
	}

      file->secthash[ELFSH_SECTION_DW2_PUBNAMES] = sect;
    }

  /* Search number of entry do we get here */
  nbr = file->secthash[ELFSH_SECTION_DW2_PUBNAMES]->shdr->sh_size;

  if (num)
    *num = nbr;

  ret = elfsh_readmem(file->secthash[ELFSH_SECTION_DW2_PUBNAMES]);

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, ret);
}

/**
 * Load dwarf str table
 * @param file file objet
 * @param num object number pointer
 * @return data pointer
 */
void		*elfsh_get_dwarf_str(elfshobj_t *file, int *num)
{
  elfshsect_t		*sect;
  int			strindex, nbr;
  void			*ret;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  if (file->secthash[ELFSH_SECTION_DW2_STR] == NULL)
    {
      sect = elfsh_get_section_by_name(file, ELFSH_SECTION_NAME_DW2_STR, 0, 
				       &strindex, &nbr);

      if (sect == NULL)
        PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
			  "Unable to find Dwarf str table", NULL);;

      if (sect->data == NULL)
	{
	  sect->data = elfsh_load_section(file, sect->shdr);
	  if (sect->data == NULL)
	    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
			      "Unable to find data for Dwarf str table", 
			      NULL); 
	}

      file->secthash[ELFSH_SECTION_DW2_STR] = sect;
    }

  /* Search number of entry do we get here */
  nbr = file->secthash[ELFSH_SECTION_DW2_STR]->shdr->sh_size;

  if (num)
    *num = nbr;

  ret = elfsh_readmem(file->secthash[ELFSH_SECTION_DW2_STR]);

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, ret);
}

/**
 * Load dwarf loc table
 * @param file file objet
 * @param num object number pointer
 * @return data pointer
 */
void		*elfsh_get_dwarf_loc(elfshobj_t *file, int *num)
{
  elfshsect_t		*sect;
  int			strindex, nbr;
  void			*ret;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  if (file->secthash[ELFSH_SECTION_DW2_LOC] == NULL)
    {
      sect = elfsh_get_section_by_name(file, ELFSH_SECTION_NAME_DW2_LOC, 0, 
				       &strindex, &nbr);

      if (sect == NULL)
        PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
			  "Unable to find Dwarf loc table", NULL);;

      if (sect->data == NULL)
	{
	  sect->data = elfsh_load_section(file, sect->shdr);
	  if (sect->data == NULL)
	    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
			      "Unable to find data for Dwarf loc table", 
			      NULL); 
	}

      file->secthash[ELFSH_SECTION_DW2_LOC] = sect;
    }

  /* Search number of entry do we get here */
  nbr = file->secthash[ELFSH_SECTION_DW2_LOC]->shdr->sh_size;

  if (num)
    *num = nbr;

  ret = elfsh_readmem(file->secthash[ELFSH_SECTION_DW2_LOC]);

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, ret);
}
