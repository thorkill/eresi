/*
** @file hammer.c
** @ingroup libstderesi
** @brief Contains libmjollnir commands bindings.
**
** Last update December 10 2006 may
**
** $Id: hammer.c,v 1.2 2008-02-16 12:32:27 thor Exp $
**
*/
#include "libstderesi.h"


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
  if (!world.mjr_session.cur->analysed)
    cmd_analyse();
  memset(logbuf,0x0,BUFSIZ);
  snprintf(logbuf, BUFSIZ - 1, " .: mjollnir : unstrip procedure %s\n",
	   (mjr_symtab_rebuild(&world.mjr_session)) ? "completed successfully" : "faild");
  revm_output(logbuf);
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

 PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

 /* Analyzing object */
 memset(logbuf, 0x0, BUFSIZ);
 snprintf(logbuf, BUFSIZ - 1, " [*] Now performing Control Flow Analysis\n");
 revm_output(logbuf);
 memset(logbuf, 0x0, BUFSIZ);

 /* Resolve the parameter */
 if (world.curjob->curcmd->param[0])
   {
     expr = revm_lookup_var(world.curjob->curcmd->param[0]);
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
   addr = 0;

 ret = mjr_analyse(&world.mjr_session, 0, 0); //addr, 0);
 
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
     revm_output(" [*] Reflected succesfully all basic blocks and functions.\n\n");
   }
 else
   revm_output(" [*] No CONTAINER reflection was performed (type definition not found)\n\n");

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
