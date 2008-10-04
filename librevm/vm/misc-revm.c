/**
 * @file misc.c
 * @ingroup librevm_vm
 * Started on  Fri Nov  2 15:21:56 2001 jfv
 * Updated on  Fri Sep 11 17:26:11 2005 jfv
 *
 * $Id: misc-revm.c,v 1.13 2007-11-29 14:01:56 may Exp $
 *
 */
#include "revm.h"

/* XXX: to remove ?
#if defined(USE_READLN)
 rl_command_func_t *rl_ctrll = NULL;
#endif
*/

char buf[BUFSIZ];


/**
 *
 */
void    revm_set_prompt(void (*func) (char *name, unsigned int size))
{
  prompt_token_setup = func;
}

/**
 *
 */
void	revm_create_default_prompt(char *name, unsigned int size)
{
  snprintf(name, size - 1,
	   "%s%s%s%s%s%s%s%s%s%s%s ",
	   revm_colorget("%s", "pspecial", "("),
	   (world.state.revm_mode == REVM_STATE_DEBUGGER ?
	    revm_colorget("%s", "psname" , E2DBG_ARGV0)    :
	    revm_colorget("%s", "psname" , ELFSH_SNAME)),
	   revm_colorget("%s", "pspecial", "-"),
	   revm_colorget("%s", "pversion", ERESI_VERSION),
	   revm_colorget("%s", "pspecial", "-"),
	   revm_colorget("%s", "prelease", ERESI_RELEASE),
	   revm_colorget("%s", "pspecial", "-"),
	   revm_colorget("%s", "pedition", ELFSH_EDITION),
	   revm_colorget("%s", "pspecial", "@"),
	   revm_colorget("%s", "psname", world.curjob->name),
	   revm_colorget("%s", "pspecial", ")"));
  revm_endline();
}

/**
 * return the right prompt 
 */
char	*revm_get_prompt()
{
  if (world.state.revm_mode == REVM_STATE_INTERACT ||
      world.state.revm_mode == REVM_STATE_DEBUGGER)
    {
      /* Setup prompt only once */
      if (prompt_token_setup == NULL)
	revm_set_prompt(revm_create_default_prompt);

      if (prompt_token_setup)
	prompt_token_setup(prompt_token, sizeof(prompt_token));
      else
	snprintf(prompt_token, sizeof(prompt_token) - 1, "prompt-error");

      /* Prompt on readline need some modifications */
      revm_prompt_update(prompt_token, sizeof(prompt_token));
      return (prompt_token);
    }

  if (world.state.revm_mode == REVM_STATE_SCRIPT)
    return "";

  return "UNKNOWN MODE> ";
}

/**
 * return the project name accordingly to mode 
 */
char		*revm_modename_get()
{
  char          *mode;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  if (world.state.revm_mode == REVM_STATE_DEBUGGER)
    mode = E2DBG_NAME;
  else
    mode = ELFSH_NAME;

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, (mode));
}


/** 
 * Our system implementation 
 */
int		revm_system(char *cmd)
{
  char		buf[BUFSIZ];
  int		ret;
  char		**av;
  int		nbr;
  int		argc;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  if (world.curjob->io.type == REVM_IO_NET)
    snprintf(buf, BUFSIZ, "%s <&%u >&0 2>&0 ", cmd, world.curjob->io.output_fd);
  else
    snprintf(buf, BUFSIZ, "%s ", cmd);

  /* If the user shell is unspecified we use system */
  nbr = revm_findblanks(cmd);
  av = revm_doargv(nbr, (unsigned int *)&argc, cmd);
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
 */
void	revm_exit(int err)
{
  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);
  _exit(err);
  PROFILER_OUT(__FILE__, __FUNCTION__, __LINE__);
}

/** 
 * Bad parameter handler 
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
  world.curjob->io.input_fd = fd;

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

  if (ac < 2 || (av[1] && av[1][0] == ELFSH_MINUS))
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
  if (!strstr(buff, av[0]) && !strstr(buff, "elfsh"))
    {
      XCLOSE(fd, 0);
      PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
			"Not an ELFsh script", 0);
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
	   ERESI_VERSION,
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
char		*revm_build_unknown(char *buf, const char *str, unsigned long type)
{

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  snprintf(buf, ERESI_MEANING, "%s %08X", str, (unsigned int) type);

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, (buf));
}


/** 
 * Retreive a file object giving its unique ID 
 */
elfshobj_t	*revm_getfile(unsigned int id)
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
revmmod_t	*revm_getmod(unsigned int index)
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

  if (world.state.revm_mode != ELFSH_VMSTATE_CMDLINE || world.state.revm_net)
    fct(str);
  else
    {
      cmd_help();
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
  if (revm_setvar_str(ERESI_EXTSHELLVAR, str) < 0)
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
