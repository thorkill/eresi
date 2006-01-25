/*
** findrel.c for elfsh
**
** Started on  Sat Feb 22 17:06:17 2003 mayhem
**
*/
#include "elfsh.h"


/* Print a buffer with a resolved virtual address and offset to a symbol name */
char		*vm_reverse(elfshobj_t *file, u_int vaddr)
{
  char		*str;
  char		*new;
  elfsh_SAddr	off;

  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);

  str = elfsh_reverse_metasym(file, vaddr, &off);
  if (str == NULL)
    {
      XALLOC(new, 8, NULL);
      sprintf(new, "?");
      ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, (new));
    }
  if (off)
    {
      XALLOC(new, strlen(str) + 20, NULL);
      sprintf(new, "%s + %u", str, (u_int) off);
    }
  else
    new = elfsh_strdup(str);
  ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, (new));
}





#if 0		/* WIP */
/* 
** Return 1 if relocation entry is a false positive
** If one of these checks are not honored, we conclude to a FP :
** - dword is not part of only 1 instruction 
** - dword is not part of only 1 operand
** - dword do not point to a referenced place
*/
static int		vm_catch_fp(asm_instr *i, u_int begin, u_int len, u_int dword)
{

  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);

#if __DEBUG_ASLR__
 snprintf(logbuf, BUFSIZ - 1, "[elfsh:vm_catch_fp] begin: %08X, totlen: %u, op1: %08X/%u, "
	 "op2: %08X/%u, op3: %08X/%u, dword: %08X \n",
	 begin, len, 
	 (i->op1 != NULL ? (u_int) i->op1->ptr : 0), (i->op1 != NULL ? i->op1->len : 0), 
	 (i->op2 != NULL ? (u_int) i->op2->ptr : 0), (i->op2 != NULL ? i->op2->len : 0), 
	 (i->op3 != NULL ? (u_int) i->op3->ptr : 0), (i->op3 != NULL ? i->op3->len : 0),
	 dword); 
  asm_display_instr_att(i, 0);
#endif

  if (begin + len < dword + 4)
    ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, (1));
  ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}

/* 
** Say wether or not this entry is a false positive
** We are using architecture dependant heuristics :
** - Instruction alignement
** - Operand alignement 
*/
static int	        vm_catch_relocfp(char *dat, u_int dword)
{
  asm_instr		ptr;
  u_int			ret;
  u_int			begin;

  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);

  while (1)
    {
      begin = (u_int) dat;
      ret = asm_read_instr(&ptr, dat, 10, &world.proc);
      if (!ret)
	{
#if __DEBUG_ASLR__
	 snprintf(logbuf, BUFSIZ - 1, "[elfsh:catch_relocfp] Libasm bad fetching..\n");
#endif
	  ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
	}
      dat += ret;
      
      /* Detect a false positive if the dword overlap many instructions */
      if (begin <= dword && begin + ret >= dword)
	return (vm_catch_fp(&ptr, begin, ret, dword));
    }
}
#else
static __inline__ int	vm_catch_relocfp(char *dat, u_int word) 
{ 
  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);
  ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, 0); 
}
#endif








/* Find the missing relocation table for an ET_EXEC object */
int		cmd_findrel()
{
  elfshobj_t	*file;
  elfshsect_t	*cur;
  elfshsect_t	*dst;
  u_int		index;
  char		soff[30];
  char		doff[30];
  u_int		count;
  u_int		dword;
  char		*reloc_type;
  char		logbuf[BUFSIZ];
  void		*data;

  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);

  /* Sanity checks */
  file = world.curjob->current;
  if (file == NULL)
    RETERR("[elfsh:findrel] Invalid NULL parameter\n");
  if (elfsh_read_obj(file) < 0)
    RETERR("[elfsh:findrel] Cannot read object\n");

  snprintf(logbuf, BUFSIZ - 1,
	   " [*] EXTRA relocs for %s \n\n", file->name);
  vm_output(logbuf);

  /* For all sections of the current object */
  for (count = 0, cur = file->sectlist; cur; cur = cur->next)
    {

      /* Do not look for cross references on unmapped or void sections */
      data = elfsh_get_raw(cur);
      if (data == NULL)
	{
	  if (!world.state.vm_quiet)
	   snprintf(logbuf, BUFSIZ - 1, " [*] Passing %-20s {NO DATA}\n", cur->name);
	  continue;
	}
      if (cur->shdr->sh_addr == NULL)
	{
	  if (!world.state.vm_quiet)
	   snprintf(logbuf, BUFSIZ - 1, " [*] Passing %-20s {UNMAPPED}\n", cur->name);
	  continue;
	}
      
      /* Find the relocation entries, and print it */
      elfsh_find_rel(cur);
      if (cur->rel == NULL || cur->srcref == 0)
	{
	  if (!world.state.vm_quiet)
	   snprintf(logbuf, BUFSIZ - 1, " [*] Passing %-20s {NO RELOC ENTRY}\n", cur->name);
	  continue;
	}
     
      /* Print the relocs for this section */
      vm_output("\n");
      for (index = 0; index < cur->srcref; index++)
	{

	  /* Detect false positives */
	  if (elfsh_get_section_execflag(cur->shdr))
	    {
	      dword = (u_int) data + cur->rel[index].off_src;

	      /* Must be done here so that libelfsh stay libasm dependant */
	      if (vm_catch_relocfp(data, dword))
		cur->rel[index].type = ELFSH_RELOC_FP;
	    }
	  
	  /* Print the offset buffers */
	  if (cur->rel[index].off_src)
	    snprintf(soff, sizeof(soff), "+ %u", cur->rel[index].off_src);
	  else
	    *soff = 0x00;
	  if (cur->rel[index].off_dst)
	    snprintf(doff, sizeof(doff), "+ %u", cur->rel[index].off_dst);
	  else
	    *doff = 0x00;
	  
	  /* Get relocation type string */
	  reloc_type = (cur->rel[index].type == ELFSH_RELOC_FP       ? "FP" :
			cur->rel[index].type == ELFSH_RELOC_SECTBASE ? "SB" :
			"UK");

	  /* Print to stdout the current rel ent */
	  dst = elfsh_get_section_by_index(file,
					   cur->rel[index].idx_dst, 
					   NULL, 
					   NULL);
	 snprintf(logbuf, BUFSIZ - 1, " [%03u] FROM %15s %12s TO %15s %12s [ %08X -> %08X ] {%s} \n",
		 index, cur->name, soff, dst->name, doff, 
		 (u_int) cur->shdr->sh_addr + cur->rel[index].off_src,
	  	 (u_int) dst->shdr->sh_addr + cur->rel[index].off_dst,
		 reloc_type); 
	  count++;
	}
      vm_output("\n");
    }
  
  /* Print final banner */
 snprintf(logbuf, BUFSIZ - 1, "\n [*] ET_EXEC relocation entries number : %u\n\n", count);
  for (cur = world.curjob->current->sectlist; cur; cur = cur->next)
   snprintf(logbuf, BUFSIZ - 1, " [*] Section %-20s srcref[%010u] dstref[%010u] \n", 
	   cur->name, cur->srcref, cur->dstref);
  vm_output("\n");

  ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}




