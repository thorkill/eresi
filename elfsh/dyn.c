/*
** dyn.c for elfsh
** 
** Started on  Fri Nov  2 15:17:36 2001 mayhem
** Last update Tue May 27 04:31:08 2003 mayhem
*/
#include "elfsh.h"


/* Handlers for value for special tags */
void		vm_do_feature1(elfshobj_t *file, Elf32_Dyn *entry, char *info)
{
  u_int		cnt;
  u_int		idx;
  char		buff[45];

  *buff = '\n';
  memset(buff + 1, ELFSH_SPACE, sizeof(buff) - 2);
  buff[44] = 0x00;
  for (cnt = idx = 0; idx < ELFSH_FEATURE_MAX; idx++)
    if (entry->d_un.d_val & elfsh_feature1[idx].val)
      cnt += snprintf(info + cnt, BUFSIZ, "%s%s", 
		      (cnt ? buff : ""), 
		      elfsh_feature1[idx].desc);
}

void      vm_do_posflag1(elfshobj_t *file, Elf32_Dyn *entry, char *info)
{
  u_int		cnt;
  u_int		idx;
  char		buff[45];

  *buff = '\n';
  memset(buff + 1, ELFSH_SPACE, sizeof(buff) - 2);
  buff[44] = 0x00;
  for (cnt = idx = 0; idx < ELFSH_POSFLAG_MAX; idx++)
    if (entry->d_un.d_val & elfsh_posflag1[idx].val)
      cnt += snprintf(info + cnt, BUFSIZ, "%s%s", 
		      (cnt ? buff : ""), 
		      elfsh_posflag1[idx].desc);
}

void      vm_do_flags(elfshobj_t *file, Elf32_Dyn *entry, char *info)
{
  u_int		cnt;
  u_int		idx;
  char		buff[45];

  *buff = '\n';
  memset(buff + 1, ELFSH_SPACE, sizeof(buff) - 2);
  buff[44] = 0x00;
  for (cnt = idx = 0; idx < ELFSH_FLAGS_MAX; idx++)
    if (entry->d_un.d_val & elfsh_flags[idx].val)
      cnt += snprintf(info + cnt, BUFSIZ, "%s%s", 
		      (cnt ? buff : ""), 
		      elfsh_flags[idx].desc);
}

void      vm_do_flags1(elfshobj_t *file, Elf32_Dyn *entry, char *info)
{
  u_int		cnt;
  u_int		idx;
  char		buff[45];

  *buff = '\n';
  memset(buff + 1, ELFSH_SPACE, sizeof(buff) - 2);
  buff[44] = 0x00;
  for (cnt = idx = 0; idx < ELFSH_FLAGS1_MAX; idx++)
    if (entry->d_un.d_val & elfsh_flags1[idx].val)
      cnt += snprintf(info + cnt, BUFSIZ, "%s%s", 
		      (cnt ? buff : ""), 
		      elfsh_flags1[idx].desc);
}





/* Provide human readable output for .dynamic entries */
void		vm_dynentinfo(elfshobj_t	*file, 
			      Elf32_Dyn		*entry, 
			      char		*info)
{
  char		*str;

  switch (entry->d_tag)
    {
    case DT_NEEDED:
    case DT_SONAME:
    case DT_RPATH:
    case DT_AUXILIARY:
    case DT_FILTER:
      str = elfsh_get_dynentry_string(file, entry);
      if (str == NULL)
	elfsh_error();
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
      sprintf(info, "%d", (int) entry->d_un.d_val);
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
      sprintf(info, "%u bytes", (unsigned int) entry->d_un.d_val);
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
      sprintf(info, "0x%08X", (u_int) entry->d_un.d_ptr);
      break;
    case DT_FEATURE_1:
      vm_do_feature1(file, entry, info);
      break;
    case DT_POSFLAG_1:
      vm_do_posflag1(file, entry, info);
      break;
    case DT_FLAGS:
      vm_do_flags(file, entry, info);
      break;
    case DT_FLAGS_1:
      vm_do_flags1(file, entry, info);
      break;
    case DT_NULL:
      sprintf(info, "END OF SECTION");
      break;
    default:
      sprintf(info, "[?]");
      break;
    }
}


/* GNU and SUN extensions for the dynamic section */
char		*vm_getdyntype(u_int type)
{
  u_int		idx;

  for (idx = 0; idx < ELFSH_EXTDYN_MAX; idx++)
    if (elfsh_extdyn_type[idx].val == type)
      return ((char *) elfsh_extdyn_type[idx].desc);
  return ("[?]");
}


/* GNU and Sun extensions for the dynamic section */
char		*vm_getdyntype_short(u_int type)
{
  u_int		idx;

  for (idx = 0; idx < ELFSH_EXTDYN_MAX; idx++)
    if (elfsh_extdyn_type[idx].val == type)
      return ((char *) elfsh_extdyn_type[idx].name);
  return ("[?]");
}




/* Display the .dynamic section entries */
int		cmd_dyn()
{
  Elf32_Dyn	*actual;
  int		num;
  int		index;
  int		typenum;
  char		*type;
  char		*type_short;
  regex_t	*tmp;
  char		buff[256];
  char		type_unk[ELFSH_MEANING + 1];
  char		info[BUFSIZ];

  /* Fetch the section and init the regex */
  if ((actual = elfsh_get_dynamic(world.current, &num)) == NULL)
    RET(-1);
  CHOOSE_REGX(tmp);
  printf(" [SHT_DYNAMIC]\n [Object %s]\n\n", world.current->name);

  /* Loop on the section */
  for (index = 0; index < num && actual[index].d_tag != DT_NULL; index++)
    {
      
      /* Get the data to be printed */
      typenum = elfsh_get_dynentry_type(actual + index);
      if (typenum >= ELFSH_DYNAMIC_MAX)
	{
	  type = vm_getdyntype(typenum);
	  type_short = vm_getdyntype_short(typenum);
	  if (type == NULL)
	    type = type_short = vm_build_unknown(type_unk, "UNK", typenum);
	}
      else
	{
	  type       = (char *) elfsh_dynentry_type[typenum].desc;
	  type_short = (char *) elfsh_dynentry_type[typenum].name;
	}

      bzero(info, sizeof(info));
      vm_dynentinfo(world.current, actual + index, info);

      /* Print if the regex match */
      snprintf(buff, sizeof(buff), " [%02u] %-33s => %19s {%s}", /* +43 */
	       index, type, info, type_short);

      if (!tmp || (tmp && !regexec(tmp, buff, 0, 0, 0)))
	puts(buff);
    }
  
  puts("");
  return (0);
}







