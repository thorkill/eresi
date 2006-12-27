/*
 * ELFsh/vm configure related code
 * 2006 thorkill, Asgard Labs Inc.
 *
 * $Id: configure.c,v 1.2 2006-12-27 22:39:46 thor Exp $
 */

#include "elfsh.h"

int	cmd_configure()
{
  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);
  t_configitem *ci;
  int		cnt, idx;
  char		**tab;

  if (world.curjob->curcmd->argc < 2)
    {
      tab = hash_get_keys(&dbgworld.config_hash,&cnt);
      
      printf(" [*] Configure parameters\n\n");

      for (idx=0;idx<cnt;idx++) 
       {
         ci = hash_get(&dbgworld.config_hash,tab[idx]);

	 if (ci->type == ELFSH_CONFIG_TYPE_INT)
          printf(" [+] (%2s) %-30s : %d\n", 
	    				(ci->mode == ELFSH_CONFIG_MODE_RW) ? "RW" : "RO",
					ci->name,
					 ci->val);

	 if (ci->type == ELFSH_CONFIG_TYPE_STR)
	  printf(" [+] (%2s) %-30s : %s\n", 
  					(ci->mode == ELFSH_CONFIG_MODE_RW) ? "RW" : "RO",
					ci->name,
					(char *)ci->data);
       }
     printf("\n");
    }
  else
    {

      ci = hash_get(&dbgworld.config_hash,world.curjob->curcmd->param[0]);

      if (ci != NULL) {
       if (ci->mode == ELFSH_CONFIG_MODE_RO)
        ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__,
                                   "Read-only key, use designed api to change this value.", (-1));

       if (ci->type == ELFSH_CONFIG_TYPE_INT)
    	 elfsh_config_update_key(ci->name,atoi(world.curjob->curcmd->param[1]));

       if (ci->type == ELFSH_CONFIG_TYPE_STR)
        elfsh_config_update_key(ci->name,world.curjob->curcmd->param[1]);

       printf(" [*] setting new configuration value for %s to %s\n\n",
        world.curjob->curcmd->param[0],
        world.curjob->curcmd->param[1]
       );

      } else {
       ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__,
                                   "Configuration key not found.", (-1));
      }
    }

  ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}
