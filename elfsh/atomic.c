/*
** arith.c for elfsh
** 
** Started on  Sun Feb  9 22:43:34 2003 mayhem
*/
#include "elfsh.h"


/* Preconditions on atomic operations */
int                     vm_preconds_atomics(elfshpath_t **o1, elfshpath_t **o2)
{
  E2DBG_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);

  /* Sanity checks */
  *o1 = vm_lookup_param(world.curjob->curcmd->param[0], 1);
  *o2 = vm_lookup_param(world.curjob->curcmd->param[1], 1);
  if (!*o1 || !*o2)
    RET(-1);


  /* Lazy typing in action */
  if ((*o1)->type != (*o2)->type)
    {
      if ((*o2)->type == ELFSH_OBJUNK)
        ELFSH_SETERROR("[elfsh:cmd_set] Source parameter undefined\n", -1);
      if ((*o1)->type == ELFSH_OBJUNK)
        vm_convert_object(*o1, (*o2)->type);
      else if (vm_convert_object(*o2, (*o1)->type) < 0)
        ELFSH_SETERROR("[elfsh:cmd_set] SET parameters type are not compatible\n", -1);
    }
  else if ((*o1)->immed && !(*o1)->perm)
    ELFSH_SETERROR("[elfsh:cmd_set] Dest. param. must not be a constant\n", -1);
  return (0);
}



/* SET command */
int                     cmd_set()
{
  elfshpath_t           *o1;
  elfshpath_t           *o2;
  elfshpath_t           *last;
  char                  *str;
  elfsh_Addr            val64;
  u_int                 val32;
  u_char                val8;
  u_short               val16;
  int                   error;

  E2DBG_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);

  /* Sanity checks and conversions */
  if (vm_preconds_atomics(&o1, &o2) < 0)
    return (-1);


  /* Do the real assignation */
  switch (o1->type)
    {
    case ELFSH_OBJSTR:
      str = (o2->immed ? o2->immed_val.str : o2->get_name(o2->root, o2->parent));
      if (o1->immed)
        {
          o1->immed_val.str = strdup(str);
          o1->size = o2->size;
        }
      else if (o1->set_name(o1->root, o1->parent, str) < 0)
        goto err;
      break;

    case ELFSH_OBJBYTE:
      val8 = (o2->immed ? o2->immed_val.byte : o2->get_obj(o2->parent));


      //printf("set_obj with val8 before = %hhu \n", val8);

      if (o1->immed)
        o1->immed_val.byte = val8;
      else if (o1->set_obj(o1->parent, val8) < 0)
        goto err;

      //printf("set_obj with val8 after = %hhu \n", val8);

      /* The $_ variable is updated as well */
      last = hash_get(&vars_hash, ELFSH_RESVAR);
      if (last == NULL)
        goto err;
      last->immed_val.word = val8;
      last->type = o1->type;
      break;


    case ELFSH_OBJSHORT:
      val16 = (o2->immed ? o2->immed_val.half : o2->get_obj(o2->parent));


      //printf("set_obj with val16 before = %hu \n", val16);

      if (o1->immed)
	o1->immed_val.half = val16;
      else if (o1->set_obj(o1->parent, val16) < 0)
	goto err;

      //printf("set_obj with val16 after = %u \n", val16);

      /* The $_ variable is updated as well */
      last = hash_get(&vars_hash, ELFSH_RESVAR);
      if (last == NULL)
	goto err;
      last->immed_val.word = val16;
      last->type = o1->type;
      break;


    case ELFSH_OBJINT:
      val32 = (o2->immed ? o2->immed_val.word : o2->get_obj(o2->parent));


      //printf("set_obj with val32 before = %u \n", val32);

      if (o1->immed)
	o1->immed_val.word = val32;
      else if (o1->set_obj(o1->parent, val32) < 0)
	goto err;

      //printf("set_obj with val32 after = %u \n", val32);

      /* The $_ variable is updated as well */
      last = hash_get(&vars_hash, ELFSH_RESVAR);
      if (last == NULL)
	goto err;
      last->immed_val.word = val32;
      last->type = o1->type;
      break;

    case ELFSH_OBJLONG:
      val64 = (o2->immed ? o2->immed_val.ent : o2->get_obj(o2->parent));


      //printf("set_obj with val64 before = %hu \n", val64);

      if (o1->immed)
	o1->immed_val.ent = val64;
      else if (o1->set_obj(o1->parent, val64) < 0)
	goto err;

      //printf("set_obj with val64 before = %hu \n", val64);

      /* The $_ variable is updated as well */
      last = hash_get(&vars_hash, ELFSH_RESVAR);
      if (last == NULL)
	goto err;
      last->immed_val.ent = val64;
      last->type = o1->type;
      break;

    default:
      ELFSH_SETERROR("[elfsh:cmd_set] Unknown type for SET parameter\n", -1);
    }

  /* Print stuff and return */
  if (!world.state.vm_quiet)
    vm_output(" [*] Field set succesfully \n\n");
  error = 0;

 err:
  if (o2->immed && o2->type == ELFSH_OBJSTR && str != NULL)
    free(str);
  if (!o2->perm)
    free(o2);
  if (!o1->perm)
    free(o1);
  return (error);
}


