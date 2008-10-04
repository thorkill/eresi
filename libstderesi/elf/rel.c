/**
 * @file rel.c
 *
 * Started on  Fri Nov  2 15:19:19 2001 jfv
 *
 *
 * $Id: rel.c,v 1.2 2008-02-16 12:32:27 thor Exp $
 *
 */
#include "libstderesi.h"


/**
 * @brief Choose relocation tables strings array giving the architecture 
 * @param file
 */
revmconst_t    *revm_getrelascii(elfshobj_t *file)
{

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  switch (elfsh_get_arch(file->hdr))
    {
    case EM_386:
      /* XXX: case EM_486: */
      PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 
			 (elfsh_rel_type_i386));
    case EM_SPARC:
    case EM_SPARC32PLUS:
    case EM_SPARCV9:
      PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 
			 (elfsh_rel_type_sparc));
    case EM_ALPHA:
#if EM_ALPHA != EM_ALPHA_EXP
    case EM_ALPHA_EXP:
#endif
      PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 
			 (elfsh_rel_type_alpha));
    case EM_IA_64:
      PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 
			 (elfsh_rel_type_ia64));
    case EM_MIPS:
    case EM_MIPS_RS3_LE:
      PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 
			 (elfsh_rel_type_mips));
    default:
      PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
			"Relocations ASCII tables not available", (NULL));
    }
}


/** 
 * @brief Return the max number of relocation type for this architecture 
 * @param file
 */
int           revm_getmaxrelnbr(elfshobj_t *file)
{

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  switch (elfsh_get_arch(file->hdr))
    {
    case EM_386:
      /* XXX: case EM_486: */
      PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, (ELFSH_RELOC_i386_MAX));
    case EM_SPARC:
    case EM_SPARC32PLUS:
      PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, (ELFSH_RELOC_SPARC_MAX));
    case EM_SPARCV9:
      PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, (ELFSH_RELOC_SPARC64_MAX));
    case EM_ALPHA:
#if EM_ALPHA != EM_ALPHA_EXP
    case EM_ALPHA_EXP:
#endif
      PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, (ELFSH_RELOC_ALPHA_MAX));
    case EM_IA_64:
      PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, (ELFSH_RELOC_IA64_MAX));
    case EM_MIPS:
    case EM_MIPS_RS3_LE:
      PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, (ELFSH_RELOC_MIPS_MAX));
    default:
      PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, "Unknown architecture", (-1));
    }
}


/** 
 * @brief Display relocation entries 
 */
int		cmd_rel()
{
  elfshsect_t	*sect;
  elfsh_Rel	*rel;
  regex_t	*tmp;
  char		*type;
  char		*typeshort;
  char		*name;
  unsigned int		index;
  unsigned int		index2;
  unsigned int		typenum;
  char		buff[256];
  unsigned int         size;
  revmconst_t  *types;
  char		addstr[32];
  char		logbuf[BUFSIZ];
  void		*data;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  /* Sanity checks */
  sect = elfsh_get_reloc(world.curjob->curfile, 0, &size);
  if (sect == NULL)
    RET(-1);

  /* Choose between global or local regx */
  FIRSTREGX(tmp);
  snprintf(logbuf, BUFSIZ - 1, " [RELOCATION TABLES]\n [Object %s]\n\n", 
	   world.curjob->curfile->name);
  revm_output(logbuf);

  /* We need to iterate as much as there is .rel* sections */
  for (index2 = 0; sect; index2++)
    {

      snprintf(logbuf, BUFSIZ - 1,
	       " {Section %s} \n", elfsh_get_section_name(world.curjob->curfile, sect));
      revm_output(logbuf);

      /* Iterate on the .rel entries array for each .rel section */
      data = elfsh_get_raw(sect);
      for (index = 0; index < size; index++)
	{

	  /* Get the current relocation entry */
	  if (sect->shdr->sh_type == SHT_RELA)
	    {

	      rel = (void *) ((elfsh_Rela *) data + index);
	      snprintf(addstr, sizeof(addstr), "add[%s]",
		       revm_colornumber("%08u", (unsigned int) ((elfsh_Rela *) rel)->r_addend));
	    }
	  else
	    {
	      rel = (elfsh_Rel *) data + index;
	      addstr[0] = 0x00;
	    }


	  /* Get linked symbol name */
	  name = elfsh_get_symname_from_reloc(world.curjob->curfile, rel);
	  typenum  = elfsh_get_reltype(rel);
	  types = revm_getrelascii(world.curjob->curfile);

	  type      = (char *) (typenum > ELFSH_RELOC_MAX(world.curjob->curfile) ? NULL :
				types[typenum].desc);
	  typeshort = (char *) (typenum > ELFSH_RELOC_MAX(world.curjob->curfile) ? NULL :
				types[typenum].name);

	  /* Output is different depending on the quiet flag */
	  if (!world.state.revm_quiet)
	    snprintf(buff, sizeof(buff),
		     " [%s] %s %s %s%s%s : %s %s => %s\n",
		     revm_colornumber("%03u", index), 
		     revm_colortypestr_fmt("%-15s", typeshort),
		     revm_coloraddress(XFMT, elfsh_get_reloffset(rel)),
		     revm_colorfieldstr("sym["),
		     revm_colornumber("%03u", elfsh_get_relsym(rel)),
		     revm_colorfieldstr("]"),
		     (name != NULL ? revm_colorstr_fmt("%-30s", name) : revm_colorwarn_fmt("%-30s", "<?>")), addstr, 
		     revm_colortypestr(type));
	  else
	    snprintf(buff, sizeof(buff),
		     " [%s] %s %s %s%s%s : %s %s\n",
		     revm_colornumber("%03u", index), 
		     revm_colortypestr_fmt("%-15s", typeshort),
		     revm_coloraddress(XFMT, elfsh_get_reloffset(rel)),
		     revm_colorfieldstr("sym["),
		     revm_colornumber("%03u", elfsh_get_relsym(rel)),
		     revm_colorfieldstr("]"),
		     (name != NULL ? revm_colorstr_fmt("%-22s", name) : revm_colorwarn_fmt("%-22s", "<?>")),
		     addstr);

	  /* Print it if it matchs the regex */
	  if (NULL == tmp || (tmp != NULL && name != NULL &&
			      0 == regexec(tmp, buff, 0, 0, 0)))
	    switch (revm_output(buff))
	      {
	      case -1:
		revm_endline();
		revm_output("\n");
		PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
	      case -2:
		revm_endline();
		goto next;
	      }
	  
	  revm_endline();
	}

    next:
       sect = elfsh_get_reloc(world.curjob->curfile, index2 + 1, &size);
       revm_output("\n");
    }

  revm_output("\n");
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}
