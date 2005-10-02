/*
** version.c for elfsh
** 
** Version feature
** 
** Started on  Sun Sep 25 15:20:18 2005 mxatone
*/
#include "elfsh.h"

int			vm_version_punk(hashneed_t *pneed, hashdef_t *pdef,
					u_int auxid, u_int index, char *id, 
					char *name, char *type)
{
  char			*svtype;
  u_int			vtype;
  char			logbuf[BUFSIZ];

  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);

  vtype = (auxid <= STB_GLOBAL ? ELFSH_VERTYPE_NONE : ELFSH_VERTYPE_UNK);
  svtype = (char*) elfsh_verentry_type[vtype].desc;

  if (!world.state.vm_quiet)
    {
      snprintf(logbuf, BUFSIZ - 1, 
	       " %s %s %s %s %s%s %s\n", 
	       vm_colornumber("[%03u]", index),
	       vm_colortypestr_fmt("%-8s", type), 
	       vm_colorstr_fmt("%-40s", name),
	       vm_colortypestr_fmt("%-7s", svtype),
	       vm_colorfieldstr("index:"),
	       id,
	       (pneed != NULL && pdef != NULL ? 
		vm_colorwarn("Conflict between need and def") : ""));
    }
  else
    {
      snprintf(logbuf, BUFSIZ - 1, 
	       " %s %s %s %s %s%s %s\n", 
	       vm_colornumber("[%03u]", index),
	       vm_colortypestr_fmt("%-8s", type), 
	       vm_colorstr_fmt("%-24s", name),
	       vm_colortypestr_fmt("%-7s", svtype),
	       vm_colorfieldstr("id:"),
	       id,
	       (pneed != NULL && pdef != NULL ?
	       vm_colorwarn("Conflict") : ""));
    }

  vm_output(logbuf);
  vm_endline();

  ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}

int			vm_version_pdef(hashdef_t *pdef, u_int auxid,
					u_int index, char *id, char *name, 
					char *type)
{
  char			*dauxnames[3];
  u_int			vtype;
  char			*svtype;
  char			logbuf[BUFSIZ];

  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);

  elfsh_get_verdauxnamelist(world.curjob->current, 
			    pdef, dauxnames, 2);

  vtype = (auxid <= STB_GLOBAL ? ELFSH_VERTYPE_NONE : ELFSH_VERTYPE_DEF);
  svtype = (char*) elfsh_verentry_type[vtype].desc;

  if (!world.state.vm_quiet)
    {
      snprintf(logbuf, BUFSIZ - 1, 
	       " %s %s %s %s %s%s %s%s %s%s %s %s\n", 
	       vm_colornumber("[%03u]", index),
	       vm_colortypestr_fmt("%-8s", type), 
	       vm_colorstr_fmt("%-40s", name),
	       vm_colortypestr_fmt("%-7s", svtype),
	       vm_colorfieldstr("index:"),
	       id,
	       vm_colorfieldstr("nume:"),
	       vm_colornumber("%02u", pdef->def->vd_cnt),
	       vm_colorfieldstr("hash:"),
	       vm_colornumber("%08x", pdef->def->vd_hash),
	       (dauxnames[0] != NULL ? 
		vm_colorstr_fmt("%-14s", dauxnames[0]) : ""),
	       (dauxnames[1] != NULL ? 
		vm_colorstr(dauxnames[1]) : ""));
    }
  else
    {
      snprintf(logbuf, BUFSIZ - 1, 
	       " %s %s %s %s %s%s %s%s %s\n", 
	       vm_colornumber("[%03u]", index),
	       vm_colortypestr_fmt("%-8s", type), 
	       vm_colorstr_fmt("%-24s", name),
	       vm_colortypestr_fmt("%-7s", svtype),
	       vm_colorfieldstr("id:"),
	       id,
	       vm_colorfieldstr("nume:"),
	       vm_colornumber("%02u", pdef->def->vd_cnt),
	       (dauxnames[0] != NULL ? 
		vm_colorstr_fmt("%-10s", dauxnames[0]) : ""));
    }

  vm_output(logbuf);
  vm_endline();

  ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}

