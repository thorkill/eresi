/*
** @file ondisk.c
** 
** @brief Implement routines to store and load analysis data on disk
**
** Started : Thu Jul 28 02:39:14 2003 jfv
**
*/
#include "libmjollnir.h"


/**
 * @brief Save and prepare a buffer which will be saved into the elfshobj.
 *
 * @param c a block container containing links to be stores
 * @param type 0 for inout/1 for output
 * @param buf buffer which contains the data
 * @return an offset where the data has been saved
 */
static int	mjr_flow_store_links(container_t *c, unsigned int type, mjrbuf_t *buf)
{
  unsigned int		nbr;
  listent_t	*cur;
  mjrlink_t	*curlink;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  /* Make sure we have correct parameters */
  switch (type)
    {
    case CONTAINER_LINK_IN:
      cur = c->inlinks->head;
      nbr = c->nbrinlinks;
      break;
    case CONTAINER_LINK_OUT:
      cur = c->outlinks->head;
      nbr = c->nbroutlinks;
      break;
    default:
      PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		   "Unknown link type", -1);  
    }
  
  /* Allocate the good size for ondisk buffer */
  if (!buf->data || !buf->allocated)
    {
      buf->allocated = nbr * sizeof(unsigned int) * 2;
      XALLOC(__FILE__, __FUNCTION__, __LINE__, buf->data, buf->allocated, -1);
    }
  else if (buf->allocated)
    {
      buf->allocated += nbr * sizeof(unsigned int) * 2;
      XREALLOC(__FILE__, __FUNCTION__, __LINE__, 
	       buf->data, buf->data, buf->allocated, -1);
    }

  /* Iterate over all links */
  while (cur)
    {
      curlink = (mjrlink_t *) cur->data;

#if __DEBUG_ONDISK__
      fprintf(D_DESC,"[D] %s.%d: stored dep id:%d dir:%d cid:%d type:%d\n",
	      __FUNCTION__, __LINE__, c->id, type, curlink->id, curlink->type);
#endif

      memcpy(buf->data + buf->maxlen, (char *) &curlink->id , sizeof(unsigned int));
      buf->maxlen += sizeof(unsigned int);
      memcpy(buf->data + buf->maxlen, (char *) &curlink->type, sizeof(unsigned char));
      buf->maxlen++;
      memcpy(buf->data + buf->maxlen, (char *) &curlink->scope, sizeof(unsigned char));
      buf->maxlen++;
      
      cur = cur->next;
    }
  
  buf->counter++;
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, buf->maxlen);
}




/**
 * @brief Link containers in the control flow graph
 */
static int	mjr_flow_load_links(mjrcontext_t	*ctxt, 
				    container_t		*container, 
				    unsigned int		linktype,
				    char		*sectdata,
				    unsigned int		*curoff)
{
  unsigned int		off;
  unsigned int		tmpid;
  unsigned char	tmptype;
  unsigned char	tmpscope;
  unsigned int		tmpnbr;
  unsigned int		findex;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  /* Make sure we deal with the right list of links */
  switch (linktype)
    {
    case CONTAINER_LINK_IN:
      off    = (unsigned int) container->inlinks;
      tmpnbr = container->nbrinlinks;
      container_linklists_create(container, CONTAINER_LINK_IN);
      break;
    case CONTAINER_LINK_OUT:
      off   = (unsigned int) container->outlinks;
      tmpnbr = container->nbroutlinks;
      container_linklists_create(container, CONTAINER_LINK_OUT);
      break;
    default:
      PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
		   "Invalid link type", -1);
    }

  /* Recreate each link */
  for (findex = 0; findex < tmpnbr; findex++)
    {	 
      tmpid     = *(unsigned int *) ((char *) sectdata + off + *curoff);
      *curoff  += sizeof (unsigned int);
      tmptype   = *(unsigned char *) sectdata + off + *curoff;
      *curoff  += sizeof (unsigned char);
      tmpscope  = *(unsigned char *) sectdata + off + *curoff;
      *curoff  += sizeof (unsigned char);
      
#if __DEBUG_ONDISK__
      fprintf(D_DESC," [__DEBUG_ONDISK__] Restored link: (%d/%d) sid:%u did:%u type:%u scope:%u\n", 
	      findex, tmpnbr, container->id, tmpid, tmptype, tmpscope);
#endif
      
      /* This function increments linklist->elmnbr */
      if (tmpid)
	mjr_container_add_link(ctxt, container, tmpid, tmptype, tmpscope, linktype);
    }

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}






