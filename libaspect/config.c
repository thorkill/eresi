/*
** state.c for elfsh
** 
** Started on  Sat Jun  2 15:20:18 2005 mayhem
**
*/
#include "libaspect.h"


aspectworld_t	aspectworld;



/* this function is a wrapper for updating data/val which depend 
   on datatype */
static void __config_update_item(configitem_t *item,void *data)
{

 switch(item->type) 
   {
   case CONFIG_TYPE_INT:
     item->val = (int)data;
     break;
     
   case CONFIG_TYPE_STR:
     item->data = aproxy_strdup(data); 
     break;
     
   default:
     break;
   }
}

/* create new item */
void	config_add_item(char *name,
			u_int type,	/* int, string ... */
			u_int mode,	/* RW, RO */
			void *data)
{
  configitem_t *tmp;
  
  PROFILER_IN(__FILE__,__FUNCTION__,__LINE__);
  XALLOC(__FILE__, __FUNCTION__, __LINE__, tmp, sizeof(configitem_t), );  
  tmp->name = aproxy_strdup(name);
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
*** This function updates the value of val/data in already
*** existing in config_hash.
**/
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

/*
  returns val/data 
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

/* Functions to turn on/off safemode */
void 	config_safemode_set()
{
  config_update_key(CONFIG_NAME_SAFEMODE, 
		    (void *) CONFIG_SAFEMODE_ON);
}

void	config_safemode_reset()
{
  config_update_key(CONFIG_NAME_SAFEMODE,
		    (void *) CONFIG_SAFEMODE_OFF);
}

/* Say if we are in safe mode or not */
int	config_safemode()
{
  return (int) config_get_data(CONFIG_NAME_SAFEMODE);
}

/* Here the functions for the profiler option */
int	profiler_enable_err()
{
  aspectworld.proflevel |= PROFILE_ERROR;
  return (0);
}

int	profiler_enable_out()
{
  aspectworld.proflevel |= PROFILE_OUTPUT;
  return (0);
}

int	profiler_enable_alloc()
{
  aspectworld.proflevel |= PROFILE_ALLOC;
  return (0);
}

int	profiler_enable_debug()
{
  aspectworld.proflevel |= PROFILE_DEBUG;
  return (0);
}

int	profiler_disable_err()
{
  aspectworld.proflevel &= PROFILE_ERROR;
  return (0);
}

int	profiler_disable_out()
{
  aspectworld.proflevel &= PROFILE_OUTPUT;
  return (0);
}

int	profiler_disable_alloc()
{
  aspectworld.proflevel &= PROFILE_ALLOC;
  return (0);
}

int	profiler_disable_debug()
{
  aspectworld.proflevel &= PROFILE_DEBUG;
  return (0);
}

int	profiler_disable_all()
{
  aspectworld.proflevel = PROFILE_NONE;
  return 0;
}

int	profiler_enable_all()
{
  profiler_enable_err();
  profiler_enable_out();
  profiler_enable_alloc();
  return 0;
}

int	profiler_enabled()
{
  return (aspectworld.proflevel);
}

/* Change the profiling output function */
/* Usually vm_output located in elfsh VM */
void	profiler_install(int (*profile)(char *), 
			 int (*profile_err)(char *))
{
  aspectworld.profile = profile;
  aspectworld.profile_err = profile_err;
}

/* We setup two functions for colors because we have 
   too many functions */

/* Change simple color functions */
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

/* Change advanced color functions */
void	profiler_setmorecolor(char *(*coloradv)(char *ty, char *p, char *te),
			      char *(*colorinstr_fmt)(char* p, char *t),
			      char *(*coloraddress)(char *p, u_long a),
			      char *(*colornumber)(char *p, u_int n),
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
