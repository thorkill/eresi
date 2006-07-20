/*
 * libMjollnir interface bindings
 * WIP STATUS
 */

#include "elfsh.h"

#if defined(USE_MJOLLNIR)

/*
 Dummy purpose will be removed soon
int             cmd_dummy()
{
 ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);
 ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}
*/

/*
 * To rebuild symtab use this command
 */

int cmd_unstrip() {

 ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);

 mjr_symtab_rebuild(world.curjob->mjr_session);

 ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);

}

/*
 * To performe analysis of the object
 * use this command. It will fill up hash
 * with blocks representing calls etc
 */

int cmd_analyse() {

 ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);

 mjr_analyse(world.curjob->mjr_session, NULL);

 ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);

}

/*
 * when you are reversing some software and you recognize
 * some function, let sa as main you can use this command
 * to rename sub_0x31337 to main
 */

int cmd_rename() {

 ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);

 mjr_symbol_rename(world.curjob->mjr_session,
  world.curjob->curcmd->param[0],
  world.curjob->curcmd->param[1]
  );

 ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);

}

#endif