/**
 * @brief Create the data dump to be saved in file 
 */
static int		mjr_unit_save(container_t *cur, mjrbuf_t *buf, unsigned int typeid)
{
  void			*curunit;
  container_t		*container;
  unsigned int			unitsize;
  eresi_Addr		addr;
  unsigned int			size;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  /* Here check what kind of structures we are saving */
  switch (typeid)
    {
    case ASPECT_TYPE_BLOC:
      unitsize = sizeof(mjrblock_t);
      break;
    case ASPECT_TYPE_FUNC:
      unitsize = sizeof(mjrfunc_t);
      break;
    default:
      PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
		   "Invalid input type id", 0);
    }

  /* At this points, no new unit allocation should be done */
  curunit = cur->data;

  /* This is why addr and size fields should always be in first in mjrblock/mjrfunc */
  addr    = *(eresi_Addr *) curunit;
  size    = *(unsigned int      *) ((char *) curunit + sizeof(eresi_Addr));
  
#if __DEBUG_ONDISK__
  fprintf(D_DESC," [*] Saving data unit %s\n", buf);
#endif

  /* Else insert the block in the global buffer for the .control section */
  if (!buf->data) 
    {
      buf->allocated = getpagesize();
      XALLOC(__FILE__, __FUNCTION__, __LINE__, buf->data, buf->allocated, -1);
      buf->maxlen = 0;
    }
  else if (buf->allocated  < (buf->maxlen + unitsize + sizeof(container_t)))
    {
      buf->allocated += getpagesize();
      XREALLOC(__FILE__, __FUNCTION__, __LINE__, 
	       buf->data, buf->data, buf->allocated, -1);
    }
  
  /* Unit copy in the buffer */
  container = (container_t *) ((char *) buf->data + buf->maxlen);
  curunit   = (char *) buf->data + buf->maxlen + sizeof(container_t);

  memcpy(container, cur, sizeof(container_t));
  memcpy(curunit, cur->data, unitsize);

  /* Return result */
  buf->maxlen += sizeof(container_t);
  buf->maxlen += unitsize;
  buf->counter++;
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}








/**
 * @brief Create the control flow graph using the information stored in .elfsh.control 
 *
 * @return Returns the number of saved data units (0 probably means something is wrong)
 */
