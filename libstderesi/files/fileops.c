/*
** @file fileops.c
** @ingroup libstderesi
** Started on  Thu Feb 13 04:06:45 2003 jfv
** Last update Wed Mar 10 12:31:49 2004 jfv
**
** $Id: fileops.c,v 1.2 2008-02-16 12:32:27 thor Exp $
**
*/
#include "libstderesi.h"



/* Write command */
int			cmd_write()
{
  revmexpr_t		*e1;
  revmexpr_t		*e2;
  revmobj_t		*o1;
  revmobj_t		*o2;
  elfshsect_t		*cur;
  void			*dat;
  void			*sdata;
  int			size;
  char			logbuf[BUFSIZ];
  elfsh_SAddr		off;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  /* Sanity checks */
  if (world.curjob->curcmd->param[0] == NULL || 
      world.curjob->curcmd->param[1] == NULL)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Needs 2 parameters", -1);
  e1 = revm_lookup_param(world.curjob->curcmd->param[0]);
  e2 = revm_lookup_param(world.curjob->curcmd->param[1]);
  if (!e1 || !e2 || !e1->value || !e2->value || !e1->type || !e2->type)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		 "Parameters must be initialized scalar objects", -1);
  o1 = e1->value;
  o2 = e2->value;
  off = 0;
  sdata = NULL;

  /* If first parameter is an address */
  if (o1->otype->type == ASPECT_TYPE_LONG)
    {
      cur = elfsh_get_parent_section(world.curjob->curfile, o1->immed_val.ent, &off);
      if (cur)
	{
	  sdata  = elfsh_get_raw(cur);
	  sdata += off;
	}

      /* FIXME: Mapped test should be done */
      else if (e2dbg_presence_get())
	sdata = (void *) o1->immed_val.ent; 
      else
	PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		     "Invalid destination address", -1);
    }

  /* Type checking */
  else if (o1->otype->type != ASPECT_TYPE_RAW && o1->otype->type != ASPECT_TYPE_STR)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		 "Parameters must be STR or RAW typed",
		 -1);
  else if (o1->immed)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		 "Destination param must not be a constant", 
		 -1);
  
  /* Convert Integers into raw data */
  if (o2->otype->type != ASPECT_TYPE_RAW && o2->otype->type != ASPECT_TYPE_STR)
    if (revm_convert_object(e2, ASPECT_TYPE_RAW) < 0)
      PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
			"Unable to convert dest object to RAW", (-1));

  /* Get the source buff */
  dat = (o2->immed                ? o2->immed_val.str                  : 
	 o2->otype->type == ASPECT_TYPE_STR ? o2->get_name(o2->root, o2->parent) :
	 o2->get_data(o2->parent, o2->off, o2->sizelem));

  /* Set size */
  cur = o2->parent;
  size = (o2->immed ? o2->size : o2->otype->type == ASPECT_TYPE_STR ? strlen(dat) :
	  world.curjob->curcmd->param[2] ? 
	  revm_lookup_index(world.curjob->curcmd->param[2]) : 
	  cur->shdr->sh_size - o2->off);
  if (size <= 0)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Source offset too big", -1);

  /* Write raw ondisk or in memory */
  if (sdata)
    memcpy(sdata, dat, size);  
  /* Write in the destination section */
  else if (o1->set_data(o1->parent, o1->off, dat, size, o1->sizelem) < 0)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Unable to set data", (-1));

  /* Print stuff and return */
  if (!world.state.revm_quiet)
    {
      snprintf(logbuf, BUFSIZ - 1, " [*] Written %u bytes \n\n", size);
      revm_output(logbuf);
    }

  if (!o2->perm)
    XFREE(__FILE__, __FUNCTION__, __LINE__,o2);
  if (!o1->perm)
    XFREE(__FILE__, __FUNCTION__, __LINE__,o1);

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}


