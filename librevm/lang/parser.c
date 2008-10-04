/**
 * @file parse.c
 * @@ingroup librevm_lang
 * The top level parser of the language
 *
 * Started on Wed Feb 28 19:19:04 2007 jfv
 *
 * $Id: parser.c,v 1.13 2007-11-29 14:01:56 may Exp $
 *
 */
#include "revm.h"

/* Nested loop labels memory : support foreach/forend construct */
static unsigned int		curnest = 0;
static char		*looplabels[REVM_MAXNEST_LOOP];
static char		*endlabl     = NULL;
static unsigned int		nextlabel    = 0;
static revmargv_t	*forend      = NULL;

/* Pending label information : support labels */
static unsigned int		pendinglabel = 0;
static revmargv_t	*newcmd      = NULL;

/* Global variables for rewrite/case/endwrt parsing */
static list_t		*condcmdlist = NULL;
static unsigned char		isdefault    = 0;

/**
 * Create a fresh label name 
 */
char			*revm_label_get(char *prefix)
{
  char			buf[BUFSIZ];
  int			idx;
  
  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);
  idx = 0;
 retry:
  snprintf(buf, sizeof(buf), "%s_DEPTH:%u_INDEX:%u", prefix, curnest, idx);
  if (hash_get(&labels_hash[world.curjob->sourced], buf))
    {
      idx++;
      goto retry;
    }
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, strdup(buf));
}


/** 
 * Recognize a special command pattern 
 */
int			revm_parse_construct(char *curtok)
{
  char			*labl;
  listent_t		*curcondcmd;
  revmargv_t		*condcmd;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);
	  
  /* If we met a foreach/forend, update the nested loop labels stack */
  if (!strcmp(curtok, CMD_FOREACH))
    {
      if (curnest >= REVM_MAXNEST_LOOP)
	PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		     "Too many nested construct", -1);
      labl = revm_label_get("foreach");
      hash_add(&labels_hash[world.curjob->sourced], labl, newcmd);
      looplabels[curnest++] = labl;
    }
  
  /* At forend, delay the label insertion to the next command */
  else if (!strcmp(curtok, CMD_FOREND))
    {
      if (curnest == 0)
	PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		     "Incorrectly nested forend statement", -1);
      endlabl = revm_label_get("forend");
      if (!strstr(looplabels[curnest - 1], "foreach"))
	PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		     "Incorrectly nested loop-ending statement", -1);
      forend = newcmd;
      forend->endlabel = looplabels[--curnest];
      nextlabel = 1;
    }

  /* Here the match parser */
  /* If we met a foreach/forend, update the nested loop labels stack */
  else if (!strcmp(curtok, CMD_MATCH))
    {
      if (curnest >= REVM_MAXNEST_LOOP)
	PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		     "Too many nested construct", -1);
      labl = revm_label_get("match");
      hash_add(&labels_hash[world.curjob->sourced], labl, newcmd);
      looplabels[curnest++] = labl;
    }

  /* Maintain a list of commands that may have a conditional continuation during a rewrite */
  else if (!strcmp(curtok, CMD_DEFAULT) || !strcmp(curtok, CMD_CASE))
    {
      if (curnest == 0)
	PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		     "Incorrect default or case statement", -1);
      if (!strstr(looplabels[curnest - 1], "match"))
	PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		     "Incorrectly nested match-ending statement", -1);
      if (!condcmdlist)
	{
	  XALLOC(__FILE__, __FUNCTION__, __LINE__, condcmdlist, sizeof(list_t), -1);
	  elist_init(condcmdlist, "parser_condcmdlist", ASPECT_TYPE_UNKNOW);
	}
      elist_add(condcmdlist, strdup(curtok), newcmd);
    }
  
  /* At forend, delay the label insertion to the next command */
  else if (!strcmp(curtok, CMD_MATCHEND))
    {
      if (curnest == 0)
	PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		     "Incorrectly nested endmatch statement", -1);
      endlabl = revm_label_get("matchend");
      
      if (!strstr(looplabels[curnest - 1], "match"))
	PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		     "Incorrectly nested match-ending statement", -1);      
      forend = newcmd;
      forend->endlabel = looplabels[--curnest];
      hash_add(&labels_hash[world.curjob->sourced], endlabl, newcmd);
      
      /* Now setting the end label to all waiting case/default commands */
      if (condcmdlist && condcmdlist->head)
	{
	  for (curcondcmd = condcmdlist->head; curcondcmd; curcondcmd = curcondcmd->next)
	    {
	      condcmd = (revmargv_t *) curcondcmd->data;
	      condcmd->endlabel = strdup(endlabl);
	    }
	  elist_destroy(condcmdlist);
	  condcmdlist = NULL;
	}
    }
  
  /* This flag says if the command opens a new scope just for its arguments */
  isdefault = ((!strcmp(curtok, CMD_DEFAULT) || 
		!strcmp(curtok, CMD_PRE) || 
		!strcmp(curtok, CMD_POST)) ? 1 : 0);

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}






