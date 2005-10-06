/*
** arith.c for elfsh
** 
** Started on  Sun Feb  9 22:43:34 2003 mayhem
*/
#include "elfsh.h"


/* Preconditions on atomic operations */
int                     vm_preconds_atomics(elfshpath_t **o1, elfshpath_t **o2)
{
  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);

  /* Sanity checks */
  *o1 = vm_lookup_param(world.curjob->curcmd->param[0], 1);
  *o2 = vm_lookup_param(world.curjob->curcmd->param[1], 1);
  if (!*o1 || !*o2)
    ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Unable to lookup a parameter", -1);

  /* Lazy typing in action */
  if ((*o1)->type != (*o2)->type)
    {
      if ((*o2)->type == ELFSH_OBJUNK)
        ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, 
			  "Source parameter undefined", -1);
      if ((*o1)->type == ELFSH_OBJUNK)
        vm_convert_object(*o1, (*o2)->type);
      else if (vm_convert_object(*o2, (*o1)->type) < 0)
        ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, 
			  "SET parameters type are not compatible", -1);
    }
  else if ((*o1)->immed && !(*o1)->perm)
    ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Dest. param. must not be a constant", -1);

  ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
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
  int			errvar;

  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);

  /* Sanity checks and conversions */
  error  = -1;
  errvar = 0;
  if (vm_preconds_atomics(&o1, &o2) < 0)
    ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Invalid variable transaction", (-1));

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
      if (o1->immed)
        o1->immed_val.byte = val8;
      else if (o1->set_obj(o1->parent, val8) < 0)
        goto err;

      /* The $_ variable is updated as well */
      last = hash_get(&vars_hash, ELFSH_RESVAR);
      if (last == NULL)
        { errvar = -1; goto err; }
      last->immed_val.word = val8;
      last->type = o1->type;
      break;


    case ELFSH_OBJSHORT:
      val16 = (o2->immed ? o2->immed_val.half : o2->get_obj(o2->parent));
      if (o1->immed)
	o1->immed_val.half = val16;
      else if (o1->set_obj(o1->parent, val16) < 0)
	goto err;

      /* The $_ variable is updated as well */
      last = hash_get(&vars_hash, ELFSH_RESVAR);
      if (last == NULL)
	{ errvar = -1; goto err; }
      last->immed_val.word = val16;
      last->type = o1->type;
      break;

    case ELFSH_OBJINT:
      val32 = (o2->immed ? o2->immed_val.word : o2->get_obj(o2->parent));
      if (o1->immed)
	o1->immed_val.word = val32;
      else if (o1->set_obj(o1->parent, val32) < 0)
	goto err;

      /* The $_ variable is updated as well */
      last = hash_get(&vars_hash, ELFSH_RESVAR);
      if (last == NULL)
	{ errvar = -1; goto err; }
      last->immed_val.word = val32;
      last->type = o1->type;
      break;

    case ELFSH_OBJLONG:
      val64 = (o2->immed ? o2->immed_val.ent : o2->get_obj(o2->parent));
      if (o1->immed)
	o1->immed_val.ent = val64;
      else if (o1->set_obj(o1->parent, val64) < 0)
	goto err;

      /* The $_ variable is updated as well */
      last = hash_get(&vars_hash, ELFSH_RESVAR);
      if (last == NULL)
	{ errvar = -1; goto err; }
      last->immed_val.ent = val64;
      last->type = o1->type;
      break;

    default:
      ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, 
			"Unknown type for SET parameter", -1);
    }

  /* Print stuff and return */
  if (!world.state.vm_quiet)
    vm_output(" [*] Field set succesfully \n\n");
  error = 0;

 err:
  if (o2->immed && o2->type == ELFSH_OBJSTR && str != NULL)
    XFREE(str);
  if (!o2->perm)
    XFREE(o2);
  if (!o1->perm)
    XFREE(o1);

  /* We have 2 different possible errors here */
  if (errvar < 0)
    ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Error while setting $_", -1);
  else if (error < 0)
    ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Unable to set object", -1);
  
  ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, error);
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
  int		error = -1;
  elfshpath_t	*last;

  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);

  /* Sanity checks */
  if (world.curjob->curcmd->param[0] == NULL)
    ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "GET need a parameter", -1);
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
	XFREE(str);
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
      ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, 
			"Unknown type for GET parameter", -1);
    }

  /* Print stuff and return OK */
  if (!world.state.vm_quiet)
    vm_output("\n");
  
  error = 0;
  
 err:
  if (!o1->perm)
    XFREE(o1);

  if (error < 0)
    ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Error while setting $_", error);

  ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, error);
}



