/*
** fileops.c for elfsh
** 
** Started on  Thu Feb 13 04:06:45 2003 mayhem
** Last update Sun Apr 20 21:21:48 2003 mayhem
*/
#include "elfsh.h"



/* Write command */
int			cmd_write()
{
  elfshpath_t		o1;
  elfshpath_t		o2;
  elfshsect_t		*cur;
  void			*dat;
  int			size;

  /* Sanity checks */
  if (world.args.param[0] == NULL || world.args.param[1] == NULL)
    ELFSH_SETERROR("[elfsh:cmd_write] Needs 2 parameters\n", -1);
  if (vm_lookup_param(world.args.param[0], &o1, 1) < 0)
    return (-1);
  if (vm_lookup_param(world.args.param[1], &o2, 1) < 0)
    return (-1);

  /* Type checking */
  if (o1.type != ELFSH_OBJRAW)
    ELFSH_SETERROR("[elfsh:cmd_write] Parameters must be STR or RAW typed\n",
		   -1);
  else if (o1.immed)
    ELFSH_SETERROR("[elfsh:cmd_write] Dest. param. must not be a constant\n", -1);

  /* Convert Integers into raw data */
  if (o2.type != ELFSH_OBJRAW && o2.type != ELFSH_OBJSTR)
    if (vm_convert_object(&o2, ELFSH_OBJSTR) < 0)
      return (-1);

  /* Get the source buff */
  dat = (o2.immed                ? o2.immed_val.str                : 
	 o2.type == ELFSH_OBJSTR ? o2.get_name(o2.root, o2.parent) :
	 o2.get_data(o2.parent, o2.off, o2.sizelem));

  /* Set size */
  cur = o2.parent;
  size = (o2.immed                ? o2.size     : 
	  o2.type == ELFSH_OBJSTR ? strlen(dat) :
	  cur->shdr->sh_size - o2.off);
  if (size <= 0)
    ELFSH_SETERROR("[elfsh:cmd_write] Source offset too big\n", -1);
  
  /* Write the destination buff */
  if (o1.set_data(o1.parent, o1.off, dat, size, o1.sizelem) < 0)
    return (-1);

  /* Print stuff and return */
  if (!world.state.vm_quiet)
    printf(" [*] Written %u bytes \n\n", size);
  return (0);
}


/* Append command : 2 parameters, a section and the string to be appended */
int		cmd_append()
{
  elfshsect_t	*sect;
  elfshpath_t	o2;
  elfshsect_t	*cur;
  u_int		size;
  char		*dat;
  int		index = -1;

  /* Fetch section using the first argument */
  if (vm_isnbr(world.args.param[0])) 
    {
      index = atoi(world.args.param[0]);
      if (!index)
	ELFSH_SETERROR("[elfsh:cmd_extend] Cannot convert section value !\n", 
		       -1);
      sect = elfsh_get_section_by_index(world.current, index, NULL, NULL);
    } 
  else
    sect = elfsh_get_section_by_name(world.current, world.args.param[0], 
				     NULL, NULL, NULL);
  if (sect == NULL)
    ELFSH_SETERROR("[elfsh:cmd_append] Cannot find requested section\n", -1);
  
  /* Object retreive */
  if (vm_lookup_param(world.args.param[1], &o2, 1) < 0)
    return (-1);

  /* Convert Integers into string data */
  if (o2.type != ELFSH_OBJRAW && o2.type != ELFSH_OBJSTR)
    vm_convert_object(&o2, ELFSH_OBJSTR);
  
  /* Get the source buff */
  dat = (o2.immed                ? o2.immed_val.str                : 
	 o2.type == ELFSH_OBJSTR ? o2.get_name(o2.root, o2.parent) :
	 o2.get_data(o2.parent, o2.off, o2.sizelem));
  
  /* Set size */
  cur = o2.parent;
  size = (o2.immed                ? o2.size     : 
	  o2.type == ELFSH_OBJSTR ? strlen(dat) :
	  cur->shdr->sh_size - o2.off);
  if (size <= 0)
    ELFSH_SETERROR("[elfsh:cmd_append] Source offset too big\n", -1);

  /* Append the data for real */
  if (elfsh_append_data_to_section(sect, dat, size) < 0)
    return (-1);

  /* Print msg */
  if (!world.state.vm_quiet) 
    printf(" [*] Appended %u bytes to section %s\n\n", size, sect->name);
  return (0);
}


/* Extend command : 2 parameters, a section and the extansion size */
int		cmd_extend()
{
  elfshsect_t	*sect;
  u_int		size;
  char		*new_data;
  int		err = 0;
  int		index = -1;
  
  /* Fetch section using first argument */
  if (vm_isnbr(world.args.param[0])) 
    {
      index = atoi(world.args.param[0]);
      if (!index)
	ELFSH_SETERROR("elfsh:cmd_extend] Cannot convert section value !\n", 
		       -1);
      sect = elfsh_get_section_by_index(world.current, index, NULL, NULL);
    } 
  else
    sect = elfsh_get_section_by_name(world.current, 
				     world.args.param[0], 
				     NULL, NULL, NULL);
  if (sect == NULL)
    ELFSH_SETERROR("[elfsh:cmd_extend] Cannot find requested section\n", -1);
  size = atoi(world.args.param[1]);

  /* Extend the section with NUL bytes */
  if (!size)
    ELFSH_SETERROR("[elfsh:cmd_extend] Extend size cannot be 0\n", -1);
  XALLOC(new_data, size, NULL);
  if (elfsh_append_data_to_section(sect, new_data, size) < 0)
    err = -1;
  free(new_data);
  if (!world.state.vm_quiet && !err)
    printf(" [*] Extended %s by %u bytes\n\n", sect->name, size);
  return (err);
}


/* Request to fixup the bss from command line */
int		cmd_fixup()
{
  if (world.current == NULL)
    ELFSH_SETERROR("[libelfsh] Invalid NULL parameter\n", -1);
  if (elfsh_fixup_bss(world.current) != NULL)
    {
      puts(" [*] BSS fixed up\n");
      return (0);
    }
  return (-1);
}
