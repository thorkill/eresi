/**
 * @file prompt.c
 * @ingroup io
 * @brief Interface for prompt management.
 * @ingroup io
 * Started on  Fri Nov  2 15:21:56 2001 jfv
 * Updated on  Mon Mar  5 17:26:11 2007 jfv
 *
 * $Id: prompt.c,v 1.11 2008-02-16 12:32:27 thor Exp $
 *
*/
#include "revm.h"


void	(*prompt_token_setup)(char *name, unsigned int size);
char	prompt_token[512];



/**
 * @brief TO COMPLETE
 * @ingroup io
 */
void    revm_set_prompt(void (*func) (char *name, unsigned int size))
{
  prompt_token_setup = func;
}

/**
 * @brief TO COMPLETE
 * @ingroup io
 */
void	revm_create_default_prompt(char *name, unsigned int size)
{
  snprintf(name, size - 1,
	   "%s%s%s%s%s%s%s%s%s%s%s ",
	   revm_colorget("%s", "pspecial", "("),
	   (world.state.revm_mode == REVM_STATE_DEBUGGER ?
	    revm_colorget("%s", "psname" , E2DBG_ARGV0)    :
	    revm_colorget("%s", "psname" , REVM_NAME)),
	   revm_colorget("%s", "pspecial", "-"),
	   revm_colorget("%s", "pversion", REVM_VERSION),
	   revm_colorget("%s", "pspecial", "-"),
	   revm_colorget("%s", "prelease", REVM_RELEASE),
	   revm_colorget("%s", "pspecial", "-"),
	   revm_colorget("%s", "pedition", REVM_EDITION),
	   revm_colorget("%s", "pspecial", "@"),
	   revm_colorget("%s", "psname", world.curjob->ws.name),
	   revm_colorget("%s", "pspecial", ")"));
  revm_endline();
}

/** 
 * @brief return the right prompt
 * @ingroup io
 */
char*	revm_get_prompt()
{
  if (world.state.revm_mode == REVM_STATE_INTERACTIVE ||
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
 * @brief Display the prompt 
 * @ingroup io
 */
int		revm_display_prompt()
{
  char		*buf;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  buf = ((world.state.revm_mode == REVM_STATE_INTERACTIVE ||
	  world.state.revm_mode == REVM_STATE_DEBUGGER ||
	  (world.state.revm_net && world.curjob->ws.io.type != REVM_IO_STD)) ?
	 revm_get_prompt() : "");
  revm_output(buf);

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}
