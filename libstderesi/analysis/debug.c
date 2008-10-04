/*
** @file debug.c
** @ingroup libstderesi
** @brief All functions dealing with libedfmt uni debugging format
**
** Started on Fev 25 2007 mxatone
**
** $Id: debug.c,v 1.2 2008-02-16 12:32:27 thor Exp $
**
*/
#include "libstderesi.h"

char buf[BUFSIZ];
char tmpbuf[BUFSIZ];
char filebuf[BUFSIZ];

char *file_title = NULL;
char *type_title = NULL;
char *var_title = NULL;

int matched;

/**
 * @todo Complete doxygen tag
 * @param type
 * @return
 */
static char 	*revm_debug_typename(unsigned int type)
{
  char 			*str;
  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  switch(type)
    {
    case EDFMT_TYPE_UNK:
      str = "?";
      break;
    case EDFMT_TYPE_BASIC:
      str = "basic";
      break;
    case EDFMT_TYPE_ARRAY:
      str = "array";
      break;
    case EDFMT_TYPE_PTR:
      str = "ptr";
      break;
    case EDFMT_TYPE_STRUCT:
      str = "struct";
      break;
    case EDFMT_TYPE_UNION:
      str = "union";
      break;
    case EDFMT_TYPE_ATTR:
      str = "attr";
      break;
    case EDFMT_TYPE_VOID:
      str = "void";
      break;
    case EDFMT_TYPE_LINK:
      str = "link";
      break;
    default:
      str = "?????";
    }

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, str);
}

/**
 * @todo Complete doxygen tags
 * @param vars
 * @param rx
 * @return
 */
static int		revm_debug_print_var(edfmtvar_t *vars, regex_t *rx)
{
  edfmtvar_t		*var;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  for (var = vars; var != NULL; var = var->next)
    {
      snprintf(buf, BUFSIZ - 1, " %s%s %s%s %s%s => %s\n",
	       revm_colorfieldstr("scope:"),
	       revm_colornumber("%u", var->scope),
	       revm_colorfieldstr("name:"),
	       revm_colorstr_fmt("%-20s", var->name), 
	       revm_colorfieldstr("addr:"),
	       revm_coloraddress(XFMT, var->addr),
	       revm_colorstr(var->type->name));
      revm_endline();

      if (rx == NULL || !regexec(rx, buf, 0, 0, 0))
	{
	  /* Print file title only once */
	  if (file_title)
	    {
	      revm_output(file_title);
	      file_title = NULL;
	    }

	  /* Print var title only once */
	  if (var_title)
	    {
	      revm_output(var_title);
	      var_title = NULL;
	    }

	  revm_output(buf);
	  matched++;
	}
    }

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}

/**
 * @todo Complete doxygen tags
 * @param types
 * @param tab
 * @param rx
 * @return
 */

