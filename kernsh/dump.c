/*
* @file kernsh/dump.c
** @ingroup kernsh
**
*/
#include "kernsh.h"
#include "libkernsh.h"

int		cmd_kdump_get_vma()
{
  int		ret;
  char		buff[BUFSIZ];
  int           index, index2;
  listent_t     *actual, *second;
  list_t	*l;
  hash_t	*h;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  memset(buff, '\0', sizeof(buff));

  l = kernsh_kdump_get_vma(atoi(world.curjob->curcmd->param[0]));

  if (l == NULL)
    {
      PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
		   "Failed to get vma", -1);
    }

  for (index = 0, actual = l->head;
       index < l->elmnbr;
       index++, actual = actual->next)
    {
      h = (hash_t *) actual->data;
      printf("%s[%d]", l->name, index);
      if (h)
	{
	  printf("[%s] ", h->name);
	  for (index2 = 0; index2 < h->size; index2++)
	    {
	      for (second = &h->ent[index2];
		   second != NULL && second->key != NULL;
		   second = second->next)
		{
		  printf("[%s] => [%p] ",
			 second->key, second->data);
		}
	    }
	  printf("\n");
	}
    }

  snprintf(buff, sizeof(buff),
	   "Vma for pid %s is in list %s\n\n",
	   revm_colornumber("%u", atoi(world.curjob->curcmd->param[0])),
	   revm_colorstr(l->name));
  revm_output(buff);

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, ret);
}

int		cmd_kdump_vma()
{
  int		ret;
  char		buff[BUFSIZ];

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  if (world.curjob->curcmd->param[0])
    {
      ret = kernsh_kdump_vma(atoi(world.curjob->curcmd->param[0]));

      if (ret < 0)
	{
	  PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		       "Failed to dump vma", -1);
	}

      snprintf(buff, sizeof(buff),
               "Dumped into %s%s%s%s\n\n",
               revm_colorstr((char *) config_get_data(LIBKERNSH_CONFIG_STORAGE_PATH)),
	       revm_colorstr((char *) config_get_data(LIBKERNSH_CONFIG_DUMP_VMA_PREFIX)),
	       revm_colorstr("_"),
	       revm_colornumber("%u", atoi(world.curjob->curcmd->param[0])));
      revm_output(buff);
      snprintf(buff, sizeof(buff),
               "Check the metadata file %s%s%s%s/metadata*\n\n",
               revm_colorstr((char *) config_get_data(LIBKERNSH_CONFIG_STORAGE_PATH)),
	       revm_colorstr((char *) config_get_data(LIBKERNSH_CONFIG_DUMP_VMA_PREFIX)),
	       revm_colorstr("_"),
	       revm_colornumber("%u", atoi(world.curjob->curcmd->param[0])));
      revm_output(buff);
    }

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, ret);
}
