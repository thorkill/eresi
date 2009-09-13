/**
* @file libaspect/init.c
** @ingroup libaspect
**
** @brief Implement the modularity for the framework.
**
** Started Dec 22 2006 02:57:03 jfv
**
**
** $Id$
**
*/
#include "libaspect.h"


static u_char aspect_initialized = 0;


/***************** ERESI Constructors help functions **********************/

static u_char called_ctors = 0;
static u_char dbgpresent   = 0;
static u_char kshpresent   = 0;
static u_char kdbgpresent  = 0;

/**
 * @brief Count the number of constructors already called in the framework (update internal variable)
 */
void		aspect_called_ctors_inc()
{
  if (!called_ctors)
    e2dbg_presence_set();
  called_ctors++;
}

/**
 * @brief Test if we called all constructors or not (currently we have 3)
 */
int		aspect_called_ctors_finished()
{
  return (called_ctors == 3);
}


/*************** E2DBG presence help functions *******************/


/** 
 * @brief Set debugger presence
 */
void		e2dbg_presence_set() 
{ 
  dbgpresent = 1; 

#if 1 //__DEBUG_E2DBG__
  //write(2, " [*] Enabled debugger presence\n", 31);
#endif

}

/** @brief Reset debugger presence */
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


/**
 * @brief Set kernsh presence flag
 */
void kernsh_present_set()
{
  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);
  kshpresent = 1;
  PROFILER_OUT(__FILE__, __FUNCTION__, __LINE__);
}

/**
 * @brief Get presence of kernsh
 * @return 1 on success, 0 on error
 */
int kernsh_is_present()
{
  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, kshpresent);
}

/**
 * @brief Set presence of kedbg
 */
void kedbg_present_set()
{
  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);
  kdbgpresent = 1;
  PROFILER_OUT(__FILE__, __FUNCTION__, __LINE__);
}

/**
 * @brief Get presence of kedbg
 * @return 1 on success, 0 on error
 */
int kedbg_is_present()
{
  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, kdbgpresent);
}

/** Are we in kernel mode */
u_char		e2dbg_kpresence_get()
{
  return (aspectworld.kernel_mode);
}

/** Enable or disable kernel mode */
void		e2dbg_kpresence_set(u_char pres)
{
  aspectworld.kernel_mode = pres;
}

/**************** LIBASPECT initiazation ******************/


/** @brief Initialize the vector hash table */
static void	aspect_vectors_init()
{
  vector_hash = (hash_t *) hash_find("type_vector");
}


/** @brief Initialize base types : each type has a hash table of typed objects */
static void	aspect_types_init()
{
  hash_init(&types_hash, "types", 11, ASPECT_TYPE_UNKNOW);
  aspect_basetypes_create();
}


/** @brief Configuration initialization in libaspect */
static void	aspect_config_init()
{
  memset(&aspectworld, 0x00, sizeof(aspectworld_t));

  // Just for debugging
  //aspectworld.profile = (void *) puts;

  hash_init(&aspectworld.config_hash, "configuration",
	    CONFIG_HASH_SIZE, ASPECT_TYPE_UNKNOW);

  aspectworld.proflevel = PROFILE_NONE;

  config_add_item(CONFIG_CFGDEPTH,
                  CONFIG_TYPE_INT,
		  CONFIG_MODE_RW,
		  (void *) CONFIG_CFGDEPTH_DEFAULT);

  config_add_item(CONFIG_NAME_SAFEMODE,
		  CONFIG_TYPE_INT,
		  CONFIG_MODE_RW,
		  CONFIG_SAFEMODE_OFF);

  config_add_item(CONFIG_ASM_ENDIAN_FLAG,
		  CONFIG_TYPE_INT,
		  CONFIG_MODE_RW,
		  (void *) CONFIG_ASM_LITTLE_ENDIAN);

 config_add_item(CONFIG_ASM_ATT_MARGIN_FLAG,
		 CONFIG_TYPE_INT,
		 CONFIG_MODE_RW,
		 (void *) CONFIG_ASM_ATT_MARGIN_DEFAULT);

}

/** @brief Initialize everything Libaspect */
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
