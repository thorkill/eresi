/*
** modremap.c for elfsh
** 
** Coded by spacewalker, grace day of 14th March 2003
**
** THIS CODE IS EXPERIMENTAL !
**
** It cannot remap some binaries because of existing false
** positives in the ET_EXEC relocation code . Finally
** merged in libelfsh/mapped.c
**
** Updated regulary by ELFsh crew to match the current interface
*/
#include "elfsh.h"


#define		CMD_REMAP	"remap"


int		remap_cmd()
{
  elfshobj_t	*file;
  elfshsect_t	*cur;
  u_int		new_base;
  u_int		real_base = 0xffffffff;
  int		diff;
  int		i;
  int		cnt;
  u_int		count_raw = 0;
  u_int		count_pht = 0;
  u_int		count_sht = 0;
  u_int		count_ent = 0;

  /* Sanity checks */
  i = sscanf(world.curjob->curcmd->param[0], "0x%X", &new_base);
  if (new_base == 0 || i != 1) 
    ELFSH_SETERROR("[elfsh] Invalid new base address\n", -1);
  file = world.curjob->current;
  if (elfsh_read_obj(file) < 0)
    return (-1);
  if (elfsh_get_symtab(file, NULL) < 0)
    elfsh_error();

  /* Calculate delta */
  real_base = elfsh_get_object_baseaddr(file);
  if (real_base == 0xffffffff)
    ELFSH_SETERROR("[elfsh:cmd_remap] Base address not found\n", -1);
  if (new_base & 0xfff)
    {
      printf(" [*] Base address adapted to be congruent pagesize\n");
      new_base &= 0xfffff000;
    }
  diff = new_base - real_base;
  printf(" [*] Delta is %08X \n", diff);

  /* Update entry point */
  if (file->hdr->e_entry > real_base)
    {
      file->hdr->e_entry += diff;
      count_ent++;
    }

  /* For all sections of the current object */
  for (cur = file->sectlist; cur != NULL; cur = cur->next) 
    {
      cnt = elfsh_relocate_section(cur, diff);
      if (cnt < 0)
	{
	  printf(" [*] MODREMAP : Section %s wont be relocated\n", cur->name);
	  continue;
	}
      count_raw += cnt;
    }

  /* Fixup SHT */
  count_sht += elfsh_reloc_sht(file, diff);

  /* Fixup PHT */
  count_pht += elfsh_reloc_pht(file, diff);

  /* Print msg */
  printf(" [*] Total number of modified references : %u \n"
         "\t PHT relocation : %u \n"
         "\t SHT relocation : %u \n"
         "\t ENT relocation : %u \n"
         "\t RAW relocation : %u \n",
         count_pht + count_sht + count_ent + count_raw,
         count_pht , count_sht , count_ent , count_raw);
  printf(" [*] Remapping at base %08X -OK-\n\n", new_base);

  return (0);
}

void elfsh_init()
{
  puts(" [*] ELFsh modremap init -OK- \n");
  vm_addcmd(CMD_REMAP, remap_cmd, vm_getoption, 1);
}

void elfsh_fini()
{
  puts(" [*] ELFsh modremap fini -OK- \n");
  vm_delcmd(CMD_REMAP);
}