int			mjr_flow_load(mjrcontext_t *ctxt, unsigned int typeid)
{
  int                   index, cnt, done;
  elfshsect_t           *sect, *flowsect;
  char			name[20];
  char			**keys;
  container_t		*container;
  container_t		*tmpcntnr;
  unsigned int			flowdone;
  unsigned int			unitsize;
  unsigned int			unitnbr;
  hash_t		*table;
  void			*curunit;
  void			*tmpunit;
  char			*debugsect1;
  char			*debugsect2;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  /* Here check what kind of structures we are saving */
  switch (typeid)
    {
    case ASPECT_TYPE_BLOC:
      unitsize   = sizeof(mjrblock_t);
      table      = &ctxt->blkhash;
      debugsect1 = ELFSH_SECTION_NAME_EDFMT_BLOCKS;
      debugsect2 = ELFSH_SECTION_NAME_EDFMT_BCONTROL;
      break;
    case ASPECT_TYPE_FUNC:
      unitsize   = sizeof(mjrfunc_t);
      table      = &ctxt->funchash;
      debugsect1 = ELFSH_SECTION_NAME_EDFMT_FUNCTIONS;
      debugsect2 = ELFSH_SECTION_NAME_EDFMT_FCONTROL;
      break;
    default:
      PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
		   "Invalid input type id", 0);
    }

  /* If mjr_flow_load() was already called, just return now */
  cnt = hash_size(table);
  if (cnt != 0)
    PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, cnt);

  /* Preliminary checks */
  sect = elfsh_get_section_by_name(ctxt->obj, debugsect1, 0, 0, 0);
  if (!sect)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
		      "No control flow section : use analyse command", 0);
  if (sect->shdr->sh_size % (unitsize + sizeof(container_t)))
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
		      "Corrupted edfmt section : modulo-test failed", 0);
  flowsect = elfsh_get_section_by_name(ctxt->obj, debugsect2, 0, 0, 0);
  if (!flowsect)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
		 "No control flow analysis section found (use analyse command)", 0);
    
  /* First pass : Iterate on the control flow section to find blocks */
  /* We do not create new data units but use the data from the section */
  unitnbr = sect->shdr->sh_size / (sizeof(container_t) + unitsize);
  for (done = index = 0; index < unitnbr; index++)
    {
      
      /* Ondisk format ondisk is the exact same format that we manipulate
	 in this lib. So its very practical to reload/store the info ondisk */
      container  = (container_t *) ((char *) sect->data + done);
      done      += sizeof(container_t);
      curunit    = (char *) sect->data + done;
      done      += unitsize;

      XALLOC(__FILE__, __FUNCTION__, __LINE__, tmpunit, unitsize, 0);
      XALLOC(__FILE__, __FUNCTION__, __LINE__, tmpcntnr, sizeof(container_t), 0);

      memcpy(tmpcntnr, container, sizeof(container_t));
      memcpy(tmpunit, curunit, unitsize);
      tmpcntnr->data = tmpunit;

      /* Because of this line, we need to make sure the addr is always in first in all structs */
      snprintf(name, sizeof(name), AFMT, *(eresi_Addr *) tmpunit);
      
#if __DEBUG_ONDISK__
      fprintf(D_DESC,"[__DEBUG_ONDISK__] %s: add new unit name: %s \n", __FUNCTION__, name);
#endif

      mjr_register_container_id(ctxt, tmpcntnr);
      hash_add(table, _vaddr2str(*(eresi_Addr *) tmpunit), tmpcntnr);
    }

  /* Relink containers in this second run */
  keys = hash_get_keys(table, &done);
  for (index = 0; index < done; index++)
    {
      container = hash_get(table, keys[index]);
      flowdone  = 0;
      mjr_flow_load_links(ctxt, container, CONTAINER_LINK_IN , flowsect->data, &flowdone);
      mjr_flow_load_links(ctxt, container, CONTAINER_LINK_OUT, flowsect->data, &flowdone);
    }

  /* Prevent double analysis */
  if (typeid == ASPECT_TYPE_BLOC)
    ctxt->analysed = 1;
  cnt = hash_size(table);
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, cnt);
}



/**
 * @brief Store the blocks and functions on disk
 *
 * This loops on all block/func, and call mjr_unit_save() on each element
 *
 */
