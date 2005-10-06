/*
** 
** skelet.c in 
** 
** Author  : <sk at devhell dot org>
** Started : Thu May 29 21:56:30 2003
** Updated : Fri Nov 28 11:48:09 2003
*/
#include "modflow.h"

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
	 "                                          \n"
	 " flowjack <oldsymbol> <newsymbol>         \n"
	 "     All relative call to oldsymbol       \n"
	 "     are hijacked to newsymbol            \n"
	 "                                          \n");
}

void	elfsh_init() 
{
  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);

  vm_output(" [*] ELFsh modflow("__DATE__"/"__TIME__") fini -OK-\n");
  vm_output("     Added commands:\n");
  vm_output("\tflow                                               : process control flow analysis\n"
       "\tgraph    <file> <symbol/address> <address|+size>   : dump graphviz graph to file\n"
       "\tinspect  <symbol/address>                          : inspect block at vaddr\n"
       "\tflowjack <sym1> <sym2>                             : hijack xref from sym1 to sym2\n");

  vm_addcmd(ELFSH_CMD_GRAPH, cmd_graph, vm_getoption3, 1, "FIXME");
  vm_addcmd(ELFSH_CMD_FLOW, cmd_flow, 0, 1, "FIXME");
  vm_addcmd(ELFSH_CMD_INSPECT, inspect_cmd, vm_getoption, 1, "FIXME");
  vm_addcmd("flowtest", cmd_testflow, 0, 1, "FIXME");
  vm_addcmd(ELFSH_CMD_FLOWJACK, cmd_flowjack, vm_getoption2, 1, "FIXME");
  //vm_addcmd(ELFSH_CMD_FLOWLOAD, cmd_flowload, 0, 1, "FIXME");
  //vm_addcmd(ELFSH_CMD_FLOWSAVE, cmd_flowsave, 0 ,1, "FIXME");

  ELFSH_PROFILE_OUT(__FILE__, __FUNCTION__, __LINE__);
}


void	elfsh_fini() 
{
  vm_output(" [*] ELFsh modflow init -OK-\n");
  vm_delcmd(ELFSH_CMD_FLOW);
  vm_delcmd(ELFSH_CMD_FLOWJACK);
  vm_delcmd(ELFSH_CMD_GRAPH);
  vm_delcmd(ELFSH_CMD_INSPECT);
  vm_delcmd("testflow");
}


/*******************************************************************
 * main
 */

int	cmd_flowsave(void)
{
  elfshsect_t	*sect;

  sect = elfsh_get_section_by_name(world.curjob->current, ".control", 0, 0, 0);
  if (!sect)
    {
      puts(" * no \".control\" section found. Abording\n");
      return (-1);
    }
  else
    {
      if (sect->altdata)
	store_blocks(world.curjob->current, (struct s_iblock *) sect->altdata, 1);
      else
	puts(" * no flow information in \".control\" section. aborting\n");
    }
  return (0);
}

int	cmd_flowload(void)
{
  struct s_iblock	*blk;
  
  blk = 0;
  load_blocks(world.curjob->current, &blk);
  return (0);
}

int	cmd_testflow(void)
{
  struct s_iblock	*blk_list;
  struct s_iblock	*blk;
  
  blk_list = 0;
  
  
  blk = block_create(0, 10);
  blk->altype = CALLER_JUMP;
  blk->contig = 10;
  blk->altern = 20;
  block_add_list(&blk_list, blk);
  
  blk = block_create(10, 10);
  blk->altype = CALLER_JUMP;
  blk->altern = 40;
  block_add_caller(blk, 9, CALLER_CONT);
  block_add_list(&blk_list, blk);
  
  blk = block_create(20, 10);
  blk->altype = CALLER_JUMP;
  blk->contig = 30;
  blk->altern = 40;
  block_add_caller(blk, 9, CALLER_JUMP);
  block_add_list(&blk_list, blk);

  blk = block_create(30, 10);
  blk->contig = 40;
  blk->altype = CALLER_CALL;
  blk->altern = 300;
  block_add_caller(blk, 29, CALLER_CONT);
  block_add_list(&blk_list, blk);

  blk = block_create(40, 10);
  blk->altype = CALLER_RET;
  block_add_caller(blk, 19, CALLER_JUMP);
  block_add_caller(blk, 39, CALLER_CONT);
  block_add_list(&blk_list, blk);
  
  blk = block_create(300, 10);
  blk->altype = CALLER_UNKN;
  block_add_caller(blk, 39, CALLER_CALL);
  block_add_list(&blk_list, blk);
  
  display_blocks(world.curjob->current, blk_list, 1);  
  printf(" * storing ..");
  store_blocks(world.curjob->current, blk_list, 1);
  puts("done.");
  
  blk_list = 0;
  load_blocks(world.curjob->current, &blk_list);
  display_blocks(world.curjob->current, blk_list, 1);
  
  printf(" - fetching block starting at %x\n", 299);
  if ((blk = block_get_by_vaddr(blk_list, 299, 0)))
    puts("* found");
  else
    puts("* unfound");

  printf(" - fetching block containing %x\n", 299);
  if ((blk = block_get_by_vaddr(blk_list, 299, 1)))
    puts("* found");
  else
    puts("* unfound");

  printf(" - fetching block starting at %x\n", 300);
  if ((blk = block_get_by_vaddr(blk_list, 300, 0)))
    puts("* found");
  else
    puts("* unfound");

  printf(" - fetching block containing %x\n", 300);
  if ((blk = block_get_by_vaddr(blk_list, 300, 1)))
    puts("* found");
  else
    puts("* unfound");

  printf(" - fetching block starting at %x\n", 301);
  if ((blk = block_get_by_vaddr(blk_list, 301, 0)))
    puts("* found");
  else
    puts("* unfound");

  printf(" - fetching block containing %x\n", 301);
  if ((blk = block_get_by_vaddr(blk_list, 301, 1)))
    puts("* found");
  else
    puts("* unfound");
  
  btree_debug(blk_list->btree, "testflow.gvz", 0);
  free_blocks(blk_list);
  
  return (0);
}