int			vm_version_pneed(hashneed_t *pneed, u_int auxid, 
					 u_int index, char *id, char *name, 
					 char *type)
{
  char			*file;
  char 			*auxname;
  char			*svtype;
  char			logbuf[BUFSIZ];

  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);  

  file = elfsh_get_verneedfile(world.curjob->current, 
			       pneed->need);
  auxname = elfsh_get_vernauxname(world.curjob->current, 
				  pneed->aux);
  svtype = (char*) elfsh_verentry_type[ELFSH_VERTYPE_NEED].desc;

  if (!world.state.vm_quiet)
    {
      snprintf(logbuf, BUFSIZ - 1, 
	       " %s %s %s %s %s%s %s%s %s%s %s %s\n", 
	       vm_colornumber("[%03u]", index),
	       vm_colortypestr_fmt("%-8s", type), 
	       vm_colorstr_fmt("%-40s", name),
	       vm_colortypestr_fmt("%-7s", svtype),
	       vm_colorfieldstr("index:"),
	       id,
	       vm_colorfieldstr("flag:"),
	       vm_colornumber("%02u", pneed->aux->vna_flags),
	       vm_colorfieldstr("hash:"),
	       vm_colornumber("%08x", pneed->aux->vna_hash),
	       vm_colorstr_fmt("%-14s", auxname),
	       vm_colorstr_fmt("%s", file));
    }
  else
    {
      snprintf(logbuf, BUFSIZ - 1, 
	       " %s %s %s %s %s%s %s %s\n", 
	       vm_colornumber("[%03u]", index),
	       vm_colortypestr_fmt("%-8s", type), 
	       vm_colorstr_fmt("%-24s", name),
	       vm_colortypestr_fmt("%-7s", svtype),
	       vm_colorfieldstr("id:"),
	       id,
	       vm_colorstr_fmt("%-10s", auxname),
	       vm_colorstr_fmt("%s", file));
    }

  vm_output(logbuf);
  vm_endline();

  ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}

