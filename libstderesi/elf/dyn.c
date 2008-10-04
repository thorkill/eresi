/**
 * @file dyn.c
 * 
 * Started on  Fri Nov  2 15:17:36 2001 jfv
 *
 *
 * $Id: dyn.c,v 1.2 2008-02-16 12:32:27 thor Exp $
 *
 */
#include "libstderesi.h"


/** 
 * Handlers for value for special tags 
 */
void		revm_do_feature1(elfshobj_t *file, elfsh_Dyn *entry, char *info)
{
  unsigned int		cnt;
  unsigned int		idx;
  char		buff[45];

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  *buff = '\n';
  memset(buff + 1, REVM_SPACE, sizeof(buff) - 2);
  buff[44] = 0x00;
  for (cnt = idx = 0; idx < ELFSH_FEATURE_MAX; idx++)
    if (entry->d_un.d_val & elfsh_feature1[idx].val)
      cnt += snprintf(info + cnt, BUFSIZ, "%s%s", 
		      (cnt ? buff : ""), 
		      elfsh_feature1[idx].desc);

  PROFILER_OUT(__FILE__, __FUNCTION__, __LINE__);
}

/**
 * Need doxygen comment.
 * @param file
 * @param entry
 * @param info
 * @return
 */
void      revm_do_posflag1(elfshobj_t *file, elfsh_Dyn *entry, char *info)
{
  unsigned int		cnt;
  unsigned int		idx;
  char		buff[45];

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  *buff = '\n';
  memset(buff + 1, REVM_SPACE, sizeof(buff) - 2);
  buff[44] = 0x00;
  for (cnt = idx = 0; idx < ELFSH_POSFLAG_MAX; idx++)
    if (entry->d_un.d_val & elfsh_posflag1[idx].val)
      cnt += snprintf(info + cnt, BUFSIZ, "%s%s", 
		      (cnt ? buff : ""), 
		      elfsh_posflag1[idx].desc);

  PROFILER_OUT(__FILE__, __FUNCTION__, __LINE__);
}

/**
 * Need doxygen comment.
 * @param file
 * @param entry
 * @param info
 */
void      revm_do_flags(elfshobj_t *file, elfsh_Dyn *entry, char *info)
{
  unsigned int		cnt;
  unsigned int		idx;
  char		buff[45];

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  *buff = '\n';
  memset(buff + 1, REVM_SPACE, sizeof(buff) - 2);
  buff[44] = 0x00;
  for (cnt = idx = 0; idx < ELFSH_FLAGS_MAX; idx++)
    if (entry->d_un.d_val & elfsh_flags[idx].val)
      cnt += snprintf(info + cnt, BUFSIZ, "%s%s", 
		      (cnt ? buff : ""), 
		      elfsh_flags[idx].desc);

  PROFILER_OUT(__FILE__, __FUNCTION__, __LINE__);
}

/**
 * Need doxygen comment.
 * @param file
 * @param entry
 * @param info
 */
void      revm_do_flags1(elfshobj_t *file, elfsh_Dyn *entry, char *info)
{
  unsigned int		cnt;
  unsigned int		idx;
  char		buff[45];

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  *buff = '\n';
  memset(buff + 1, REVM_SPACE, sizeof(buff) - 2);
  buff[44] = 0x00;
  for (cnt = idx = 0; idx < ELFSH_FLAGS1_MAX; idx++)
    if (entry->d_un.d_val & elfsh_flags1[idx].val)
      cnt += snprintf(info + cnt, BUFSIZ, "%s%s", 
		      (cnt ? buff : ""), 
		      elfsh_flags1[idx].desc);

  PROFILER_OUT(__FILE__, __FUNCTION__, __LINE__);
}

/**
 * Need doxygen comment.
 * @param file
 * @param entry
 * @param info
 */
void      revm_do_mipsflags(elfshobj_t *file, elfsh_Dyn *entry, char *info)
{
  unsigned int		cnt;
  unsigned int		idx;
  char		buff[45];

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  *buff = '\n';
  memset(buff + 1, REVM_SPACE, sizeof(buff) - 2);
  buff[44] = 0x00;
  for (cnt = idx = 0; idx < ELFSH_MIPSFLAGS_MAX; idx++)
    if (entry->d_un.d_val & elfsh_mipsflags[idx].val)
      cnt += snprintf(info + cnt, BUFSIZ, "%s%s", 
		      (cnt ? buff : ""), 
		      elfsh_mipsflags[idx].desc);

  PROFILER_OUT(__FILE__, __FUNCTION__, __LINE__);
}



/** 
 * Provide human readable output for .dynamic entries.
 * @param file
 * @param entry
 * @param info
 */