hash_t		block_hash;



int			cmd_flow(void) 
{

  char                  *buffer;
  asm_instr             instr;
  elfshsect_t           *sect;
  // elfshsect_t		*csect;
  elfsh_Shdr		*shtlist;
  elfsh_Shdr            *shdr;
  elfsh_Sym             *sym;
  u_int                 idx_sht;
  u_int                 num_sht;
  u_int                 disassembled;
  u_int                 ilen;
  u_int                 max_len;
  u_int                 foff;
  u_int                 e_point;
  u_int                 main_addr;
  u_int                 vaddr;
  char                  *str;
  struct s_iblock       *binary_blks;
  /* char		*str; */
  
  /*
    parse arguments
    load binary and resolve symbol 
  */
  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);

  if (!(sect = elfsh_get_section_by_name(world.curjob->current, ".control", 0, 0, 0)))
    {
      binary_blks = 0;
    }
  else
    {
      puts(" [*] \".control\" section present. flow analysis may corrupt\n"
	   "     current stored information\nContinue?[N/y]");
      ilen = getchar();
      if (ilen != 'y')
	ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, 
			  "Flow analysis Aborted", -1);

      if (sect->altdata)
	binary_blks = (struct s_iblock *) sect->altdata;
      else
	load_blocks(world.curjob->current, (struct s_iblock **) &binary_blks);
    }
  
  if (!(shtlist = elfsh_get_sht(world.curjob->current, &num_sht)))
    ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__,
		      "[MODFLOW] cannot get sectionlist", -1);

  printf(" * %i sections\n", num_sht);
  
  for (idx_sht = 0; idx_sht < num_sht; idx_sht++) 
  // for (sect = world.curjob->current)
    {
      shdr = (shtlist + idx_sht);
      sym = elfsh_get_sym_from_shtentry(world.curjob->current, shdr);
      str = elfsh_get_symbol_name(world.curjob->current, sym);
      printf(" * section name=(%14s) index=(%02i) ", str, idx_sht);      
      
      if (!(elfsh_get_section_allocflag(shdr)))
        {
          printf("%22s", "not allocatable ->skipping ...\n");
          continue;
        }
      else
        printf("%22s", "allocatable  ");
      
      if (!(elfsh_get_section_execflag(shdr)))
        {
          printf("%s", "not executable -> skipping ...\n");
          continue;
        }
      printf("%s", "executable -> analysing\n");
      
      max_len = elfsh_get_symbol_size(sym);
      vaddr = sym->st_value;
      foff = elfsh_get_foffset_from_vaddr(world.curjob->current, vaddr);

      printf("[MODFLOW] loading code... vaddr = %08x foffset = %i len = %i\n", vaddr, foff, max_len);
      buffer = malloc(max_len);
      elfsh_raw_read(world.curjob->current, foff, buffer, max_len);

      hash_init(&block_hash, max_len);
      
      /*
      ** Fetch main from entry point
      */
  
      e_point = elfsh_get_entrypoint(elfsh_get_hdr(world.curjob->current));
      printf(" [*] Entry point: %08x\n", e_point);
      if (vaddr == e_point) 
	{
	  main_addr = trace_start(world.curjob->current, buffer, max_len, e_point,
				  &binary_blks, 0);
	  printf(" [*] main located at %8x\n", main_addr);
	}
  
      vm_output(" [*] starting disassembly");
  
      /*
	main loop
	for each instruction disassembled, pass it to
	the trace_control function which may build dynamically
	a linked list of blocks, with additionnal relationship
	informations in a linked list contained in the block
      */
  
      for (disassembled = 0; disassembled < max_len; disassembled += ilen) 
	{
	  if ((ilen = asm_read_instr(&instr, buffer + disassembled, 
				     max_len - disassembled, &world.proc))) 
	    {
	  
	      /*
		str = asm_display_instr_att(&instr, vaddr + disassembled);
		printf("%8x:\t%s\n", vaddr + disassembled, str);
	      */
	      trace_control(world.curjob->current, &instr, vaddr + disassembled, 
			    &binary_blks);
	    } 
	  else
	    ilen = 1;
	}
  
      /*
	cleanup & ouput
	save file.
      */
    }

  vm_output("[MODFLOW] done");
  
  //btree_debug(binary_blks->btree, "flow.gvz", 0);

  store_blocks(world.curjob->current, binary_blks, 1);
  // display_blocks(world.curjob->current, binary_blks, 1);
  // free_blocks(binary_blks);
  /* 
     puts(" - done\n[blocks path recursion]");

 
     trace_functions(world.curjob->current, &binary_functions, binary_blk);
 
     puts("-done\n[Functions]");
  
     display_functions(world.curjob->current, binary_functions);
  */

  
  // XFREE(buffer);
  
  return (0);
}



