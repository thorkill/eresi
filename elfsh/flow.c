/*
** flow.c in elfsh
** 
** Author  : <sk at devhell dot org> for the modflow ELFsh module
**
** Merged in the ELFsh VM by the ELFsh crew.
**
*/
#include "elfsh.h"


/*****************************************************************************
 * High level block analysis.
 *
 * this function builds a linked list of function
 * which each contains a list linked of blocks
 *
 * for each block, 
 *
 * until a new block starting a function, current blocks
 * are supposed to be part of current function
 *
 ************************************************************************/

//hash_t		   block_hash;


void	elfsh_help() 
{
  printf(" flow                                     \n"
	 "  Launch control flow analysis.           \n"
	 "                                          \n"
	 " graph       <file> <min>  {+}<max>       \n"
	 "   Dump graph in file from starting       \n"
	 "    address min to ending address <max>   \n"
	 "   ie: graph debfunc.gvz func +200        \n"
	 "   ie: graph 0x805de40 0x805ff00          \n"
	 "                                          \n"
	 " inspect  <symbol/address>                \n"
	 "                                          \n"
	 "                                          \n"
	 " flowjack <oldsymbol> <newsymbol>         \n"
	 "     All relative call to oldsymbol       \n"
	 "     are hijacked to newsymbol            \n"
	 "                                          \n"
	 " addgoto <vaddr> <func_addr>              \n"
	 "     'redirect' for functions called      \n"
	 "     trough a pointer                     \n"
	 "                                          \n");
}

/* Flowsave command */
int		cmd_flowsave(void)
{
  elfshsect_t	*sect;

  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);
  sect = elfsh_get_section_by_name(world.curjob->current, 
				   ELFSH_SECTION_NAME_CONTROL,
				   0, 0, 0);
  if (!sect || !sect->altdata)
    ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__,
                      "No control flow section found", -1);

  mjr_store_blocks(world.curjob->current, (elfshiblock_t *) sect->altdata, 1);
  return (0);
}


int	cmd_flowload(void)
{
  elfshiblock_t	*blk = 0;

  mjr_load_blocks(world.curjob->current, &blk);
  return (0);
}


int	cmd_addgoto(void)
{
  if (!world.curjob->curcmd->param[0] || !world.curjob->curcmd->param[1])
    return (-1);
  
  printf(" [*] at %s goto %s\n",
	 world.curjob->curcmd->param[0],
	 world.curjob->curcmd->param[1]);
  
  hash_del(&goto_hash, world.curjob->curcmd->param[0]);
  hash_add(&goto_hash, world.curjob->curcmd->param[0],
	   world.curjob->curcmd->param[1]);
  return (0);
}

int		cmd_testflow(void)
{
  elfshiblock_t	*blk_list;
  elfshiblock_t	*blk;
  
  blk_list = 0;
  blk = mjr_block_create(0, 10);
  blk->altype = CALLER_JUMP;
  blk->contig = 10;
  blk->altern = 20;
  mjr_block_add_list(&blk_list, blk);
  
  blk = mjr_block_create(10, 10);
  blk->altype = CALLER_JUMP;
  blk->altern = 40;
  mjr_block_add_caller(blk, 9, CALLER_CONT);
  mjr_block_add_list(&blk_list, blk);
  
  blk = mjr_block_create(20, 10);
  blk->altype = CALLER_JUMP;
  blk->contig = 30;
  blk->altern = 40;
  mjr_block_add_caller(blk, 9, CALLER_JUMP);
  mjr_block_add_list(&blk_list, blk);

  blk = mjr_block_create(30, 10);
  blk->contig = 40;
  blk->altype = CALLER_CALL;
  blk->altern = 300;
  mjr_block_add_caller(blk, 29, CALLER_CONT);
  mjr_block_add_list(&blk_list, blk);

  blk = mjr_block_create(40, 10);
  blk->altype = CALLER_RET;
  mjr_block_add_caller(blk, 19, CALLER_JUMP);
  mjr_block_add_caller(blk, 39, CALLER_CONT);
  mjr_block_add_list(&blk_list, blk);
  
  blk = mjr_block_create(300, 10);
  blk->altype = CALLER_UNKN;
  mjr_block_add_caller(blk, 39, CALLER_CALL);
  mjr_block_add_list(&blk_list, blk);
  
  mjr_display_blocks(world.curjob->current, blk_list, 1);  
  printf(" * storing ..");
  mjr_store_blocks(world.curjob->current, blk_list, 1);
  puts("done.");
  
  blk_list = 0;
  mjr_load_blocks(world.curjob->current, &blk_list);
  mjr_display_blocks(world.curjob->current, blk_list, 1);
  
  printf(" - fetching block starting at %x\n", 299);
  if ((blk = mjr_block_get_by_vaddr(blk_list, 299, 0)))
    puts("* found");
  else
    puts("* unfound");

  printf(" - fetching block containing %x\n", 299);
  if ((blk = mjr_block_get_by_vaddr(blk_list, 299, 1)))
    puts("* found");
  else
    puts("* unfound");

  printf(" - fetching block starting at %x\n", 300);
  if ((blk = mjr_block_get_by_vaddr(blk_list, 300, 0)))
    puts("* found");
  else
    puts("* unfound");

  printf(" - fetching block containing %x\n", 300);
  if ((blk = mjr_block_get_by_vaddr(blk_list, 300, 1)))
    puts("* found");
  else
    puts("* unfound");

  printf(" - fetching block starting at %x\n", 301);
  if ((blk = mjr_block_get_by_vaddr(blk_list, 301, 0)))
    puts("* found");
  else
    puts("* unfound");

  printf(" - fetching block containing %x\n", 301);
  if ((blk = mjr_block_get_by_vaddr(blk_list, 301, 1)))
    puts("* found");
  else
    puts("* unfound");
  
  btree_debug(blk_list->btree, "testflow.gvz", 0);
  mjr_free_blocks(blk_list);
  
  return (0);
}

