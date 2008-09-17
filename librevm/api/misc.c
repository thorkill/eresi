/*
** @file misc.c
** @ingroup librevm_api
** Started on  Fri Nov  2 15:21:56 2001 jfv
** Updated on  Fri Sep 11 17:26:11 2005 jfv
**
** $Id: misc.c,v 1.15 2008-02-16 12:32:27 thor Exp $
**
*/
#include "revm.h"


/** 
 * return the project name accordingly to mode 
 * @return
 */
char		*revm_modename_get()
{
  char          *mode;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  if (world.state.revm_mode == REVM_STATE_DEBUGGER)
    mode = E2DBG_NAME;
  else
    mode = REVM_NAME;

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, (mode));
}


/** 
 * Our system implementation 
 * @param cmd
 */
int		revm_system(char *cmd)
{
  char		buf[BUFSIZ];
  int		ret;
  char		**av;
  int		nbr;
  int		argc;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  if (world.curjob->ws.io.type == REVM_IO_NET)
    snprintf(buf, BUFSIZ, "%s <&%u >&0 2>&0 ", cmd, world.curjob->ws.io.output_fd);
  else
    snprintf(buf, BUFSIZ, "%s ", cmd);

  /* If the user shell is unspecified we use system */
  nbr = revm_findblanks(cmd);
  av = revm_doargv(nbr, (u_int *)&argc, cmd);
  av++;
  if (!fork())
    ret = execvp(av[0], av);
  else
    wait(NULL);

  /* Report result */
  if (ret < 0)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
		      "Shell not found", 0);
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, (0));
}


/** 
 * Decide what to do for exiting depending on the current input 
 * @param err
 * @return
 */
void	revm_exit(int err)
{
  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);
  _exit(err);
  PROFILER_OUT(__FILE__, __FUNCTION__, __LINE__);
}

/** 
 * Bad parameter handler 
 * @param str
 * @return
 */
void	revm_badparam(char *str)
{
  char	buf[BUFSIZ];

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  snprintf(buf, BUFSIZ,
	   "\n [!] Invalid parameters for command %s .::. "
	   "type 'help' for command list \n\n", str);
  revm_output(buf);

  PROFILER_OUT(__FILE__, __FUNCTION__, __LINE__);
}

/** 
 * Unknow command handler 
 * @param str
 */
void	revm_unknown(char *str)
{
  char	buf[BUFSIZ];

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  snprintf(buf, BUFSIZ, "\n [!] Unknown command %s .::. "
	   "type 'help' for command list \n\n", str);
  revm_output(buf);
  PROFILER_OUT(__FILE__, __FUNCTION__, __LINE__);
}

/**
 * Generic error message handler 
 *
 * @param label
 * @param param
 */

void	revm_error(char *label, char *param)
{
  char	buf[BUFSIZ];

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  snprintf(buf, BUFSIZ, " [!] %s [%s] \n\n", label, param);
  revm_output(buf);
  PROFILER_OUT(__FILE__, __FUNCTION__, __LINE__);
}


/** 
 * Open the script file 
 */
int		revm_openscript(char **av)
{
  int		fd;
  int		idx;
  char		actual[16];
  revmobj_t	*new;
  revmexpr_t	*expr;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  /* Open script file */
  XOPEN(fd, av[0], O_RDONLY, 0, -1);
  world.curjob->ws.io.input_fd = fd;

  /* Add argument variables */
  for (idx = 1; av[idx]; idx++)
    {
      snprintf(actual, sizeof(actual), "$%u", idx);
      new = revm_create_IMMEDSTR(1, strdup(av[idx]));
      expr = revm_expr_create_from_object(new, actual);
    }

  new = revm_create_IMMED(ASPECT_TYPE_INT, 1, idx - 1);
  expr = revm_expr_create_from_object(new, REVM_VAR_ARGC);
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}

/** 
 * Say if we are in script mode 
 */
int		revm_testscript(int ac, char **av)
{
  int		fd;
  char		buff[30];

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  if (ac < 2 || (av[1] && av[1][0] == REVM_DASH))
    PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);

  XOPEN(fd, av[1], O_RDONLY, 0, 0);
  XREAD(fd, buff, 30, 0);
  buff[29] = 0x00;
  if (strncmp(buff, "#!", 2))
    {
      XCLOSE(fd, 0);
      PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
			"Invalid script interpreter", 0);
    }

  /* FIXME-XXX: Make it possible to register its interpreter name */
  if (!strstr(buff, av[0]) && 
      !strstr(buff, "elfsh") &&
      !strstr(buff, "etrace") &&
      !strstr(buff, "e2dbg") &&
      !strstr(buff, "evarista"))
    {
      XCLOSE(fd, 0);
      PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
			"Not an ERESI script", 0);
    }
  XCLOSE(fd, 0);
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, (1));
}

