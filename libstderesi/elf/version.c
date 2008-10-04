/**
 * @file version.c
 * 
 * Version feature
 * 
 * Started on  Sun Sep 25 15:20:18 2005 mxatone
 *
 * $Id: version.c,v 1.2 2008-02-16 12:32:27 thor Exp $
 *
 */
#include "libstderesi.h"

/**
 * Need doxygen comment.
 * @param pneed
 * @param pdef
 * @param auxid
 * @param index
 * @param id
 * @param name
 * @param type
 * @param regx
 * @return
 */

int			revm_version_punk(hashneed_t *pneed, hashdef_t *pdef,
					unsigned int auxid, unsigned int index, char *id, 
					char *name, char *type, regex_t	*regx)
{
  char			*svtype;
  unsigned int			vtype;
  char			logbuf[BUFSIZ];

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  vtype = (auxid <= STB_GLOBAL ? ELFSH_VERTYPE_NONE : ELFSH_VERTYPE_UNK);
  svtype = (char*) elfsh_verentry_type[vtype].desc;

  if (!world.state.revm_quiet)
    {
      snprintf(logbuf, BUFSIZ - 1, 
	       " %s %s %s %s %s%s %s\n", 
	       revm_colornumber("[%03u]", index),
	       revm_colortypestr_fmt("%-8s", type), 
	       revm_colorstr_fmt("%-40s", name),
	       revm_colortypestr_fmt("%-7s", svtype),
	       revm_colorfieldstr("index:"),
	       id,
	       (pneed != NULL && pdef != NULL ? 
		revm_colorwarn("Conflict between need and def") : ""));
    }
  else
    {
      snprintf(logbuf, BUFSIZ - 1, 
	       " %s %s %s %s %s%s %s\n", 
	       revm_colornumber("[%03u]", index),
	       revm_colortypestr_fmt("%-8s", type), 
	       revm_colorstr_fmt("%-24s", name),
	       revm_colortypestr_fmt("%-7s", svtype),
	       revm_colorfieldstr("id:"),
	       id,
	       (pneed != NULL && pdef != NULL ?
	       revm_colorwarn("Conflict") : ""));
    }

     if (regx == NULL || 
	  (regx != NULL && regexec(regx, logbuf, 0, 0, 0) == 0))
       revm_output(logbuf);

  revm_endline();

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}


/**
 * Need doxygen comment.
 * @param pdef
 * @param auxid
 * @param index
 * @param id
 * @param name
 * @param type
 * @param regx
 */
int			revm_version_pdef(hashdef_t *pdef, unsigned int auxid,
					unsigned int index, char *id, char *name, 
					char *type, regex_t *regx)
{
  char			*dauxnames[3];
  unsigned int			vtype;
  char			*svtype;
  char			logbuf[BUFSIZ];

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  dauxnames[0] = dauxnames[1] = dauxnames[2] = NULL;

  elfsh_get_verdauxnamelist(world.curjob->curfile, 
			    pdef, dauxnames, 2);

  vtype = (auxid <= STB_GLOBAL ? ELFSH_VERTYPE_NONE : ELFSH_VERTYPE_DEF);
  svtype = (char*) elfsh_verentry_type[vtype].desc;

  if (!world.state.revm_quiet)
    {
      snprintf(logbuf, BUFSIZ - 1, 
	       " %s %s %s %s %s%s %s%s %s%s %s %s\n", 
	       revm_colornumber("[%03u]", index),
	       revm_colortypestr_fmt("%-8s", type), 
	       revm_colorstr_fmt("%-40s", name),
	       revm_colortypestr_fmt("%-7s", svtype),
	       revm_colorfieldstr("index:"),
	       id,
	       revm_colorfieldstr("nume:"),
	       revm_colornumber("%02u", pdef->def->vd_cnt),
	       revm_colorfieldstr("hash:"),
	       revm_colornumber("%08x", pdef->def->vd_hash),
	       (dauxnames[0] != NULL ? 
		revm_colorstr_fmt("%-14s", dauxnames[0]) : ""),
	       (dauxnames[1] != NULL ? 
		revm_colorstr(dauxnames[1]) : ""));
    }
  else
    {
      snprintf(logbuf, BUFSIZ - 1, 
	       " %s %s %s %s %s%s %s%s %s\n", 
	       revm_colornumber("[%03u]", index),
	       revm_colortypestr_fmt("%-8s", type), 
	       revm_colorstr_fmt("%-24s", name),
	       revm_colortypestr_fmt("%-7s", svtype),
	       revm_colorfieldstr("id:"),
	       id,
	       revm_colorfieldstr("nume:"),
	       revm_colornumber("%02u", pdef->def->vd_cnt),
	       (dauxnames[0] != NULL ? 
		revm_colorstr_fmt("%-10s", dauxnames[0]) : ""));
    }
     if (regx == NULL || 
	  (regx != NULL && regexec(regx, logbuf, 0, 0, 0) == 0))
       revm_output(logbuf);

  revm_endline();

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}

