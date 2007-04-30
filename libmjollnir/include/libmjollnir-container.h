/*
 * 2001-2007 Devhell Labs
 * 2006-2007 Asgard Labs, thorolf
 * 2007      Rfdlabs, Strauss 
 *
 * Container structures for libmjollnir
 *
 * $Id: libmjollnir-container.h,v 1.8 2007-04-30 11:54:13 thor Exp $
 *
 */

#if !defined(__MJR_CONTAINER__)
 #define __MJR_CONTAINER__ 1

#include "libelfsh.h"

#define MJR_CNTNRS_INCREMENT 	200
#define MJR_MAX_INCREMENTS	200

#define MJR_LINK_IN		0
#define MJR_LINK_OUT		1

/*
 * struct s_link is to reference links between functions or blocks
 * field type may help us to analyse blocks to build higher
 * logical structures. It contains :
 *
 * - a type field specifying which kind of link this is
 * - an id field to reference the destination container of this link
 * - a pointer to the next link in this list of links
 */
typedef	struct		s_link 
{
#define MJR_LINK_FUNC_CALL		0 /* a call between functions	*/
#define MJR_LINK_FUNC_RET		1 /* a function returning control */
#define MJR_LINK_BLOCK_COND_TRUE	2 /* 'true' condition of a branch */
#define MJR_LINK_BLOCK_COND_FALSE	3 /* 'false' condition of a branch */
#define MJR_LINK_BLOCK_COND_ALWAYS	4 /* uncoditional branch */
#define MJR_LINK_DELETE			5 /* internal mark for link delete */
#define MJR_LINK_TYPE_DELAY		6 /* should be ignored on 
					     graph based analysis */
#define	MJR_LINK_UNKNOWN		7 /* unknown type */
  unsigned int		id;
  int			type;
  struct s_link		*next;
}			mjrlink_t;

typedef struct		s_container
{
#define MJR_CNTNR_FUNC		0 /* object contained is a mjrfunc_t */
#define MJR_CNTNR_BLOCK		1 /* object contained is a mjrblock_t */
#define MJR_CNTNR_CNTNR		2 /* object contained is a mjrcontainer_t */
#define MJR_CNTNR_UNKNOWN	3 /* object contained is of unknown type */
  unsigned int 	id;	  	  /* unique id of this container */
  void		*data;		  /* points to the desired object */
  u_int		type;		  /* type of the object */
  mjrlink_t	*input;		  /* a way we came to this container */
  unsigned int	in_nbr;		  /* number of input links */
  mjrlink_t	*output;	  /* a way to the next container */
  unsigned int	out_nbr;	  /* number of output links */
}	mjrcontainer_t;

#endif
