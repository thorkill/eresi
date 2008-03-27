/*
** @file alloc.c 
** @ingroup kernsh
** $Id: alloc.c,v 1.3 2007-11-29 15:33:39 may Exp $
**
*/
#include "kernsh.h"
#include "libkernsh.h"


/* Alloc contiguous kernel memory */
int		cmd_kalloc()
{
  char          *param;
  char		buff[BUFSIZ];
  unsigned long	addr;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  param = world.curjob->curcmd->param[0];
  if (param)
    {
      if (kernsh_alloc_contiguous(atoi(param), &addr))
	    {
	      revm_setvar_int("_", -1);
	      PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
			   "Cannot alloc contiguous memory", -1);
	    }
      memset(buff, '\0', sizeof(buff));
      snprintf(buff, sizeof(buff), 
	       "%s %s %s %s %s %s\n\n",
	       revm_colorfieldstr("ALLOCATE"),
	       revm_colornumber("%u", (unsigned int)atoi(param)),
	       revm_colorfieldstr("octet(s)"),
	       revm_colorfieldstr("OF CONTIGUOUS MEMORY"),
	       revm_colorstr("@"),
	       revm_coloraddress(XFMT, (eresi_Addr) addr));
      revm_output(buff);
      revm_setvar_long("_", addr);
    }
   
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}



/* Free contiguous kernel memory */
int		cmd_kfree()
{
  char          *param;
  char		buff[BUFSIZ];
  unsigned long	addr;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  param = world.curjob->curcmd->param[0];

  if (param)
    {
      addr = strtoul( param, NULL, 16 );
      if(kernsh_free_contiguous(addr))
	{
	  revm_setvar_int("_", -1);
	  PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		       "Cannot free contiguous memory", -1);
	}
      memset(buff, '\0', sizeof(buff));
      snprintf(buff, sizeof(buff), 
	       "%s %s %s %s\n\n",
	       revm_colorfieldstr("FREE"),
	       revm_colorfieldstr("CONTIGUOUS MEMORY"),
		   revm_colorstr("@"),
	       revm_coloraddress(XFMT, (eresi_Addr) addr));
      revm_output(buff);
      revm_setvar_int("_", 0);
    }

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}



/* Alloc non contiguous kernel memory */
int		cmd_kallocnc()
{
  char          *param;
  char		buff[BUFSIZ];
  unsigned long	addr;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  param = world.curjob->curcmd->param[0];

  if (param)
    {
      if(kernsh_alloc_noncontiguous(atoi(param), &addr))
	{
	  revm_setvar_int("_", -1);
	  PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		       "Cannot get alloc non contiguous memory", -1);
	}
      memset(buff, '\0', sizeof(buff));
      snprintf(buff, sizeof(buff), 
		   "%s %s %s %s %s %s\n\n",
	       revm_colorfieldstr("ALLOCATE"),
	       revm_colornumber("%u", (unsigned int)atoi(param)),
	       revm_colorfieldstr("octet(s)"),
	       revm_colorfieldstr("OF NON CONTIGUOUS MEMORY"),
	       revm_colorstr("@"),
	       revm_coloraddress(XFMT, (eresi_Addr) addr));
      revm_output(buff);
      revm_setvar_long("_", addr);
    }
  
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}


/* Free non contiguous kernel memory */
int		cmd_kfreenc()
{
  char          *param;
  char		buff[BUFSIZ];
  unsigned long	addr;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  param = world.curjob->curcmd->param[0];

  if (param)
    {
      addr = strtoul( param, NULL, 16 );
      if(kernsh_free_noncontiguous(addr))
	{
	  revm_setvar_int("_", -1);
	  PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		       "Cannot get free non contiguous memory", -1);
	}
      memset(buff, '\0', sizeof(buff));
      snprintf(buff, sizeof(buff), 
	       "%s %s %s %s\n\n",
	       revm_colorfieldstr("FREE"),
	       revm_colorfieldstr("NONCONTIGUOUS MEMORY"),
	       revm_colorstr("@"),
	       revm_coloraddress(XFMT, (eresi_Addr) addr));
      revm_output(buff);
      revm_setvar_int("_", 0);
    }
 
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}