/**
 * Need doxygen comment
 * @param pneed
 * @param auxid
 * @param index
 * @param id
 * @param name
 * @param type
 * @param regx
 * @return
 */
int			revm_version_pneed(hashneed_t *pneed, unsigned int auxid, 
					 unsigned int index, char *id, char *name, 
					 char *type, regex_t *regx)
{
  char			*file;
  char 			*auxname;
  char			*svtype;
  char			logbuf[BUFSIZ];

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);  

  file = elfsh_get_verneedfile(world.curjob->curfile, 
			       pneed->need);
  auxname = elfsh_get_vernauxname(world.curjob->curfile, 
				  pneed->aux);
  svtype = (char*) elfsh_verentry_type[ELFSH_VERTYPE_NEED].desc;

  if (!world.state.revm_quiet)
    {
      snprintf(logbuf, BUFSIZ - 1, 
	       " %s %s %s %s %s%s %s%s %s%s %s %s\n", 
	       revm_colornumber("[%03u]", index),
	       revm_colortypestr_fmt("%-8s", type), 
	       revm_colorstr_fmt("%-40s", name),
	       revm_colortypestr_fmt("%-7s", svtype),
	       revm_colorfieldstr("index:"),
	       id,
	       revm_colorfieldstr("flag:"),
	       revm_colornumber("%02u", pneed->aux->vna_flags),
	       revm_colorfieldstr("hash:"),
	       revm_colornumber("%08x", pneed->aux->vna_hash),
	       revm_colorstr_fmt("%-14s", auxname),
	       revm_colorstr_fmt("%s", file));
    }
  else
    {
      snprintf(logbuf, BUFSIZ - 1, 
	       " %s %s %s %s %s%s %s %s\n", 
	       revm_colornumber("[%03u]", index),
	       revm_colortypestr_fmt("%-8s", type), 
	       revm_colorstr_fmt("%-24s", name),
	       revm_colortypestr_fmt("%-7s", svtype),
	       revm_colorfieldstr("id:"),
	       id,
	       revm_colorstr_fmt("%-10s", auxname),
	       revm_colorstr_fmt("%s", file));
    }
     if (regx == NULL || 
	  (regx != NULL && regexec(regx, logbuf, 0, 0, 0) == 0))
       revm_output(logbuf);

  revm_endline();

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}

/**
 * Need doxygen comments
 * @param sym_table
 * @param dsym_table
 * @param symsize
 * @param t_need
 * @param t_def
 * @param tmp
 * @return
 */
