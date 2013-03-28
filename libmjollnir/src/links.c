/**
* @file libmjollnir/src/links.c
 * @ingroup libmjollnir
 *
 * @brief All the functions that deal with linking other objects, such
 * as functions, blocks, or others.
 *
 * Started on Fri Jun 22 2007 jfv
 */

#include <libmjollnir.h>



/**
 * @brief Link function layer
 * @param ctxt mjollnir context structure
 * @param str source address
 * @param dst destination address
 * @param ret return address
 */
int                     mjr_link_func_call(mjrcontext_t *ctxt,
                                           eresi_Addr src,
                                           eresi_Addr dst,
                                           eresi_Addr ret)
{
  container_t           *fun;
  char                  *tmpstr;
  eresi_Addr            tmpaddr;
  elfshsect_t           *dstsect;
  u_char                scope;
  u_char		isnew;

#if __DEBUG_FLOW__
  mjrfunc_t             *tmpfunc;
  mjrfunc_t             *tmpfunc2;
  //char                  *md5;
#endif

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

#if __DEBUG_LINKS__
  fprintf(D_DESC, "[D] %s: src:"XFMT" dst:"XFMT" ret:"XFMT"\n",
          __FUNCTION__, src, dst, ret);
#endif

  /* Check if we are not pointing into BSS */
  dstsect = elfsh_get_parent_section(ctxt->obj, dst, NULL);
  if (!dstsect || !dstsect->data)
    PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
  scope = (!dstsect || !strcmp(dstsect->name, ELFSH_SECTION_NAME_PLT) ?
           MJR_LINK_SCOPE_GLOBAL : MJR_LINK_SCOPE_LOCAL);

  /* Link/Prepare function layer. We use an intermediate variable, else
     the compiler optimize too hard and that make segfault (bug in gcc ?) */
  tmpaddr = dst;
  fun = mjr_function_get_by_vaddr(ctxt, tmpaddr);
  if (!fun)
    {
      tmpstr = _vaddr2str(tmpaddr);
      fun = mjr_create_function_container(ctxt, tmpaddr, 0,
                                          tmpstr, NULL, NULL);
      mjr_function_register(ctxt, tmpaddr, fun);
      mjr_function_symbol(ctxt, fun);
      isnew = 1;
    }
  else
    isnew = 0;

  /* Add links between functions */
  if (ctxt->curfunc)
    {
#if __DEBUG_FLOW__
      tmpfunc = (mjrfunc_t *) ctxt->curfunc->data;
      tmpfunc2 = (mjrfunc_t *) fun->data;
      fprintf(stderr, " [*] Linking function " XFMT " to func " XFMT "\n",
              tmpfunc->vaddr, tmpfunc2->vaddr);
#endif
      mjr_container_add_link(ctxt, fun, ctxt->curfunc->id,
                             MJR_LINK_FUNC_RET, scope, CONTAINER_LINK_IN);
      mjr_container_add_link(ctxt, ctxt->curfunc, fun->id,
                             MJR_LINK_FUNC_CALL, scope, CONTAINER_LINK_OUT);
    }

  /* Fingerprint function */
  /*
  ** XXX: this segfaults on 64bits architecture - MAX_FUNC_LEN should
  ** not be used. Instead, we should fingerprint all functions with
  ** their infered size at the moment of saving debug sections.
  **
  md5 = mjr_fingerprint_function(ctxt, tmpaddr, MJR_FPRINT_TYPE_MD5);
  tmpfunc = (mjrfunc_t *) fun->data;
  if (md5)
    memcpy(tmpfunc->md5, md5, sizeof(tmpfunc->md5));
  */

#if __DEBUG_FLOW__
  tmpfunc = fun->data;
#endif

  if (scope == MJR_LINK_SCOPE_LOCAL && isnew)
    {
      elist_push(ctxt->func_stack, fun);
      ctxt->curfunc = fun;
#if __DEBUG_FLOW__
      fprintf(stderr, " [*] New CURFUNC @ %s \n", tmpfunc->name);
    }
  else if (!isnew)
    fprintf(stderr, " ******** ALREADY SEEN FUNCTION : NOT CHANGING CURFUNC @ %s \n", tmpfunc->name);
  else
    fprintf(stderr, " ******** GLOBAL FUNCTION CALL : NOT CHANGING CURFUNC @ %s \n", tmpfunc->name);
#else
    }
#endif

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}


/**
 * @brief Create a link between blocks on a call
 */