int 			cmd_version()
{
  elfshsect_t		*sym_sect, *need_sect, *dsym_sect, *def_sect;
  elfsh_Sym		*dynsym;
  elfsh_Half		*h_table;
  int			index, num, knum;
  u_int			snum, auxid;
  char			logbuf[BUFSIZ];
  elfsh_Sym		*table;
  char	       		*name;
  char		      	*type;
  char			*id;
  u_int			typenum;
  char			type_unk[ELFSH_MEANING + 1];
  hash_t		t_need, t_def;
  hashneed_t		*pneed;
  hashdef_t		*pdef;
  char			s_temp[9];
  char			**keys;

  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);

  /* Load dynsym table (like in cmd_dynsym()) */
  dynsym = elfsh_get_dynsymtab(world.curjob->current, &snum);
  if (dynsym == NULL)
    ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Unable to load dynsym section", -1);

  dsym_sect = elfsh_get_section_by_name(world.curjob->current, 
					ELFSH_SECTION_NAME_ALTDYNSYM, 
					NULL, NULL, &snum);
  if (!dsym_sect)
    dsym_sect = elfsh_get_section_by_type(world.curjob->current, 
					  SHT_DYNSYM, NULL, 
					  NULL, NULL, &snum);
      
  if (!dsym_sect)
    ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Unable to load dynsym section", -1);

  snum = snum / sizeof(elfsh_Sym);
    
  table = elfsh_get_raw(dsym_sect);

  hash_init(&t_need, snum);
  hash_init(&t_def, snum);

  /* Load need table */
  need_sect = elfsh_get_verneedtab(world.curjob->current, &num);
  if (need_sect == NULL)
    ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Unable to load version need section", -1);

  if (elfsh_load_needtable(&t_need, need_sect) != 0)
    ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Unable to load version need section", -1);

  /* Load def table (we wanna know if a sym is for def or need) */
  def_sect = elfsh_get_verdeftab(world.curjob->current, &num);
  if (def_sect != NULL)
    {
      if (elfsh_load_deftable(&t_def, def_sect) != 0)
	ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, 
			  "Unable to load version def section", -1);
    }

  /* Load Version symbols & print with need/def table */
  sym_sect = elfsh_get_versymtab(world.curjob->current, &num);
  if (sym_sect == NULL)
	ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, 
			  "Unable to load version symbol section", -1);

  snprintf(logbuf, BUFSIZ - 1,
	   " [VERSION SYMBOL TABLE]\n [Object %s]\n [Section %s]\n\n", 
	   world.curjob->current->name, sym_sect->name);
  
  vm_output(logbuf);

  for (index = 0, h_table = sym_sect->data; index < num; index++)
    {
      /* Load tables */
      auxid = (u_int) *(h_table + index);
      snprintf(s_temp, 8, "%u", auxid);
      pneed = hash_get(&t_need, s_temp);
      pdef = hash_get(&t_def, s_temp);

      /* dynsym informations */
      typenum = elfsh_get_symbol_type(table + index);
      type = (char *) (typenum >= ELFSH_SYMTYPE_MAX ? 
		       vm_build_unknown(type_unk, "type", typenum) : 
		       elfsh_sym_type[typenum].desc);
      name = elfsh_get_dynsymbol_name(world.curjob->current, table + index);

      dsym_sect = elfsh_get_parent_section(world.curjob->current, 
				      table[index].st_value, 
				      NULL);
      if (dsym_sect == NULL && table[index].st_shndx)
	dsym_sect = elfsh_get_section_by_index(world.curjob->current, 
					  table[index].st_shndx,
					  NULL, NULL);

      /* Set id numer/type */
      id = (auxid <= STB_GLOBAL ? 
	    vm_colortypestr((char*) elfsh_sym_bind[auxid].desc) :
	    vm_colornumber("%02u", auxid));

      /* Fixup names */
      if (name == NULL || *name == 0)
	name = ELFSH_NULL_STRING;
      if (type == NULL || *type == 0)
	type = ELFSH_NULL_STRING;


      if (pdef != NULL && pneed == NULL)
	vm_version_pdef(pdef, auxid, index, id, name, type);	
      else if (pneed != NULL && pdef == NULL)
	vm_version_pneed(pneed, auxid, index, id, name, type);
      else
	vm_version_punk(pneed, pdef, auxid, index, id, name, type);
       
    }

  vm_output("\n");

  /* free need table */
  keys = hash_get_keys(&t_need, &knum);
  for (index = 0; keys[index] != NULL; index++)
    hash_del(&t_need, keys[index]);
  hash_destroy(&t_need);
  free(keys);

  /* free def table */
  keys = hash_get_keys(&t_def, &knum);
  for (index = 0; keys[index] != NULL; index++)
    hash_del(&t_def, keys[index]);
  hash_destroy(&t_def);
  free(keys);

  ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}

