/*
** @file cat.c
** @ingroup libstderesi
** Started on  Fri Nov  2 15:21:56 2001 jfv
** Updated on  Thu Jan 04 11:26:11 2007 jfv
**
** $Id$: cat.c,v 1.1 2007-11-29 14:01:56 may Exp $
**
*/
#include "libstderesi.h"


/* Simple internal cat builtin */
int		cmd_cat()
{
  char		*tocat;
  char          buf[BUFSIZ];
  int		fd;
  int		len = 0;
  int		tmplen = 0;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  tocat = revm_lookup_string(world.curjob->curcmd->param[0]);
  if ((fd = open(tocat, O_RDONLY, 0)) < 0)
    {
      revm_output("Can't open file\n");
      PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, "Open failed", (-1));
    }

  while (1)
    {
      tmplen = read(fd, buf + len, 1);
      if (tmplen < 0)
	{
	  XCLOSE(fd,0);
	  PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, "Read failed", (-1));
	}
      if (tmplen == 0)
	break;
      len += tmplen;
      if (len > BUFSIZ - 1)
	{
	  XCLOSE(fd, 0);
	  PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
			    "File too long", (-1));
	}
    }

  buf[len] = '\0';
  revm_output(buf);
  revm_output("\n");

  XCLOSE(fd, 0);
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}