/* GET command */
int		cmd_get()
{
  elfshpath_t	*o1;
  char		*str;
  elfsh_Addr	val;
  u_int		val32;
  u_char	val8;
  u_short	val16;
  char		logbuf[BUFSIZ];
  int		error;
  elfshpath_t	*last;

  E2DBG_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);

  /* Sanity checks */
  if (world.curjob->curcmd->param[0] == NULL)
    ELFSH_SETERROR("[elfsh:cmd_get] GET need a parameter\n", -1);
  o1 = vm_lookup_param(world.curjob->curcmd->param[0], 1);
  if (!o1)
    RET(-1);

  /* Switch on the object type */
  switch (o1->type)
    {
    case ELFSH_OBJSTR:
      str = (o1->immed ? o1->immed_val.str : o1->get_name(o1->root, o1->parent));
      vm_output(str);
      if (o1->immed)
	free(str);
      break;

    case ELFSH_OBJBYTE:
      val8 = (o1->immed ? o1->immed_val.byte : o1->get_obj(o1->parent));
      snprintf(logbuf, BUFSIZ - 1, "%c\n", val8);
      vm_output(logbuf);
      /* The $_ variable is updated as well */
      last = hash_get(&vars_hash, ELFSH_RESVAR);
      if (last == NULL)
	goto err;
      last->immed_val.word = val8;
      last->type = o1->type;
      break;
    case ELFSH_OBJSHORT:
      val16 = (o1->immed ? o1->immed_val.half : o1->get_obj(o1->parent));
      snprintf(logbuf, BUFSIZ - 1, "%4hu \n", val16);
      vm_output(logbuf);

      /* The $_ variable is updated as well */
      last = hash_get(&vars_hash, ELFSH_RESVAR);
      if (last == NULL)
	goto err;
      last->immed_val.word = val16;
      last->type = o1->type;
      break;
    case ELFSH_OBJINT:
      val32 = (o1->immed ? o1->immed_val.word : o1->get_obj(o1->parent));
      snprintf(logbuf, BUFSIZ - 1, "%8X\n", val32);
      vm_output(logbuf);

      /* The $_ variable is updated as well */
      last = hash_get(&vars_hash, ELFSH_RESVAR);
      if (last == NULL)
	goto err;
      last->immed_val.word = val32;
      last->type = o1->type;
      break;
    case ELFSH_OBJLONG:
      val = (o1->immed ? o1->immed_val.ent : o1->get_obj(o1->parent));
      snprintf(logbuf, BUFSIZ - 1, XFMT "\n", val);
      vm_output(logbuf);

      /* The $_ variable is updated as well */
      last = hash_get(&vars_hash, ELFSH_RESVAR);
      if (last == NULL)
	goto err;
      last->immed_val.word = val;
      last->type = o1->type;
      break;
      
    default:
      ELFSH_SETERROR("[elfsh:cmd_get] Unknown type for GET parameter\n", -1);
    }

  /* Print stuff and return OK */
  if (!world.state.vm_quiet)
    vm_output("\n");
  
  error = 0;
  
 err:
  if (!o1->perm)
    free(o1);
  return (error);
}



