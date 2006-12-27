/*
 * ELFsh - Configuration related api
 * 2006 thorkill, Asgard Labs Inc.
 *
 * $Id: config.c,v 1.2 2006-12-27 22:13:29 thor Exp $
 *
 */

#include "libelfsh.h"

libworld_t      dbgworld;

/* This function is started by elfsh_init() */
void	elfsh_config_init()
{
    hash_init(&dbgworld.config_hash,LIBELFSH_CONFIG_HASH_SIZE);
}

/* this function is a wraper for updating data/val which depend on datatype */
void	__elfsh_config_update_item(t_configitem *item,void *data)
{

 switch(item->type) {
  case ELFSH_CONFIG_TYPE_INT:
   item->val = (int)data;
   break;

  case ELFSH_CONFIG_TYPE_STR:
   item->data = elfsh_strdup(data);
   break;

  default:
   break;
 }
}

/* create new item */
void	elfsh_config_add_item(char *name,
				 u_int type,	/* int, string ... */
				 u_int mode,	/* RW, RO */
				 void *data)
{
 t_configitem *tmp;

 tmp = elfsh_malloc(sizeof(t_configitem));

 tmp->name = elfsh_strdup(name);
 tmp->type = type;
 tmp->mode = mode;
 tmp->val  = -1;
 tmp->data = NULL;

 __elfsh_config_update_item(tmp,data);
  
 if (hash_get(&dbgworld.config_hash,tmp->name) == NULL) {
  hash_add(&dbgworld.config_hash,tmp->name,tmp);
 }

}

/* 
   this function updates the value of val/data in allready
   existing in config_hash 
 */
void	elfsh_config_update_key(char *name,void *data)
{
 t_configitem *tmp;

 tmp = (t_configitem *) hash_get(&dbgworld.config_hash,name);
 if (tmp == NULL) {
  return;
 }

 __elfsh_config_update_item(tmp,data);
 hash_del(&dbgworld.config_hash,name);
 hash_add(&dbgworld.config_hash,tmp->name,tmp);

}

/*
  returns val/data 
 */
void	*elfsh_config_get_data(char *name) {
 t_configitem *item;
 
 item = (t_configitem *) hash_get(&dbgworld.config_hash,name);

 if (item == NULL)
  return NULL;

 switch(item->type) {
  case ELFSH_CONFIG_TYPE_INT:
   return (int *)item->val;
  case ELFSH_CONFIG_TYPE_STR:
   return (char *)item->data;
  default:
   return NULL;
 }
}
