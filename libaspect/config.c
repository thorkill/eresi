/*
** @file state.c
** @ingroup libaspect
** 
** Started on  Sat Jun  2 15:20:18 2005 jfv
** $Id: config.c,v 1.12 2008-02-16 12:32:27 thor Exp $
**
*/
#include "libaspect.h"


aspectworld_t	aspectworld;



/**
 * @brief Wrapper for updating value depending on data type
 * @param item Configuration item structure
 * @param data Configuration item value
 */
static void __config_update_item(configitem_t *item, void *data)
{

 switch(item->type) 
   {
   case CONFIG_TYPE_INT:
     item->val = (int)data;
     break;
     
   case CONFIG_TYPE_STR:
     item->data = strdup(data); 
     break;
     
   default:
     break;
   }
}

/**
 * @brief Add a configure item to the configuration items hash table
 * @param name Configuration item string name
 * @param type Configuration item type identifier
 * @param mode Configuration mode (RW or RO)
 * @param data Configuration item value
 */
void	config_add_item(char *name,
			unsigned int type,	/* int, string ... */
			unsigned int mode,	/* RW, RO */
			void *data)
{
  configitem_t *tmp;
  
  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);
  XALLOC(__FILE__, __FUNCTION__, __LINE__, tmp, sizeof(configitem_t), );  
  tmp->name = strdup(name);
  tmp->type = type;
  tmp->mode = mode;
  tmp->val  = -1;
  tmp->data = NULL;
  
  __config_update_item(tmp,data);
  
  if (hash_get(&aspectworld.config_hash,tmp->name) == NULL) 
    hash_add(&aspectworld.config_hash,tmp->name,tmp);
    
  PROFILER_OUT(__FILE__,__FUNCTION__,__LINE__);
}

/**
 * @brief Update value of an existing configuration item
 * @param name Configuration item string name
 * @param data Configuration item value
 */
void		config_update_key(char *name,void *data)
{
  configitem_t	*tmp;
  
  tmp = (configitem_t *) hash_get(&aspectworld.config_hash,name);
  if (tmp == NULL)
    return;

  __config_update_item(tmp,data);
  hash_del(&aspectworld.config_hash, name);
  hash_add(&aspectworld.config_hash, tmp->name, tmp);
}


/**
 * @brief Retreive the value of a configuration item
 * @param name Requested configuration item name
 * @return A pointer on the configuration item value
 */
void		*config_get_data(char *name) 
{
  configitem_t	*item;
  
  item = (configitem_t *) hash_get(&aspectworld.config_hash,name);
  
  if (item == NULL)
    return NULL;
  
  switch (item->type) 
    {
    case CONFIG_TYPE_INT:
      return (int *)item->val;
    case CONFIG_TYPE_STR:
      return (char *)item->data;
    default:
      return NULL;
    }
}

/** 
 * @brief Function to turn on safemode 
 */
void 	config_safemode_set()
{
  config_update_key(CONFIG_NAME_SAFEMODE, 
		    (void *) CONFIG_SAFEMODE_ON);
}

/** 
 * @brief Functions to turn off safemode 
 */
void	config_safemode_reset()
{
  config_update_key(CONFIG_NAME_SAFEMODE,
		    (void *) CONFIG_SAFEMODE_OFF);
}

/**
 * @brief Indicate safemode state
 * @return Current safemode state
 */
int	config_safemode()
{
  return (int) config_get_data(CONFIG_NAME_SAFEMODE);
}

/** 
 * @brief Enable profile verbose printing for internal ERESI warnings
 * @return Always 0
 */
int	profiler_enable_err()
{
  aspectworld.proflevel |= PROFILE_WARN;
  return (0);
}

/** 
 * @brief Enable profile verbose printing for internal ERESI functions tracing
 * @return Always 0
 */
int	profiler_enable_out()
{
  aspectworld.proflevel |= PROFILE_FUNCS;
  return (0);
}

/** 
 * @brief Enable profile verbose printing for internal memory allocations
 * @return Always 0
 */
int	profiler_enable_alloc()
{
  aspectworld.proflevel |= PROFILE_ALLOC;
  return (0);
}

/** 
 * @brief Enable profile verbose printing for internal debugging informations
 * @return Always 0
 */
int	profiler_enable_debug()
{
  aspectworld.proflevel |= PROFILE_DEBUG;
  return (0);
}

/** 
 * @brief Disable profile verbose printing for internal warnings
 * @return Always 0
 */
int	profiler_disable_err()
{
  aspectworld.proflevel &= (~PROFILE_WARN);
  return (0);
}

/** 
 * @brief Disable profile verbose printing for internal ERESI functions tracing
 * @return Always 0
 */
int	profiler_disable_out()
{
  aspectworld.proflevel &= (~PROFILE_FUNCS);
  return (0);
}

/** 
 * @brief Disable profile verbose printing for internal memory allocations
 * @return Always 0
 */
int	profiler_disable_alloc()
{
  aspectworld.proflevel &= (~PROFILE_ALLOC);
  return (0);
}

