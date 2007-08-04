/*
 * 2001-2007 Devhell Labs
 *
 * @file containers.c
 * @brief An API for expression containers in REVM
 *
 * Started Aug 4 2007 04:17 jfv
 * $Id: containers.c,v 1.1 2007-08-04 04:00:45 may Exp $
 */
#include "revm.h"

/**
 * @brief Creates an expression container
 */
mjrcontainer_t		*revm_exprcontainer_create(revmexpr_t *expr)
{
  mjrcontainer_t	*newcntnr;
  aspectype_t		*instrtype;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  /* Preliminary checks */
  instrtype = aspect_type_get_by_name("revmexpr_t");
  if (!instrtype)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		 "Unable to get instruction type", NULL);

  XALLOC(__FILE__, __FUNCTION__, __LINE__, 
	 newcntnr, sizeof(mjrcontainer_t), NULL);
  
  newcntnr->data = expr;
  newcntnr->type = instrtype->type;
  mjr_create_container_linklist(newcntnr, MJR_LINK_IN);
  mjr_create_container_linklist(newcntnr, MJR_LINK_OUT);

  /* fill the btree */
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, newcntnr);
}
