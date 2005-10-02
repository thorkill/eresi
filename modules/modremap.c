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
** Fixed by ELFsh crew for the ELFsh project
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

  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);

  /* Sanity checks */
  i = sscanf(world.curjob->curcmd->param[0], "0x%X", &new_base);
  if (new_base == 0 || i != 1) 
    ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Invalid new base address", -1);
  file = world.curjob->current;

  if (elfsh_read_obj(file) < 0)
    ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__,
                      "Unable to read object file", -1);

  if (elfsh_get_symtab(file, NULL) < 0)
    ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__,
                      "Unable to read symbol table", -1);

  /* Calculate delta */
  real_base = elfsh_get_object_baseaddr(file);
  if (real_base == 0xffffffff)
    ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Base address not found", -1);

  if (new_base & 0xfff)
    {
      vm_output(" [*] Base address adapted to be congruent pagesize \n");
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

  ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}

void elfsh_init()
{
  vm_output(" [*] ELFsh modremap init -OK- \n");
  vm_addcmd(CMD_REMAP, remap_cmd, vm_getoption, 1, 
	    "Try to remap a non-relocatable file");
}

void elfsh_fini()
{
  vm_output(" [*] ELFsh modremap fini -OK- \n");
  vm_delcmd(CMD_REMAP);
}