int                     mjr_link_block_call(mjrcontext_t *ctxt,
                                            eresi_Addr src,
                                            eresi_Addr dst,
                                            eresi_Addr ret)
{
  container_t           *csrc,*cdst,*cret;
  elfshsect_t           *dstsect;
  elfshsect_t		*retsect;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

#if __DEBUG_LINKS__
  fprintf(D_DESC,"[D] %s: linking " AFMT " CALL " AFMT " RET " AFMT "\n",
          __FUNCTION__, src, dst, ret);
#endif

  if (dst < 0)
    {
#if __DEBUG_LINKS__
      fprintf(D_DESC,"[D] %s: dst < 0: "XFMT"\n",
              __FUNCTION__, ret);
#endif
      dst = src + dst;
    }

  /* Check if we are not pointing into BSS */
  dstsect = elfsh_get_parent_section(ctxt->obj, dst, NULL);
  retsect = elfsh_get_parent_section(ctxt->obj, ret, NULL);

  /* at this point we must have src block */
  csrc = mjr_block_get_by_vaddr(ctxt, src, MJR_BLOCK_GET_FUZZY);

#if __DEBUG_LINKS__
  fprintf(stderr, "[D] Calling location source " XFMT "\n", src);
#endif

  assert(csrc != NULL);
  mjr_block_symbol(ctxt, csrc);

  /* link src and dst */
  if (dst != MJR_BLOCK_INVALID && dstsect && dstsect->data)
    {
      cdst = mjr_block_split(ctxt, dst, MJR_LINK_FUNC_CALL);
      if (!cdst)
        PROFILER_ERR(__FILE__,__FUNCTION__,__LINE__,
                     "Could not split the dst", 0);

      mjr_container_add_link(ctxt, csrc, cdst->id,
                             MJR_LINK_FUNC_CALL, MJR_LINK_SCOPE_GLOBAL, CONTAINER_LINK_OUT);
      mjr_container_add_link(ctxt, cdst, csrc->id,
                             MJR_LINK_FUNC_CALL, MJR_LINK_SCOPE_GLOBAL, CONTAINER_LINK_IN);
    }

  /* Link src and ret at the block level */
  if (ret != MJR_BLOCK_INVALID && retsect && retsect->data)
    {
      cret = mjr_block_split(ctxt, ret, MJR_LINK_FUNC_RET);
      if (!cret)
        PROFILER_ERR(__FILE__,__FUNCTION__,__LINE__,
                     "Could not split the ret", 0);

      mjr_container_add_link(ctxt, csrc, cret->id,
                             MJR_LINK_TYPE_DELAY, MJR_LINK_SCOPE_LOCAL, CONTAINER_LINK_OUT);
      mjr_container_add_link(ctxt, cret, csrc->id,
                             MJR_LINK_TYPE_DELAY, MJR_LINK_SCOPE_LOCAL, CONTAINER_LINK_IN);
    }

#if __DEBUG_BLOCKS__
  mjr_block_dump(ctxt,csrc);
  if (dst != MJR_BLOCK_INVALID)
    mjr_block_dump(ctxt,cdst);
  mjr_block_dump(ctxt,cret);
#endif

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 1);
}


/**
 * @brief This function does prepare linking of blocks on conditional jumps
 */
int             mjr_link_block_jump(mjrcontext_t *ctxt,
                                    eresi_Addr   src,
                                    eresi_Addr   dst,
                                    eresi_Addr   ret)
{
  container_t   *csrc, *cdst, *cret;
  mjrblock_t	*bsrc;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

#if __DEBUG_LINKS__
  fprintf(D_DESC,"[D] %s: linking JMP "XFMT" TRUE "XFMT" FALSE "XFMT"\n",
          __FUNCTION__, src, dst, ret);
#endif

  /* Insert symbol on terminated block */
  csrc = mjr_block_get_by_vaddr(ctxt, src, MJR_BLOCK_GET_FUZZY);

  /* Get block data */
  bsrc = (mjrblock_t *) csrc->data;

  /* If TRUE < SRC: split it and set the new one as csrc */
  if (dst > bsrc->vaddr && dst <= src)
    {
      csrc = mjr_block_split(ctxt, src, MJR_LINK_BLOCK_COND_ALWAYS);
    }

  if (!csrc)
    {
      printf("missing block at address " XFMT "\n", src);
      assert(csrc != NULL);
    }

  mjr_block_symbol(ctxt, csrc);

  /* Now split destination blocks */
  cdst = mjr_block_split(ctxt, dst, MJR_LINK_BLOCK_COND_ALWAYS);
  if (!cdst)
    PROFILER_ERR(__FILE__,__FUNCTION__,__LINE__,
                 "Could not split destination block", 0);

  if (ret != MJR_BLOCK_INVALID)
    {
      cret = mjr_block_split(ctxt, ret, MJR_LINK_BLOCK_COND_ALWAYS);
      if (!cret)
        PROFILER_ERR(__FILE__,__FUNCTION__,__LINE__,
                     "Could not split return block", 0);
    }
  else
    cret = NULL;

  mjr_container_add_link(ctxt, csrc, cdst->id,
                         MJR_LINK_BLOCK_COND_TRUE, MJR_LINK_SCOPE_LOCAL, CONTAINER_LINK_OUT);
  mjr_container_add_link(ctxt, cdst, csrc->id,
                         MJR_LINK_BLOCK_COND_TRUE, MJR_LINK_SCOPE_LOCAL, CONTAINER_LINK_IN);


#if __DEBUG_LINKS__
  fprintf(D_DESC,"[D] %s: cret: "XFMT"\n",
          __FUNCTION__, ret);
#endif  

  if (cret)
    {
      mjr_container_add_link(ctxt, csrc, cret->id,
                             MJR_LINK_BLOCK_COND_FALSE, MJR_LINK_SCOPE_LOCAL, CONTAINER_LINK_OUT);
      mjr_container_add_link(ctxt, cret, csrc->id,
                             MJR_LINK_BLOCK_COND_FALSE, MJR_LINK_SCOPE_LOCAL, CONTAINER_LINK_IN);
    }

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 1);
}



