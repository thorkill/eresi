/*
** arith.c for elfsh
** 
** Started on  Sun Feb  9 22:43:34 2003 mayhem
** Last update Fri May 16 14:43:11 2003 mayhem
*/
#include "elfsh.h"



/* SET command */
int			cmd_set()
{
  elfshpath_t		o1;
  elfshpath_t		o2;
  char			*str;
  long			val;
  int			err;

  /* Sanity checks */
  if (vm_lookup_param(world.args.param[0], &o1, 1) < 0)
    return (-1);
  if (vm_lookup_param(world.args.param[1], &o2, 1) < 0)
    return (-1);

  if (o1.type != o2.type && vm_convert_object(&o2, o1.type) < 0)
    ELFSH_SETERROR("[elfsh] SET parameters type are not compatible\n", -1);
  else if (o1.immed)
    ELFSH_SETERROR("[elfsh] Dest. param. must not be a constant\n", -1);

  err = -1;
  str = NULL;

  /* Do the real assignation */
  switch (o1.type)
    {
    case ELFSH_OBJSTR:
      str = (o2.immed ? o2.immed_val.str : o2.get_name(o2.root, o2.parent));
      if (o1.set_name(o1.root, o1.parent, str) < 0)
	goto err;
      break;
    case ELFSH_OBJINT:
      val = (o2.immed ? o2.immed_val.ent : o2.get_obj(o2.parent));
      if (o1.set_obj(o1.parent, val) < 0)
	goto err;
      break;
    default:
      ELFSH_SETERROR("[elfsh] Unknown type for SET parameter\n", -1);
    }

  /* Print stuff and return */
  if (!world.state.vm_quiet)
    printf(" [*] Field set succesfully \n\n");
  err = 0;

 err:
  if (o2.immed && o2.type == ELFSH_OBJSTR && str != NULL)
    free(str);
  return (err);
}


/* GET command */
int		cmd_get()
{
  elfshpath_t	o1;
  char		*str;
  long		val;

  /* Sanity checks */
  if (world.args.param[0] == NULL)
    ELFSH_SETERROR("[libelfsh] GET need a parameter\n", -1);
  if (vm_lookup_param(world.args.param[0], &o1, 1) < 0)
    RET(-1);

  /* Switch on the object type */
  switch (o1.type)
    {
    case ELFSH_OBJSTR:
      str = (o1.immed ? o1.immed_val.str : o1.get_name(o1.root, o1.parent));
      printf("%s\n", str);
      if (o1.immed)
	free(str);
      break;
    case ELFSH_OBJINT:
      val = (o1.immed ? o1.immed_val.ent : o1.get_obj(o1.parent));
      printf("%08lX\n", val);
      break;
    default:
      ELFSH_SETERROR("[elfsh] Unknown type for GET parameter\n", -1);
    }

  /* Print stuff and return OK */
  if (!world.state.vm_quiet)
    puts("");
  return (0);
}



/* ADD command */
int			cmd_add()
{
  elfshpath_t		o1;
  elfshpath_t		o2;
  u_int			val;

  /* Sanity checks */
  if (world.args.param[0] == NULL || world.args.param[1] == NULL)
    ELFSH_SETERROR("[libelfsh] ADD needs 2 parameters", -1);
  if (vm_lookup_param(world.args.param[0], &o1, 1) < 0)
    return (-1);
  if (vm_lookup_param(world.args.param[1], &o2, 1) < 0)
    return (-1);

  if (o1.type != ELFSH_OBJINT || o2.type != ELFSH_OBJINT)
    ELFSH_SETERROR("[elfsh] MOD parameter has not INTEGER type\n", -1);

  val  = (o1.immed ? o1.immed_val.ent : o1.get_obj(o1.parent));
  val += (o2.immed ? o2.immed_val.ent : o2.get_obj(o2.parent));
  o1.set_obj(o1.parent, val);
  if (!world.state.vm_quiet)
    printf("\n [*] Field modified succesfully\n\n");
  return (0);
}

