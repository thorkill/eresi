/*
** vectors.c for libaspect in elfsh
**
** Implement the modularity for the framework
**
** Started Dec 22 2006 02:57:03 mayhem
**
**
** $Id: init.c,v 1.5 2007-05-01 15:56:00 may Exp $
**
*/
#include "libaspect.h"


static u_char aspect_initialized = 0;


/***************** ERESI Constructors help functions **********************/

static u_char called_ctors = 0;
static u_char dbgpresent   = 0;

/* Count the number of constructors already called in the framework */
void		aspect_called_ctors_inc()
{
  if (!called_ctors)
    e2dbg_presence_set();
  called_ctors++;
}

/* Test if we called all constructors or not, currently we have 3 */
int		aspect_called_ctors_finished()
{
  return (called_ctors == 3);
}


/*************** E2DBG presence help functions *******************/


/* Debugger presence set */
void		e2dbg_presence_set() 
{ 
  dbgpresent = 1; 

#if 1 //__DEBUG_E2DBG__
  //write(2, " [*] Enabled debugger presence\n", 31);
#endif

}

/* Debugger presence reset */
void		e2dbg_presence_reset() 
{ 
  dbgpresent = 0; 

#if 1 //__DEBUG_E2DBG__
  //write(2, " [*] Disabled debugger presence\n", 32);
#endif

}

/* Get the Debugger presence information */
u_char		e2dbg_presence_get()          
{ 
#if 1 //__DEBUG_E2DBG__
  //write(2, " [*] Probbing debugger presence\n", 32);
#endif

  return (dbgpresent); 
}	


/**************** LIBASPECT initiazation ******************/


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
