/*
** opt.c
** 
** Started on  Fri Nov  2 15:17:02 2001 mayhem
** Last update Thu May 22 17:25:54 2003 mayhem
*/
#include "elfsh.h"


/* Precise a general weak bounded regex for all options */
int		vm_getglregx(u_int index, u_int argc, char **argv)
{
  if (index + 1 < argc && argv[index + 1][0] != ELFSH_MINUS)		
    {
      if (regcomp(&world.state.vm_regx, argv[index + 1], REG_EXTENDED) < 0)	
	return (-1);							
      world.state.vm_use_regx = 1;
      world.state.vm_sregx = argv[index + 1];
      if (!world.state.vm_quiet)
	printf("\n [*] Changed global regex to %s \n", argv[index + 1]);	
      return (1);
    }				
  return (-1);
}

/* Read the input file parameter */
int		vm_getoption(u_int index, u_int argc, char **argv)
{
  if (index + 1 >= argc)			
    return (-1);				
  world.args.param[0] = argv[index + 1];
  return (1);
}

/* Read the input file parameter */
int		vm_getinput(u_int index, u_int argc, char **argv)
{
  if (index + 1 >= argc)			
    return (-1);				
  world.state.input = argv[index + 1];
  return (1);
}

/* Read the output file parameter */
int		vm_getoutput(u_int index, u_int argc, char **argv)
{
  if (index + 1 >= argc)			
    return (-1);				
  world.state.output = argv[index + 1];
  return (1);
}

/* Read the sorting parameter */
int		vm_getsort(u_int index, u_int argc, char **argv)
{
  char		*str;

  if (index + 1 >= argc)			
    return (-1);
  if (argv[index + 1][0] != ELFSH_SORT_BY_ADDR && 
      argv[index + 1][0] != ELFSH_SORT_BY_SIZE)
    return (-1);
  str = (argv[index + 1][0] == ELFSH_SORT_BY_ADDR ? "ADDR" : "SIZE");
  if (!world.state.vm_quiet)
    printf(" [*] Switched to %s sorting\n\n", str);
  world.state.sort = argv[index + 1];
  return (1);
}

/* Activate a 2-non-regx-mandatory-parameters option */
int		vm_getoption2(u_int index, u_int argc, char **argv)
{
  if (index + 2 >= argc)			
    return (-1);				
  world.args.param[0] = argv[index + 1];
  world.args.param[1] = argv[index + 2];
  return (2);
}

/* Activate a 2-non-regx-mandatory-parameters option */
int		vm_getoption3(u_int index, u_int argc, char **argv)
{
  if (index + 3 >= argc)			
    return (-1);				
  world.args.param[0] = argv[index + 1];
  world.args.param[1] = argv[index + 2];
  world.args.param[2] = argv[index + 3];
  return (3);
}

/* Activate a non-mandatory-regex-parameter option */
int		vm_getregxoption(u_int index, u_int argc, char **argv)
{
  if (index + 1 < argc && argv[index + 1][0] != ELFSH_MINUS)		
    {									
      if (regcomp(&world.args.regx, argv[index + 1], REG_EXTENDED) < 0)		
	return (-1);						
      world.args.use_regx = 1;
      return (1);
    }				
  return (0);
}

/* Fetch parameters until we find NULL or something starting by '-' */
int		vm_getvarparams(u_int index, u_int argc, char **argv)
{
  u_int		idx;

  for (idx = 0; 
       idx < 254 && index + idx + 1 < argc;
       idx++)
    world.args.param[idx] = argv[index + idx + 1];
  return (idx ? idx : -1);
}

/* Add an entry to the requested dump list */
static int      vm_add2list(char outtype, u_int index, int argc, char **argv)
{
  char		*off;							
  
  if (argv[index + 1] == NULL)								
    return (-1);
  world.args.disasm.rname = argv[index + 1];							
  world.args.disasm.otype = outtype;							
  off = strchr(argv[index + 1], '%');						  
  if (off)     			
    {									
      world.args.disasm.size = atoi(off + 1);					
      *off = 0;								
    }									
  off = strchr(argv[index + 1], ':');						
  if (off)		       
    {									
      world.args.disasm.off = atoi(off + 1);						
      *off = 0;								
    }									
  if (regcomp(&world.args.disasm.name, argv[index + 1], REG_EXTENDED) < 0)		        
    return (-1);
  return (1);
}

/* Add an DISASM typed entry */
int		vm_getdisasm(u_int index, u_int argc, char **argv)
{
  return (vm_add2list(ELFSH_DISASM_VIEW, index, argc, argv));
}

/* Add an HEXA typed entry */
int		vm_gethexa(u_int index, u_int argc, char **argv)
{
  return (vm_add2list(ELFSH_HEXA_VIEW, index, argc, argv));
}

/* Parse the command line options and interactive mode commands */
int		vm_parseopt(int argc, char **argv)
{
  u_int		index;
  int		ret;
  elfshcmd_t	*actual;

  /* Main option reading loop : using the command hash table */
  bzero(&world.args, sizeof (elfshargv_t));
  for (index = 1; index < argc; index++)
    {
      actual = hash_get(&cmd_hash, argv[index] + 
			(world.state.vm_mode == ELFSH_VMSTATE_CMDLINE));

      /* We matched a command : call the reg() and exec() handlers */
      if (actual != NULL)
	{
	  if (actual->reg != NULL)
	    {
	      ret = actual->reg(index, argc, argv);
	      if (ret < 0)
		return (vm_doerror(vm_badparam, argv[index]));
	      else
		index += ret;
	    }
	  if (actual->exec != NULL)
	    {
	      if (!vm_implicit(actual, argv) && actual->exec() < 0)
		elfsh_error();
	    }
	}

      /* We did -NOT- matched a command, do error */
      else 
	vm_doerror(vm_unknown, argv[index]);
    }
  return (0);
}





