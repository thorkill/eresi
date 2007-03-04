/*
** dwarf2.c for libedfmt (The Elf debug format library of ELFsh)
**
** Started Dec 26 2006 10:49:45 mxatone
**
*/

#include "libedfmt.h"

/* This file implements DWARF2 support for libedfmt.
specificitation @ http://dwarf.freestandards.org/Download.php */

/* Dwarf2 structures */
edfmtdw2info_t	   *dwarf2_info = NULL;

/* Current compil unit pointer */
edfmtdw2cu_t *current_cu = NULL;

/* Retrieve dwarf2 info pointer from a file pointer */
edfmtdw2info_t 		*edfmt_dwarf2_getinfo(elfshobj_t *file)
{
  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 
		     (edfmtdw2info_t *) file->debug_format.dwarf2);
}

/* Parse the DWARF-2 debug format */
int 			edfmt_dwarf2_parse(elfshobj_t *file)
{
  edfmtdw2sect_t 	*pointers[9];
  char			*names[9];
  u_int			hash[9];
  u_int			i;
  edfmtdw2sectlist_t	dw2_sections;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  if (edfmt_dwarf2_getinfo(file) != NULL)
    PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);

  /* Full fill with 0 */
  memset(&dw2_sections, 0x00, sizeof(edfmtdw2sectlist_t));

  /* We already have it ! */
  dw2_sections.info.sect = edfmt_get_sect(file, ELFSH_SECTION_DW2_INFO, 
					  ELFSH_SECTION_NAME_DW2_INFO, 0);

  dw2_sections.info.data = elfsh_get_raw(dw2_sections.info.sect);

  if (dw2_sections.info.sect == NULL || dw2_sections.info.data == NULL)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Main DWARF2 section unavailable", -1);

  XALLOC(__FILE__, __FUNCTION__, __LINE__, dwarf2_info, sizeof(edfmtdw2info_t), -1);
  memcpy(&(dwarf2_info->sections), &dw2_sections, sizeof(edfmtdw2sectlist_t));

  /* Local table system to full fill every pointers correctly */
  pointers[0] = &(dwarf2_info->sections.abbrev);	// Supported
  pointers[1] = &(dwarf2_info->sections.aranges);	// Unsupported
  pointers[2] = &(dwarf2_info->sections.frame);		// Supported
  pointers[3] = &(dwarf2_info->sections.line);		// Supported
  pointers[4] = &(dwarf2_info->sections.macinfo);	// Supported
  pointers[5] = &(dwarf2_info->sections.pubnames);	// Unsupported
  pointers[6] = &(dwarf2_info->sections.str);		// Supported
  pointers[7] = &(dwarf2_info->sections.loc);		// Supported
  pointers[8] = NULL;

  names[0] = ELFSH_SECTION_NAME_DW2_ABBREV;
  names[1] = ELFSH_SECTION_NAME_DW2_ARANGES;
  names[2] = ELFSH_SECTION_NAME_DW2_FRAME;
  names[3] = ELFSH_SECTION_NAME_DW2_LINE;
  names[4] = ELFSH_SECTION_NAME_DW2_MACINFO;
  names[5] = ELFSH_SECTION_NAME_DW2_PUBNAMES;
  names[6] = ELFSH_SECTION_NAME_DW2_STR;
  names[7] = ELFSH_SECTION_NAME_DW2_LOC;
  names[8] = NULL;

  hash[0] = ELFSH_SECTION_DW2_ABBREV;
  hash[1] = ELFSH_SECTION_DW2_ARANGES;
  hash[2] = ELFSH_SECTION_DW2_FRAME;
  hash[3] = ELFSH_SECTION_DW2_LINE;
  hash[4] = ELFSH_SECTION_DW2_MACINFO;
  hash[5] = ELFSH_SECTION_DW2_PUBNAMES;
  hash[6] = ELFSH_SECTION_DW2_STR;
  hash[7] = ELFSH_SECTION_DW2_LOC;
  hash[8] = NULL;

  /* Use the table to fill every pointers and report a problem */
  for (i = 0; names[i] != NULL; i++)
    {
      pointers[i]->sect = edfmt_get_sect(file, hash[i], names[i], 0);

      if (pointers[i]->sect != NULL)
	pointers[i]->data = elfsh_get_raw(pointers[i]->sect);
    }

  /* Start into the block entrie */
  edfmt_dwarf2_block_entrie(file);

  /* Parse cfa informations */
  /* TODO: reimplement it when it will be usefull
    edfmt_dwarf2_cfa();
  */

  /* Update file pointer */
  file->debug_format.dwarf2 = (void *) dwarf2_info;
  dwarf2_info = NULL;
  current_cu = NULL;

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}

