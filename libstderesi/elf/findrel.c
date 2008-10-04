/**
** @file findrel.c
**
** Started on  Sat Feb 22 17:06:17 2003 jfv
**
**
** $Id: findrel.c,v 1.2 2008-02-16 12:32:27 thor Exp $
**
*/
#include "libstderesi.h"


/** 
 * @brief Print a buffer with a resolved virtual address and offset 
 * to a symbol name.
 * @param file
 * @param vaddr
 * @return
 */
char		*revm_reverse(elfshobj_t *file, unsigned int vaddr)
{
  char		*str;
  char		*new;
  elfsh_SAddr	off;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  str = elfsh_reverse_metasym(file, vaddr, &off);
  if (str == NULL)
    {
      XALLOC(__FILE__, __FUNCTION__, __LINE__,new, 8, NULL);
      sprintf(new, "?");
      PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, (new));
    }
  if (off)
    {
      XALLOC(__FILE__, __FUNCTION__, __LINE__,new, strlen(str) + 20, NULL);
      sprintf(new, "%s + %u", str, (unsigned int) off);
    }
  else
    new = strdup(str);
 
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, (new));
}


#if 0		/* WIP */
/** 
 * Return 1 if relocation entry is a false positive
 * If one of these checks are not honored, we conclude to a FP :
 * - dword is not part of only 1 instruction 
 * - dword is not part of only 1 operand
 * - dword do not point to a referenced place
 */
static int		revm_catch_fp(asm_instr *i, unsigned int begin, unsigned int len, unsigned int dword)
{

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

#if __DEBUG_ASLR__
 snprintf(logbuf, BUFSIZ - 1, "[elfsh:revm_catch_fp] begin: %08X, totlen: %u, op1: %08X/%u, "
	 "op2: %08X/%u, op3: %08X/%u, dword: %08X \n",
	 begin, len, 
	 (i->op1 != NULL ? (unsigned int) i->op1->ptr : 0), (i->op1 != NULL ? i->op1->len : 0), 
	 (i->op2 != NULL ? (unsigned int) i->op2->ptr : 0), (i->op2 != NULL ? i->op2->len : 0), 
	 (i->op3 != NULL ? (unsigned int) i->op3->ptr : 0), (i->op3 != NULL ? i->op3->len : 0),
	 dword); 
  asm_display_instr_att(i, 0);
#endif

  if (begin + len < dword + 4)
    PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, (1));
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}

/** 
 * Say wether or not this entry is a false positive.
 * We are using architecture dependant heuristics :
 * - Instruction alignement
 * - Operand alignement 
 * @param dat
 * @param dword
 */
static int	        revm_catch_relocfp(char *dat, unsigned int dword)
{
  asm_instr		ptr;
  unsigned int			ret;
  unsigned int			begin;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  while (1)
    {
      begin = (unsigned int) dat;
      ret = asm_read_instr(&ptr, dat, 10, &world.proc);
      if (!ret)
	{
#if __DEBUG_ASLR__
	 snprintf(logbuf, BUFSIZ - 1, "[elfsh:catch_relocfp] Libasm bad fetching..\n");
#endif
	  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
	}
      dat += ret;
      
      /* Detect a false positive if the dword overlap many instructions */
      if (begin <= dword && begin + ret >= dword)
	return (revm_catch_fp(&ptr, begin, ret, dword));
    }
}
#else
static __inline__ int	revm_catch_relocfp(char *dat, unsigned int word) 
{ 
  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0); 
}
#endif


/** 
 * Find the missing relocation table for an ET_EXEC object 
 */
int		cmd_findrel()
{
  elfshobj_t	*file;
  elfshsect_t	*cur;
  elfshsect_t	*dst;
  unsigned int		index;
  char		soff[30];
  char		doff[30];
  unsigned int		count;
  unsigned int		dword;
  char		*reloc_type;
  char		logbuf[BUFSIZ];
  void		*data;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  /* Sanity checks */
  file = world.curjob->curfile;
  if (file == NULL)
    RETERR("[elfsh:findrel] Invalid NULL parameter\n");
  if (elfsh_read_obj(file) < 0)
    RETERR("[elfsh:findrel] Cannot read object\n");

  snprintf(logbuf, BUFSIZ - 1,
	   " [*] EXTRA relocs for %s \n\n", file->name);
  revm_output(logbuf);

  /* For all sections of the current object */
  for (count = 0, cur = file->sectlist; cur; cur = cur->next)
    {

      /* Do not look for cross references on unmapped or void sections */
      data = elfsh_get_raw(cur);
      if (data == NULL)
	{
	  if (!world.state.revm_quiet)
	   snprintf(logbuf, BUFSIZ - 1, " [*] Passing %-20s {NO DATA}\n", cur->name);
	  continue;
	}
      if (cur->shdr->sh_addr == 0)
	{
	  if (!world.state.revm_quiet)
	   snprintf(logbuf, BUFSIZ - 1, " [*] Passing %-20s {UNMAPPED}\n", cur->name);
	  continue;
	}
      
      /* Find the relocation entries, and print it */
      elfsh_find_rel(cur);
      if (cur->rel == NULL || cur->srcref == 0)
	{
	  if (!world.state.revm_quiet)
	   snprintf(logbuf, BUFSIZ - 1, " [*] Passing %-20s {NO RELOC ENTRY}\n", cur->name);
	  continue;
	}
     
      /* Print the relocs for this section */
      revm_output("\n");
      for (index = 0; index < cur->srcref; index++)
	{

	  /* Detect false positives */
	  if (elfsh_get_section_execflag(cur->shdr))
	    {
	      dword = (unsigned int) data + cur->rel[index].off_src;

	      /* Must be done here so that libelfsh stay libasm dependant */
	      if (revm_catch_relocfp(data, dword))
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
		 (unsigned int) cur->shdr->sh_addr + cur->rel[index].off_src,
	  	 (unsigned int) dst->shdr->sh_addr + cur->rel[index].off_dst,
		 reloc_type); 
	  count++;
	}
      revm_output("\n");
    }
  
  /* Print final banner */
 snprintf(logbuf, BUFSIZ - 1, "\n [*] ET_EXEC relocation entries number : %u\n\n", count);
  for (cur = world.curjob->curfile->sectlist; cur; cur = cur->next)
   snprintf(logbuf, BUFSIZ - 1, " [*] Section %-20s srcref[%010u] dstref[%010u] \n", 
	   cur->name, cur->srcref, cur->dstref);
  revm_output("\n");

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}




