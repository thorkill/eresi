/**
** @file color.c
** @ingroup libstderesi
** @brief All functions about colors
**
** Started on Sept 11 2005 mxatone
**
** $Id: color.c,v 1.1 2007-11-29 14:01:56 may Exp $
**
*/
#include "libstderesi.h"

/* Use setcolor */
int 		cmd_color()
{
  unsigned int 	fg, bg, bd, ul, f, z, icolor;
  color_t 	*ex;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  /*
  revmworld_t *pworld = &world;
  revmjob_t *pcurjob = pworld->curjob;
  revmargv_t *pcurcmd = pcurjob->curcmd;
  char *param0 = pcurcmd->param[0];
  char *param1 = pcurcmd->param[1];
  char *param2 = pcurcmd->param[2];
  */

  if (world.curjob->curcmd->param[0] == NULL || 
      world.curjob->curcmd->param[1] == NULL || 
      world.curjob->curcmd->param[2] == NULL)
    {
      revm_output(" [!] Need 3 paramters\n\n");
      PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
			"Need 3 parameters", -1);
    }

  ex = hash_get(&t_color_hash, world.curjob->curcmd->param[0]);
  if (ex == NULL)
    {
      revm_output(" [!] Unknown type\n\n");
      PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
			"Unknown type", -1);  
    }

  fg = strcmp(world.curjob->curcmd->param[1], "fg");
  bg = strcmp(world.curjob->curcmd->param[1], "bg");
  bd = strcmp(world.curjob->curcmd->param[1], "bold");
  ul = strcmp(world.curjob->curcmd->param[1], "underline");

  if (fg && bg && bd && ul)
    {
      revm_output(" [!] Need fg, bg, bold or underline\n\n");
      PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
			"Need fg, bg, bold or underline", -1);
    }
     
  if (!fg || !bg) 
    {
      if (!fg)
	icolor = (unsigned int) hash_get(&fg_color_hash, 
				  world.curjob->curcmd->param[2]);
      else
	icolor = (unsigned int) hash_get(&bg_color_hash, 
				  world.curjob->curcmd->param[2]);

      if (icolor == 0)
	{
	  revm_output(" [!] Unknown color (see help for list)\n\n");
	  PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
			    "Unknown color", -1);
	}

      if (icolor == COLOR_RESET)
	icolor = COLOR_NONE;

      if (!fg)
	ex->fground = icolor;
      else
	ex->bground = icolor;
    } 
  else if (!bd || !ul) 
    {
      f = strcmp(world.curjob->curcmd->param[2], "1");
      z = strcmp(world.curjob->curcmd->param[2], "0"); 

      if (f && z)
	{
	  revm_output(" [!] Need 1 or 0 (1 = true, 0 = false)\n\n");
	  PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
			    "Need 1 or 0 (1 = true, 0 = false)", -1);
	}

      if (!f)
	icolor = 1;
      else
	icolor = 0;

      if (!bd)
	ex->bold = icolor;
      else
	ex->underline = icolor;
    }

  if (!world.state.revm_quiet)
    revm_output(" [*] Color modified succesfully\n\n");

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}

int 		cmd_nocolor()
{
  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  if (nocolor == 0) 
    {
      revm_output(" [*] Color enabled succesfully\n\n");
      nocolor = 1;
    }
  else
    {
      revm_output(" [*] Color disabled succesfully\n\n");
      nocolor = 0;
    } 

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}
