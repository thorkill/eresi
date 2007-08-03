/**
 * 2001-2007 Devhell Labs, Asgardlabs, Rfdlabs
 * @file libmjollnir-container.h
 * @brief Container structures for libmjollnir
 *
 * Implementation by jfv, heroine, thorkill, and strauss
 *
 * $Id: libmjollnir-container.h,v 1.11 2007-08-03 11:50:59 heroine Exp $
 *
 */
#if !defined(__MJR_CONTAINER__)
 #define __MJR_CONTAINER__ 1

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

/* XXX: the type field has to be changed for a real eresi type id if 
   we hope to do any shape analysis in ERESI */
typedef	struct	s_link 
{
#define MJR_LINK_FUNC_CALL		0 /* !< @brief a call between functions	*/
#define MJR_LINK_FUNC_RET		1 /* !< @brief a function returning control */
#define MJR_LINK_BLOCK_COND_TRUE	2 /* !< @brief 'true' condition of a branch */
#define MJR_LINK_BLOCK_COND_FALSE	3 /* !< @brief 'false' condition of a branch */
#define MJR_LINK_BLOCK_COND_ALWAYS	4 /* !< @brief uncoditional branch */
#define MJR_LINK_TYPE_DELAY		5 /* !< @brief generally ignored but useful */
#define	MJR_LINK_UNKNOWN		6 /* !< @brief unknown type */
  unsigned int	id;
  int		type;
}		mjrlink_t;

/**
 * @brief Structure to define a generic container 
 */
typedef struct	s_container	
{
  unsigned int 	id;	  	  /* !< @brief unique id of this container */
  void		*data;		  /* !< @brief points to the desired object */
  u_int		type;		  /* !< @brief Contained object type */
  u_int		nbrinlinks;	  /* !< @brief Number of -ondisk- input links */
  u_int		nbroutlinks;	  /* !< @brief Number of -ondisk- output links */
  list_t	*inlinks;	  /* !< @brief Input links for this container */
  list_t	*outlinks;	  /* !< @brief Output links for this container */
}		mjrcontainer_t;

#endif