int			revm_version_print(elfsh_Half *sym_table, elfsh_Sym *dsym_table, 
					 unsigned int symsize, hash_t *t_need, hash_t *t_def,
					 regex_t *tmp)
{
  elfshsect_t		*dsym_sect;
  unsigned int			index;
  char			s_temp[9];
  unsigned int			typenum;
  char	       		*name;
  char		      	*type;
  unsigned int			auxid;
  hashneed_t		*pneed;
  hashdef_t		*pdef;
  char			*id;
  char			type_unk[ERESI_MEANING + 1];

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  for (index = 0; index < symsize; index++)
    {
      /* Load tables */
      auxid = (unsigned int) *(sym_table + index);
      snprintf(s_temp, 8, "%u", auxid);
      pneed = hash_get(t_need, s_temp);
      pdef = hash_get(t_def, s_temp);

      /* dynsym informations */
      typenum = elfsh_get_symbol_type(dsym_table + index);
      type = (char *) (typenum >= ELFSH_SYMTYPE_MAX ? 
		       revm_build_unknown(type_unk, "type", typenum) : 
		       elfsh_sym_type[typenum].desc);
      name = elfsh_get_dynsymbol_name(world.curjob->curfile, dsym_table + index);

      dsym_sect = elfsh_get_parent_section(world.curjob->curfile, 
				      dsym_table[index].st_value, 
				      NULL);
      if (dsym_sect == NULL && dsym_table[index].st_shndx)
	dsym_sect = elfsh_get_section_by_index(world.curjob->curfile, 
					  dsym_table[index].st_shndx,
					  NULL, NULL);

      /* Set id numer/type */
      id = (auxid <= STB_GLOBAL ? 
	    revm_colortypestr((char*) elfsh_sym_bind[auxid].desc) :
	    revm_colornumber("%02u", auxid));

      /* Fixup names */
      if (name == NULL || *name == 0)
	name = ELFSH_NULL_STRING;
      if (type == NULL || *type == 0)
	type = ELFSH_NULL_STRING;


      if (pdef != NULL && pneed == NULL)
	revm_version_pdef(pdef, auxid, index, id, name, type, tmp);	
      else if (pneed != NULL && pdef == NULL)
	revm_version_pneed(pneed, auxid, index, id, name, type, tmp);
      else
	revm_version_punk(pneed, pdef, auxid, index, id, name, type, tmp);
       
    }

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, index > 0 ? 0 : -1);
}

/**
 * Need doxygen comment
 */
int 			cmd_version()
{
  elfshsect_t		*sect;
  void			*need, *dynsym, *def;
  int			symsize, needsize, dsymsize, defsize;
  int			index, knum;
  char			logbuf[BUFSIZ];
  hash_t		t_need, t_def;
  char			**keys;
  regex_t		*tmp; 
  unsigned int			range = 0;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  /* Load dynsym table */
  dynsym = elfsh_get_dynsymtab(world.curjob->curfile, &dsymsize);
  if (dynsym == NULL)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Unable to load dynsym section", -1);

  hash_init(&t_need, "versions_need", dsymsize, ASPECT_TYPE_UNKNOW);
  hash_init(&t_def , "versions_defs", dsymsize, ASPECT_TYPE_UNKNOW);

  /* Load need table */
  need = elfsh_get_verneedtab(world.curjob->curfile, &needsize);
  if (need == NULL)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Unable to load version need section", -1);

  if (elfsh_load_needtable(&t_need, need, needsize*sizeof(elfsh_Verneed)) != 0)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Unable to load version need section", -1);

  /* Load def table (we wanna know if a sym is for def or need) */
  def = elfsh_get_verdeftab(world.curjob->curfile, &defsize);
  if (def != NULL)
    {
      if (elfsh_load_deftable(&t_def, def, defsize*sizeof(elfsh_Verdef)) != 0)
	PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
			  "Unable to load version def section", -1);
    }

  /* Load sym table */
  sect = elfsh_get_versymtab_by_range(world.curjob->curfile, 0, &symsize);

  if (sect == NULL)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Unable to find Version Symbol table", -1);

  snprintf(logbuf, BUFSIZ - 1,
	   " [VERSION SYMBOL TABLE]\n [Object %s]\n\n", 
	   world.curjob->curfile->name);
  revm_output(logbuf);

  FIRSTREGX(tmp);

  do {
    snprintf(logbuf, BUFSIZ - 1, " {Section %s}\n", 
	     elfsh_get_section_name(world.curjob->curfile, sect));
    revm_output(logbuf);    

    if (revm_version_print(sect->data, dynsym, symsize, &t_need, &t_def, tmp) < 0)
      revm_output("No entry found\n");

    revm_output("\n");

    sect = elfsh_get_versymtab_by_range(world.curjob->curfile, ++range, &symsize);
  } while (sect != NULL);

  /* free need table */
  keys = hash_get_keys(&t_need, &knum);

  for (index = 0; keys[index] != NULL; index++)
    hash_del(&t_need, keys[index]);
  hash_destroy(&t_need);

  if (keys != NULL)
    XFREE(__FILE__, __FUNCTION__, __LINE__,keys);

  /* free def table */
  keys = hash_get_keys(&t_def, &knum);
  for (index = 0; keys[index] != NULL; index++)
    hash_del(&t_def, keys[index]);
  hash_destroy(&t_def);

  if (keys != NULL)
    XFREE(__FILE__, __FUNCTION__, __LINE__,keys);

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}

