/*
** (C) 2006-2008 The ERESI team
**
** @file core.c
** @brief Implement low-level functions of the libmjollnir library
**
*/

#include "libmjollnir.h"

/**
 * @brief Core control flow analysis function at a given address
 * @param sess Mjollnir session
 * @param ptr Code buffer pointer to analyze
 * @param offset Offset into the code buffer to analyze
 * @param vaddr Entry point address for analysis
 * @param len Size of code to analyse
 * @param curdepth current depth of cfg being analyzed
 * @param maxdepth depth limit of analysis (== MJR_MAX_DEPTH for limitless)
 */
int		mjr_analyse_code(mjrsession_t *sess, unsigned char *ptr, 
				 unsigned int offset, eresi_Addr vaddr, int len, 
				 int curdepth, int maxdepth)
{
  asm_instr     instr;
  unsigned int  curr, ilen;
  eresi_Addr	dstaddr, retaddr;
  container_t	*curblock;
  mjrblock_t	*block;
  int		symoff;
  int		newoff;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  /* Check if we have reached the limit bound */
  if (curdepth == maxdepth)
    PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);

  //limit = (u_int) config_get_data(CONFIG_CFGDEPTH);
  // Please use this config variable when doing CFG recursive analysis

  dstaddr = retaddr = MJR_BLOCK_INVALID;

  /* Create a new block if current address is out of all existing ones */
  curblock = (container_t *) hash_get(&sess->cur->blkhash, _vaddr2str(vaddr));
  assert(curblock != NULL);
  block = (mjrblock_t *) curblock->data;
  
#if __DEBUG_MJOLLNIR__
  fprintf(D_DESC, "[D] bloc %x: seen %d\n",
          __FUNCTION__,
          ((mjrblock_t *) curblock->data)->vaddr,
          ((mjrblock_t *) curblock->data)->seen);
#endif

  /* Avoid loops */
  if (block->seen)
    PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
  block->seen = 1;

  /* Read all instructions of the section */
  for (curr = 0; curr + offset < len; curr += ilen)
    {
      ilen = asm_read_instr(&instr, ptr + offset + curr,
                            len - curr - offset, &sess->cur->proc);

#if __DEBUG_READ__
      fprintf(D_DESC,"[D] %s/%s,%d: ilen=%d\n", 
	      __FUNCTION__, __FILE__, __LINE__, ilen);
#endif
      
      if (ilen <= 0) 
	PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
                     "asm_read_instr returned <= 0 lenght", -1);

      block->size += ilen;	  
      mjr_history_shift(sess->cur, instr, vaddr + curr);

      mjr_trace_control(sess->cur, sess->cur->obj, &instr, 
			vaddr + curr, &dstaddr, &retaddr);

#if __DEBUG_READ__
      fprintf(stderr, " [D] curaddr analyzed: %08x (dstaddr = %08X, retaddr = %08X)\n",
	      vaddr + curr, dstaddr, retaddr);
#endif

      if (dstaddr != MJR_BLOCK_INVALID) 
	{
	  newoff = offset + (dstaddr - vaddr);
	  mjr_analyse_code(sess, ptr, newoff, dstaddr, 
			   len, curdepth + 1, maxdepth);
	}	      
      if (retaddr != MJR_BLOCK_INVALID) 
	{
	  newoff = offset + (retaddr - vaddr);
	  mjr_analyse_code(sess, ptr, newoff, retaddr, 
			   len, curdepth + 1, maxdepth);
	}
      
      /* If we have recursed, the current block is over */
      if (retaddr != MJR_BLOCK_INVALID || dstaddr != MJR_BLOCK_INVALID || 
	  (instr.type & ASM_TYPE_RETPROC))
	break;
    }
  
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}


/**
 * @brief Analyse control flow at a given address
 * @param sess Mjollnir session
 * @param addr Entry point address for analysis
 * @param maxdepth Maximum depth of the analysis in the CFG
 * @param flags <FIXME:NotImplemented>
 * @return Success (0) or Error (-1).
 */
