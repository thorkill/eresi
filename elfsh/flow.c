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
	 " flowjack <oldsymbol> <newsymbol>         \n"
	 "     All relative call to oldsymbol       \n"
	 "     are hijacked to newsymbol            \n"
	 "                                          \n"
	 " addgoto <vaddr> <func_addr>              \n"
	 "     'redirect' for functions called      \n"
	 "     trough a pointer                     \n"
	 "                                          \n");
}


/* Manually add function pointer information */
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

/* Test function ... start to get old and unused */
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


/* Print the control information */
int		cmd_control()
{
  elfshsect_t	*sect;

  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);  
  sect = elfsh_get_section_by_name(world.curjob->current, 
				   ELFSH_SECTION_NAME_CONTROL,
				   0, 0, 0);
  if (!sect || !sect->altdata)
    ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__,
                      "No control flow section found", -1);
  mjr_display_blocks(world.curjob->current, sect->altdata, 1);
  vm_output("\n [*] Control flow information dumped \n\n");
  ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);  
}


/* Change the graph verbose level */
int	cmd_setgvl(void) 
{
  world.state.vm_gvl = atoi(world.curjob->curcmd->param[0]);
  return (0);
}
