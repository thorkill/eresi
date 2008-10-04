/**
** @file got.c
** 
** Started on  Fri Nov  2 15:20:18 2001 jfv
**
**
** $Id: got.c,v 1.2 2008-02-16 12:32:27 thor Exp $
**
*/
#include "libstderesi.h"



/** 
 * Command handler for GOT command 
 */
int		cmd_got()
{
  regex_t	*tmp;
  elfshsect_t	*got;
  int		size;
  int		index;
  int		index2;
  elfsh_SAddr	offset;
  char		*name;
  char		off[50];
  char		buff[256];
  char		logbuf[BUFSIZ];
  void		*data;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  /* Get the regx and fetch the GOT data */
  FIRSTREGX(tmp);

  index2 = 0;
  if ((got = elfsh_get_got_by_idx(world.curjob->curfile, index2, (unsigned int *)&size))
		 	== NULL)
    RET(-1);
  
  /* Loop on all .got */
  for (index2 = 0; got; index2++)
    {
  
      data = elfsh_get_raw(got);
    
      snprintf(logbuf, BUFSIZ - 1, " [Global Offset Table .::. GOT : %s ]\n [Object %s]\n\n", 
	       elfsh_get_section_name(world.curjob->curfile, got),
	       world.curjob->curfile->name);
      revm_output(logbuf);
      off[0] = 0;

      /* Loop and print all entry and their resolve */
      for (index = 0; index < size; index++)
	{

	  name = revm_resolve(world.curjob->curfile, *((eresi_Addr *) data + index), &offset);

	  if (off != NULL)
	    snprintf(off, sizeof(off), " %s %s", 
		     revm_colorstr((offset < 0 ? "-" : "+")), 
		     revm_colornumber("%u", (unsigned int) (offset > 0 ? offset : offset - offset - offset)));


	  snprintf(buff, sizeof(buff), "%s : %s %s \t <%s%s>\n", 
		   revm_coloraddress(XFMT, got->shdr->sh_addr + (index * sizeof(eresi_Addr))),
		   revm_colornumber("[%02u]", index), 
		   revm_coloraddress(XFMT, (eresi_Addr) ((eresi_Addr *) data)[index]), 
		   (name != NULL ? revm_colorstr(name) : revm_colorwarn("?")), 
		   (off[0] && name && offset ? off : ""));

	  if (!tmp || (tmp && !regexec(tmp, buff, 0, 0, 0)))
	    switch (revm_output(buff))
	      {
	      case -1:
		revm_endline();
		revm_output("\n");
		PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
	      case -2:
		revm_endline();
		goto next;
	      }
	  
	  revm_endline();
	}
      
    next:
      got = elfsh_get_got_by_idx(world.curjob->curfile, index2 + 1, (unsigned int *)&size);
      revm_output("\n");
    }

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0); 
}