/* Those functions are static and should not be used by external API */



/**
 * @brief Update link information when splitting a block
 */
static int      mjr_block_relink(mjrcontext_t *ctx,
                                 container_t *src,
                                 container_t *dst,
                                 int direction)
{
  list_t        *linklist;
  listent_t     *curent;
  listent_t     *savednext;
  mjrlink_t     *lnk;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);
  linklist = mjr_link_get_by_direction(src, direction);

#if __DEBUG_LINKS__
  fprintf(D_DESC, "[D] Relinking blocks %s: src:%d dst:%d dir:%d\n",
          __FUNCTION__, src->id, dst->id, direction);
#endif

  /* First same links to dest, then remove links from source */
  for (curent = linklist->head; curent; curent = savednext)
    {
      savednext = curent->next;
      lnk = (mjrlink_t *) curent->data;
      mjr_container_add_link(ctx, dst, lnk->id, lnk->type, lnk->scope, direction);
      elist_del(linklist, curent->key);
    }

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 1);
}




/**
 * @brief This function does split a block carried by given container
 * to 2 pieces, a new container will be added by vaddr dst
 *
 * @param ctxt mjollnir context strucutre
 * @param dst  destination address of wanted block
 * @param link_with link splitted blocks with specified link type
 */
container_t             *mjr_block_split(mjrcontext_t   *ctxt,
                                         eresi_Addr     dst,
                                         u_char         link_with)
{
  container_t           *tmpdst,*dstend;
  mjrblock_t            *blkdst;
  int                   new_size;
  elfsh_Sym             *sym;
  int			symoff;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  /* Check if we need to create a new bloc for destination addr */
  tmpdst = mjr_block_get_by_vaddr(ctxt, dst, MJR_BLOCK_GET_FUZZY);
  if (!tmpdst)
    {
      tmpdst = mjr_create_block_container(ctxt, 0, dst, 0, 0);
      hash_add(&ctxt->blkhash, _vaddr2str(dst), tmpdst);
      symoff = mjr_block_symbol(ctxt, tmpdst);
      ((mjrblock_t *) tmpdst->data)->symoff = symoff;
      PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, tmpdst);
    }

  /* Or we might find an existing symbol and split existing bloc */
  blkdst = (mjrblock_t *) tmpdst->data;
  sym = elfsh_get_symbol_by_value(ctxt->obj, blkdst->vaddr, NULL, ELFSH_EXACTSYM);
  assert(sym != NULL);

#if __DEBUG_LINKS__
  fprintf(D_DESC,"[D] %s:%d: wanted dst:"XFMT" got:"XFMT"\n",
          __FUNCTION__, __LINE__, dst, blkdst->vaddr);
#endif

  /* Recompute sizes */
  if (blkdst->vaddr != dst)
    {
      new_size = blkdst->size - (dst - blkdst->vaddr);

#if __DEBUG_LINKS__
      fprintf(D_DESC,"[D] %s:%d: new_size %d for "XFMT"\n", __FUNCTION__, __LINE__, new_size, dst);
      fprintf(D_DESC,"[D] %s:%d: turncate "XFMT" to %d\n",
              __FUNCTION__, __LINE__, blkdst->vaddr, blkdst->size);
#endif

      blkdst->size -= new_size;
      sym->st_size = blkdst->size;

      assert(blkdst->size > 0);

      /* Correct existing symbol size and add new symbol pointing on new split block */
      dstend = mjr_create_block_container(ctxt, 0, dst, new_size, (new_size ? 1 : 0));
      hash_add(&ctxt->blkhash, _vaddr2str(dst), dstend);
      mjr_block_symbol(ctxt, dstend);
      mjr_block_relink(ctxt, tmpdst, dstend, CONTAINER_LINK_OUT);
      mjr_container_add_link(ctxt, tmpdst, dstend->id, MJR_LINK_BLOCK_COND_ALWAYS,
                             MJR_LINK_SCOPE_LOCAL, CONTAINER_LINK_OUT);
      mjr_container_add_link(ctxt, dstend, tmpdst->id, MJR_LINK_BLOCK_COND_ALWAYS,
                             MJR_LINK_SCOPE_LOCAL, CONTAINER_LINK_IN);
    }
  else
    dstend = tmpdst;

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, (dstend));
}