int		mjr_analyse_addr(mjrsession_t *sess, eresi_Addr addr, int maxdepth, int flags)
{
  elfshsect_t   *parent;
  elfsh_SAddr   offset;
  container_t   *block;
  unsigned char *ptr;
  unsigned long len;


  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  if (!addr)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, "Failed to analyse control flow", -1);

  parent = elfsh_get_parent_section(sess->cur->obj, addr, &offset);
  if (!parent)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, "Unable to find parent section", -1);

  block = mjr_create_block_container(sess->cur, 0, addr, 0, 0);
  if (!block)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
                 "Unable to create parent container", -1);

  hash_add(&sess->cur->blkhash, _vaddr2str(addr), block);

  ptr      = elfsh_get_raw(parent);
  len      = parent->shdr->sh_size;

  if (mjr_analyse_code(sess, ptr, offset, addr, len, 0, maxdepth) < 0)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
                 "Error during code analysis", -1);

  if (mjr_analyse_finished(sess) < 0)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
                 "Error during storage of analysis info", -1);

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}


/**
 * @brief This function will find calls including calls trought a pointer
 * @param sess Mjollnir session
 * @param section_name The name of the section we want to analyse
 */
int             mjr_analyse_section(mjrsession_t *sess, char *section_name)
{
  container_t   *cntnr;
  unsigned char *ptr;
  unsigned long len;
  eresi_Addr    e_point, vaddr;
  eresi_Addr    main_addr;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  /* load section */
#if __DEBUG_MJOLLNIR__
  fprintf(D_DESC, "[__DEBUG_MJOLLNIR__] %s: loading %s\n",
          __FUNCTION__, section_name);
#endif

  /* We just started here so we can clear the current section in context */
  sess->cur->cursct = NULL;

  if (!section_name)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, "Section name empty", -1);

  sess->cur->cursct = elfsh_get_section_by_name(sess->cur->obj, section_name, NULL, NULL, NULL);
  if (!sess->cur->cursct)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, "Section not found", -1);

#if __DEBUG_CALLS__
  fprintf(D_DESC, "[__DEBUG_CALLS__] %s: vaddr:%x size:%d foff:%d\n",
          __FUNCTION__, sess->cur->cursct->shdr->sh_addr,
          sess->cur->cursct->shdr->sh_size,
          sess->cur->cursct->shdr->sh_offset);
#endif

  /* Setup initial conditions */
  ptr      = elfsh_get_raw(sess->cur->cursct);
  len      = sess->cur->cursct->shdr->sh_size;
  vaddr    = sess->cur->cursct->shdr->sh_addr;
  e_point  = elfsh_get_entrypoint(elfsh_get_hdr(sess->cur->obj));

  /* Create block pointing to this section */
  if (sess->cur->cursct->shdr->sh_addr == e_point)
    {
      printf(" [*] Entry point: " AFMT "\n", e_point);
      main_addr = mjr_trace_start(sess->cur, ptr, sess->cur->cursct->shdr->sh_size, e_point);
      printf(" [*] main located at " AFMT "\n", main_addr);
    }
  else
    {
      cntnr = mjr_create_function_container(sess->cur, vaddr, 0, _vaddr2str(vaddr), 0, NULL);
      sess->cur->curfunc = cntnr;
      mjr_function_register(sess->cur,vaddr, cntnr);
      main_addr = 0;
    }

  /* Read all instructions of the section */
  if (mjr_analyse_code(sess, ptr, 0, vaddr, len, 0, MJR_MAX_DEPTH) < 0)
     PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		  "Error during section entry code analysis", -1);

  /* Also analyse the main code -- it is generally not directly linked with the entry point */
  if (main_addr && mjr_analyse_code(sess, ptr, 0, main_addr, len, 0, MJR_MAX_DEPTH) < 0)
     PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		  "Error during main code analysis", -1);

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}