/* ADD command */
int			cmd_add()
{
  elfshpath_t		*o1;
  elfshpath_t		*o2;
  elfsh_Addr		val;

  E2DBG_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);

  /* Sanity checks */
  if (world.curjob->curcmd->param[0] == NULL || world.curjob->curcmd->param[1] == NULL)
    ELFSH_SETERROR("[elfsh:cmd_add] Need 2 parameters", -1);
  o1 = vm_lookup_param(world.curjob->curcmd->param[0], 1);
  o2 = vm_lookup_param(world.curjob->curcmd->param[1], 1);
  if (!o1 || !o2)
    RET(-1);
  if (o1->type == ELFSH_OBJUNK && o1->perm)
    o1->type = ELFSH_OBJINT;

  else if (o1->type == ELFSH_OBJSTR)
    vm_convert_object(o1, ELFSH_OBJINT);
    
  if ((o1->type != ELFSH_OBJINT   &&
       o1->type != ELFSH_OBJBYTE  && 
       o1->type != ELFSH_OBJSHORT && 
       o1->type != ELFSH_OBJLONG) ||
      (o1->type != o2->type && vm_convert_object(o2, o1->type)))
    ELFSH_SETERROR("[elfsh:cmd_add] Parameter has not INTEGER type\n", -1);

  /* Compute */
  val  = (o1->immed ? o1->immed_val.ent : o1->get_obj(o1->parent));
  val += (o2->immed ? o2->immed_val.ent : o2->get_obj(o2->parent));
  if (o1->immed)
    o1->immed_val.ent = val;
  else
    o1->set_obj(o1->parent, val);


  if (!world.state.vm_quiet)
    vm_output(" [*] Field modified succesfully\n\n");
  if (!o2->perm)
    free(o2);
  if (!o1->perm)
    free(o1);
  return (0);
}

/* SUB command */
int			cmd_sub()
{
  elfshpath_t		*o1;
  elfshpath_t		*o2;
  elfsh_Addr	       	val;

  E2DBG_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);

  /* Sanity checks */
  if (world.curjob->curcmd->param[0] == NULL || world.curjob->curcmd->param[1] == NULL)
    ELFSH_SETERROR("[elfsh:cmd_sub] Need 2 parameters\n", -1);
  o1 = vm_lookup_param(world.curjob->curcmd->param[0], 1);
  o2 = vm_lookup_param(world.curjob->curcmd->param[1], 1);
  if (!o1 || !o2)
    RET(-1);
  if (o1->type == ELFSH_OBJUNK && o1->perm)
    o1->type = ELFSH_OBJINT;
  if ((o1->type != ELFSH_OBJINT   &&
       o1->type != ELFSH_OBJBYTE  && 
       o1->type != ELFSH_OBJSHORT && 
       o1->type != ELFSH_OBJLONG) ||
      (o1->type != o2->type && vm_convert_object(o2, o1->type)))
    ELFSH_SETERROR("[elfsh:cmd_sub] Parameter has not INTEGER type\n", -1);

  /* Compute */
  val  = (o1->immed ? o1->immed_val.ent : o1->get_obj(o1->parent));
  val -= (o2->immed ? o2->immed_val.ent : o2->get_obj(o2->parent));
  if (o1->immed)
    o1->immed_val.ent = val;
  else
    o1->set_obj(o1->parent, val);
  
  if (!world.state.vm_quiet)
    vm_output(" [*] Field modified succesfully \n\n");
  if (!o2->perm)
    free(o2);
  if (!o1->perm)
    free(o1);
  return (0);
}