/* Append command : 2 parameters, a section and the string to be appended */
int		cmd_append()
{
  elfshsect_t	*sect;
  revmobj_t	*o2;
  revmexpr_t	*e2;
  elfshsect_t	*cur;
  unsigned int		size;
  char		*dat;
  int		index = -1;
  char		logbuf[BUFSIZ];

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  /* Fetch section using the first argument */
  if (revm_isnbr(world.curjob->curcmd->param[0])) 
    {
      index = atoi(world.curjob->curcmd->param[0]);
      if (!index)
	PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		     "Cannot convert section value !", -1);
      sect = elfsh_get_section_by_index(world.curjob->curfile, index, NULL, NULL);
    } 
  else
    sect = elfsh_get_section_by_name(world.curjob->curfile, world.curjob->curcmd->param[0], 
				     NULL, NULL, NULL);
  if (sect == NULL)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		 "Cannot find requested section", -1);
  
  /* Object retreive */
  e2 = revm_lookup_param(world.curjob->curcmd->param[1]);
  if (!e2 || !e2->value || !e2->type)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		 "Invalid destination object", (-1));
  o2 = e2->value;
  
  /* Convert Integers into string data */
  if (o2->otype->type != ASPECT_TYPE_RAW && o2->otype->type != ASPECT_TYPE_STR)
    {
      revm_convert_object(e2, ASPECT_TYPE_RAW);
      if (o2->otype->type != ASPECT_TYPE_RAW || !o2->perm)
	PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		     "Invalid destination object", (-1));
    }
  
  /* Get the source buff */
  dat = (o2->immed ? o2->immed_val.str : 
	 o2->otype->type == ASPECT_TYPE_STR ? o2->get_name(o2->root, o2->parent) :
	 o2->get_data(o2->parent, o2->off, o2->sizelem));
  
  /* Set size */
  cur = o2->parent;
  size = (o2->immed ? o2->size : 
	  o2->otype->type == ASPECT_TYPE_STR ? strlen(dat) :
	  cur->shdr->sh_size - o2->off);
  if (size <= 0)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		 "Source offset too big", -1);
  
  /* Append the data for real */
  if (elfsh_append_data_to_section(sect, dat, size) < 0)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		 "Unable to append data to section", (-1));
  
  /* Print msg */
  if (!world.state.revm_quiet) 
    {
      snprintf(logbuf, BUFSIZ - 1,
	       " [*] Appended %u bytes to section %s\n\n", size, sect->name);
      revm_output(logbuf);
    }
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}


/* Extend command : 2 parameters, a section and the extansion size */
int		cmd_extend()
{
  elfshsect_t	*sect;
  unsigned int		size;
  char		*new_data;
  int		index = -1;
  char		logbuf[BUFSIZ];

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  /* Fetch section using first argument */
  if (revm_isnbr(world.curjob->curcmd->param[0])) 
    {
      index = atoi(world.curjob->curcmd->param[0]);
      if (!index)
	PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
			  "Cannot convert section value", 
			  -1);
      sect = elfsh_get_section_by_index(world.curjob->curfile, index, NULL, NULL);
    } 
  else
    sect = elfsh_get_section_by_name(world.curjob->curfile, 
				     world.curjob->curcmd->param[0], 
				     NULL, NULL, NULL);
  if (sect == NULL)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Cannot find requested section", -1);
  size = atoi(world.curjob->curcmd->param[1]);

  /* Extend the section with NUL bytes */
  if (!size)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Extend size cannot be 0", -1);
  XALLOC(__FILE__, __FUNCTION__, __LINE__,new_data, size, -1);

  if (elfsh_append_data_to_section(sect, new_data, size) < 0)
    {
      XFREE(__FILE__, __FUNCTION__, __LINE__,new_data);      
      PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
			"Unable to append data", -1);
    }

  if (!world.state.revm_quiet)
    {
      snprintf(logbuf, BUFSIZ - 1,
	       " [*] Extended %s by %u bytes\n\n", sect->name, size);
      revm_output(logbuf);
    }

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}


/* Request to fixup the bss from command line */
int		cmd_fixup()
{

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  if (world.curjob->curfile == NULL)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Invalid NULL parameter", -1);
  if (elfsh_fixup_bss(world.curjob->curfile) != NULL)
    {
      revm_output(" [*] BSS fixed up\n");
      PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
    }
  PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		    "Failed to fixup BSS", (-1));
}