/* ADD command */
int			cmd_add()
{
  elfshpath_t		*o1;
  elfshpath_t		*o2;
  elfsh_Addr		val;

  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);

  /* Sanity checks */
  if (world.curjob->curcmd->param[0] == NULL || world.curjob->curcmd->param[1] == NULL)
    ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Need 2 parameters", -1);
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
    ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Parameter has not INTEGER type", -1);

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
    XFREE(o2);
  if (!o1->perm)
    XFREE(o1);
  ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}

/* SUB command */
int			cmd_sub()
{
  elfshpath_t		*o1;
  elfshpath_t		*o2;
  elfsh_Addr	       	val;

  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);

  /* Sanity checks */
  if (world.curjob->curcmd->param[0] == NULL || world.curjob->curcmd->param[1] == NULL)
    ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Need 2 parameters", -1);
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
    ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Parameter has not INTEGER type", -1);

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
    XFREE(o2);
  if (!o1->perm)
    XFREE(o1);
  ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}


/* MUL command */
int			cmd_mul()
{
  elfshpath_t		*o1;
  elfshpath_t		*o2;
  elfsh_Addr		val;

  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);

  /* Sanity checks */
  if (world.curjob->curcmd->param[0] == NULL || world.curjob->curcmd->param[1] == NULL)
    ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Needs 2 parameters", -1);
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
    ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Parameter has not INTEGER type", -1);

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
    XFREE(o2);
  if (!o1->perm)
    XFREE(o1);
  ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}

/* DIV command */
int			cmd_div()
{
  elfshpath_t		*o1;
  elfshpath_t		*o2;
  elfsh_Addr		val;
  elfsh_Addr		lav;

  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);

  /* Sanity checks */
  if (world.curjob->curcmd->param[0] == NULL || world.curjob->curcmd->param[1] == NULL)
    ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Need 2 parameters", -1);
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
    ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Parameter has not INTEGER type", -1);

  /* Compute */
  val  = (o1->immed ? o1->immed_val.ent : o1->get_obj(o1->parent));
  lav  = (o2->immed ? o2->immed_val.ent : o2->get_obj(o2->parent));
  if (lav == 0)
    ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__,
		      "Cannot divide by 0", -1);
  if (o1->immed)
    o1->immed_val.ent = val / lav;
  else
    o1->set_obj(o1->parent, val / lav);
  
  if (!world.state.vm_quiet)
    vm_output(" [*] Field modified succesfully \n\n");
  if (!o2->perm)
    XFREE(o2);
  if (!o1->perm)
    XFREE(o1);

  ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}


/* MOD command */
int			cmd_mod()
{
  elfshpath_t		*o1;
  elfshpath_t		*o2;
  elfsh_Addr	       	val;
  elfsh_Addr	      	lav;

  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);

  /* Sanity checks */
  if (world.curjob->curcmd->param[0] == NULL || world.curjob->curcmd->param[1] == NULL)
    ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "MOD needs 2 parameters", -1);
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
    ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Parameter has not INTEGER type", -1);

  /* Compute */
  val  = (o1->immed ? o1->immed_val.ent : o1->get_obj(o1->parent));
  lav  = (o2->immed ? o2->immed_val.ent : o2->get_obj(o2->parent));
  if (lav == 0)
    ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Cannot divide by 0", -1);
  if (o1->immed)
    o1->immed_val.ent = val % lav;
  else
    o1->set_obj(o1->parent, val % lav);

  if (!world.state.vm_quiet)
    vm_output(" [*] Field modified succesfully \n\n");
  if (!o2->perm)
    XFREE(o2);
  if (!o1->perm)
    XFREE(o1);

  ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
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

  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);

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
      ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, 
			"Invalid parameters", -1);
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
      ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, 
			"Unknown parameter type", -1);
    }

  /* Set the last result variable */
  last = hash_get(&vars_hash, ELFSH_RESVAR);
  if (last == NULL)
    goto err;
  last->immed_val.ent = val;
  last->type = o1->type;
  if (o1->type == ELFSH_OBJSTR)
    last->type = ELFSH_OBJINT;
  error = 0;

 err:
  if (!o2->perm && o2->immed && o2->type == ELFSH_OBJSTR && str != NULL)
    XFREE(str);
  if (!o2->perm)
    XFREE(o2);
  if (!o1->perm)
    XFREE(o1);
  if (!world.state.vm_quiet)
    {
      snprintf(logbuf, BUFSIZ - 1, " [*] Objects are %s. \n\n", (!val ? "EQUALS" : "INEQUALS"));
      vm_output(logbuf);
    }
  
  if (error < 0)
    ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, "Error while setting $_", error);
  
  ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, error);
}