void		revm_dynentinfo(elfshobj_t	*file, 
			      elfsh_Dyn		*entry, 
			      char		*info)
{
  char		*str;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  switch (entry->d_tag)
    {
    case DT_NEEDED:
    case DT_SONAME:
    case DT_RPATH:
    case DT_AUXILIARY:
    case DT_FILTER:
      str = elfsh_get_dynentry_string(file, entry);
      snprintf(info, 99, "%s", (str != NULL ? str : ELFSH_NULL_STRING));
      break;
    case DT_SYMBOLIC:
      sprintf(info, "Current object first checked during symbol resolving");
      break;
    case DT_TEXTREL:
      sprintf(info, "Relocation can overwrite .text");
      break;
    case DT_BIND_NOW:
      sprintf(info, "Symbol binding is now completed before execution");
      break;
    case DT_PLTREL:
    case DT_MOVEENT:
    case DT_SYMINENT:
    case DT_RELACOUNT:
    case DT_RELCOUNT:
    case DT_VERDEFNUM:
    case DT_VERNEEDNUM:
      sprintf(info, UFMT, entry->d_un.d_val);
      break;
    case DT_PLTRELSZ:
    case DT_RELASZ:
    case DT_RELAENT:
    case DT_STRSZ:
    case DT_RELSZ:
    case DT_RELENT:
    case DT_SYMENT:
    case DT_SYMINSZ:
    case DT_PLTPADSZ:
    case DT_MOVESZ:
      sprintf(info, UFMT " bytes", entry->d_un.d_val);
      break;
    case DT_PLTGOT:
    case DT_STRTAB:
    case DT_SYMTAB:
    case DT_HASH:
    case DT_RELA:
    case DT_INIT:
    case DT_FINI:
    case DT_REL:
    case DT_DEBUG:
    case DT_JMPREL:
    case DT_CHECKSUM:
    case DT_VERSYM:
    case DT_VERDEF:
    case DT_VERNEED:
      sprintf(info, XFMT, entry->d_un.d_ptr);
      break;
    case DT_FEATURE_1:
      revm_do_feature1(file, entry, info);
      break;
    case DT_POSFLAG_1:
      revm_do_posflag1(file, entry, info);
      break;
    case DT_FLAGS:
      revm_do_flags(file, entry, info);
      break;
    case DT_FLAGS_1:
      revm_do_flags1(file, entry, info);
      break;
    case DT_NULL:
      sprintf(info, "END OF SECTION");
      break;
      /* MIPS dynamic entry type */
    case DT_MIPS_RLD_VERSION:
      sprintf(info, UFMT,  entry->d_un.d_val);
      break;
    case DT_MIPS_TIME_STAMP:
      sprintf(info, UFMT,  entry->d_un.d_val);
      break;
    case DT_MIPS_ICHECKSUM:
      sprintf(info, UFMT,  entry->d_un.d_val);
      break;
    case DT_MIPS_IVERSION:
      sprintf(info, UFMT,  entry->d_un.d_val);
      break;
    case DT_MIPS_FLAGS:
      revm_do_mipsflags(file, entry, info);
      break;
    case DT_MIPS_BASE_ADDRESS:
      sprintf(info, XFMT,  entry->d_un.d_ptr);
      break;
    case DT_MIPS_MSYM:
      sprintf(info, XFMT,  entry->d_un.d_ptr);
      break;
    case DT_MIPS_CONFLICT:
      sprintf(info, XFMT,  entry->d_un.d_ptr);
      break;
    case DT_MIPS_LIBLIST:
      sprintf(info, XFMT,  entry->d_un.d_ptr);
      break;
    case DT_MIPS_LOCAL_GOTNO:
      sprintf(info, UFMT,  entry->d_un.d_val);
      break;
    case DT_MIPS_CONFLICTNO:
      sprintf(info, UFMT,  entry->d_un.d_val);
      break;
    case DT_MIPS_LIBLISTNO:
      sprintf(info, UFMT,  entry->d_un.d_val);
      break;
    case DT_MIPS_SYMTABNO:
      sprintf(info, UFMT,  entry->d_un.d_val);
      break;
    case DT_MIPS_UNREFEXTNO:
      sprintf(info, UFMT,  entry->d_un.d_val);
      break;
    case DT_MIPS_GOTSYM:
      sprintf(info, UFMT,  entry->d_un.d_val);
      break;
    case DT_MIPS_HIPAGENO:
      sprintf(info, UFMT,  entry->d_un.d_val);
      break;
    case DT_MIPS_RLD_MAP:
      sprintf(info, XFMT,  entry->d_un.d_ptr);
      break;
    case DT_MIPS_DELTA_CLASS:
    case DT_MIPS_DELTA_CLASS_NO:
    case DT_MIPS_DELTA_INSTANCE:
    case DT_MIPS_DELTA_INSTANCE_NO:
    case DT_MIPS_DELTA_RELOC:
    case DT_MIPS_DELTA_RELOC_NO:
    case DT_MIPS_DELTA_SYM:
    case DT_MIPS_DELTA_SYM_NO:
    case DT_MIPS_DELTA_CLASSSYM:
    case DT_MIPS_DELTA_CLASSSYM_NO:
    case DT_MIPS_CXX_FLAGS:
    case DT_MIPS_PIXIE_INIT:
    case DT_MIPS_SYMBOL_LIB:
    case DT_MIPS_LOCALPAGE_GOTIDX:
    case DT_MIPS_LOCAL_GOTIDX:
    case DT_MIPS_HIDDEN_GOTIDX:
    case DT_MIPS_PROTECTED_GOTIDX:
    case DT_MIPS_OPTIONS:
    case DT_MIPS_INTERFACE:
    case DT_MIPS_DYNSTR_ALIGN:
    case DT_MIPS_INTERFACE_SIZE:
    case DT_MIPS_RLD_TEXT_RESOLVE_ADDR:
    case DT_MIPS_PERF_SUFFIX:
    case DT_MIPS_COMPACT_SIZE:
    case DT_MIPS_GP_VALUE:
    case DT_MIPS_AUX_DYNAMIC:
    default:
      sprintf(info, "[?]");
      break;
    }

  PROFILER_OUT(__FILE__, __FUNCTION__, __LINE__);
}


