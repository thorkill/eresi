#include "kedbg.h"
#include "interface.h"

/**
 * Retrieve a character string from the remote server.
 * @param addr : address to retreive
 * @param buf  : buffer to write in
 */
char            *kedbg_getstr(void *addr, char *buf, unsigned maxsize)
{
  unsigned      i = 0;
  char          c;
  
  do
    {
      kedbg_readmema(NULL, (eresi_Addr)((char *)addr + i), &c, sizeof(char));
      buf[i] = c;
    } while (c != '\0' && ++i <= maxsize);

  return buf;
}


elfshlinkmap_t   *kedbg_linkmap_getaddr(void)
{
  elfshsect_t    *got;
  void           *data;
  elfshlinkmap_t *lm;
  got           = elfsh_get_gotsct(world.curjob->curfile);
  data          = elfsh_readmem(got);
  lm            = (elfshlinkmap_t *)(*elfsh_get_got_entry_by_index(data, 1));
  
  DEBUGMSG(fprintf(stderr, "Found got[1]: %#x\n", (eresi_Addr)lm));
  return        lm;
  //  kedbg_readmema(NULL, linkmap_entry, &linkmap_copy, sizeof(elfshlinkmap_t));
}


void             kedbg_continue(void)
{
  gdbwrap_t     *loc = gdbwrap_current_get();
  gdbwrap_continue(loc);
}


/* void            *kedbg_linkmap_lnext() */
/* { */
/* } */


/* void            *kedbg_linkmap_lprev() */
/* { */
/* } */


/* void            *kedbg_linkmap_laddr() */
/* { */
/* } */


/* void            *kedbg_linkmap_lname() */
/* { */
/* } */