/** 
 * Print the banner 
 */
void		revm_banner_print()
{
  char		logbuf[BUFSIZ];

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  snprintf(logbuf, BUFSIZ - 1,
	   "\n\n\t The %s %s (%s) .::. \n\n %s",
	   revm_modename_get(),
	   REVM_VERSION,
#if defined(ERESI32)
	   "32 bits built",
#elif defined(ERESI64)
	   "64 bits built",
#else
	   "Unknown built",
#endif
	   "\t .::. This software is under the General Public License V.2 \n"
	   "\t .::. Please visit http://www.gnu.org \n\n");

  revm_output(logbuf);
  PROFILER_OUT(__FILE__, __FUNCTION__, __LINE__);
}

/** 
 * Print the Unknown buffer 
 */
char		*revm_build_unknown(char *buf, const char *str, u_long type)
{

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  snprintf(buf, ERESI_MEANING, "%s %08X", str, (u_int) type);

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, (buf));
}


/** 
 * Retreive a file object giving its unique ID 
 */
elfshobj_t	*revm_getfile(u_int id)
{
  elfshobj_t	*cur;
  elfshobj_t	*subcur;
  char		**keys;
  int		idx;
  int		keynbr;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  /* Check in private objects of the workspace */
  if (hash_size(&world.curjob->loaded))
    {
      keys = hash_get_keys(&world.curjob->loaded, &keynbr);
      for (idx = 0; idx < keynbr; idx++)
	{
	  cur = hash_get(&world.curjob->loaded, keys[idx]);
	  if (cur->id == id)
	    PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, (cur));

	  if ((subcur = revm_is_depid(cur, id)) != NULL)
	    PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, (subcur));
	}
    }

  /* Check in shared objects */
  if (hash_size(&world.shared_hash))
    {
      keys = hash_get_keys(&world.shared_hash, &keynbr);
      for (idx = 0; idx < keynbr; idx++)
	{
	  cur = hash_get(&world.shared_hash, keys[idx]);
	  if (cur->id == id)
	    PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, (cur));

	  if ((subcur = revm_is_depid(cur, id)) != NULL)
	    PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, (subcur));
	}
    }

  PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
		    "Unable to find file", (NULL));
}

/** 
 * Retreive a module object giving its unique ID 
 */
revmmod_t	*revm_getmod(u_int index)
{
  revmmod_t	*cur;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  for (cur = world.modlist; cur; cur = cur->next)
    if (cur->id == index)
      PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, (cur));
  PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
		    "Unable to find module", (NULL));
}

/** 
 * Print error depending on the state of the machine 
 */
int		revm_doerror(void (*fct)(char *str), char *str)
{
  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  if (world.state.revm_mode != REVM_STATE_CMDLINE || world.state.revm_net)
    fct(str);
  else
    {
      revm_help(NULL);
      exit(-1);
    }
  PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
		    "Bypassed error printing", (-1));
}


/** 
 * Change the shell variable 
 */
int		revm_setshell(char *str)
{
  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);
  if (revm_setvar_str(REVM_VAR_SHELL, str) < 0)
   PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
		     "Cannot modify shell var", -1);
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}


/** 
 * The internal basename function 
 */
char		*revm_basename(char *str)
{
  char		*cur;
  char		*ret;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  ret = NULL;
  cur = str;
  while ((cur = strchr(cur, '/')))
    if (!*(cur + 1))
      PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, "No basename", (NULL));
    else
      ret = ++cur;
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, (ret));
}



/** 
 * Useful to differentiate 0 and a string 
 */
int	revm_isnbr(char *string)
{
  size_t len = strlen(string);
  size_t ii;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  for (ii=0; ii < len; ii++)
    if (!isdigit((int) string[ii]))
      PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, (1));
}



/**
 *  Concatenate all parameters and return a single string 
 */
char	*revm_string_get(char **params)
{
  char	buff[BUFSIZ];
  int	idx;
  int	len;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);
  for (len = idx = 0; params[idx]; idx++)
    len += snprintf(buff + len, BUFSIZ - len, "%s%s", 
		    (idx ? " " : ""), params[idx]);
  if (len)
    PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 
		  strdup(buff));
  PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
	       "Empty parameter", NULL);
}


/**
 *  Get and Set fields of structures
 */
char     *revm_get_cur_job_parameter(uint8_t p)
{
  return world.curjob->curcmd->param[p];
}


char     *revm_get_cur_job_parameter_with_job(revmjob_t *job, uint8_t p)
{
  return job->curcmd->param[p];
}