/**
 * Need doxygen comment
 */
int 			cmd_verneed()
{
  elfshsect_t		*sect;
  void			*data;
  elfsh_Verneed		*table;
  elfsh_Vernaux		*tableaux;
  unsigned int			offset;
  unsigned int			auxset;
  int			size;
  unsigned int			index;
  unsigned int			aux;
  int			strindex;
  int			nbr;
  char			logbuf[BUFSIZ];
  char			*file;
  char			*auxname;
  regex_t		*regx;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  /* Load need table */
  data = elfsh_get_verneedtab(world.curjob->curfile, &size);
  if (data == NULL)
  	PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
			  "Unable to load Version Need table", -1);

  sect = elfsh_get_section_by_type(world.curjob->curfile, SHT_GNU_verneed, 0, 
				   NULL, &strindex, &nbr);

  if (sect == NULL)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Unable to find Version Need section", 0);

  snprintf(logbuf, BUFSIZ - 1,
	   " [VERSION NEED TABLE]\n [Object %s]\n"
	   " [Section %s]\n\n", 
	   world.curjob->curfile->name, sect->name);
  
  revm_output(logbuf);

  FIRSTREGX(regx);

  for (index = 0, offset = 0; offset < sect->shdr->sh_size; index++)
    {
      table = data + offset;

      file = elfsh_get_verneedfile(world.curjob->curfile, 
				   table);

      snprintf(logbuf, BUFSIZ - 1, 
	       " %s %s %s%s %s%s %s%s\n", 
	       revm_colornumber("[%02u]", index),
	       revm_colorstr_fmt("%-11s", file),
	       revm_colorfieldstr("cnt:"),
	       revm_colornumber("%02u", table->vn_cnt),
	       revm_colorfieldstr("aux:"),
	       revm_colornumber("%02x", table->vn_aux),
	       revm_colorfieldstr("next:"),
	       revm_colornumber("%02x", table->vn_next));

     if (regx == NULL || 
	  (regx != NULL && regexec(regx, logbuf, 0, 0, 0) == 0))
       {
	 if (index > 0)
	         revm_output("\n");

	 revm_output(logbuf);
	 revm_endline();
       }

      auxset = offset + table->vn_aux;
      for (aux = 0; aux < table->vn_cnt; aux++) 
	{
	  tableaux = data + auxset;

	  auxname = elfsh_get_vernauxname(world.curjob->curfile, 
					  tableaux);

	  snprintf(logbuf, BUFSIZ - 1, 
		   " \t %s%s %s%s %s%s %s%s %s%s\n", 
		   revm_colorfieldstr("index:"),
		   revm_colornumber("%02u", tableaux->vna_other),
		    revm_colorfieldstr("name:"),
		   revm_colorstr_fmt("%-20s", auxname),
		    revm_colorfieldstr("hash:"),
		   revm_colornumber("%08x", tableaux->vna_hash),
		    revm_colorfieldstr("flags:"),
		   revm_colornumber("%02u", tableaux->vna_flags),
		    revm_colorfieldstr("next:"),
		   revm_colornumber("%02x", tableaux->vna_next));

	  if (regx == NULL || 
	      (regx != NULL && regexec(regx, logbuf, 0, 0, 0) == 0))
		 revm_output(logbuf);

	  revm_endline();	  

	  if (tableaux->vna_next == 0)
	    break;

	  auxset += tableaux->vna_next;
	}

      if (table->vn_next == 0)
	break;

      offset += table->vn_next;
    }

  revm_output("\n");

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}

