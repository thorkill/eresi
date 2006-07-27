
/*
 * (C) 2006 Asgard Labs, thorolf
 * BSD License
 * $Id: init.c,v 1.5 2006-07-27 16:50:53 thor Exp $
 *
 */

#include "libmjollnir.h"

/*
 Init stuff
 */

int mjr_init_session(mjrSession *sess) {

 ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);

 hash_init(&sess->ctx,mjrHashSmall);

 ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, (1));

}

int mjr_set_current_context(mjrSession *sess, char *name) {

 ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);

 if ((sess->cur = hash_get(&sess->ctx,name)) == NULL)
  ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, (0));

 ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, (1));

}

int mjr_add_context(mjrSession *sess, mjrContext *ctx) {

 ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);

 hash_add(&sess->ctx,ctx->obj->name,ctx);

 ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, (1));
 
}


int mjr_create_context_as_current(mjrSession *sess, elfshobj_t *obj) {

 mjrContext *ctx;

 ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);

 ctx = mjr_create_context(obj);
 mjr_add_context(sess,ctx);
 mjr_set_current_context(sess,obj->name);

 ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, (1));

}

mjrContext * mjr_create_context(elfshobj_t *obj) {

 mjrContext *ctx;

 ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);
 
 XALLOC(ctx,sizeof(mjrContext),NULL);

 ctx->obj = obj;
 ctx->curVaddr = 0;
 ctx->analysed_calls = 0;
 ctx->st_calls_seen = 0;
 ctx->st_calls_found = 0;
 hash_init(&ctx->blocks,mjrHashLarge);

 ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, (ctx));
}

int mjr_setup_processor(mjrSession *sess) {
 u_int         arch;
 arch =  elfsh_get_arch(sess->cur->obj->hdr);
 switch(arch)
 {
  case EM_SPARC:
//   asm_init_sparc(&sess->proc);
   break;
  case EM_386:
   asm_init_i386(&sess->cur->proc);
   break;
  default:
   printf("unsupported architecture\n");
   return (-1);
 }

 return 1;
}
