/*
** @file control.c
**
** Started : Thu May 29 20:44:39 2003 sk
** Updated : Sun Dec 30 16:45:48 2006 jfv
**
*/

#include "libmjollnir.h"


/**
 * @brief Search main function from binary entry point.
 *
 * This function inspect the code at entry point and inject the resolved
 * symbol for the main function. This is a separate function so that it
 * can be called from the tracer and benefit from the main symbol presence
 */
eresi_Addr	   mjr_find_main(elfshobj_t	*obj,
				 asm_processor	*proc,
				 unsigned char		*buf,
				 unsigned int		len,
				 eresi_Addr	vaddr,
				 unsigned int		*dis)
{
  int		   stop;
  elfsh_Sym	   *sym;
  unsigned int		   ilen,done;
  eresi_Addr	   init_addr;
  asm_instr	   ins;
  int		   arch_bin;
  int		   fetch_next = 0;
  eresi_Addr	   main_addr;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  done = 0;

  for (*dis = stop = 0; !stop; *dis += ilen) 
    {
      ilen = asm_read_instr(&ins, (unsigned char *) buf + *dis, 
			    len - *dis, proc);

      if (!*dis)
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
	  else if (proc->type == ASM_PROC_MIPS)
	    {
	      // FIXME: this should be expanded
	      arch_bin = MJR_BIN_LINUX;
	    }

	  else 
	    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
			 "Architecture not supported", -1);

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
		ins.op[0].type == ASM_OTYPE_IMMEDIATE)
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
     	    if (ins.op[0].baser == ASM_REG_O0)
     	      main_addr = ins.op[1].imm << 10;
     	    break;
     	  case ASM_SP_OR:
     	    if (ins.op[0].baser == ASM_REG_O0 &&
		ins.op[2].baser == ASM_REG_O0 &&
		ins.op[1].type == ASM_SP_OTYPE_IMMEDIATE)
     	      main_addr |= ins.op[1].imm;
     	    break;
     	  case ASM_SP_CALL:
	    stop = 1;
     	    break;      	
   	  }
      }
    else if (proc->type == ASM_PROC_MIPS)
      {
	// FIXME: this should be expanded (WIP)
	main_addr = vaddr;
	stop = 1;
      }
    else
      PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
		   "Architecture not supported", -1);
 
    done += ilen;
    /* If we reached this point it probably means that we analyse 
       self made software, written in asm and compiled with nasm */
    if (done >= len)
      {
	main_addr = vaddr;
	stop = 1;
      }
    }
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, main_addr);
}

/** 
 * This function traces the entry point and save the last push argument until 
 * a call is found. This allow to fetch the main address in an OS-dependent 
 * manner 
 */
eresi_Addr	mjr_trace_start(mjrcontext_t	*context,
				unsigned char		*buf,
				unsigned int		len,
				eresi_Addr	vaddr)
{
  eresi_Addr	main_addr;
  container_t   *main_container;
  container_t   *tmpcntnr;
  unsigned int		dis;
  elfsh_Sym	*sym;
  elfsh_Sym	bsym;
  
  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);
  if (!context || !buf)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
		 "Invalid parameters", 0);
  
  if (!(elfsh_get_objtype(elfsh_get_hdr(context->obj)) == ET_EXEC))
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
		 "Object is not ET_EXEC", 0);
  
#if defined(__DEBUG_MJOLLNIR__)
  printf(" [*] _start found at 0x%lx\n", (unsigned long) vaddr);
#endif

 //  sym = elfsh_get_symbol_by_name(context->obj, "main");
  sym = elfsh_get_metasym_by_name(context->obj, "main");
  if (sym && sym->st_value)
    {
      main_addr = sym->st_value;
      dis = 0;
    }
  else
    main_addr = mjr_find_main(context->obj, &context->proc, buf, len, vaddr, &dis);
  
  if (main_addr == -1)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
		 "Could not find address of main", 0);

  if (main_addr != vaddr) 
    {
      tmpcntnr = mjr_create_function_container(context, vaddr, 0, "_start", 0, NULL);
      mjr_function_register(context, vaddr, tmpcntnr);
      
#if defined(__DEBUG_MJOLLNIR__)
      fprintf(stderr, " [D] Creating MAIN Func at " XFMT "\n", main_addr);  
#endif
      
      main_container = mjr_create_function_container(context, main_addr, 0, "main", 0, NULL);
      mjr_function_register(context, main_addr, main_container);
      
      mjr_container_add_link(context, tmpcntnr, main_container->id, 
			     MJR_LINK_FUNC_CALL, MJR_LINK_SCOPE_LOCAL, CONTAINER_LINK_OUT);
      mjr_container_add_link(context, main_container, tmpcntnr->id, 
			     MJR_LINK_FUNC_RET, MJR_LINK_SCOPE_LOCAL, CONTAINER_LINK_IN);
      mjr_link_block_call(context, vaddr, main_addr, vaddr + dis);
      
      /* Create symbols for main */
      /* Then we create the symbol for the bloc and returns */
      if (!sym || !sym->st_value)
	{
	  bsym = elfsh_create_symbol(main_addr, 0, STT_FUNC, 0, 0, 0);
	  elfsh_insert_symbol(context->obj->secthash[ELFSH_SECTION_SYMTAB], &bsym, "main");
	}
    }
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, main_addr);
}
