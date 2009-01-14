/**
 * @file configure.c
 * @ingroup libstderesi
 * @brief ELFsh/vm configure related code
 * 2006 thorkill, Asgard Labs Inc.
 *
 * $Id$
 */
#include "libstderesi.h"

int		cmd_configure()
{
  configitem_t *ci;
  int		cnt;
  int		idx;
  char		**tab;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  /* We list the configuration */
  if (world.curjob->curcmd->argc < 2)
    {
      tab = hash_get_keys(&aspectworld.config_hash, &cnt);
      
      printf(" [*] Configure parameters\n\n");
      
      for (idx = 0; idx < cnt; idx++) 
	{
	  ci = hash_get(&aspectworld.config_hash, tab[idx]);
	  
	  if (ci->type == CONFIG_TYPE_INT)
	    printf(" [+] (%2s) %-30s : %d\n", 
		   (ci->mode == CONFIG_MODE_RW) ? "RW" : "RO",
		   ci->name, ci->val);
	  
	  if (ci->type == CONFIG_TYPE_STR)
	    printf(" [+] (%2s) %-30s : %s\n", 
		   (ci->mode == CONFIG_MODE_RW) ? "RW" : "RO",
		   ci->name, (char *) ci->data);
	}
     printf("\n");
    }

  /* We change a configuration option */
  else
    {
      ci = hash_get(&aspectworld.config_hash, world.curjob->curcmd->param[0]);

      if (ci != NULL) 
	{
	  if (ci->mode == CONFIG_MODE_RO)
	    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
			 "Cannot update a READONLY key", (-1));
	  
	  if (ci->type == CONFIG_TYPE_INT)
	    config_update_key(ci->name, 
			      (void *) atoi(world.curjob->curcmd->param[1]));
	  
	  if (ci->type == CONFIG_TYPE_STR)
	    config_update_key(ci->name, 
			      (void *) world.curjob->curcmd->param[1]);
	  
	  printf(" [*] setting new configuration value for %s to %s\n\n",
		 world.curjob->curcmd->param[0],
		 world.curjob->curcmd->param[1]);
	} 
      else
	PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
		     "Configuration key not found.", (-1));
    }

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}
