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
 char logbuf[BUFSIZ];

 ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);

 memset(logbuf,0x0,BUFSIZ);
 snprintf(logbuf, BUFSIZ - 1, " .: mjollnir : unstrip procedure started\n");
 vm_output(logbuf);

 if (!world.mjr_session.cur->analysed_calls)
  cmd_analyse();

  memset(logbuf,0x0,BUFSIZ);

  snprintf(logbuf, BUFSIZ - 1, " .: mjollnir : unstrip procedure %s\n",
   (mjr_symtab_rebuild(&world.mjr_session)) ? "completed successfully" : "faild");

  vm_output(logbuf);

 ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);

}


/*
 * To performe analysis of the object
 * use this command. It will fill up hash
 * with blocks representing calls etc
 */

int cmd_analyse() {

 char logbuf[BUFSIZ];
 int ret = 0;

 ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);

 memset(logbuf,0x0,BUFSIZ);
 snprintf(logbuf, BUFSIZ - 1, " .: mjollnir : performing object analysis\n");
 vm_output(logbuf);

 memset(logbuf,0x0,BUFSIZ);
 ret = mjr_analyse(&world.mjr_session, NULL);
 snprintf(logbuf, BUFSIZ - 1, " .: mjollnir : object analysis %s\n",
 (ret) ? "completed successfully" : "faild");

 vm_output(logbuf);
 
 if (ret) {
 memset(logbuf,0x0,BUFSIZ);
 snprintf(logbuf, BUFSIZ - 1, " .: mjollnir : calls seen: %d, recognized destination: %d\n",
  world.mjr_session.cur->st_calls_seen,
  world.mjr_session.cur->st_calls_found
  );
 vm_output(logbuf);
 }

 ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);

}

/*
 * when you are reversing some software and you recognize
 * some function, let sa as main you can use this command
 * to rename sub_0x31337 to main
 */

int cmd_rename() {

 ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);

 mjr_symbol_rename(&world.mjr_session,
  world.curjob->curcmd->param[0],
  world.curjob->curcmd->param[1]
  );

 ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);

}

#endif
