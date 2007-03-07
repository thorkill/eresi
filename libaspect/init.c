/*
** vectors.c for libaspect in elfsh
**
** Implement the modularity for the framework
**
** Started Dec 22 2006 02:57:03 mayhem
**
**
** $Id: init.c,v 1.3 2007-03-07 16:45:35 thor Exp $
**
*/
#include "libaspect.h"


static u_char aspect_initialized = 0;


/* Initialize the vector hash */
static void	aspect_vectors_init()
{
  vector_hash = (hash_t *) hash_find("type_vector");
}


/* Initialize base types : each type has a hash table of typed objects */
static void	aspect_types_init()
{
  hash_init(&types_hash, "types", 11, ASPECT_TYPE_UNKNOW);
  aspect_basetypes_create();
}


/** Configuration initialization in libaspect */
static void	aspect_config_init()
{
  memset(&aspectworld, 0x00, sizeof(aspectworld_t));

  // Just for debugging
  //aspectworld.profile = (void *) puts;

  hash_init(&aspectworld.config_hash, "configuration",
	    CONFIG_HASH_SIZE, ASPECT_TYPE_UNKNOW);

  aspectworld.proflevel = PROFILE_NONE;

  config_add_item(CONFIG_NAME_SAFEMODE,
		  CONFIG_TYPE_INT,
		  CONFIG_MODE_RW,
		  CONFIG_SAFEMODE_OFF);
}


/* Initialize Libaspect */
int		aspect_init()
{
  if (!aspect_initialized)
    {
      aspect_initialized = 1;
      aspect_config_init();
      aspect_types_init();
      aspect_vectors_init();
      aspectworld.profstarted = 1;
    }
  return (0);
}
