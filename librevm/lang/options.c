/**
 * @file opt.c
 * @ingroup librevm_lang
 * @brief Implement the commands registration handlers.
 * 
 * Started on  Fri Nov  2 15:17:02 2001 jfv
 *
 * $Id: options.c,v 1.10 2007-08-07 07:13:27 may Exp $
 *
 */
#include "revm.h"


/** 
 * Read the input file parameter 
 */
int		revm_getoption(unsigned int index, unsigned int argc, char **argv)
{
  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  if (index + 1 >= argc)			
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Parameter not available", (-1));				
  world.curjob->curcmd->param[0] = argv[index + 1];
  world.curjob->curcmd->argc = 1;
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, (1));
}

/** 
 * Read the input file parameter 
*/
int		revm_getinput(unsigned int index, unsigned int argc, char **argv)
{

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  if (index + 1 >= argc)			
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Parameter not available", (-1));				
  world.state.input = argv[index + 1];
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, (1));
}

/** 
 * Read the output file parameter 
*/
int		revm_getoutput(unsigned int index, unsigned int argc, char **argv)
{

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  if (index + 1 >= argc)			
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Parameter not available", (-1));				
  world.state.output = argv[index + 1];
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, (1));
}

/** 
 * Activate a 2-non-regx-mandatory-parameters option 
*/
int		revm_getoption2(unsigned int index, unsigned int argc, char **argv)
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

/** 
 * Activate a 2-non-regx-mandatory-parameters option 
*/
int		revm_getoption3(unsigned int index, unsigned int argc, char **argv)
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

/**
 * Activate a non-mandatory-regex-parameter option 
*/
int		revm_getregxoption(unsigned int index, unsigned int argc, char **argv)
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

/** 
 * Fetch parameters until we find NULL or something starting by '-' 
 */
int		revm_getvarparams(unsigned int index, unsigned int argc, char **argv)
{
  unsigned int		idx;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);
  for (world.curjob->curcmd->argc = idx = 0; 
       idx < 254 && index + idx + 1 < argc;
       idx++)
    {
      /* Add the '-' check which was not here and create bugs with cmdline */
      if ((world.state.revm_mode == REVM_STATE_CMDLINE 
	   || world.state.revm_mode == REVM_STATE_TRACER)
	  && argv[index + idx + 1] && argv[index + idx + 1][0] == '-')
	break;

      world.curjob->curcmd->param[idx] = argv[index + idx + 1];
      world.curjob->curcmd->argc++;
    }

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, idx);
}

/** 
 * Format the input of a loop 
*/
int		revm_getforparams(unsigned int index, unsigned int argc, char **argv)
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
  world.curjob->curcmd->listidx = REVM_IDX_UNINIT;
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 
		revm_getvarparams(index, argc, argv));
}


/**
 * Format the input of a case command
 */
int		revm_getcaseparams(unsigned int index, unsigned int argc, char **argv)
{
  unsigned int		idx;
  char		tokens[3][BUFSIZ];
  int		curtok;
  int		curidx;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);
  curidx = curtok = 0;
  for (world.curjob->curcmd->argc = idx = 0; 
       idx < 254 && index + idx + 1 < argc;
       idx++)
    {
      if (!strcmp(argv[index + idx + 1], REVM_CASE_ARROW))
	{
	  if (curtok != 0)
	    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
			 "Invalid syntax for case command", (-1));
	  curtok++;
	  curidx = 0;
	  continue;
	}
      else if (!strcmp(argv[index + idx + 1], REVM_CASE_QMARK))
	{
	  if (curtok != 1)
	    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
			 "Invalid syntax for case command", (-1));
	  curtok++;
	  curidx = 0;
	  continue;
	}

      curidx += snprintf(&tokens[curtok][curidx], BUFSIZ - curidx, "%s", argv[index + idx + 1]);
    }

  for (idx = 0; idx < curtok + 1; idx++)
    world.curjob->curcmd->param[idx] = strdup(tokens[idx]);
  world.curjob->curcmd->argc = curtok + 1;

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, idx);
}



/** 
 * Format the input of a match/rewrite command
 */
int		revm_getmatchparams(unsigned int index, unsigned int argc, char **argv)
{
  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);
  if (argc - index != 3)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, "Wrong match format", -1);
  if (strcmp(argv[index + 2], CMD_PARAM_INTO))
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, "Wrong match format", -1);

  world.curjob->curcmd->listidx = REVM_IDX_UNINIT;
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 
		revm_getoption2(index, argc, argv));
}




/** 
 * Add an entry to the requested dump list 
*/
static int      revm_add2list(char outtype, unsigned int index, int argc, char **argv)
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

/** 
 * Add an DISASM typed entry 
*/
int		revm_getdisasm(unsigned int index, unsigned int argc, char **argv)
{
  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 
		     revm_add2list(REVM_VIEW_DISASM, index, argc, argv));
}

/** 
 * Add an HEXA typed entry 
*/
int		revm_gethexa(unsigned int index, unsigned int argc, char **argv)
{

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__,
		     revm_add2list(REVM_VIEW_HEX, index, argc, argv));
}