int			mjr_flow_store(mjrcontext_t *ctxt, unsigned int typeid) 
{
  elfsh_Shdr		shdr;
  elfshsect_t		*sect;
  mjrbuf_t		buf, cfbuf;
  container_t		*container;
  int			err;
  char			**keys;
  int			keynbr;
  int			index;
  int			flow_off_in, flow_off_out;
  list_t		*tmpin, *tmpout;
  hash_t		*table;
  char			*debugsect1;
  char			*debugsect2;
  void			*curunit;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  /* Here check what kind of structures we are saving */
  switch (typeid)
    {
    case ASPECT_TYPE_BLOC:
      table      = &ctxt->blkhash;
      debugsect1 = ELFSH_SECTION_NAME_EDFMT_BLOCKS;
      debugsect2 = ELFSH_SECTION_NAME_EDFMT_BCONTROL;
      break;
    case ASPECT_TYPE_FUNC:
      table      = &ctxt->funchash;
      debugsect1 = ELFSH_SECTION_NAME_EDFMT_FUNCTIONS;
      debugsect2 = ELFSH_SECTION_NAME_EDFMT_FCONTROL;
      break;
    default:
      PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
		   "Invalid input type id", 0);
    }  

  /* Remove previous control sections if any */
  sect = elfsh_get_section_by_name(ctxt->obj, debugsect1, 0, 0, 0);
  if (sect)
    elfsh_remove_section(ctxt->obj, debugsect1);
  sect = elfsh_get_section_by_name(ctxt->obj, debugsect2, 0, 0, 0);
  if (sect)
    elfsh_remove_section(ctxt->obj, debugsect2);
	
  /* Initialize data buffer */
  buf.allocated = 0;
  buf.maxlen    = 0;
  buf.counter   = 0;
  buf.data      = 0;
  buf.obj       = ctxt->obj;

  /* Initialize data buffer */
  cfbuf.allocated = 0;
  cfbuf.maxlen    = 0;
  cfbuf.counter   = 0;
  cfbuf.data      = 0;
  cfbuf.obj       = ctxt->obj;

  flow_off_in = flow_off_out = 0;

  /* Iteratively save all data units */
  keys = hash_get_keys(table, &keynbr);
  for (index = 0; index < keynbr; index++)
    {
      tmpin    = tmpout = NULL;
      container = hash_get(table, keys[index]);

#if __DEBUG_ONDISK__
      switch (typeid)
	{
	case ASPECT_TYPE_BLOC:
	  mjr_block_dump(ctxt, container);
	  break;
	case ASPECT_TYPE_FUNC:
	  mjr_function_dump(ctxt, (char *) __FUNCTION__, container);
	  break;
	default:
	  break;
	}
#endif

      /* We update the number of links stored ondisk */
      container->nbrinlinks  = container->inlinks->elmnbr;
      container->nbroutlinks = container->outlinks->elmnbr;

      /* Setting references to link information inside the file */
      tmpin               = container->inlinks;
      tmpout              = container->outlinks;
      container->inlinks  = (list_t *) flow_off_in;
      container->outlinks = (list_t *) flow_off_out;
      mjr_unit_save(container, &buf, typeid);

      /* Restore list pointers */
      container->inlinks  = tmpin;
      container->outlinks = tmpout;

#if __DEBUG_ONDISK__
      fprintf(D_DESC, " [D] %s: IN  Offset: %d\n", __FUNCTION__, flow_off_in);
      fprintf(D_DESC, " [D] %s: OUT Offset: %d\n", __FUNCTION__, flow_off_out);
#endif

      /* Store block flow and get new offsets */
      flow_off_in  = mjr_flow_store_links(container, CONTAINER_LINK_IN, &cfbuf);
      flow_off_out = mjr_flow_store_links(container, CONTAINER_LINK_OUT, &cfbuf);
      if (flow_off_in < 0 || flow_off_out < 0)
	PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
		     "Unable to save flow analysis information", 0);
      curunit = container->data;

      /* This is just a help code, it does not do really
	 anything and it kept for intelligence purpose */
      if (typeid == ASPECT_TYPE_BLOC && mjr_block_funcstart(container))
	{
	  container = mjr_function_get_by_vaddr(ctxt, *(eresi_Addr *) curunit);

	  /* Can happens rarely - should not be fatal */
	  if (container == NULL || container->data == NULL)
	    {
#if __DEBUG_ONDISK__
	      fprintf(stderr, " [*] Failed to find parent function at 0x%08lx \n", 
		      (unsigned long) *(eresi_Addr *) curunit);
#endif
	      continue;
	    }

#if __DEBUG_ONDISK__
	  fprintf(stderr, " [*] Found block start for function 0x%08lx \n", 
		  (unsigned long) *(eresi_Addr *) curunit);
#endif

	}
    }

  /* Create control section */
  sect = elfsh_create_section(debugsect1);
  shdr = elfsh_create_shdr(0, SHT_PROGBITS, 0, 0, 0, buf.maxlen, 0, 0, 0, 0);

  fprintf(stderr, " [*] Saving %s section of %u bytes \n", debugsect1, buf.maxlen);

  err = elfsh_insert_unmapped_section(ctxt->obj, sect, shdr, buf.data);
  if (err < 0)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		 "Unable to save edfmt section1", -1);
  
  sect = elfsh_create_section(debugsect2);
  shdr = elfsh_create_shdr(0, SHT_PROGBITS, 0, 0, 0, cfbuf.maxlen, 0, 0, 0, 0);

  fprintf(stderr, " [*] Saving %s section of %u bytes\n", debugsect2, cfbuf.maxlen);

  err = elfsh_insert_unmapped_section(ctxt->obj, sect, shdr, cfbuf.data);
  if (err < 0)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
		 "Unable to save edfmt section2", -1);

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, buf.counter);
}