/**
 * Need doxygen comment
 */
int 			cmd_verdef()
{
  elfshsect_t		*sect;
  void			*data;
  elfsh_Verdef		*table;
  elfsh_Verdaux		*tableaux;
  int			index, offset, num, aux, auxset;
  int			strindex;
  int			nbr;
  char			*auxname;
  char			logbuf[BUFSIZ];
  regex_t		*regx;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  data = elfsh_get_verdeftab(world.curjob->curfile, &num);
  if (data == NULL)
	PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
			  "Unable to load Version Def table", -1);

  sect = elfsh_get_section_by_type(world.curjob->curfile, SHT_GNU_verdef, 0, 
				   NULL, &strindex, &nbr);

  if (sect == NULL)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Unable to find Definition Version section", 0);

  snprintf(logbuf, BUFSIZ - 1,
	   " [VERSION DEFINITION TABLE]\n [Object %s]\n"
	   " [Section %s]\n\n", 
	   world.curjob->curfile->name, sect->name);
  
  revm_output(logbuf);

  FIRSTREGX(regx);
  
  for (index = 0, offset = 0; offset < sect->shdr->sh_size; index++)
    {
      table = data + offset;
      snprintf(logbuf, BUFSIZ - 1, 
	       " %s %s%s %s%s %s%s %s%s %s%s %s%s\n", 
	       revm_colornumber("[%02u]", index),
	       revm_colorfieldstr("index:"),
	       revm_colornumber("%02u", table->vd_ndx),	       
	       revm_colorfieldstr("cnt:"),
	       revm_colornumber("%02u", table->vd_cnt),
	       revm_colorfieldstr("hash:"),
	       revm_colornumber("%08x", table->vd_hash),
	       revm_colorfieldstr("flags:"),
	       revm_colornumber("%02u", table->vd_flags),
	       revm_colorfieldstr("aux:"),
	       revm_colornumber("%02x", table->vd_aux),
	       revm_colorfieldstr("next:"),
	       revm_colornumber("%02x", table->vd_next));

      if (regx == NULL || 
	  (regx != NULL && regexec(regx, logbuf, 0, 0, 0) == 0))
	{
	  if (index > 0)
	    revm_output("\n");

	  revm_output(logbuf);
	  revm_endline();
	}

      auxset = offset + table->vd_aux;
      for (aux = 0; aux < table->vd_cnt; aux++) 
	{
	  tableaux = data + auxset;

	  auxname = elfsh_get_verdauxname(world.curjob->curfile, tableaux);

	  snprintf(logbuf, BUFSIZ - 1, 
		   " \t %s%s %s%s\n", 
		   revm_colorfieldstr("name:"),
		   revm_colorstr_fmt("%-15s", auxname),
		   revm_colorfieldstr("next:"),
		   revm_colornumber("%02x", tableaux->vda_next));

	  if (regx == NULL || 
	      (regx != NULL && regexec(regx, logbuf, 0, 0, 0) == 0))
	    revm_output(logbuf);

	  revm_endline();

	  if (tableaux->vda_next == 0)
	    break;

	  auxset += tableaux->vda_next;
	}

      if (table->vd_next == 0)
	break;

      offset += table->vd_next;
    }

 revm_output("\n");

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}