/* MUL command */
int			cmd_mul()
{
  elfshpath_t		*o1;
  elfshpath_t		*o2;
  elfsh_Addr		val;

  E2DBG_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);

  /* Sanity checks */
  if (world.curjob->curcmd->param[0] == NULL || world.curjob->curcmd->param[1] == NULL)
    ELFSH_SETERROR("[elfsh:cmd_mul] Needs 2 parameters", -1);
  o1 = vm_lookup_param(world.curjob->curcmd->param[0], 1);
  o2 = vm_lookup_param(world.curjob->curcmd->param[1], 1);
  if (!o1 || !o2)
    RET(-1);

  if (o1->type == ELFSH_OBJUNK && o1->perm)
    o1->type = ELFSH_OBJINT;
  if ((o1->type != ELFSH_OBJINT   &&
       o1->type != ELFSH_OBJBYTE  && 
       o1->type != ELFSH_OBJSHORT && 
       o1->type != ELFSH_OBJLONG) ||
      (o1->type != o2->type && vm_convert_object(o2, o1->type)))
    ELFSH_SETERROR("[elfsh:cmd_mul] Parameter has not INTEGER type\n", -1);

  /* Compute */
  val  = (o1->immed ? o1->immed_val.ent : o1->get_obj(o1->parent));
  val *= (o2->immed ? o2->immed_val.ent : o2->get_obj(o2->parent));
  if (o1->immed)
    o1->immed_val.ent = val;
  else
    o1->set_obj(o1->parent, val);

  if (!world.state.vm_quiet)
    vm_output(" [*] Field modified succesfully \n\n");
  if (!o2->perm)
    free(o2);
  if (!o1->perm)
    free(o1);
  return (0);
}

/* DIV command */
int			cmd_div()
{
  elfshpath_t		*o1;
  elfshpath_t		*o2;
  elfsh_Addr		val;
  elfsh_Addr		lav;

  E2DBG_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);

  /* Sanity checks */
  if (world.curjob->curcmd->param[0] == NULL || world.curjob->curcmd->param[1] == NULL)
    ELFSH_SETERROR("[elfsh:cmd_div] Need 2 parameters", -1);
  o1 = vm_lookup_param(world.curjob->curcmd->param[0], 1);
  o2 = vm_lookup_param(world.curjob->curcmd->param[1], 1);
  if (!o1 || !o2)
    RET(-1);
  if (o1->type == ELFSH_OBJUNK && o1->perm)
    o1->type = ELFSH_OBJINT;
  if ((o1->type != ELFSH_OBJINT   &&
       o1->type != ELFSH_OBJBYTE  && 
       o1->type != ELFSH_OBJSHORT && 
       o1->type != ELFSH_OBJLONG) ||
      (o1->type != o2->type && vm_convert_object(o2, o1->type)))
    ELFSH_SETERROR("[elfsh:cmd_div] Parameter has not INTEGER type\n", -1);

  /* Compute */
  val  = (o1->immed ? o1->immed_val.ent : o1->get_obj(o1->parent));
  lav  = (o2->immed ? o2->immed_val.ent : o2->get_obj(o2->parent));
  if (lav == 0)
    ELFSH_SETERROR("[elfsh:cmd_div] Cannot divide by 0\n", -1);
  if (o1->immed)
    o1->immed_val.ent = val / lav;
  else
    o1->set_obj(o1->parent, val / lav);
  
  if (!world.state.vm_quiet)
    vm_output(" [*] Field modified succesfully \n\n");
  if (!o2->perm)
    free(o2);
  if (!o1->perm)
    free(o1);

  return (0);
}


