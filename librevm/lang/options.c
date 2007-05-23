/*
** opt.c for librevm in ERESI
**
** Implement the commands registration handlers
** 
** Started on  Fri Nov  2 15:17:02 2001 mayhem
**
** $Id: options.c,v 1.3 2007-05-23 13:50:39 may Exp $
**
*/
#include "revm.h"


/* Read the input file parameter */
int		vm_getoption(u_int index, u_int argc, char **argv)
{
  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  if (index + 1 >= argc)			
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Parameter not available", (-1));				
  world.curjob->curcmd->param[0] = argv[index + 1];
  world.curjob->curcmd->argc = 1;
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, (1));
}

/* Read the input file parameter */
int		vm_getinput(u_int index, u_int argc, char **argv)
{

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  if (index + 1 >= argc)			
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Parameter not available", (-1));				
  world.state.input = argv[index + 1];
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, (1));
}

/* Read the output file parameter */
int		vm_getoutput(u_int index, u_int argc, char **argv)
{

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  if (index + 1 >= argc)			
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Parameter not available", (-1));				
  world.state.output = argv[index + 1];
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, (1));
}

/* Activate a 2-non-regx-mandatory-parameters option */
int		vm_getoption2(u_int index, u_int argc, char **argv)
{

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  if (index + 2 >= argc)			
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Parameter not available", (-1));				
  world.curjob->curcmd->param[0] = argv[index + 1];
  world.curjob->curcmd->param[1] = argv[index + 2];
  world.curjob->curcmd->argc = 2;
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, (2));
}

/* Activate a 2-non-regx-mandatory-parameters option */
int		vm_getoption3(u_int index, u_int argc, char **argv)
{

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  if (index + 3 >= argc)			
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Parameter not available", (-1));				
  world.curjob->curcmd->param[0] = argv[index + 1];
  world.curjob->curcmd->param[1] = argv[index + 2];
  world.curjob->curcmd->param[2] = argv[index + 3];
    world.curjob->curcmd->argc = 3;
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, (3));
}

/* Activate a non-mandatory-regex-parameter option */
int		vm_getregxoption(u_int index, u_int argc, char **argv)
{

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  if (index + 1 < argc && argv[index + 1][0] != REVM_DASH)		
    {									
      if (regcomp(&world.curjob->curcmd->regx[0], argv[index + 1], 
		  REG_EXTENDED) != 0 || !strcmp(argv[index + 1], "*"))
	
	PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
			  "Parameter not available", (-1));
      world.curjob->curcmd->use_regx[0] = 1;
      PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, (1));
    }				
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}

/* Fetch parameters until we find NULL or something starting by '-' */
int		vm_getvarparams(u_int index, u_int argc, char **argv)
{
  u_int		idx;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);
  for (world.curjob->curcmd->argc = idx = 0; 
       idx < 254 && index + idx + 1 < argc;
       idx++)
    {
      world.curjob->curcmd->param[idx] = argv[index + idx + 1];
      world.curjob->curcmd->argc++;
    }

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, idx);
}

/* Format the input of a loop */
int		vm_getforparams(u_int index, u_int argc, char **argv)
{
  char		*p;
  char		flag;
  char		*last;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  /* Sanity checks */
  if (((argc - index != 6) && (argc - index != 4)) ||
      strcmp(argv[index + 2], "of"))
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, "Wrong loop format", -1);

  p     = argv[index + 4];
  last  = argv[index + 5];
  flag  = 0;
  
  /* Intermediate checks */
  if ((argc - index) == 6)
    {
      flag = (!strcmp("matching", p) ? 1 : !strcmp("until", p) ? 2 : 0);
      if (!flag)
	PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, "Wrong loop format", -1);

      /* Compute the regex if necessary */
      if (flag == 1)
	{
	  if (regcomp(&world.curjob->curcmd->regx[0], last, REG_EXTENDED) != 0 ||
	      !strcmp(last, "*"))    
	    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
			 "Invalid requested regex", (-1));
	  world.curjob->curcmd->use_regx[0] = 1;
	}
      }

  /* Set the current and maximum indexes to uninitialized */
  world.curjob->curcmd->curidx = REVM_IDX_UNINIT;
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 
		vm_getvarparams(index, argc, argv));
}


/* Format the input of a match */
int		vm_getmatchparams(u_int index, u_int argc, char **argv)
{
  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);
  if (argc - index != 3)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, "Wrong match format", -1);
  if (strcmp(argv[index + 2], "with"))
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, "Wrong match format", -1);
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 
		vm_getoption2(index, argc, argv));
}




/* Add an entry to the requested dump list */
static int      vm_add2list(char outtype, u_int index, int argc, char **argv)
{
  char		*off;							
  int		idx = index;
  revmlist_t	*cur;
  char		*used;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  while (idx < index + 2)
    {
      cur  = world.curjob->curcmd->disasm   + idx - index;
      used = world.curjob->curcmd->use_regx + idx - index;
	
      if (argv[idx + 1] == NULL)								
	{
	  *used = 0;
	  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 
			     (idx == index ? -1 : 1));
	}

      cur->rname = argv[idx + 1];
      cur->otype = outtype;	
      cur->size = 0;					
      cur->off = 0;
      off = strchr(argv[idx + 1], '%');						  

      if (off)     			
	{									
	  cur->size = atoi(off + 1);					
	  *off = 0;								
	}									
      off = strchr(argv[idx + 1], ':');						
      if (off)		       
	{									
	  cur->off = atoi(off + 1);
	  *off = 0;								
	}									
      
      if (regcomp(&cur->name, argv[idx + 1], REG_EXTENDED | REG_ICASE) != 0 ||
	  *cur->rname == '*')
	PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
			  "Regular computation failed", (-1));
      *used = 1;
      idx++;
    }
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, (2));
}

/* Add an DISASM typed entry */
int		vm_getdisasm(u_int index, u_int argc, char **argv)
{
  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 
		     vm_add2list(REVM_VIEW_DISASM, index, argc, argv));
}

/* Add an HEXA typed entry */
int		vm_gethexa(u_int index, u_int argc, char **argv)
{

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__,
		     vm_add2list(REVM_VIEW_HEX, index, argc, argv));
}
