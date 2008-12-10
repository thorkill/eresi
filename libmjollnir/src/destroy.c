/*
** (C) 2006-2008 The ERESI team
**
** @file destroy.c
** @brief Cleaning API for removing previously saved analysis.
*/
#include "libmjollnir.h"


/** Remove the whole content of as hash table of containers */
void		mjr_hash_destroy(hash_t *hash, u_char haslists)
{
  char		**keys;
  int		nbr;
  u_int		index;
  container_t	*data;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);
  keys = hash_get_keys(hash, &nbr);
  for (index = 0; index < nbr; index++)
    {
      data = (container_t *) hash_get(hash, keys[index]);
      if (haslists)
	{
	  elist_destroy(data->inlinks);
	  elist_destroy(data->outlinks);
	}
      hash_del(hash, keys[index]);
      XFREE(__FILE__, __FUNCTION__, __LINE__, data);
      XFREE(__FILE__, __FUNCTION__, __LINE__, keys[index]);
    }
  hash->elmnbr = 0;
  XFREE(__FILE__, __FUNCTION__, __LINE__, keys);
  PROFILER_OUT(__FILE__, __FUNCTION__, __LINE__);
}


/** Remove existing control flow analysis stored information */
void		mjr_analyse_destroy(mjrcontext_t *ctx)
{
  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);
  elfsh_remove_section(ctx->obj, ELFSH_SECTION_NAME_EDFMT_BLOCKS);
  elfsh_remove_section(ctx->obj, ELFSH_SECTION_NAME_EDFMT_BCONTROL);
  elfsh_remove_section(ctx->obj, ELFSH_SECTION_NAME_EDFMT_FUNCTIONS);
  elfsh_remove_section(ctx->obj, ELFSH_SECTION_NAME_EDFMT_FCONTROL);
  ctx->func_stack = elist_empty(ctx->func_stack->name);
  ctx->next_id = 1;
  ctx->block_btree = NULL;
  ctx->curfunc = NULL;
  ctx->curblock = NULL;
  ctx->cntnrs_size = MJR_CNTNRS_INCREMENT;
  ctx->calls_seen = ctx->calls_found = 0;
  bzero(&ctx->hist, sizeof(mjrhistory_t) * MJR_HISTORY_LEN);
  mjr_hash_destroy(&ctx->funchash, 1);
  mjr_hash_destroy(&ctx->blkhash, 1);
  mjr_hash_destroy(&ctx->linkhash, 0);
  XFREE(__FILE__, __FUNCTION__, __LINE__, ctx->reg_containers);
  mjr_init_containers(ctx);
  PROFILER_OUT(__FILE__, __FUNCTION__, __LINE__);
}
