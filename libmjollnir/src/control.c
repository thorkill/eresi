/*
** control.c in libmjollnir for elfsh
**
** Started : Thu May 29 20:44:39 2003 sk
** Updated : Sun Dec 30 16:45:48 2006 mayhem
**
** $Id: control.c,v 1.27 2007-05-01 15:56:01 may Exp $
**
*/
#include "libmjollnir.h"


/**
 * This function inspect the code at entry point and inject the resolved
 * symbol for the main function. This is a separate function so that it
 * can be called frmo the tracer and benefit from the main symbol presence
 */
elfsh_Addr	   mjr_find_main(elfshobj_t	*obj,
				 asm_processor	*proc,
				 u_char		*buf,
				 u_int		len,
				 elfsh_Addr	vaddr,
				 u_int		*dis)
{
  int		   stop;
  elfsh_Sym	   *sym;
  u_int		   ilen;
  elfsh_Addr	   init_addr;
  asm_instr	   ins;
  int		   arch_bin;
  int		   fetch_next = 0;
  elfsh_Addr	   main_addr;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);
  for (*dis = stop = 0; !stop; *dis += ilen) 
    {
      ilen = asm_read_instr(&ins, (u_char *) buf + *dis, 
			    len - *dis, proc);
      if (!dis) 
	{
	  if (proc->type == ASM_PROC_IA32)
	    {
	      switch (ins.instr) 
		{
		case ASM_XOR:      
		  arch_bin = MJR_BIN_LINUX;
		  break;
		case ASM_PUSH:
		  arch_bin = MJR_BIN_FREEBSD;
		  sym = elfsh_get_metasym_by_name(obj, ELFSH_SECTION_NAME_INIT);
		  init_addr = sym->st_value;
		  fprintf(stderr, 
			  " [*] locating call to .init: %lx\n", 
			  (unsigned long) init_addr);
		  break;
		}
	    }
	  else if (proc->type == ASM_PROC_SPARC) 
	    {
	      switch (ins.instr)
		{
		case ASM_SP_MOV:
		  arch_bin = MJR_BIN_LINUX;
		  break;
		}
	    }
	  // FIXME: This should go to librevm
	  fprintf(stderr, " [*] %s-like start\n", arch_bin ? "FreeBSD" : "Linux");  
	}
      
    /* Now fingerprint depending on the architecture */
    if (proc->type == ASM_PROC_IA32)
      {  
    	switch (ins.instr) 
  	  {
    	  case ASM_PUSH:
    	    if (*dis && (arch_bin == MJR_BIN_LINUX) && 
		ins.op1.type == ASM_OTYPE_IMMEDIATE)
    	      asm_operand_get_immediate(&ins, 1, 0, &main_addr);
    	    break;
    	  case ASM_CALL:
    	    if (arch_bin == MJR_BIN_FREEBSD) 
	      {
		asm_operand_get_immediate(&ins, 1, 0, &main_addr);
		if (fetch_next)
		  stop = 1;
		
		main_addr += vaddr + *dis + ilen;
		if (main_addr == init_addr)
		  fetch_next = 1;
		
		break;	
  	      }
    	    else if ((arch_bin == MJR_BIN_LINUX)) 
  	      {
		stop = 1;
		break;
	      }
	  }
      }
    else if (proc->type == ASM_PROC_SPARC)
      {
	switch (ins.instr)
	  {
     	  case ASM_SP_SETHI:
     	    if (ins.op1.base_reg == ASM_REG_O0)
     	      main_addr = ins.op2.imm << 10;
     	    break;
     	  case ASM_SP_OR:
     	    if (ins.op1.base_reg == ASM_REG_O0 &&
		ins.op3.base_reg == ASM_REG_O0 &&
		ins.op2.type == ASM_SP_OTYPE_IMMEDIATE)
     	      main_addr |= ins.op2.imm;
     	    break;
     	  case ASM_SP_CALL:
	    stop = 1;
     	    break;      	
   	  }
      }    
  }
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, main_addr);
}

/** 
 * This function traces the entry point and save the last push argument until 
 * a call is found. This allow to fetch the main address in an OS-dependent 
 * manner 
 */
elfsh_Addr	   mjr_trace_start(mjrcontext_t	*context,
				   u_char	*buf,
				   u_int	len,
				   elfsh_Addr	vaddr)
{
  elfsh_Addr	    main_addr;
  mjrcontainer_t   *main_container;
  mjrcontainer_t   *tmpcntnr;
  u_int		   dis;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);
  if (!context || !buf)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
		 "Invalid parameters", 0);
  
  if (!(elfsh_get_objtype(elfsh_get_hdr(context->obj)) == ET_EXEC))
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
		      "Object is not ET_EXEC", 0);
  
#if defined(__DEBUG_MJOLLNIR__)
  printf(" [*] _start found at %lx\n", (unsigned long) vaddr);
#endif

  main_addr = mjr_find_main(context->obj, &context->proc, buf, len, vaddr, &dis);
  tmpcntnr = mjr_create_function_container(context, vaddr, 0, "_start", 0, NULL);
  mjr_function_register(context, vaddr, tmpcntnr);
  main_container = mjr_create_function_container(context, main_addr, 0, "main", 0, NULL);
  mjr_function_register(context, main_addr, main_container);
  mjr_container_add_link(tmpcntnr, main_container->id, 
			 MJR_LINK_FUNC_CALL, MJR_LINK_OUT);
  mjr_container_add_link(main_container, tmpcntnr->id, 
			 MJR_LINK_FUNC_CALL, MJR_LINK_IN);
  mjr_blocks_link_call(context, vaddr, main_addr, vaddr + dis);
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, main_addr);
}


/**
 * This function trace execution flow and creates block depending on instruction.
 *
 * If instruction break execution flow, block is considerated finished and added 
 * to linked list of blocks (that is the content of the .edfmt.blocks section)
 *
 */
int			mjr_trace_control(mjrcontext_t	*context,
					  elfshobj_t    *obj, 
					  asm_instr     *ins, 
					  elfsh_Addr	vaddr)
{
  int				ret;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  mjr_history_write(context, ins, vaddr, MJR_HISTORY_CUR);
  
  /* Now abstract interpret the operational semantics of the current 
     instruction set using the typed instruction system of libasm */
  context->obj = obj;

  mjr_history_write(context, ins, vaddr, 0);

  ret = mjr_asm_flow(context);
  
  if (ret < 0)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
		 "Unable to interpret instruction type", -1);
  
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}

