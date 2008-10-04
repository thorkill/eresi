/**
 * @file ctors.c
 * 
 * Started on  Tue Feb 26 22:07:58 2002 jfv
 *
 *
 * $Id: ctors.c,v 1.2 2008-02-16 12:32:27 thor Exp $
 *
*/
#include "libstderesi.h"




/** 
 * Display the constructor section 
 */
int		cmd_ctors()
{
  regex_t	*tmp;
  eresi_Addr	*ctors;
  int		size;
  int		index;
  elfsh_SAddr  	offset;
  elfsh_SAddr	doffset;
  char		*name;
  char		*dname;
  char		off[50];
  char		buff[256];
  char		logbuf[BUFSIZ];

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  ctors = elfsh_get_ctors(world.curjob->curfile, &size);
  if (ctors == NULL)
    RET(-1);
  FIRSTREGX(tmp);
  snprintf(logbuf, BUFSIZ - 1,
	   " [Constructors array .::. CTORS]\n [Object %s]\n\n", 
	   world.curjob->curfile->name);
  revm_output(logbuf);
  off[0] = 0;

  for (index = 0; index < size; index++)
    {

      name = elfsh_reverse_symbol(world.curjob->curfile, ctors[index], &offset);
      dname = elfsh_reverse_dynsymbol(world.curjob->curfile, ctors[index], &doffset);
      if (dname && doffset < offset)
	{
	  name = dname;
	  offset = doffset;
	}

      if (offset)
	snprintf(off, sizeof(off), " %s %s", 
		 revm_colorstr((offset < 0 ? "-" : "+")), 
		 revm_colornumber("%u", (unsigned int) (offset > 0 ? offset : -offset)));
      
      snprintf(buff, sizeof(buff), " %s  %s \t <%s%s>\n", 
	       revm_colornumber("[%02u]", index), 
	       revm_coloraddress(XFMT, (eresi_Addr) ctors[index]), 
	       (name ? revm_colorstr(name) : revm_colorwarn("?")), 
	       (name && offset ? off : ""));
      
      if (!tmp || (tmp && !regexec(tmp, buff, 0, 0, 0)))
	revm_output(buff);

       revm_endline();
    }


  revm_output("\n");
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0); 
}