static int		revm_debug_print_type(edfmttype_t *types, unsigned int tab, regex_t *rx)
{
  edfmttype_t		*type;
  unsigned int			index;
  unsigned int			added = 0;
  unsigned int			bufsiz = 0;
  int			len;
  int			save_matched;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  for (type = types; type != NULL; type = type->next)
    {
      buf[0] = tmpbuf[0] = 0x00;

      len = snprintf(tmpbuf, BUFSIZ - 1,
		     " %s%s %s%s %s%s %s%s %s%s %s%s ", 
		     revm_colorfieldstr("type:"),
		     revm_colortypestr_fmt("%-10s", revm_debug_typename(type->type)), 
		     revm_colorfieldstr("name:"),
		     revm_colorstr_fmt("%-20s", type->name), 
		     revm_colorfieldstr("offset:"),
		     revm_colornumber("%04u", type->start), 
		     revm_colorfieldstr("size:"),
		     revm_colornumber("%04u", type->size), 
		     revm_colorfieldstr("parsed:"),
		     revm_colortypestr_fmt("%-4s", type->parsed ? "yes" : "no"), 
		     revm_colorfieldstr("valid:"),
		     revm_colortypestr_fmt("%-4s", type->valid ? "yes" : "no"));
      revm_endline();

      if (bufsiz + len < BUFSIZ)
	{
	  strcat(buf, tmpbuf);
	  bufsiz += len;
	}

      if (type->type != EDFMT_TYPE_ATTR && type->parent)
	{
	  snprintf(tmpbuf, BUFSIZ - 1,
		   "%s%s ", 
		   revm_colorfieldstr("parent:"),
		   revm_colorstr_fmt("%-10s", type->parent->name));
	  revm_endline();

	  if (bufsiz + len < BUFSIZ)
	    {
	      strcat(buf, tmpbuf);
	      bufsiz += len;
	    }
	}

      if (type->type == EDFMT_TYPE_ATTR && type->child)
	{
	  snprintf(tmpbuf, BUFSIZ - 1,
		   "%s%s ", 
		   revm_colorfieldstr("child:"),
		   revm_colorstr_fmt("%-10s", type->child->name));
	  revm_endline();

	  if (bufsiz + len < BUFSIZ)
	    {
	      strcat(buf, tmpbuf);
	      bufsiz += len;
	    }
	}

      /* Print only if it match a submited regex */
      if (rx == NULL || !regexec(rx, buf, 0, 0, 0))
	{
	  /* Print file title only once */
	  if (file_title)
	    {
	      revm_output(file_title);
	      file_title = NULL;
	    }

	  /* Print type title only once */
	  if (type_title)
	    {
	      revm_output(type_title);
	      type_title = NULL;
	    }

	  if (tab > 0 && added == 0)
	    {
	      revm_output(" .: Childs\n");
	    }
	  else if (added > 0 && type->type != EDFMT_TYPE_ATTR && type->child)
	    revm_output("\n");

	  added = 1;

	  for (index = 0; index < tab; index++)
	    revm_output(" ");

	  revm_output(buf);
	  revm_output("\n");
	  matched++;

	  /* We will print childs without match anything */
	  save_matched = matched;

	  /* Print childs if we got some (without regex) */
	  if (type->type != EDFMT_TYPE_ATTR && type->child)
	    revm_debug_print_type(type->child, tab+1, NULL);

	  matched = save_matched;
	}
      else
	{
	  /* Print childs if we got some (with regex) */
	  if (type->type != EDFMT_TYPE_ATTR && type->child)
	    revm_debug_print_type(type->child, tab+1, rx);
	}
    }

  if (added)
    revm_output("\n");

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}

static int		revm_debug_unit_print_file(edfmtfile_t *files, regex_t *rx)
{
  edfmtfile_t		*file;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  for (file = files; file != NULL; file = file->next)
    {
      snprintf(filebuf, BUFSIZ - 1,
	       "\n <: %s %s %s %s\n",
	       revm_colorfieldstr("File:"),
	       revm_colorstr_fmt("%-20s", file->name),
	       file->parent ? revm_colorfieldstr("Parent:") : "",
	       file->parent ? revm_colorstr(file->parent->name) : "");
      file_title = filebuf;
      revm_endline();

      type_title = "\n .: Types :. \n\n";
      var_title = "\n .: Vars :. \n\n";

      revm_debug_print_type(file->types, 0, rx);
      revm_debug_print_var(file->vars, rx);

      if (file->child)
	revm_debug_unit_print_file(file->child, rx);
    }

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}

int			cmd_debug()
{
  char			*regexname = NULL;
  regex_t	      	rx, *submit_rx = NULL;
  edfmtinfo_t		*info;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  if (world.curjob->curcmd->argc > 1)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		 "Too many arguments for debug command", -1);

  matched = 0;

  /* Support filter by regex */
  if (world.curjob->curcmd->param[0])
    {
      regexname = world.curjob->curcmd->param[0];

      if (regcomp(&rx, regexname, REG_EXTENDED) < 0)
	PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
		     "Failed to compute regex", -1);

      submit_rx = &rx;
    }
  
  info = edfmt_get_uniinfo(world.curjob->curfile);

  if (!info)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		 "No informations on this file", -1);

  type_title = "\n .: Global Types :. \n\n";
  var_title = "\n .: Global Vars :. \n\n";
  
  revm_debug_print_type(info->types, 0, submit_rx);
  revm_debug_print_var(info->vars, submit_rx);

  revm_debug_unit_print_file(info->files, submit_rx);

  snprintf(buf, BUFSIZ - 1, " [*] Matched %u entries  \n\n", matched);
  revm_output(buf);

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}