/* SUB command */
int			cmd_sub()
{
  elfshpath_t		o1;
  elfshpath_t		o2;
  u_int			val;

  /* Sanity checks */
  if (world.args.param[0] == NULL || world.args.param[1] == NULL)
    ELFSH_SETERROR("[libelfsh] SUB needs 2 parameters\n", -1);
  if (vm_lookup_param(world.args.param[0], &o1, 1) < 0)
    return (-1);
  if (vm_lookup_param(world.args.param[1], &o2, 1) < 0)
    return (-1);

  if (o1.type != ELFSH_OBJINT || o2.type != ELFSH_OBJINT)
    ELFSH_SETERROR("[elfsh] MOD parameter has not INTEGER type\n", -1);

  val  = (o1.immed ? o1.immed_val.ent : o1.get_obj(o1.parent));
  val -= (o2.immed ? o2.immed_val.ent : o2.get_obj(o2.parent));
  o1.set_obj(o1.parent, val);
  if (!world.state.vm_quiet)
    printf("\n [*] Field modified succesfully \n\n");
  return (0);
}

/* MUL command */
int			cmd_mul()
{
  elfshpath_t		o1;
  elfshpath_t		o2;
  u_int			val;

  /* Sanity checks */
  if (world.args.param[0] == NULL || world.args.param[1] == NULL)
    ELFSH_SETERROR("[libelfsh] MUL needs 2 parameters", -1);
  if (vm_lookup_param(world.args.param[0], &o1, 1) < 0)
    return (-1);
  if (vm_lookup_param(world.args.param[1], &o2, 1) < 0)
    return (-1);

  if (o1.type != ELFSH_OBJINT || o2.type != ELFSH_OBJINT)
    ELFSH_SETERROR("[elfsh] MOD parameter has not INTEGER type\n", -1);

  val  = (o1.immed ? o1.immed_val.ent : o1.get_obj(o1.parent));
  val *= (o2.immed ? o2.immed_val.ent : o2.get_obj(o2.parent));
  o1.set_obj(o1.parent, val);
  if (!world.state.vm_quiet)
    printf("\n [*] Field modified succesfully \n\n");
  return (0);
}

/* DIV command */
int			cmd_div()
{
  elfshpath_t		o1;
  elfshpath_t		o2;
  u_int			val;
  u_int			lav;

  /* Sanity checks */
  if (world.args.param[0] == NULL || world.args.param[1] == NULL)
    ELFSH_SETERROR("[libelfsh] DIV needs 2 parameters", -1);
  if (vm_lookup_param(world.args.param[0], &o1, 1) < 0)
    return (-1);
  if (vm_lookup_param(world.args.param[1], &o2, 1) < 0)
    return (-1);

  if (o1.type != ELFSH_OBJINT || o2.type != ELFSH_OBJINT)
    ELFSH_SETERROR("[elfsh] MOD parameter has not INTEGER type\n", -1);

  val  = (o1.immed ? o1.immed_val.ent : o1.get_obj(o1.parent));
  lav  = (o2.immed ? o2.immed_val.ent : o2.get_obj(o2.parent));
  if (lav == 0)
    ELFSH_SETERROR("[libelfsh] Cannot divide by NUL \n", -1);
  o1.set_obj(o1.parent, val / lav);
  if (!world.state.vm_quiet)
    printf("\n [*] Field modified succesfully \n\n");
  return (0);
}


/* MOD command */
int			cmd_mod()
{
  elfshpath_t		o1;
  elfshpath_t		o2;
  u_int			val;
  u_int			lav;

  /* Sanity checks */
  if (world.args.param[0] == NULL || world.args.param[1] == NULL)
    ELFSH_SETERROR("[libelfsh] MOD needs 2 parameters", -1);
  if (vm_lookup_param(world.args.param[0], &o1, 1) < 0)
    return (-1);
  if (vm_lookup_param(world.args.param[1], &o2, 1) < 0)
    return (-1);

  if (o1.type != ELFSH_OBJINT || o2.type != ELFSH_OBJINT)
    ELFSH_SETERROR("[elfsh] MOD parameter has not INTEGER type\n", -1);

  val  = (o1.immed ? o1.immed_val.ent : o1.get_obj(o1.parent));
  lav  = (o2.immed ? o2.immed_val.ent : o2.get_obj(o2.parent));
  o1.set_obj(o1.parent, val % lav);
  if (lav == 0)
    ELFSH_SETERROR("[libelfsh] Cannot divide by NUL \n", -1);
  if (!world.state.vm_quiet)
    printf("\n [*] Field modified succesfully \n\n");
  return (0);
}







