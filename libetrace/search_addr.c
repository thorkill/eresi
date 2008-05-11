/**
 * @file func_add.c
** @ingroup libetrace
** 
** @brief add function to a trace table
** 
** Started Jul 2 2005 00:03:44 mxatone
** 
**
** $Id: search_addr.c,v 1.2 2007-11-29 10:25:02 rival Exp $
**
*/
#include "libelfsh.h"
#include "libasm.h"
#include "libetrace.h"
#include "libetrace-extern.h"

/**
 * Setup asm_processor structure correctly
 * @param file Pointer to elfsh object.
 * @param proc pointer to asm_processor structure
 */
static int		etrace_setup_proc(elfshobj_t *file, asm_processor *proc)
{
  u_int         	arch;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  if (!file || !proc)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		 "Invalid parameters", -1);
  
  arch = elfsh_get_arch(file->hdr);

  switch(arch)
    {
    case EM_SPARC:
    case EM_SPARCV9:
    case EM_SPARC32PLUS:
      asm_init_arch(proc, ASM_PROC_SPARC);
      break;
    case EM_386:
      asm_init_arch(proc, ASM_PROC_IA32);
      break;
    case EM_MIPS:
    case EM_MIPS_RS3_LE:
    case EM_MIPS_X:
      asm_init_arch(proc, ASM_PROC_MIPS);
      break;
    default:
      PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		   "Unsupported architecture for address tracing", -1);
    }

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}

/**
 * Get function addr list from call search basic
 * @param file target file
 * @param addr address list
 */
int			elfsh_addr_get_func_list(elfshobj_t *file, eresi_Addr **addr)
{
  int 			ret;
  int			index;
  asm_instr		instr;
  elfsh_SAddr		foffset;
  elfsh_Word		len;
  char			*base;
  asm_processor		proc;
  eresi_Addr		base_vaddr, caddr;
  u_char		found = 0;
  elfshsect_t		*text;
  eresi_Addr		*vaddr;
  const int		astep = 20;
  u_int			apos = 0;
  btree_t		*broot = NULL;
  u_int			diff;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  if (!file || !addr)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		 "Invalid parameters", -1);

  /* Search entrypoint section, our address must be in this section */
  text = elfsh_get_parent_section(file, elfsh_get_entrypoint(file->hdr), &foffset);

  if (!text)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		 "Cannot find parent section from entry point", -1);
  
  if (!elfsh_get_anonymous_section(file, text))
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
		 "Unable to get an anonymous section", -1);
  
  base = elfsh_get_raw(text);
  len = text->shdr->sh_size;

  /* Get the virtual address */
  base_vaddr = (elfsh_is_debug_mode() && !elfsh_section_is_runtime(text) ?
		file->rhdr.base + elfsh_get_section_addr(text->shdr) :
		elfsh_get_section_addr(text->shdr));

  /* Setup asm_processor structure */
  if (etrace_setup_proc(file, &proc) < 0)
        PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
		 "Failed during proc structure setup", -1);

  XALLOC(__FILE__, __FUNCTION__, __LINE__, vaddr, sizeof(eresi_Addr)*astep, -1);
  
  /* Despite the fact that we choose the right architecture to init asm,
     Our approach is totally architecture independant as we search using
     global type ASM_TYPE_CALLPROC and we know that op[0].imm will contain a
     relative value. */
  for (index = 0; index < len; index += ret)
    {
      /* Read an instruction */
      if ((ret = asm_read_instr(&instr, (u_char *) (base + index), len -  index, &proc)))
	{
	  /* Global assembler filter */
	  if ((instr.type & ASM_TYPE_CALLPROC)
	      && instr.op[0].imm != 0)
	    {
	      caddr = base_vaddr + index + instr.op[0].imm + instr.len;

	      /* Found a call check its local */
	      if (INTERVAL(base_vaddr, caddr, base_vaddr + len))
		{
		  found = 1;

		  diff = (u_int) caddr;

		  /* Avoid double entrie */
		  if (btree_get_elem(broot, diff) != NULL)
		    goto next;

		  btree_insert(&broot, diff, (void *)0x1);

		  /* Next will be the last of the current list
		     then realloc */
		  if ((apos+1) % astep == 0)
		    {
		      XREALLOC(__FILE__, __FUNCTION__, __LINE__, vaddr, vaddr,
			       sizeof(eresi_Addr)*(apos+1+astep), -1);

		      /* Blank new elements */
		      memset(&vaddr[apos], 0x00, astep*sizeof(eresi_Addr));
		    }

		  vaddr[apos++] = caddr;
		}
	    }
	}

    next:

      if (ret <= 0)
	ret = 1;
    }

  /* If nothing found we free allocated buffer and
     return an error */
  if (!found)
    {
      XFREE(__FILE__, __FUNCTION__, __LINE__, vaddr);
      PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		   "No call internal found", -3);
    }
  
  btree_free(broot, 0);

  *addr = vaddr;
  
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}

/**
 * Search a call for a given address
 * @param file target file
 * @param addr supose to be a function
 */
int			elfsh_addr_is_called(elfshobj_t *file, eresi_Addr addr)
{
  int 			ret;
  int			index;
  asm_instr		instr;
  elfsh_SAddr		foffset;
  elfsh_Word		len;
  char			*base;
  asm_processor		proc;
  eresi_Addr		base_vaddr;
  u_char		found = 0;
  elfshsect_t		*text;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  if (!file)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		 "Invalid parameter", -1);

  /* Search entrypoint section, our address must be in this section */
  text = elfsh_get_parent_section(file, elfsh_get_entrypoint(file->hdr), &foffset);

  if (!text)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		 "Cannot find parent section from entry point", -1);
  
  if (!elfsh_get_anonymous_section(file, text))
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
		 "Unable to get an anonymous section", -1);
  
  base = elfsh_get_raw(text);
  len = text->shdr->sh_size;

  /* Get the virtual address */
  base_vaddr = (elfsh_is_debug_mode() && !elfsh_section_is_runtime(text) ?
		file->rhdr.base + elfsh_get_section_addr(text->shdr) :
		elfsh_get_section_addr(text->shdr));

  /* Our address is valid ? */
  if (!INTERVAL(base_vaddr, addr, (base_vaddr + len)))
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
		 "Not in entrypoint section", -4);

  /* Setup asm_processor structure */
  if (etrace_setup_proc(file, &proc) < 0)
        PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
		 "Failed during proc structure setup", -1);
  
  /* Despite the fact that we choose the right architecture to init asm,
     Our approach is totally architecture independant as we search using
     global type ASM_TYPE_CALLPROC and we know that op[0].imm will contain a
     relative value. */
  for (index = 0; index < len; index += ret)
    {
      /* Read an instruction */
      if ((ret = asm_read_instr(&instr, (u_char *) (base + index), len -  index, &proc)))
	{
	  /* Global assembler filter */
	  if ((instr.type & ASM_TYPE_CALLPROC)
	      && instr.op[0].imm != 0)
	    {
	      /* Found the correct call */
	      if (base_vaddr + index + instr.op[0].imm + instr.len == addr)
		{
		  found = 1;
		  break;
		}
	    }
	}

      if (ret <= 0)
	ret = 1;
    }

  if (!found)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		 "No call found", -3);

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}
