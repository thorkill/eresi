/*
** prompt.c for librevm in ERESI
**
** Interface for prompt management
**
** Started on  Fri Nov  2 15:21:56 2001 mayhem
** Updated on  Mon Mar  5 17:26:11 2007 mayhem
**
** $Id: prompt.c,v 1.4 2007-03-25 14:27:34 may Exp $
**
*/
#include "revm.h"


//char buf[BUFSIZ];


void    vm_set_prompt(void (*func) (char *name, u_int size))
{
  prompt_token_setup = func;
}

void	vm_create_default_prompt(char *name, u_int size)
{
  snprintf(name, size - 1,
	   "%s%s%s%s%s%s%s%s%s%s%s ",
	   vm_colorget("%s", "pspecial", "("),
	   (world.state.vm_mode == REVM_STATE_DEBUGGER ?
	    vm_colorget("%s", "psname" , E2DBG_ARGV0)    :
	    vm_colorget("%s", "psname" , ELFSH_SNAME)),
	   vm_colorget("%s", "pspecial", "-"),
	   vm_colorget("%s", "pversion", ELFSH_VERSION),
	   vm_colorget("%s", "pspecial", "-"),
	   vm_colorget("%s", "prelease", ELFSH_RELEASE),
	   vm_colorget("%s", "pspecial", "-"),
	   vm_colorget("%s", "pedition", ELFSH_EDITION),
	   vm_colorget("%s", "pspecial", "@"),
	   vm_colorget("%s", "psname", world.curjob->ws.name),
	   vm_colorget("%s", "pspecial", ")"));
  vm_endline();
}

/* return the right prompt */
char*	vm_get_prompt()
{
  if (world.state.vm_mode == REVM_STATE_INTERACTIVE ||
      world.state.vm_mode == REVM_STATE_DEBUGGER)
    {
      /* Setup prompt only once */
      if (prompt_token_setup == NULL)
	vm_set_prompt(vm_create_default_prompt);

      if (prompt_token_setup)
	prompt_token_setup(prompt_token, sizeof(prompt_token));
      else
	snprintf(prompt_token, sizeof(prompt_token) - 1, "prompt-error");

      /* Prompt on readline need some modifications */
#if defined(USE_READLN)
      readln_prompt_update(prompt_token, sizeof(prompt_token));
#endif

      return prompt_token;
    }

  if (world.state.vm_mode == REVM_STATE_SCRIPT)
    return "";

  return "UNKNOWN MODE> ";
}


/* Display the prompt */
int		vm_display_prompt()
{
  char		*buf;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  buf = ((world.state.vm_mode == REVM_STATE_INTERACTIVE ||
	  world.state.vm_mode == REVM_STATE_DEBUGGER ||
	  (world.state.vm_net && world.curjob->ws.io.type != REVM_IO_STD)) ?
	 vm_get_prompt() : "");
  vm_output(buf);

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}
