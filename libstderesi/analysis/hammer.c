/*
** @file hammer.c
** @ingroup libstderesi
** @brief Contains libmjollnir commands bindings.
**
** Last update December 10 2006 may
**
** $Id$
**
*/
#include "libstderesi.h"


/** Check if we have to re-analyse the binary */
int	revm_analysed(eresi_Addr addr)
{
  int	analysed;
  char	logbuf[BUFSIZ];
  char	c;

  /* Make sure we do what the user desires (remove previously stored analysis) */
  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);
  analysed = mjr_analysed(&world.mjr_session, addr);
  if (analysed)
    {
      snprintf(logbuf, sizeof(logbuf), 
	       " [*] Address " XFMT " has already been analysed. \n"
	       "     Analysis will remove currently stored information. Continue ? [N/y]", 
	       addr);
      revm_output(logbuf);
      c = getchar();
      revm_output("");
      if (c != 'y')
	PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
		     "Flow analysis aborted", 0);
      mjr_analyse_destroy(world.mjr_session.cur);
    }
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 1);
}


/** 
 * To rebuild symtab use this command 
 */
int	cmd_unstrip() 
{
  char logbuf[BUFSIZ];
  
  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);
  memset(logbuf,0x0,BUFSIZ);
  snprintf(logbuf, BUFSIZ - 1, " .: mjollnir : unstrip procedure started\n");
  revm_output(logbuf);

  // The analysed flag does not exist anymore since we do demand-driven analysis
  //if (!world.mjr_session.cur->analysed)
  //cmd_analyse();

  memset(logbuf,0x0,BUFSIZ);
  snprintf(logbuf, BUFSIZ - 1, " .: mjollnir : unstrip procedure %s\n",
	   (mjr_symtab_rebuild(&world.mjr_session)) ? "completed successfully" : "failed");
  revm_output(logbuf);
  elfsh_sync_sorted_symtab(world.mjr_session.cur->obj->secthash[ELFSH_SECTION_SYMTAB]);
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}


/** 
 * This command construct the call graph and the control flow graph for each function.
 * Additionally, it reflects all analysed objects in the ERESI language.
 */
int		cmd_analyse() 
{
  char		logbuf[BUFSIZ];
  int		ret;
  aspectype_t	*curtype;
  aspectype_t	*linktype;
  eresi_Addr     addr;
  revmexpr_t     *expr; 
  revmobj_t      *obj;
  int		maxdepth;

 PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

 /* Resolve the parameter */
 if (world.curjob->curcmd->param[0])
   {
     expr = revm_lookup_var(world.curjob->curcmd->param[0]);
     if (expr)
       {
	 obj = expr->value;
	 switch (obj->otype->type)
	   {
	   case ASPECT_TYPE_LONG:
	   case ASPECT_TYPE_CADDR:
	   case ASPECT_TYPE_DADDR:
	     addr = (obj->immed ? obj->immed_val.ent : obj->get_obj(obj->parent));
	     break;
	     
	   case ASPECT_TYPE_INT:
	     addr = (obj->immed ? obj->immed_val.word : obj->get_obj(obj->parent));
	     break;
	   }
       }
     else
       addr = revm_lookup_addr(world.curjob->curcmd->param[0]);
   }
 else
   addr = world.curjob->curfile->hdr->e_entry;

 /* Check if we have to remove previously saved analysis section */
 if (!revm_analysed(addr))
   PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		"Canceled control flow analysis", -1);

 /* Analyzing object */
 snprintf(logbuf, BUFSIZ - 1, 
	  " [*] Now performing Control Flow Analysis at " XFMT " (curfile = %s)\n", 
	  addr, world.mjr_session.cur->obj->name);
 revm_output(logbuf);
 
 /* Do control flow analysis */
 maxdepth = (int) config_get_data(CONFIG_CFGDEPTH);
 ret = mjr_analyse(&world.mjr_session, addr, maxdepth, 0);

 /* We certainly have inserted many new symbols */
 elfsh_sync_sorted_symtab(world.curjob->curfile->secthash[ELFSH_SECTION_SYMTAB]);

 snprintf(logbuf, BUFSIZ - 1, " [*] Control Flow Analysis %s.\n",
	  (!ret ? "completed successfully" : "failed"));
 revm_output(logbuf);
 if (ret) 
   PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		"Error during control flow analysis", -1);

 memset(logbuf, 0x0, BUFSIZ);
 snprintf(logbuf, BUFSIZ - 1, 
	  " [*] Calls seen: %d, Recognized destination: %d\n",
	  world.mjr_session.cur->calls_seen,
	  world.mjr_session.cur->calls_found);
 revm_output(logbuf);

 /* Now informing about all existing blocks and functions */
 curtype = aspect_type_get_by_name("container");
 if (curtype)
   {
     revm_type_reflect(&world.mjr_session.cur->funchash, "func");
     revm_type_reflect(&world.mjr_session.cur->blkhash , "bloc");
     revm_output(" [*] Reflected succesfully all basic blocks and functions.\n");
   }
 else
   revm_output(" [*] No CONTAINER reflection was performed (type definition not found)\n");

 /* Now informing about all existing links */
 linktype = aspect_type_get_by_name("link");
 if (linktype)
   {
     revm_type_reflect(&world.mjr_session.cur->linkhash, linktype->name);
     revm_output(" [*] Reflected succesfully all blocks links and function links.\n\n");
   }
 else
   revm_output(" [*] No LINK reflection was performed (type definition not found)\n\n");

 PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}



/** 
 * Shortcut command to rename symbols 
 */
int	cmd_rename() 
{
 PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);
 mjr_symbol_rename(&world.mjr_session,
		   world.curjob->curcmd->param[0],
		   world.curjob->curcmd->param[1]);
 PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}


/** 
 * Remove previously done analysis
 */
int	cmd_astrip()
{
  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);
  mjr_analyse_destroy(world.mjr_session.cur);
  revm_output("[*] Removed previously stored control flow analysis\n\n");
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}