/** 
 * GNU and SUN extensions for the dynamic section.
 * @param type
 */
char		*revm_getdyntype(unsigned int type)
{
  unsigned int		idx;

  for (idx = 0; idx < ELFSH_EXTDYN_MAX; idx++)
    if (elfsh_extdyn_type[idx].val == type)
      return ((char *) elfsh_extdyn_type[idx].desc);
  for (idx = 0; idx < ELFSH_MIPSDYN_MAX; idx++)
    if (elfsh_mipsdyn_type[idx].val == type)
      return ((char *) elfsh_mipsdyn_type[idx].desc);
  return ("[?]");
}


/** 
 * GNU and Sun extensions for the dynamic section.
 * @param type
 */
char		*revm_getdyntype_short(unsigned int type)
{
  unsigned int		idx;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  for (idx = 0; idx < ELFSH_EXTDYN_MAX; idx++)
    if (elfsh_extdyn_type[idx].val == type)
      PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 
			 ((char *) elfsh_extdyn_type[idx].name));

  for (idx = 0; idx < ELFSH_MIPSDYN_MAX; idx++)
    if (elfsh_mipsdyn_type[idx].val == type)
      PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 
			 ((char *) elfsh_mipsdyn_type[idx].name));

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, "[?]");
}




/** 
 * Display the .dynamic section entries 
 */
int		cmd_dyn()
{
  elfsh_Dyn	*actual;
  unsigned int		num;
  int		index;
  int		typenum;
  char		*type;
  char		*type_short;
  char		*p;
  regex_t	*tmp;
  char		buff[256];
  char		type_unk[ERESI_MEANING + 1];
  char		info[BUFSIZ];

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  /* Fetch the section and init the regex */
  if ((actual = elfsh_get_dynamic(world.curjob->curfile, &num)) == NULL)
    RET(-1);
  FIRSTREGX(tmp);
  snprintf(info, BUFSIZ - 1, 
	   " [SHT_DYNAMIC]\n [Object %s]\n\n", world.curjob->curfile->name);
  revm_output(info);

  /* Loop on the section */
  for (index = 0; index < num && actual[index].d_tag != DT_NULL; index++)
    {
      
      /* Get the data to be printed */
      typenum = elfsh_get_dynentry_type(actual + index);
      if (typenum >= ELFSH_DYNAMIC_MAX)
	{
	  type = revm_getdyntype(typenum);
	  type_short = revm_getdyntype_short(typenum);
	  if (type == NULL)
	    type = type_short = revm_build_unknown(type_unk, "UNK", typenum);
	}
	else
	{
	  type       = (char *) elfsh_dynentry_type[typenum].desc;
	  type_short = (char *) elfsh_dynentry_type[typenum].name;
	}
	
      bzero(info, sizeof(info));
      revm_dynentinfo(world.curjob->curfile, actual + index, info);

      p = NULL;
      if (strlen(info) == 14)
	p = info+10;

      if (IS_VADDR(info))
	revm_coloradv("address", "%19s", info);
      else if ((p != NULL && !strcmp(p, "bytes")) || info[0] == '0')
	revm_coloradv("number", "%19s", info);
      else
	revm_coloradv("string", "%19s", info);

      /* Print if the regex match */
      snprintf(buff, sizeof(buff), " %s %s => %s {%s}\n", /* +43 */
	       revm_colornumber("[%02u]", index), 
	       revm_colortypestr_fmt("%-33s", type), 
	       info, 
	       revm_colortypestr(type_short));

      if (!tmp || (tmp && !regexec(tmp, buff, 0, 0, 0)))
	revm_output(buff);

      revm_endline();
    }
  
  revm_output("\n");
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}







