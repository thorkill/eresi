/**
 * (C) 2006-2008 Asgard Labs, thorolf
 *
 * @file init.c
 * @brief Initialization functions
 *
 */

#include "libmjollnir.h"

/**
 * @brief Initialize the world for libmjollnir 
 * @param sess The session structure
 */
int		mjr_init_session(mjrsession_t *sess) 
{
  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  hash_init(&goto_hash, "gotos"      , 100         , ASPECT_TYPE_STR);
  hash_init(&sess->ctx, "mjrcontexts", mjrHashSmall, ASPECT_TYPE_UNKNOW);

  /* register configurable parameters */
  config_add_item(MJR_CONFIG_BLOC_PREFIX,
		  CONFIG_TYPE_STR,
		  CONFIG_MODE_RW,
		  MJR_BLOC_PREFIX);
  config_add_item(MJR_CONFIG_BLOC_POSTFIX,
		  CONFIG_TYPE_STR,
		  CONFIG_MODE_RW,
		  MJR_BLOC_POSTFIX);
  config_add_item(MJR_CONFIG_FUNC_PREFIX,
		  CONFIG_TYPE_STR,
		  CONFIG_MODE_RW,
		  MJR_FUNC_PREFIX);
  config_add_item(MJR_CONFIG_LINK_PREFIX,
		  CONFIG_TYPE_STR,
		  CONFIG_MODE_RW,
		  MJR_LINK_PREFIX);

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, (1));
}

/**
 * @brief Change the current analysis context
 * @param sess The session structure
 * @param name The name of the session
 */
int		mjr_set_current_context(mjrsession_t *sess, char *name) 
{
  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);
  if ((sess->cur = hash_get(&sess->ctx,name)) == NULL)
    PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, (0));
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, (1));
}

/**
 * @brief Create a new analysis context 
 * @param sess The session structure
 * @param ctx The context structure
 */
int		mjr_add_context(mjrsession_t *sess, mjrcontext_t *ctx) 
{
 PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);
 hash_add(&sess->ctx, ctx->obj->name, ctx);
 PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, (1));
}

/**
 * @brief Create a context and set it as the current one 
 * @param sess The session strucutre
 * @param obj elfshell obj
 */
int		mjr_create_context_as_current(mjrsession_t *sess, elfshobj_t *obj) 
{
 mjrcontext_t	*ctx;

 PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);
 ctx = mjr_create_context(obj);
 mjr_add_context(sess, ctx);
 mjr_set_current_context(sess, obj->name);
 PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, (1));
}


/**
 * @brief Create a new context 
 * @param obj elf shell object
 */
mjrcontext_t	*mjr_create_context(elfshobj_t *obj) 
{
 mjrcontext_t	*ctx;

 PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);
 XALLOC(__FILE__, __FUNCTION__, __LINE__,ctx, sizeof(mjrcontext_t), NULL);
 bzero(ctx, sizeof(mjrcontext_t));
 ctx->obj = obj;
 hash_init(&ctx->funchash , "functions", mjrHashVerySmall, ASPECT_TYPE_FUNC);
 hash_init(&ctx->blkhash  , "blocks"   , mjrHashMedium    , ASPECT_TYPE_BLOC);
 hash_init(&ctx->linkhash  , "links"    , mjrHashMedium    , ASPECT_TYPE_LINK);
 XALLOC(__FILE__, __FUNCTION__, __LINE__, ctx->func_stack, sizeof(list_t), NULL);
 elist_init(ctx->func_stack, "funcpath", ASPECT_TYPE_FUNC);
 ctx->cntnrs_size = MJR_CNTNRS_INCREMENT;
 ctx->next_id = 1;
 ctx->block_btree = NULL;
 ctx->curfunc = NULL;
 mjr_init_containers(ctx);
 PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, (ctx));
}

/**
 * Virtualisation of the libasm initialization depending on the architecture 
 * @param sess Mjollnir session strucutre
 */
int		mjr_setup_processor(mjrsession_t *sess) 
{
 unsigned int         arch;

 arch =  elfsh_get_arch(sess->cur->obj->hdr);
 switch(arch)
   {
   case EM_SPARC:
   case EM_SPARCV9:
   case EM_SPARC32PLUS:
     asm_init_arch(&sess->cur->proc, ASM_PROC_SPARC);
     break;
   case EM_386:
     asm_init_arch(&sess->cur->proc, ASM_PROC_IA32);
     break;
   case EM_MIPS:
     asm_init_arch(&sess->cur->proc, ASM_PROC_MIPS);
     break;
   default:
     fprintf(D_DESC," [E] Libmjollnir unsupported architecture\n");
     return (-1);
   }
 return 1;
}
