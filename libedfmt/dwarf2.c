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
edfmtdw2info_t debug_info;
edfmtdw2sectlist_t dw2_sections;

/* References */
edfmtdw2fref_t *ref_global = NULL;
edfmtdw2fref_t *ref_cu = NULL;
hash_t ref_global_table;
hash_t ref_cu_table;

/* Current compil unit pointer */
edfmtdw2cu_t *current_cu = NULL;

/* Update references global/local */
static int	edfmt_dwarf2_updatefref(u_char global)
{
  edfmtdw2fref_t *ref, *tmp, *prev, *base;
  hash_t	*ht;
  char		ckey[DW2_CKEY_SIZE];
  edfmtdw2abbent_t *ent;
  char		**keys;
  u_int		index;
  u_int		keynbr;

  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);

  ht = global ? &ref_global_table : &ref_cu_table;

  /* For each elements in the ref list */
  for (ref = (global ? ref_global : ref_cu); ref != NULL; ref = ref->next)
    {
      edfmt_dwarf2_ckey(ckey, DW2_CKEY_SIZE - 1, ref->offset);
      ent = (edfmtdw2abbent_t *) hash_get(ht, ckey);

      if (ent == NULL || ref->attr == NULL)
	continue;

      /* Set the correct reference */
      ref->attr->u.ref = ent;
    }

  /* Clean */
  base = global ? ref_global : ref_cu;

  if (base)
    {
      /* Only one element */
      if (base->next == NULL)
	{
	  XFREE(base);
	}
      else
	{
	  for (tmp = base, prev = NULL; tmp != NULL; prev = tmp, tmp = tmp->next)
	    {
	      if (prev != NULL)
		XFREE(prev);
	    }
	}

      if (global)
	ref_global = NULL;
      else
	ref_cu = NULL;
    }
 
  /* Clean hash tables */
  keys = hash_get_keys(ht, &keynbr);
  for (index = 0; index < keynbr; index++)
    XFREE(keys[index]);
  hash_free_keys((char *) keys);

  /* Destroy hash table */
  hash_destroy(ht);

  ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}

/* Parse the DWARF-2 debug format */
int 		edfmt_dwarf2_parse(elfshobj_t *file)
{
  edfmtdw2sect_t *pointers[7];
  char		*names[7];
  u_int		hash[7];
  u_int		i;

  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);

  /* Full fill with 0 */
  memset(&dw2_sections, 0x00, sizeof(edfmtdw2sectlist_t));

  /* Init global reference table (for DW_FORM_ref entities) */
  hash_init(&ref_global_table, 50, ELEM_TYPE_ANY);

  /* We already have it ! */
  dw2_sections.info.sect = edfmt_get_sect(file, ELFSH_SECTION_DW2_INFO, 
					     ELFSH_SECTION_NAME_DW2_INFO);

  if (dw2_sections.info.sect == NULL)
    ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Main DWARF2 section unavailable", -1);

  /* Local table system to full fill every pointers correctly */
  pointers[0] = &dw2_sections.abbrev;	// Supported
  pointers[1] = &dw2_sections.aranges;	// Unsupported
  pointers[2] = &dw2_sections.frame;	// Supported
  pointers[3] = &dw2_sections.line;	// Supported
  pointers[4] = &dw2_sections.macinfo;	// Supported
  pointers[5] = &dw2_sections.pubnames;	// Unsupported
  pointers[6] = &dw2_sections.str;	// Supported
  pointers[7] = NULL;

  names[0] = ELFSH_SECTION_NAME_DW2_ABBREV;
  names[1] = ELFSH_SECTION_NAME_DW2_ARANGES;
  names[2] = ELFSH_SECTION_NAME_DW2_FRAME;
  names[3] = ELFSH_SECTION_NAME_DW2_LINE;
  names[4] = ELFSH_SECTION_NAME_DW2_MACINFO;
  names[5] = ELFSH_SECTION_NAME_DW2_PUBNAMES;
  names[6] = ELFSH_SECTION_NAME_DW2_STR;
  names[7] = NULL;

  hash[0] = ELFSH_SECTION_DW2_ABBREV;
  hash[1] = ELFSH_SECTION_DW2_ARANGES;
  hash[2] = ELFSH_SECTION_DW2_FRAME;
  hash[3] = ELFSH_SECTION_DW2_LINE;
  hash[4] = ELFSH_SECTION_DW2_MACINFO;
  hash[5] = ELFSH_SECTION_DW2_PUBNAMES;
  hash[6] = ELFSH_SECTION_DW2_STR;
  hash[7] = NULL;

  /* Use the table to fill every pointers and report a problem */
  for (i = 0; names[i] != NULL; i++)
    pointers[i]->sect = edfmt_get_sect(file, hash[i], names[i]);

  /* Start into the block entrie */
  edfmt_dwarf2_block_entrie(file);

  /* Clean references */
  edfmt_dwarf2_updatefref(1);

  ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}