/* Central parse function that find every compilation unit */
int			edfmt_dwarf2_block_entrie(elfshobj_t *file)
{
  edfmtdw2cu_t		*tcu;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  while (dwarf2_pos(info) < dwarf2_size(info))
    {
      /* Alloc a new compil unit */
      XALLOC(__FILE__, __FUNCTION__, __LINE__,tcu, sizeof(edfmtdw2cu_t), -1);

      tcu->fileobj = file;
      
      /* Update current pointer */
      if (current_cu != NULL)
	{
	  current_cu->next = tcu;
	}
      else
	{
	  /* Init info structure */
	  dwarf2_info->cu_list = tcu;
	}

      current_cu = current_cu ? current_cu->next : tcu;

      /* Set compil unit start position */
      current_cu->start_pos = dwarf2_pos(info);

      /* Read DWARF2 .dwarf2_info header */
      dwarf2_ipos(current_cu->length, info, u_int);
      current_cu->end_pos = dwarf2_pos(info) + current_cu->length;
      dwarf2_ipos(current_cu->version, info, u_short);
      dwarf2_ipos(current_cu->offset, info, u_int);
      dwarf2_ipos(current_cu->addr_size, info, u_char);

      current_cu->info_pos = dwarf2_pos(info);
      
      /* Update abbrev position */
      dwarf2_pos(abbrev) = current_cu->offset;
     
      /* Loop on the current compil unit */
      edfmt_dwarf2_block_loop(current_cu->end_pos);

      /* We update info position, to finish at the right offset */
      dwarf2_pos(info) = current_cu->end_pos;
    }
      
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}

/* Retrieve en abbrev entity rom a compil unit */
int 			edfmt_dwarf2_getent(edfmtdw2cu_t *cu, edfmtdw2abbent_t *abbent, 
					    u_int pos)
{
  edfmtdw2info_t	*pinfo;
  edfmtdw2cu_t		*pcu;
  int			ret = 0;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);   

  if (cu == NULL || abbent == NULL || cu->fileobj == NULL)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Invalid parameters", -1);

  /* Save current pointers */
  pinfo = dwarf2_info;
  dwarf2_info = (edfmtdw2info_t *) cu->fileobj->debug_format.dwarf2;
  pcu = current_cu;
  current_cu = cu;

  memset(abbent, 0x00, sizeof(edfmtdw2abbent_t));

  /* Parse info part */
  if (edfmt_dwarf2_form(abbent, pos) < 0)
    ret = -1;

  /* Restore ex pointers */
  dwarf2_info = pinfo;
  current_cu = pcu;
  
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, ret);
}

/* Manage parsing loops */
int			edfmt_dwarf2_block_loop(u_int endpos)
{
  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);   

  if (current_cu->abbrev_table.ent == NULL)
    hash_init(&(current_cu->abbrev_table), NULL, 30, ASPECT_TYPE_UNKNOW);
  
  /* Parse abbrev table on the appropriate offset */
  edfmt_dwarf2_abbrev_enum(&(current_cu->abbrev_table));
  
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}

/* Clean allocated buffer for this file */
int 			edfmt_dwarf2_clean(elfshobj_t *file)
{
  edfmtdw2cu_t		*cu, *cunext = NULL;
  edfmtdw2info_t	*dw2_info;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__); 

  if (file == NULL)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Invalid parmaters", -1);

  dw2_info = edfmt_dwarf2_getinfo(file);

  if (dw2_info == NULL)
    PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);

  /* Each compile unit */
  for (cu = dw2_info->cu_list; cu != NULL; cunext = cu, cu = cu->next)
    {
      XFREE(__FILE__, __FUNCTION__, __LINE__, cunext);

      /* Clean the abbrev table */
      hash_destroy(&(cu->abbrev_table));

      /* Filenames and directories */
      if (cu->dirs)
	XFREE(__FILE__, __FUNCTION__, __LINE__, cu->dirs);

      if (cu->files_name)
	XFREE(__FILE__, __FUNCTION__, __LINE__, cu->files_name);
      
      if (cu->files_dindex)
	XFREE(__FILE__, __FUNCTION__, __LINE__, cu->files_dindex);
     
      if (cu->files_time)
	XFREE(__FILE__, __FUNCTION__, __LINE__, cu->files_time);

      if (cu->files_len)
	XFREE(__FILE__, __FUNCTION__, __LINE__, cu->files_len);
    }

  if (cunext)
    XFREE(__FILE__, __FUNCTION__, __LINE__, cunext);

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}