/** 
 * Parse the commands 
 */
int			revm_parseopt(int argc, char **argv)
{
  unsigned int			index;
  int			ret;
  volatile revmcmd_t	*actual;
  volatile revmargv_t   *loopstart;
  char			*name;
  char			label[16];
  char			c;
  char			cmdline;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  /* We are at command line */
  cmdline = ((world.state.revm_mode == REVM_STATE_CMDLINE 
	     || world.state.revm_mode == REVM_STATE_TRACER)
	     && !world.state.revm_net);

  /* Main option reading loop : using the command hash table */
  for (index = 1; index < argc; index++)
    {

      /* Allocate command descriptor */
      bzero(label, sizeof(label));
      if (!pendinglabel)
	{
	  XALLOC(__FILE__, __FUNCTION__, __LINE__,
		 newcmd, sizeof(revmargv_t), -1);
	  world.curjob->curcmd = newcmd;
	  if (world.curjob->script[world.curjob->sourced] == NULL)
	    world.curjob->script[world.curjob->sourced] = newcmd;
	}
      else
	pendinglabel = 0;
      
      /* Retreive command descriptor in the hash table */
      name = argv[index] + cmdline;
      actual = hash_get(&cmd_hash, name);
    
      /* We matched a command : call the registration handler */
      if (actual != NULL)
	{

	  /* If there is a forend/matchend label waiting, insert it here */
	  /* The forend label must point on the -following- command */
	  /* Annotate meta-command foreach, case, and default with  the "end label" */
	  if (nextlabel)
	    {
	      hash_add(&labels_hash[world.curjob->sourced], endlabl, newcmd);
	      loopstart = hash_get(&labels_hash[world.curjob->sourced], looplabels[curnest]);

	      /* If we are executing "default", we search for the loop start in the parent scope */
	      /*
	      if (world.curjob->sourced && isdefault)
		for (ret = 0; world.curjob->sourced >= ret; ret++)
		  {
		    loopstart = hash_get(&labels_hash[world.curjob->sourced - ret], looplabels[curnest]);
		    if (loopstart)
		      break;
		  }
	      */

	      if (!loopstart)
		PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
			     "Invalid nesting of language construct", -1);
	      loopstart->endlabel = strdup(endlabl);
	      /* isdefault = */ nextlabel = 0;
	    }

	  /* Dont call registration handler if there is not (0 param commands) */
	  if (actual->reg != NULL)
	    {
	      ret = actual->reg(index, argc, argv);
	      if (ret < 0)
		PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
			     "Command not found", 
			     (revm_doerror(revm_badparam, argv[index])));
	    }
	  else ret = 0;

	  /* Handle control flow constructs of the eresi script */
	  if (revm_parse_construct(argv[index]) < 0)
	    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
			 "Invalid eresi program nesting construct", -1);
	  index += ret;
	}

      /* We did -NOT- match command */
      else if (world.state.revm_mode == REVM_STATE_SCRIPT)
	{
	  /* Try to match a label */
	  ret = sscanf(name, "%15[^:]%c", label, &c);
	  if (ret == 2 && c == ':')
	    {
	      hash_add(&labels_hash[world.curjob->sourced], 
		       strdup(label), newcmd); 
	      pendinglabel = 1;
	      continue;
	    }
	  
	  /* No command matched, enable runtime lookup : module could be loaded later */
	  ret = revm_getvarparams(index - 1, argc, argv);
	  index += ret;
	}
      
      /* We matched nothing known, error */
      else
	PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		     "Unknown parsing error", 
		     (revm_doerror(revm_unknown, argv[index])));

      /* Put the newcmd command at the end of the list */
      newcmd->name = name;
      newcmd->cmd  = (revmcmd_t *) actual;      
      if (!world.curjob->lstcmd[world.curjob->sourced])
	world.curjob->lstcmd[world.curjob->sourced] = newcmd;
      else
	{
	  world.curjob->lstcmd[world.curjob->sourced]->next = newcmd;
	  newcmd->prev = world.curjob->lstcmd[world.curjob->sourced];
	  world.curjob->lstcmd[world.curjob->sourced] = newcmd;
	}
    }

  /* Return success */
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}