/* MOD command */
int			cmd_mod()
{
  elfshpath_t		*o1;
  elfshpath_t		*o2;
  elfsh_Addr	       	val;
  elfsh_Addr	      	lav;

  E2DBG_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);

  /* Sanity checks */
  if (world.curjob->curcmd->param[0] == NULL || world.curjob->curcmd->param[1] == NULL)
    ELFSH_SETERROR("[elfsh:cmd_mod] MOD needs 2 parameters", -1);
  o1 = vm_lookup_param(world.curjob->curcmd->param[0], 1);
  o2 = vm_lookup_param(world.curjob->curcmd->param[1], 1);
  if (!o1 || !o2)
    RET(-1);
  if (o1->type == ELFSH_OBJUNK && o1->perm)
    o1->type = ELFSH_OBJINT;

  if ((o1->type != ELFSH_OBJINT   &&
       o1->type != ELFSH_OBJBYTE  && 
       o1->type != ELFSH_OBJSHORT && 
       o1->type != ELFSH_OBJLONG) ||
      (o1->type != o2->type && vm_convert_object(o2, o1->type)))
    ELFSH_SETERROR("[elfsh:cmd_mod] Parameter has not INTEGER type\n", -1);

  /* Compute */
  val  = (o1->immed ? o1->immed_val.ent : o1->get_obj(o1->parent));
  lav  = (o2->immed ? o2->immed_val.ent : o2->get_obj(o2->parent));
  if (lav == 0)
    ELFSH_SETERROR("[elfsh:cmd_mod] Cannot divide by 0\n", -1);
  if (o1->immed)
    o1->immed_val.ent = val % lav;
  else
    o1->set_obj(o1->parent, val % lav);

  if (!world.state.vm_quiet)
    vm_output(" [*] Field modified succesfully \n\n");
  if (!o2->perm)
    free(o2);
  if (!o1->perm)
    free(o1);

  return (0);
}


/* Comparison command */
int			cmd_cmp()
{
  elfshpath_t		*o1;
  elfshpath_t		*o2;
  elfshpath_t		*last;
  char			*str;
  char			*str2;
  elfsh_Addr	       	val;
  elfsh_Addr	       	val2;
  int			error;
  char			logbuf[BUFSIZ];

  E2DBG_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);

  /* Sanity checks */
  o1 = vm_lookup_param(world.curjob->curcmd->param[0], 1);
  o2 = vm_lookup_param(world.curjob->curcmd->param[1], 1);
  if (!o1 || !o2)
    RET(-1);

  /* Lazy typing in action */
  if ((o1->type != ELFSH_OBJINT   && 
       o1->type != ELFSH_OBJBYTE  && 
       o1->type != ELFSH_OBJSHORT && 
       o1->type != ELFSH_OBJLONG  && 
       o1->type != ELFSH_OBJSTR)  ||
      (o1->type != o2->type && vm_convert_object(o2, o1->type)))
    {	
      snprintf(logbuf, BUFSIZ - 1, "o1type = %u, o2type = %u \n",
	       o1->type, o2->type);
      vm_output(logbuf);
      ELFSH_SETERROR("[elfsh:cmd_cmp] Invalid parameters\n", -1);
    }

  error = -1;
  str = NULL;

  /* Do the real assignation */
  switch (o1->type)
    {
    case ELFSH_OBJSTR:
      str2 = (o2->immed ? o2->immed_val.str : o2->get_name(o2->root, o2->parent));
      str  = (o1->immed ? o1->immed_val.str : o1->get_name(o1->root, o1->parent));
      if (!str || !str2)
	val = 1;
      else
	val = strcmp(str, str2);
      break;
    case ELFSH_OBJBYTE:
    case ELFSH_OBJSHORT:
    case ELFSH_OBJLONG:
    case ELFSH_OBJINT:
      val2 = (o2->immed ? o2->immed_val.ent : o2->get_obj(o2->parent));
      val  = (o1->immed ? o1->immed_val.ent : o1->get_obj(o1->parent));
      val -= val2;
      break;
    default:
      ELFSH_SETERROR("[elfsh:cmd_cmp] Unknown parameter type\n", -1);
    }

  /* Set the last result variable */
  //printf("setting lastres variable to %u \n", val);

  last = hash_get(&vars_hash, ELFSH_RESVAR);
  if (last == NULL)
    goto err;
  last->immed_val.ent = val;
  last->type = o1->type;
  error = 0;

 err:
  if (!o2->perm && o2->immed && o2->type == ELFSH_OBJSTR && str != NULL)
    free(str);
  if (!o2->perm)
    free(o2);
  if (!o1->perm)
    free(o1);
  if (!world.state.vm_quiet)
    {
      snprintf(logbuf, BUFSIZ - 1, " [*] Objects are %s. \n\n", (!val ? "EQUALS" : "INEQUALS"));
      vm_output(logbuf);
    }

  return (error);
}






