/*
** rel.c for elfsh
**
** Started on  Fri Nov  2 15:19:19 2001 mayhem
** Last update Sat Aug 16 14:05:11 2003 jv
*/
#include "elfsh.h"


/* Choose relocation tables strings array giving the architecture */
elfshconst_t    *vm_getrelascii(elfshobj_t *file)
{
   switch (elfsh_get_arch(file->hdr))
     {
      case EM_386:
      /* XXX: case EM_486: */
	return (elfsh_rel_type_i386);
      case EM_SPARC:
      case EM_SPARC32PLUS:
	return (elfsh_rel_type_sparc);
      default:
	return (NULL);
     }
}


/* Return the max number of relocation type for this architecture */
int           vm_getmaxrelnbr(elfshobj_t *file)
{
   switch (elfsh_get_arch(file->hdr))
     {
      case EM_386:
      /* XXX: case EM_486: */
	return (ELFSH_RELOC_i386_MAX);
      case EM_SPARC:
      case EM_SPARC32PLUS:
	return (ELFSH_RELOC_SPARC_MAX);
      default:
	return (-1);
     }
}


/* Display relocation entries */
int		cmd_rel()
{
  elfshsect_t	*sect;
  Elf32_Rel	*rel;
  regex_t	*tmp;
  char		*type;
  char		*typeshort;
  char		*name;
  u_int		index;
  u_int		index2;
  u_int		typenum;
  char		buff[256];
  u_int         size;
  elfshconst_t  *types;
  char		addstr[32];

  /* Sanity checks */
  sect = elfsh_get_reloc(world.current, 0, &size);
  if (sect == NULL)
    RET(-1);

  /* Choose between global or local regx */
  CHOOSE_REGX(tmp);
  printf(" [RELOCATION TABLES]\n [Object %s]\n\n", world.current->name);

  /* We need to iterate as much as there is .rel* sections */
  for (index2 = 0; sect; index2++)
    {

      printf(" {Section %s} \n", elfsh_get_section_name(world.current, sect));

      /* Iterate on the .rel entries array for each .rel section */
      for (index = 0; index < size; index++)
	{

	  /* Get the current relocation entry */
	  if (sect->shdr->sh_type == SHT_RELA)
	    {

	      rel = (void *) ((Elf32_Rela *) sect->data + index);
	      snprintf(addstr, sizeof(addstr), "add[%05d]",
		       ((Elf32_Rela *) rel)->r_addend);
	    }
	  else
	    {
	      rel = (Elf32_Rel *) sect->data + index;
	      addstr[0] = 0x00;
	    }


	  /* Get linked symbol name */
	  name = elfsh_get_symname_from_reloc(world.current, rel);
	  typenum  = elfsh_get_reltype(rel);
	  types = vm_getrelascii(world.current);

	  type      = (char *) (typenum > ELFSH_RELOC_MAX(world.current) ? NULL :
				types[typenum].desc);
	  typeshort = (char *) (typenum > ELFSH_RELOC_MAX(world.current) ? NULL :
				types[typenum].name);

	  /* Output is different depending on the quiet flag */
	  if (!world.state.vm_quiet)
	    snprintf(buff, sizeof(buff),
		     " [%03u] %-15s %-10p sym[%03u] : %-30s %s => %s",
		     index, typeshort,
		     (void *) elfsh_get_reloffset(rel),
		     elfsh_get_relsym(rel),
		     (name != NULL ? name : "<?>"), addstr, type);
	  else
	    snprintf(buff, sizeof(buff),
		     " [%03u] %-15s %-10p sym[%03u] : %-22s %s",
		     index, typeshort,
		     (void *) elfsh_get_reloffset(rel),
		     elfsh_get_relsym(rel),
		     (name != NULL ? name : "<?>"),
		     addstr);

	  /* Print it if it matchs the regex */
	  if (NULL == tmp || (tmp != NULL && name != NULL &&
			      NULL == regexec(tmp, buff, 0, 0, 0)))
	    puts(buff);
	}

       sect = elfsh_get_reloc(world.current, index2 + 1, &size);
       puts("");
    }

  puts("");
  return (0);
}