/* Central parse function that find every compilation unit */
int		edfmt_dwarf2_block_entrie(elfshobj_t *file)
{
  edfmtdw2cu_t	*tcu;
  u_int		last_pos;

  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);

  while (i_pos(info) < i_size(info))
    {
      /* Init local reference compil unit table */
      hash_init(&ref_cu_table, 30, ELEM_TYPE_ANY);

      /* Alloc a new compil unit */
      XALLOC(tcu, sizeof(edfmtdw2cu_t), -1);

      tcu->fileobj = file;
      
      /* Update current pointer */
      if (current_cu != NULL)
	{
	  current_cu->next = tcu;
	}
      else
	{
	  /* Init info structure */
	  debug_info.cu_list = tcu;
	  hash_init(&debug_info.file_line, 50, ELEM_TYPE_ANY);
	  hash_init(&debug_info.addr, 50, ELEM_TYPE_ANY);
	  hash_init(&debug_info.macros, 50, ELEM_TYPE_ANY);
	  hash_init(&debug_info.cfa, 50, ELEM_TYPE_ANY);
	}

      current_cu = current_cu ? current_cu->next : tcu;

      /* Init local hashes */
      hash_init(&current_cu->file_line, 50, ELEM_TYPE_ANY);
      hash_init(&current_cu->addr, 50, ELEM_TYPE_ANY);
      hash_init(&current_cu->macros, 50, ELEM_TYPE_ANY);
      hash_init(&current_cu->cfa, 50, ELEM_TYPE_ANY);

      /* Set compil unit start position */
      current_cu->start_pos = i_pos(info);

      /* Read DWARF2 .debug_info header */
      ci_pos(current_cu->length, info, u_int);
      last_pos = i_pos(info);
      ci_pos(current_cu->version, info, u_short);
      ci_pos(current_cu->offset, info, u_int);
      ci_pos(current_cu->addr_size, info, u_char);
      
      /* Update abbrev position */
      i_pos(abbrev) = current_cu->offset;
     
      /* Loop on the current compil unit */
      edfmt_dwarf2_block_loop(last_pos + current_cu->length);

      /* We didn't finish at the exact correct offset ? */
      if (last_pos + current_cu->length != dw2_sections.info.pos)
	dw2_sections.info.pos = last_pos + current_cu->length;

      edfmt_dwarf2_updatefref(0);
    }
      
  ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}

/* Manage parsing loops */
int		edfmt_dwarf2_block_loop(u_int endpos)
{
  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);   

  hash_init(&abbrev_table, 30, ELEM_TYPE_ANY);
  
  /* Parse abbrev table on the appropriate offset */
  if (edfmt_dwarf2_abbrev() == 0)
    { 
      /* Print dwarf only if we have .debug_abbrev pattern */
      edfmt_dwarf2_form(endpos);
    }

  /* Parse cfa informations */
  edfmt_dwarf2_cfa();

  hash_destroy(&abbrev_table);
  
  ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}
