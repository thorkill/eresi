/*
** (C) 2006 Devhell Labs / Asgard Labs : sk, mayhem, thorolf
** 
** Implement low-level functions of the libmjollnir library
**
** $Id: core.c,v 1.25 2007-03-14 18:37:57 strauss Exp $
*/

#include "libmjollnir.h"


/**
 * This function will find calls including calls trought a pointer 
 * @param sess Mjollnir session
 * @param section_name The name of the section we want to analyse
 */
int		  mjr_analyse_section(mjrsession_t *sess, char *section_name) 
{
  
  elfshsect_t    	*sct;
  asm_instr       instr;
  unsigned char  	*ptr;
  unsigned long   curr, len;
  u_int           vaddr, ilen;
  elfsh_Addr      e_point;
  elfsh_Addr	  	main_addr;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);
  
  /* load section */
#if __DEBUG_MJOLLNIR__
  fprintf(D_DESC, "[__DEBUG_MJOLLNIR__] mjrFindCalls: loading %s\n",section_name);
#endif
  
  sct = elfsh_get_section_by_name(sess->cur->obj, section_name, NULL, NULL, NULL);
  if (!sct)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, "Section not found", -1);
      
#if __DEBUG_CALLS__
  fprintf(D_DESC, "[__DEBUG_CALLS__] mjrFindCalls: %x %d %d\n",
	  sct->shdr->sh_addr, sct->shdr->sh_size, sct->shdr->sh_offset);
#endif
  
  /* Setup initial conditions */
  ptr      = elfsh_get_raw(sct);
  len      = sct->shdr->sh_size;
  curr     = 0;
  vaddr    = sct->shdr->sh_addr;
  e_point  = elfsh_get_entrypoint(elfsh_get_hdr(sess->cur->obj));

  if (sct->shdr->sh_addr == e_point)
  {
    printf(" [*] Entry point: %lx\n", (unsigned long) e_point);
    main_addr = mjr_trace_start(sess->cur, ptr, sct->shdr->sh_size, e_point);
    printf(" [*] main located at %lx\n", (unsigned long) main_addr);
  }

  /* Read all instructions of the section */
  while (curr < len) 
  {
    ilen = asm_read_instr(&instr, ptr + curr, len - curr, &sess->cur->proc);
    if (ilen > 0) 
		{
		  mjr_history_shift(sess->cur, instr, vaddr + curr);
		  mjr_trace_control(sess->cur, sess->cur->obj, &instr, vaddr + curr);
		} 
		curr += ilen;
  }
  
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}

/**
 * Main analysis function 
 * @param sess Mjollnir session strucutre
 * @param flags <FIXME:NotImplemented>
 */
int		mjr_analyse(mjrsession_t *sess, int flags) 
{
  char				*shtName;
  elfsh_Shdr	*shtlist, *shdr;
  elfsh_Sym		*sym;
  int					num_sht, idx_sht;
  char				c;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);
  
  /* Preliminary checks */
  if ((NULL == sess) || (NULL == sess->cur))
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Invalid NULL parameters", -1);
  
  /* Get section table */
  shtlist = elfsh_get_sht(sess->cur->obj, &num_sht);
  if (!shtlist)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Failed to get SHT", -1);
  
  /* Make sure we do what the user desires */
  /* XXX: should go in elfsh analyse command and use vm_output */
  // Just to make sure we remove previously done analysis if user call
  // analyse a second time
  if (sess->cur->analysed)
  {
    printf(" [*] %s section present ! \n"
				     "     Analysis will remove currently stored information. "
				     "continue ? [N/y]", ELFSH_SECTION_NAME_EDFMT_BLOCKS);

    c = getchar();
    puts("");

    if (c != 'y')
			PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
									  "Flow analysis aborted", 0);

    elfsh_remove_section(sess->cur->obj, ELFSH_SECTION_NAME_EDFMT_BLOCKS);
  }

#if __DEBUG_MJOLLNIR__
  fprintf(D_DESC,"[__DEBUG__] mjr_analize: Found %d sections.\n",num_sht);
#endif
  
  /* Analyse all executable sections */
  for (idx_sht = 0; idx_sht < num_sht; idx_sht++) 
  {
    shdr    = (shtlist + idx_sht);
    sym     = elfsh_get_sym_from_shtentry(sess->cur->obj, shdr);
    shtName = elfsh_get_symbol_name(sess->cur->obj, sym);
      
    if (!elfsh_get_section_execflag(shdr) ||
			  !elfsh_get_section_allocflag(shdr))
			continue;
      
#if __DEBUG_MJOLLNIR__
    fprintf(D_DESC, "[__DEBUG__] mjr_analize: Executable section name=(%14s) "
							      "index=(%02i)\n", shtName, idx_sht);
#endif
    mjr_analyse_section(sess, shtName);
  }

  /* Store analyzed blocks in file */
  if (mjr_blocks_store(sess->cur) < 0)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
		      "Unable to store blocks in file", -1);

  /* Store analyzed functions in file */
  if (mjr_functions_store(sess->cur) < 0)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
		      "Unable to store functions in file", -1);
  
  /* Set the flag and return */
  sess->cur->analysed = 1;
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}