/** 
 * @brief Disable profile verbose printing for internal debugging informations
 * @return Always 0
 */
int	profiler_disable_debug()
{
  aspectworld.proflevel &= (~PROFILE_DEBUG);
  return (0);
}

/** 
 * @brief Disable all profiling printing
 * @return Always 0
 */
int	profiler_disable_all()
{
  aspectworld.proflevel = PROFILE_NONE;
  return 0;
}

/** 
 * @brief Enable all profiling printing
 * @return Always 0
 */
int	profiler_enable_all()
{
  profiler_enable_err();
  profiler_enable_out();
  profiler_enable_alloc();
  return 0;
}

/** 
 * @brief Test if profiling is enabled
 * @return Integer value of the profiling level
 */
int	profiler_enabled()
{
  return (aspectworld.proflevel);
}

/** 
 * @brief Test if profiler is enabled under certain flags
 * @param mask Bitmask of bits to be tested
 * @return A Bitmask of enabled profiling flags
 */
int	profiler_is_enabled(unsigned char mask)
{
  return (aspectworld.proflevel & mask);
}

/**
 * @brief Change the profiling output function (default: revm_output in librevm)
 * @param profile Function pointer of type int fct(char*) to be registered for standard profiling
 * @param profile_err Function pointer of type int fct(char*) to be registered for error profiling
 */
void	profiler_install(int (*profile)(char *), 
			 int (*profile_err)(char *))
{
  aspectworld.profile = profile;
  aspectworld.profile_err = profile_err;
}

/* We setup two functions for colors because we have too many functions */

/** 
 * @brief Change simple color functions handlers
 * @param endline Function pointer of type void fct() handling ends of line (default: revm_endline)
 * @param colorinstr Function pointer of type char *fct(char *str) handling instruction printing
 * @param colorstr Function pointer of type char *fct(char *str) handling string printing
 * @param colorfieldstr Function pointer of type char *fct(char *str) handling field printing
 * @param colortypestr Function pointer of type char *fct(char *str) handling type printing
 * @param colorend Function pointer of type char *fct(char *str) handling ending lines printing
 * @param colorwarn Function pointer of type char *fct(char *str) handling priting of warnings
 * @param colorfunction Function pointer of type char *fct(char *str) handling function name printing
 * @param colorfilename Function pointer of type char *fct(char *str) handling file name printing
 */
void	profiler_setcolor(void (*endline)(), 
			  char *(*colorinstr)(char *text),
			  char *(*colorstr)(char *t), 
			  char *(*colorfieldstr)(char *t),
			  char *(*colortypestr)(char *t), 
			  char *(*colorend)(char *text),
			  char *(*colorwarn)(char *text), 
			  char *(*colorfunction)(char *text),
			  char *(*colorfilename)(char *text))
{
  aspectworld.endline = endline;
  aspectworld.colorinstr = colorinstr;
  aspectworld.colorstr = colorstr;
  aspectworld.colorfieldstr = colorfieldstr;
  aspectworld.colortypestr = colortypestr;
  aspectworld.colorend = colorend;
  aspectworld.colorwarn = colorwarn;
  aspectworld.colorfunction = colorfunction;
  aspectworld.colorfilename = colorfilename;
}

/** 
 * @brief Change advanced color functions 
 * @param coloradv Function pointer of type char *fct(char*, char*, char*) formatting end lines
 * @param colorinstr_fmt Function pointer of type char *fct(char *, char *) formatting instructions
 * @param coloraddress Function pointer of type char *fct(char *, unsigned long) formatting addresses
 * @param colornumber Function pointer of type char *fct(char *, unsigned int) formatting decimal numbers
 * @param colorstr_fmt Function pointer of type char *fct(char *, char *) formatting strings
 * @param colorfieldstr_fmt Function pointer of type char *fct(char *, char *) formatting field strings
 * @param colortypestr_fmt Function pointer of type char *fct(char *, char *) formatting type names
 * @param colorwarn_fmt Function pointer of type char *fct(char *, char *) formatting warnings
 */
void	profiler_setmorecolor(char *(*coloradv)(char *ty, char *p, char *te),
			      char *(*colorinstr_fmt)(char* p, char *t),
			      char *(*coloraddress)(char *p, eresi_Addr a),
			      char *(*colornumber)(char *p, eresi_Off n),
			      char *(*colorstr_fmt)(char *p, char *t),
			      char *(*colorfieldstr_fmt)(char *p, char *t),
			      char *(*colortypestr_fmt)(char *p, char *t),
			      char *(*colorwarn_fmt)(char *pattern, char *text))
{
  aspectworld.coloradv = coloradv;
  aspectworld.colorinstr_fmt = colorinstr_fmt;
  aspectworld.coloraddress = coloraddress;
  aspectworld.colornumber = colornumber;
  aspectworld.colorstr_fmt = colorstr_fmt;
  aspectworld.colorfieldstr_fmt = colorfieldstr_fmt;
  aspectworld.colortypestr_fmt = colortypestr_fmt;
  aspectworld.colorwarn_fmt = colorwarn_fmt;
}
