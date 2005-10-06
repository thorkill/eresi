/*
** fileops.c for elfsh
** 
** Started on  Thu Feb 13 04:06:45 2003 mayhem
** Last update Wed Mar 10 12:31:49 2004 mayhem
*/
#include "elfsh.h"



/* Write command */
int			cmd_write()
{
  elfshpath_t		*o1;
  elfshpath_t		*o2;
  elfshsect_t		*cur;
  void			*dat;
  int			size;
  char			logbuf[BUFSIZ];

  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);

  /* Sanity checks */
  if (world.curjob->curcmd->param[0] == NULL || world.curjob->curcmd->param[1] == NULL)
    ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Needs 2 parameters", -1);

  o1 = vm_lookup_param(world.curjob->curcmd->param[0], 1);
  o2 = vm_lookup_param(world.curjob->curcmd->param[1], 1);
  if (!o1 || !o2)
    RET(-1);

  /* Type checking */
  if (o1->type != ELFSH_OBJRAW && o1->type != ELFSH_OBJSTR)
    ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Parameters must be STR or RAW typed",
		      -1);
  else if (o1->immed)
    ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Dest. param. must not be a constant", -1);

  /* Convert Integers into raw data */
  if (o2->type != ELFSH_OBJRAW && o2->type != ELFSH_OBJSTR)
    if (vm_convert_object(o2, ELFSH_OBJRAW) < 0)
      ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, 
			"Unable to convert dest object to RAW", (-1));

  /* Get the source buff */
  dat = (o2->immed                ? o2->immed_val.str                  : 
	 o2->type == ELFSH_OBJSTR ? o2->get_name(o2->root, o2->parent) :
	 o2->get_data(o2->parent, o2->off, o2->sizelem));

  /* Set size */
  cur = o2->parent;
  size = (o2->immed                ? o2->size    : 
	  o2->type == ELFSH_OBJSTR ? strlen(dat) :
	  world.curjob->curcmd->param[2]   ? vm_lookup_index(world.curjob->curcmd->param[2]) : 
	  cur->shdr->sh_size - o2->off);
  if (size <= 0)
    ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Source offset too big", -1);
  
  /* Write the destination buff */
  if (o1->set_data(o1->parent, o1->off, dat, size, o1->sizelem) < 0)
    ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Unable to set data", (-1));

  /* Print stuff and return */
  if (!world.state.vm_quiet)
    {
      snprintf(logbuf, BUFSIZ - 1, " [*] Written %u bytes \n\n", size);
      vm_output(logbuf);
    }

  if (!o2->perm)
    XFREE(o2);
  if (!o1->perm)
    XFREE(o1);

  ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}


/* Append command : 2 parameters, a section and the string to be appended */
int		cmd_append()
{
  elfshsect_t	*sect;
  elfshpath_t	*o2;
  elfshsect_t	*cur;
  u_int		size;
  char		*dat;
  int		index = -1;
  char		logbuf[BUFSIZ];

  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);

  /* Fetch section using the first argument */
  if (vm_isnbr(world.curjob->curcmd->param[0])) 
    {
      index = atoi(world.curjob->curcmd->param[0]);
      if (!index)
	ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, 
			  "Cannot convert section value !", 
			  -1);
      sect = elfsh_get_section_by_index(world.curjob->current, index, NULL, NULL);
    } 
  else
    sect = elfsh_get_section_by_name(world.curjob->current, world.curjob->curcmd->param[0], 
				     NULL, NULL, NULL);
  if (sect == NULL)
    ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Cannot find requested section", -1);
  
  /* Object retreive */
  o2 = vm_lookup_param(world.curjob->curcmd->param[1], 1);
  if (!o2)
    ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Unable to lookup dest object", (-1));

  /* Convert Integers into string data */
  if (o2->type != ELFSH_OBJRAW && o2->type != ELFSH_OBJSTR)
    vm_convert_object(o2, ELFSH_OBJRAW);
  
  /* Get the source buff */
  dat = (o2->immed                ? o2->immed_val.str                  : 
	 o2->type == ELFSH_OBJSTR ? o2->get_name(o2->root, o2->parent) :
	 o2->get_data(o2->parent, o2->off, o2->sizelem));
  
  /* Set size */
  cur = o2->parent;
  size = (o2->immed                ? o2->size    : 
	  o2->type == ELFSH_OBJSTR ? strlen(dat) :
	  cur->shdr->sh_size - o2->off);
  if (size <= 0)
    ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Source offset too big", -1);

  /* Append the data for real */
  if (elfsh_append_data_to_section(sect, dat, size) < 0)
    ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Unable to append data to section", (-1));

  /* Print msg */
  if (!world.state.vm_quiet) 
    {
      snprintf(logbuf, BUFSIZ - 1,
	       " [*] Appended %u bytes to section %s\n\n", size, sect->name);
      vm_output(logbuf);
    }
  ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}


/* Extend command : 2 parameters, a section and the extansion size */
int		cmd_extend()
{
  elfshsect_t	*sect;
  u_int		size;
  char		*new_data;
  int		err = 0;
  int		index = -1;
  char		logbuf[BUFSIZ];

  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);

  /* Fetch section using first argument */
  if (vm_isnbr(world.curjob->curcmd->param[0])) 
    {
      index = atoi(world.curjob->curcmd->param[0]);
      if (!index)
	ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, 
			  "Cannot convert section value", 
			  -1);
      sect = elfsh_get_section_by_index(world.curjob->current, index, NULL, NULL);
    } 
  else
    sect = elfsh_get_section_by_name(world.curjob->current, 
				     world.curjob->curcmd->param[0], 
				     NULL, NULL, NULL);
  if (sect == NULL)
    ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Cannot find requested section", -1);
  size = atoi(world.curjob->curcmd->param[1]);

  /* Extend the section with NUL bytes */
  if (!size)
    ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Extend size cannot be 0", -1);
  XALLOC(new_data, size, NULL);

  if (elfsh_append_data_to_section(sect, new_data, size) < 0)
    {
      XFREE(new_data);      
      ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, 
			"Unable to append data", -1);
    }

  if (!world.state.vm_quiet)
    {
      snprintf(logbuf, BUFSIZ - 1,
	       " [*] Extended %s by %u bytes\n\n", sect->name, size);
      vm_output(logbuf);
    }

  ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}


/* Request to fixup the bss from command line */
int		cmd_fixup()
{

  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);

  if (world.curjob->current == NULL)
    ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Invalid NULL parameter", -1);
  if (elfsh_fixup_bss(world.curjob->current) != NULL)
    {
      vm_output(" [*] BSS fixed up\n");
      ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
    }
  ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, 
		    "Failed to fixup BSS", (-1));
}

