/*
** 
** skelet.c in 
** 
** Author  : <sk at devhell dot org>
** Started : Thu May 29 21:56:30 2003
** Updated : Wed Jun 18 19:26:23 2003
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

void	elfsh_init() {
  puts(" [*] ELFsh modflow("__DATE__"/"__TIME__") fini -OK-\n");
  puts("     Added commands:\n");
  puts("\tgraph   <file>    : dump graphviz graph to file"
       "\tinspect <vaddr>   : inspect block at vaddr\n"
       "\tflow    <symbol>  : build a .control section\n");
  vm_addcmd(CMD_GRAPH, cmd_graph, vm_getoption, 1);
  vm_addcmd(ELFSH_CMD_FLOW, modflow_cmd, vm_getoption, 1);
  vm_addcmd(ELFSH_CMD_INSPECT, inspect_cmd, vm_getoption, 1);
}


void	elfsh_fini() {
  puts(" [*] ELFsh modflow init -OK-\n");
  vm_delcmd(ELFSH_CMD_FLOW);
  vm_delcmd(CMD_GRAPH);
  vm_delcmd(ELFSH_CMD_INSPECT);
}


/*******************************************************************
 * main
 */

hash_t		block_hash;

int	modflow_cmd(void) 
{
  char		*buffer;
  asm_instr	instr;
  elfshobj_t	*hdl;
  Elf32_Sym	*sym;
  u_int		disassembled;
  u_int		ilen;
  u_int		max_len;
  u_int		foff;
  u_int		e_point;
  u_int		main_addr;
  u_int		vaddr;
  struct s_list	*args;
  struct s_iblock	*binary_blk;
  struct s_function	*binary_functions;

  /* char		*str; */
  

  /*
    parse arguments
    load binary and resolve symbol 
  */
  
  hdl = world.current;
  
  if (!(sym = elfsh_get_metasym_by_name(hdl, world.args.param[0])))
    ELFSH_SETERROR("[elfsh] Cannont find symbol\n", -1);
  
  
  foff = elfsh_get_symbol_foffset(hdl, sym);
  max_len = elfsh_get_symbol_size(sym);
  vaddr = sym->st_value;
  
  
  printf("[MODFLOW] loading code... vaddr = %08x foffset = %i len = %i\n", vaddr, foff, max_len);
  buffer = malloc(max_len);
  elfsh_raw_read(hdl, foff, buffer, max_len);

  hash_init(&block_hash, max_len);
  binary_blk = 0;
  binary_functions = 0;
  args = 0;
  
  /*
  ** Fetch main from entry point
  */
  
  e_point = elfsh_get_entrypoint(elfsh_get_hdr(hdl));
  printf(" [*] Entry point: %08x\n", e_point);
  if (vaddr == e_point) 
    {
      main_addr = trace_start(hdl, buffer, max_len, e_point,
			      &binary_blk, &binary_functions);
      printf(" [*] main located at %8x\n", main_addr);
    }
  
  printf(" [*] starting disassembly\n");
  
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
	  trace_control(hdl, &instr, vaddr + disassembled, &binary_blk);
	} 
      else
	ilen = 1;
    }
  
  /*
    cleanup & ouput
    save file.
  */
  
  puts("[MODFLOW] done\n");
    
  store_blocks(hdl, binary_blk);
  display_blocks(hdl, binary_blk, 1);

 
  /* 
  puts(" - done\n[blocks path recursion]");

 
  trace_functions(hdl, &binary_functions, binary_blk);
 
  puts("-done\n[Functions]");
  
  display_functions(hdl, binary_functions);
  */

  
  free(buffer);
  
  return (0);
}