int 			cmd_verneed()
{
  elfshsect_t		*sect;
  elfsh_Verneed		*table;
  elfsh_Vernaux		*tableaux;
  u_int			offset;
  u_int			auxset;
  u_int			size;
  u_int			index;
  u_int			aux;
  u_int			num;
  void			*ps;
  char			logbuf[BUFSIZ];
  char			*file;
  char			*auxname;

  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);

  /* Load need table */
  sect = elfsh_get_verneedtab(world.curjob->current, &num);
  if (sect == NULL)
    RET(-1);

  snprintf(logbuf, BUFSIZ - 1,
	   " [VERSION NEED TABLE]\n [Object %s]\n"
	   " [Section %s]\n\n", 
	   world.curjob->current->name, sect->name);
  
  vm_output(logbuf);

  ps = sect->data;
  size = sect->shdr->sh_size;
  for (index = 0, offset = 0; offset < size; index++)
    {
      table = ps + offset;

      file = elfsh_get_verneedfile(world.curjob->current, 
				   table);

      snprintf(logbuf, BUFSIZ - 1, 
	       " %s %s %s%s %s%s %s%s\n", 
	       vm_colornumber("[%02u]", index),
	       vm_colorstr_fmt("%-11s", file),
	       vm_colorfieldstr("cnt:"),
	       vm_colornumber("%02u", table->vn_cnt),
	       vm_colorfieldstr("aux:"),
	       vm_colornumber("%02u", table->vn_aux),
	       vm_colorfieldstr("next:"),
	       vm_colornumber("%02u", table->vn_next));
      vm_output(logbuf);
      vm_endline();

      auxset = offset + table->vn_aux;
      for (aux = 0; aux < table->vn_cnt; aux++) 
	{
	  tableaux = ps + auxset;

	  auxname = elfsh_get_vernauxname(world.curjob->current, 
					  tableaux);

	  snprintf(logbuf, BUFSIZ - 1, 
		   " \t %s%s %s%s %s%s %s%s %s%s\n", 
		   vm_colorfieldstr("index:"),
		   vm_colornumber("%02u", tableaux->vna_other),
		    vm_colorfieldstr("name:"),
		   vm_colorstr_fmt("%-20s", auxname),
		    vm_colorfieldstr("hash:"),
		   vm_colornumber("%08x", tableaux->vna_hash),
		    vm_colorfieldstr("flags:"),
		   vm_colornumber("%02u", tableaux->vna_flags),
		    vm_colorfieldstr("next:"),
		   vm_colornumber("%02u", tableaux->vna_next));
	  vm_output(logbuf);
	  vm_endline();	  

	  auxset += tableaux->vna_next;
	}

      vm_output("\n");

      if (table->vn_next == NULL)
	break;

      offset += table->vn_next;
    }

  ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}

int 			cmd_verdef()
{
  elfshsect_t		*sect;
  elfsh_Verdef		*table;
  elfsh_Verdaux		*tableaux;
  int			index, num, offset, size, aux, auxset;
  void			*ps;
  char			*auxname;
  char			logbuf[BUFSIZ];

  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);

  sect = elfsh_get_verdeftab(world.curjob->current, &num);
  if (sect == NULL)
    RET(-1);

  snprintf(logbuf, BUFSIZ - 1,
	   " [VERSION DEFINITION TABLE]\n [Object %s]\n"
	   " [Section %s]\n\n", 
	   world.curjob->current->name, sect->name);
  
  vm_output(logbuf);

  ps = sect->data;
  size = sect->shdr->sh_size;
  for (index = 0, offset = 0; offset < size; index++)
    {
      table = ps + offset;
      snprintf(logbuf, BUFSIZ - 1, 
	       " %s %s%s %s%s %s%s %s%s %s%s %s%s\n", 
	       vm_colornumber("[%02u]", index),
	       vm_colorfieldstr("index:"),
	       vm_colornumber("%02u", table->vd_ndx),	       
	       vm_colorfieldstr("cnt:"),
	       vm_colornumber("%02u", table->vd_cnt),
	       vm_colorfieldstr("hash:"),
	       vm_colornumber("%08x", table->vd_hash),
	       vm_colorfieldstr("flags:"),
	       vm_colornumber("%02u", table->vd_flags),
	       vm_colorfieldstr("aux:"),
	       vm_colornumber("%02u", table->vd_aux),
	       vm_colorfieldstr("next:"),
	       vm_colornumber("%02u", table->vd_next));
      vm_output(logbuf);
      vm_endline();

      auxset = offset + table->vd_aux;
      for (aux = 0; aux < table->vd_cnt; aux++) 
	{
	  tableaux = ps + auxset;

	  auxname = elfsh_get_verdauxname(world.curjob->current, tableaux);

	  snprintf(logbuf, BUFSIZ - 1, 
		   " \t %s%s %s%s\n", 
		   vm_colorfieldstr("name:"),
		   vm_colorstr_fmt("%-15s", auxname),
		   vm_colorfieldstr("next:"),
		   vm_colornumber("%02u", tableaux->vda_next));
	  vm_output(logbuf);
	  vm_endline();

	  auxset += tableaux->vda_next;
	}
      vm_output("\n");
      
      if (table->vd_next == NULL)
	break;

      offset += table->vd_next;
    }

  ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}