/* Flow analysis command : create the .control section */
int			cmd_flow() 
{

  char                  *buffer;
  asm_instr             instr;
  elfshsect_t           *sect;
  elfsh_Shdr		*shtlist;
  elfsh_Shdr            *shdr;
  elfsh_Sym             *sym;
  u_int                 idx_sht;
  int                   num_sht;
  u_int                 disassembled;
  u_int                 ilen;
  u_int                 max_len;
  u_int                 foff;
  u_int                 e_point;
  u_int                 main_addr;
  u_int                 vaddr;
  char                  *str;
  elfshiblock_t        *binary_blks;
  char			buflog[BUFSIZ];

  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);  

  /* Parse arguments, load binary and resolve symbol */
  sect = elfsh_get_section_by_name(world.curjob->current, 
				   ELFSH_SECTION_NAME_CONTROL, 
				   0, 0, 0);
  if (!sect)
    binary_blks = 0;
  else
    {
      snprintf(buflog, sizeof(buflog),
	       " [*] %s section present ! \n"
	       "     Analysis will remove currently stored information. "
	       "continue ? [N/y]",
	       ELFSH_SECTION_NAME_CONTROL);
      vm_output(buflog);
      ilen = getchar();
      vm_output("\n");
      if (ilen != 'y')
	ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, 
			  "Flow analysis aborted", -1);
      if (sect->altdata)
	binary_blks = (elfshiblock_t *) sect->altdata;
      else
	mjr_load_blocks(world.curjob->current, 
			(elfshiblock_t **) &binary_blks);
    }
  
  shtlist = elfsh_get_sht(world.curjob->current, &num_sht);
  if (!shtlist)
    ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__,
		      "[MODFLOW] cannot get sectionlist", -1);

  /* Parse SHT */
  for (idx_sht = 0; idx_sht < num_sht; idx_sht++) 
    {
      shdr = (shtlist + idx_sht);
      sym = elfsh_get_sym_from_shtentry(world.curjob->current, shdr);
      str = elfsh_get_symbol_name(world.curjob->current, sym);
      if (!elfsh_get_section_allocflag(shdr) ||
	  !elfsh_get_section_execflag(shdr))
	continue;
      max_len = elfsh_get_symbol_size(sym);
      vaddr   = sym->st_value;
      foff    = elfsh_get_foffset_from_vaddr(world.curjob->current, vaddr);
      buffer = elfsh_malloc(max_len);
      elfsh_raw_read(world.curjob->current, foff, buffer, max_len);

      //hash_init(&block_hash, max_len); (this hash doesnt exist anymore)
      
      /* Fetch main from entry point */
      e_point = elfsh_get_entrypoint(elfsh_get_hdr(world.curjob->current));
      if (vaddr == e_point) 
	{
	  printf(" [*] Entry point: %08x\n", e_point);
	  main_addr = mjr_trace_start(world.mjr_session.cur,
				      world.curjob->current, buffer, 
				      max_len, e_point, &binary_blks);
	  printf(" [*] main located at %08x\n", main_addr);
	}
  
      /*
      ** Main loop : For each instruction disassembled, pass it to
      ** the trace_control function which may build dynamically
      ** a linked list of blocks, with additionnal relationship
      ** informations in a linked list contained in the block
      */
      for (disassembled = 0; disassembled < max_len; disassembled += ilen) 
	if ((ilen = asm_read_instr(&instr, (u_char *) buffer + disassembled, 
				   max_len - disassembled, &world.proc))) 
	  {
	    mjr_history_shift(world.mjr_session.cur, instr);
	    mjr_trace_control(world.mjr_session.cur, world.curjob->current, 
			      &instr, vaddr + disassembled, &binary_blks);
	  } 
	else
	  ilen = 1;
    }

  vm_output(" [*] Flow analysis done \n\n");
  
  //btree_debug(binary_blks->btree, "flow.gvz", 0);

  if (mjr_store_blocks(world.curjob->current, binary_blks, 1) < 0)
    ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__,
		      "Unable to store blocks in file", -1);    

  /* 
     display_blocks(world.curjob->current, binary_blks, 1);
     free_blocks(binary_blks);
     puts(" - done\n[blocks path recursion]"); 
     trace_functions(world.curjob->current, &binary_functions, binary_blk);
     puts("-done\n[Functions]")
     display_functions(world.curjob->current, binary_functions);
     free(buffer);
  */

  ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);  
}


/* Change the graph verbose level */
int	cmd_setgvl(void) 
{
  world.state.vm_gvl = atoi(world.curjob->curcmd->param[0]);
  return (0);
}