int             mjr_analyse_finished(mjrsession_t *sess)
{
  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  /* Store analyzed functions in file */
  if (mjr_flow_store(sess->cur, ASPECT_TYPE_FUNC) <= 0)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
                 "Unable to store functions in file", -1);

  /* Store analyzed blocks in file */
  if (mjr_flow_store(sess->cur, ASPECT_TYPE_BLOC) <= 0)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
                 "Unable to store blocks in file", -1);

  /* Set the flag and return */
  sess->cur->analysed = 1;
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}


/**
 * @brief Main analysis function
 * @param sess Mjollnir session strucutre
 * @param entry Address of starting point to analyze
 * @param maxdepth Maximum depth of the analysis in the CFG
 * @param flags <FIXME:NotImplemented>
 */
int             mjr_analyse(mjrsession_t *sess, eresi_Addr entry, int maxdepth, int flags)
{
  char          *shtName;
  elfsh_Shdr    *shtlist, *shdr;
  elfsh_Sym     *sym, *lastsym;
  elfshsect_t   *sct;
  container_t   *fcnt;
  int           num_sht, idx_sht;
  int           index, blocksize;
  eresi_Addr    addr;
  char          c;
  int           ret;

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

  /* Make sure we do what the user desires (remove previously stored analysis) */
  /* FIXME: should go in libstderesi/cmd_analyse command and use revm_output */
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

  /* In case we are provided an entry point */
  if (entry)
    {
      ret = mjr_analyse_addr(sess, entry, maxdepth, flags);
      if (ret < 0)
        PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
                     "Unable to analyse control flow by addr", -1);

      PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
    }

  /*
  ** First run, create blocks starting at section vaddr
  ** and in size of the section
  */
  for (idx_sht = 0; idx_sht < num_sht; idx_sht++)
    {
      shdr    = (shtlist + idx_sht);
      sym     = elfsh_get_sym_from_shtentry(sess->cur->obj, shdr);

      if (!elfsh_get_section_execflag(shdr) ||
          !elfsh_get_section_allocflag(shdr) ||
          !sym)
        continue;

      shtName = elfsh_get_symbol_name(sess->cur->obj, sym);
      sct = elfsh_get_section_by_name(sess->cur->obj, shtName, NULL, NULL, NULL);

      if (!sct)
        PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
                     "Can't get section", -1);

#if __DEBUG_MJOLLNIR__
      fprintf(D_DESC, "[__DEBUG__] %s: 1st run - Executable section name=(%14s) "
              "index=(%02i) vaddr=(%x) size=(%d)\n",
              __FUNCTION__, shtName, idx_sht, sct->shdr->sh_addr, sct->shdr->sh_size);
#endif

      fcnt = mjr_create_block_container(sess->cur, 0, sct->shdr->sh_addr, 0, 0);
      if (!fcnt)
        PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
                     "Can't create initial blocks", -1);

      hash_add(&sess->cur->blkhash, _vaddr2str(sct->shdr->sh_addr), fcnt);
    }


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

  /* Find all unseen blocks (may be dead code) */
  for (lastsym = NULL, index = 0, sym = sess->cur->obj->secthash[ELFSH_SECTION_SYMTAB]->altdata;
       index < sess->cur->obj->secthash[ELFSH_SECTION_SYMTAB]->shdr->sh_size / sizeof(elfsh_Sym);
       index++)
    {
      if (elfsh_get_symbol_type(sym + index) != STT_BLOCK)
        continue;
      if (!lastsym)
        {
          lastsym = sym;
          continue;
        }
      if (lastsym->st_value + lastsym->st_size != sym->st_value)
        {
          addr = lastsym->st_value + lastsym->st_size;
          blocksize = sym->st_value - addr;
          mjr_create_block_container(sess->cur, 0, addr, blocksize, 0);
          lastsym = sym;

          fprintf(stderr, " [D] Adding UNLINKED block at " XFMT " ! \n", addr);

        }
    }

  if (mjr_analyse_finished(sess) < 0)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
                 "Error during storage of analysis info", -1);

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}
