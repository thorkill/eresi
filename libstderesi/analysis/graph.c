/*
** @file graph.c
** @ingroup libstderesi
** @brief Dump graphviz output of call graph and control flow graph.
**
** Started : Fri Mar  7 07:18:03 2003 jfv
** $Id: graph.c,v 1.2 2008-02-16 12:32:27 thor Exp $
*/
#include "libstderesi.h"

static hash_t   dumped;


/**
 * @brief Same than system() but gives hand without waiting.
 * @param cmd The command to execute.
 * @return Executed (0) or Failed (-1)
 */
int		revm_system_nowait(char *cmd)
{
  char		**argv;
  int		blanks;
  u_int		argc;
  int		ret;

  blanks = revm_findblanks(cmd);
  argv = revm_doargv(blanks, &argc, cmd);
  argv++;
  switch (fork())
    {
    case 0:
      ret = execvp(argv[0], argv);
      printf("Failed to execute system(%s) without wait (line = %s) \n", 
	     argv[0], cmd);
      exit(ret);
      break;
    case -1:
      return (-1);
    default:
      return (0);
    }
}

/** 
 * Generate the legend for the graph in HTML format 
 * @param fd
 * @param fnc
 * @param min
 * @param max
 */
void		revm_graph_legend(int fd, char *fnc, u_int min, u_int max)
{
  char	buf[BUFSIZ];

  snprintf(buf,BUFSIZ-1,"graph [label=<\n\
		<table border=\"1\">\n\
			<tr><td>Legend:</td><td>%s</td></tr>\
			<tr><td>min: 0x%08X</td><td>max: 0x%08X</td></tr>\
			<tr><td align=\"left\"><font color=%s>COND TRUE</font></td>\n\
			    <td align=\"left\"><font color=%s>COND FALSE</font></td></tr>\n\
			<tr><td align=\"left\"><font color=%s>CALL</font></td>\n\
			    <td align=\"left\"><font color=%s>RET</font></td></tr>\n\
			<tr><td align=\"left\"><font color=%s>DELAY</font></td><td></td></tr>\n\
		</table>\n\
	>]\n",fnc, min, max,
	   LNK_COLOR_TRUE,
	   LNK_COLOR_FALSE,
	   LNK_COLOR_CALL,
	   LNK_COLOR_RET,
	   LNK_COLOR_DELAY);

  write(fd,buf,strlen(buf));

}


/**
 * Disassemble a block 
 * @param fd
 * @param blk
 */
void		revm_disasm_block(int fd, mjrblock_t *blk)
{
  char		*buffer;
  char		*name;
  elfsh_SAddr	off;
  u_int		index = 0;
  int		revm_quiet,revm_colors;
  int		ret,cur;
  u_int		foffset;
  u_int		reloff;
  char		tmpbuf[20];
  u_int		len;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);
  revm_quiet = world.state.revm_quiet;
  world.state.revm_quiet = 0;
  revm_colors = nocolor;
  reloff = nocolor = 0;
  cur = 1;

  XALLOC(__FILE__, __FUNCTION__, __LINE__, buffer, blk->size, );
  foffset = elfsh_get_foffset_from_vaddr(world.curjob->curfile, blk->vaddr);
  ret     = elfsh_raw_read(world.curjob->curfile, foffset, buffer, blk->size);
  if (ret > 0)
    {
      name = elfsh_reverse_metasym(world.curjob->curfile, blk->vaddr, &off);
      while ((index < blk->size) && cur)
	{
	  len = snprintf(tmpbuf, sizeof(tmpbuf), "%3u: ", reloff);
	  write(fd, tmpbuf, len);
	  cur = revm_instr_display(fd, index, blk->vaddr, 0, blk->size,
				 name, index + off, buffer);

	  if (cur <= 0)
	    goto end;

	  index += cur;
	  reloff += cur;
	  write(fd, "\\l", 2);
	  revm_endline();
	}
    }
 end:
  XFREE(__FILE__, __FUNCTION__, __LINE__,buffer);
  world.state.revm_quiet = revm_quiet;
  nocolor = revm_colors;
  PROFILER_OUT(__FILE__, __FUNCTION__, __LINE__);
}



/**
 * Create .dot -> .png files and execute the graphic viewer
 * @param dotfile
 */
int	revm_graph_compile_graphic(char *dotfile)
{
  char	buf[BUFSIZ];

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  if ((int) config_get_data(ERESI_VMCONFIG_GRAPH_AUTOBUILD))
    {
      snprintf(buf,sizeof(buf),"dot -Tpng -o %s.png %s",dotfile,dotfile);
      system(buf);

      if ((int) config_get_data(ERESI_VMCONFIG_GRAPH_AUTOVIEW))
	{
	  snprintf(buf,sizeof(buf),
		   "%s %s.png",
		   (char *)config_get_data(ERESI_VMCONFIG_GRAPH_VIEWCMD),
		   dotfile);
	  revm_system_nowait(buf);
	}
    }
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__,(1));
}


/**
 * Returns converted path name into string without '.'
 * @param path
 */
char		*revm_flattern_path(char *path)
{
  char *r,*p;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__); 

  r = strdup(path);
  p = r;

  while(*p != '\0')
    {
      if (*p == '.')
	*p = '_';
      p++;
    }

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__,r);
}



/**
 * Prepare directory tree for graph storage
 */
int		revm_prepare_storage_dir(void)
{
  mode_t	omode = S_IRWXU | S_IRWXG | S_IRWXO;
  char		*token, *brkt;
  char		*sep = "/";
  char		tmp[BUFSIZ], path[BUFSIZ];
  u_int		cur;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  cur = 0;
  memset(tmp,0x00, BUFSIZ);
  memset(path,0x00, BUFSIZ);

  snprintf(path,BUFSIZ-1,"%s/%s",(char *)config_get_data(ERESI_VMCONFIG_GRAPH_STORAGEPATH),
	   revm_flattern_path(world.mjr_session.cur->obj->name));

  for (token = strtok_r(path,sep,&brkt);
       token;
       token = strtok_r(NULL,sep,&brkt))
    {
      tmp[cur++] = '/';
      strncat(tmp, token, strlen(token));
      cur += strlen(token);
      
      mkdir(tmp, omode);
    }
  
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 1);
}

/**
 * This function prepares a path string to .dot file
 * @param opt
 * @param prefix
 * @return
 */
char		*revm_get_dotfile_name(char *opt,char *prefix)
{
  int		namelen;
  char		*dotfile;
  char		*str1;
  char		*str2;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  if (!opt)
    opt = "dump";
  if (!prefix)
    prefix = "";

  str1 = (char *) config_get_data(ERESI_VMCONFIG_GRAPH_STORAGEPATH);
  str2 = revm_flattern_path(world.mjr_session.cur->obj->name);
  namelen = strlen(str1) + strlen(str2) + strlen(opt) + strlen(prefix) + 32;

  XALLOC(__FILE__,__FUNCTION__,__LINE__, dotfile, namelen, NULL);
  if (!revm_prepare_storage_dir())
     PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
		 "Cannot prepare storage directory", NULL);
  
  snprintf(dotfile,namelen,"%s%s/%s-%s.dot",
	   (char *)config_get_data(ERESI_VMCONFIG_GRAPH_STORAGEPATH),
	   revm_flattern_path(world.mjr_session.cur->obj->name),
	   prefix,opt);

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, (dotfile));
}

/**
 * Converts string to vaddr by looking up the symbol name
 * or converting it to hex
 * @param s
 * @return
 */
u_int		revm_get_vaddr(char *s)
{
  elfsh_Sym	*sym;
  u_int		min;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  /* Get parameters */
  if ((sym = elfsh_get_metasym_by_name(world.curjob->curfile,s)))
    min = sym->st_value;
  else
    min = strtoul(s, 0, 16);

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__,(min));
}

/**
 * Returns next function vaddress 
 */
u_int		revm_get_next_function_vaddr(u_int min)
{
  container_t *cntnr;
  u_int		max,tmp,ltmp;
  char		**keys;
  int		idx,fnbr;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  keys = hash_get_keys(&world.mjr_session.cur->funchash, &fnbr);
  max = ltmp = min;

  for (idx = 0; idx < fnbr; idx++)
    {
      cntnr = hash_get(&world.mjr_session.cur->funchash, keys[idx]);
      tmp = ((mjrfunc_t *)cntnr->data)->vaddr;
      
      if (((max == min) || (tmp < max)) && (tmp > min))
	max = tmp;
    }

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__,(max));
}

/**
 * Get color name for string, it does check alert
 * and lookups the color name
 */
char		*revm_get_colored_str(char *str,int type)
{
  color_t	*t;
  int		idx,c,t2;
  char		**keys;
  char		*color;
  char		buf[BUFSIZ];

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  color = FNC_COLOR_DEFAULT;
  
  if (world.state.revm_use_alert && 
      !regexec(&world.state.revm_alert, str, 0, 0, 0))
    {
      t = hash_get(&t_color_hash, "warnstring");
      
      keys = hash_get_keys(&fg_color_hash, &idx);
      
      for (c = 0; c < idx; c++)
	{
	  t2 = (int) hash_get(&fg_color_hash, keys[c]);
	  if (t2 == t->fground)
	    {
	      snprintf(buf, sizeof(buf), "\"%s\"", keys[c]);
	      color = strdup(buf);
	      break;
	    }
	}
      PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, (color));
    }
  
  switch (type)
    {
    case GVZ_NODE_EPOINT:
      color = FNC_COLOR_EPOINT;
	break;
    case GVZ_NODE_MAIN:
      color = FNC_COLOR_MAIN;
      break;
    case GVZ_NODE_INTERN:
      color = FNC_COLOR_INTERN;
      break;
    case GVZ_NODE_EXTERN:
      color = FNC_COLOR_EXTERN;
      break;
    case GVZ_NODE_UNKNOWN:
      color = GVZ_COLOR_CORAL;
      break;
    default:
      color = FNC_COLOR_DEFAULT;
    }

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, (color));
}

int		revm_graph_get_function_type(mjrfunc_t *fnc)
{
  elfshsect_t   *parent;
  int		ftype;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  parent = elfsh_get_parent_section(world.curjob->curfile, fnc->vaddr, NULL);

  if (fnc->vaddr == elfsh_get_entrypoint(elfsh_get_hdr(world.mjr_session.cur->obj)))
    ftype = GVZ_NODE_EPOINT;
  else if ((parent == NULL)||(parent->name == NULL))
    ftype = GVZ_NODE_UNKNOWN;
  else if (parent && parent->name && strcmp(".text",parent->name))
    ftype = GVZ_NODE_EXTERN;
  else
    ftype = GVZ_NODE_INTERN;

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, ftype);
}


/* Write the header dot description for a node */
char		*revm_write_dotnode(int fd, elfshobj_t *obj, eresi_Addr addr)
{
  char		buf[BUFSIZ];
  char		*name;
  elfsh_SAddr	offset;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);
  name = elfsh_reverse_metasym(obj, addr, &offset);
  if (name && !offset)
    snprintf(buf, sizeof(buf),
	     "\"%s\" [shape=\"box\" color=%s label=\"<%s>:\\l",
	     name, "\"grey\"", name);
  else
    snprintf(buf, sizeof(buf),
	     "\"" AFMT "\" [shape=\"box\" color=%s label=\"<" AFMT ">:\\l",
	     addr, "\"grey\"", addr);
  write(fd, buf, strlen(buf));
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, (!offset ? name : NULL));
}

/* Write the footer dot description for a node */
int		revm_write_endnode(int fd)
{
  char		buf[BUFSIZ];

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);
  snprintf(buf, sizeof(buf), "\"];\n");
  write(fd, buf, strlen(buf));
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}



/* A recursive function for graphing in dot format */
int		revm_graph_blocks(container_t   *cntnr,
				  int		 fd,
				  eresi_Addr	min,
				  eresi_Addr	max,
				  int		dir,
				  int		maxdepth,
				  int		curdepth)
{
  mjrblock_t	*blk,*cblk;
  mjrlink_t	*lnk;
  char		buf[BUFSIZ];
  char		*vaddr_str;
  char		*col_arrow;
  container_t	*nextcnt;
  mjrblock_t	*nextblk;    
  list_t	*linklist;
  listent_t	*curent;
  char		*name;
  char		*sname;
  elfsh_SAddr	soffset;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  blk = (mjrblock_t *) cntnr->data;

  vaddr_str =_vaddr2str(blk->vaddr);

  if (hash_get(&dumped, vaddr_str) || curdepth == maxdepth)
    PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);

  hash_add(&dumped, vaddr_str, cntnr);

  linklist = cntnr->outlinks;

  name = revm_write_dotnode(fd, world.mjr_session.cur->obj, blk->vaddr);
  revm_disasm_block(fd, blk);
  revm_write_endnode(fd);

  /* Print all links */
  for (curent = linklist->head; curent; curent = curent->next)
    {
      lnk  = (mjrlink_t *)  curent->data;
      cblk = (mjrblock_t *) mjr_lookup_container(world.mjr_session.cur, lnk->id)->data;

      switch (lnk->type)
	{
	case MJR_LINK_BLOCK_COND_ALWAYS:
	case MJR_LINK_BLOCK_COND_TRUE:
	  col_arrow = LNK_COLOR_TRUE;
	  break;
	case MJR_LINK_BLOCK_COND_FALSE:
	  col_arrow = LNK_COLOR_FALSE;
	  break;
	case MJR_LINK_FUNC_CALL:
	  col_arrow = LNK_COLOR_CALL;
	  break;
	case MJR_LINK_FUNC_RET:
	  col_arrow = LNK_COLOR_RET;
	  break;
	case MJR_LINK_TYPE_DELAY:
	  col_arrow = LNK_COLOR_DELAY;
	  break;
	default:
	  fprintf(D_DESC, "[D] %s 1st: lnk->type:%d\n", __FUNCTION__, lnk->type);
	  col_arrow = "\"black\"";
	  break;
	}

      sname = elfsh_reverse_metasym(world.mjr_session.cur->obj, cblk->vaddr, &soffset);
      if (name)
	{
	  if (sname && !soffset)
	    snprintf(buf, sizeof(buf), "\"%s\" -> \"%s\" [color=%s];\n",
		     name, sname, col_arrow);
	  else
	    snprintf(buf, sizeof(buf), "\"%s\" -> \"" AFMT "\" [color=%s];\n",
		     name, cblk->vaddr, col_arrow);
	}
      else
	{
	  if (sname && !soffset)
	    snprintf(buf, sizeof(buf), "\"" AFMT "\" -> \"%s\" [color=%s];\n",
		     blk->vaddr, sname, col_arrow);
	  else
	    snprintf(buf, sizeof(buf), "\"" AFMT "\" -> \"" AFMT "\" [color=%s];\n",
		     blk->vaddr, cblk->vaddr, col_arrow);
	}

#if __DEBUG_GRAPH__
      fprintf(D_DESC,"[D] %s: " AFMT " -> " AFMT " %d\n",
	      __FUNCTION__, blk->vaddr, cblk->vaddr, lnk->type);
#endif

      write(fd, buf, strlen(buf));
    }

  /* Now recurse on the children */
  for (curent = linklist->head; curent; curent = curent->next)
    {
      lnk = (mjrlink_t *) curent->data;
      if (lnk->scope == MJR_LINK_SCOPE_GLOBAL)
	continue;
      
      nextcnt = mjr_lookup_container(world.mjr_session.cur, lnk->id);
      nextblk = (mjrblock_t *) nextcnt->data;

      //if ((nextblk->vaddr >= min) && (nextblk->vaddr <= max))
	revm_graph_blocks(nextcnt, fd, min, max, 0, 0, curdepth + 1);
    }

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}



/**
 * This function does dump a function container in graphviz format
 * @param fd
 * @param direction input/output
 * @param type 0 - we are dumping only out, 1 - we generate .dot file for
 *			both directions
 * @param maxdepth
 * @param curdepth
 */
int		revm_graph_function(container_t		*cntnr,
				    int			fd,
				    int			direction,
				    int			type,
				    int			maxdepth, 
				    int			curdepth)
{
  elfsh_SAddr	offset;
  int	    	ftype;
  mjrfunc_t	*fnc, *tmpfnc;
  mjrlink_t	*curlnk;
  char		*n1, *n2, *vaddr_str;
  char		buf[BUFSIZ];
  list_t	*linklist;
  listent_t	*curent;
  container_t	*child;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  if (!cntnr)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
		 "No container found", -1);

  fnc = (mjrfunc_t *)cntnr->data;
  vaddr_str =_vaddr2str(fnc->vaddr);

  if (hash_get(&dumped, vaddr_str) || curdepth == maxdepth)
    PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);

  hash_add(&dumped, vaddr_str, cntnr);
  
  n1 = elfsh_reverse_metasym(world.curjob->curfile,fnc->vaddr, &offset);

  ftype = revm_graph_get_function_type(fnc);
  
  snprintf(buf, sizeof(buf), "\"%s\" [color=%s];\n",
      	   (n1 && !offset ? n1 : fnc->name), 
	   revm_get_colored_str((n1) ? n1 : fnc->name, ftype));
  
  write(fd,buf,strlen(buf));

  linklist = mjr_link_get_by_direction(cntnr,direction);

  /* Print all links of this function */
  for (curent = linklist->head; curent; curent = curent->next)
    {
      curlnk = (mjrlink_t *) curent->data;
      tmpfnc = (mjrfunc_t *) mjr_lookup_container(world.mjr_session.cur,curlnk->id)->data;
      
      /* resolve the symbols */
      n2 = elfsh_reverse_metasym(world.curjob->curfile, tmpfnc->vaddr, &offset);
      ftype = revm_graph_get_function_type(tmpfnc);
      
      if (type)
	{
	  snprintf(buf, sizeof(buf),
		   "\"%s\" [color=%s];\n",
		   (n2) ? n2 : tmpfnc->name,
		   revm_get_colored_str((n2) ? n2 : tmpfnc->name, ftype));
	  
	  write(fd,buf,strlen(buf));
	}
      
      if (direction == CONTAINER_LINK_OUT)
	snprintf(buf,sizeof(buf),
		 "\"%s\" -> \"%s\";\n",
		 (n1) ? n1 : fnc->name,
		 (n2) ? n2 : tmpfnc->name);
      
      else if (direction == CONTAINER_LINK_IN)
	{
	  snprintf(buf,sizeof(buf),
		   "\"%s\" -> \"%s\";\n",
		   (n2) ? n2 : tmpfnc->name,
		   (n1) ? n1 : fnc->name);
	}
      
      write(fd,buf,strlen(buf));
    }
  
  /* Recurse on children */
  for (curent = linklist->head; curent; curent = curent->next)
    {
      curlnk = (mjrlink_t *) curent->data;
      child = mjr_lookup_container(world.mjr_session.cur, curlnk->id);
      revm_graph_function(child, fd, direction, type, maxdepth, curdepth + 1);
    }
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}

/** 
 * Open the file for putting the dot description 
 * @param dotfile
 * @param fd
 * @returm
 */
int	revm_open_dot_file(char *dotfile, int *fd)
{
  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  printf(" [*] .dot file: %s\n\n", dotfile);

  *fd = open(dotfile, O_RDWR | O_CREAT | O_TRUNC, 0644);
  if (*fd == -1)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
		 "Cannot create graphviz file", -1);
  
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 1);
}



u_int		revm_get_min_param(void)
{
  revmexpr_t	*expr;
  revmobj_t	*var;
  u_int		min;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);
  expr = revm_lookup_var(world.curjob->curcmd->param[1]);
  if (expr && expr->value)
    {
      var = expr->value;
      min = revm_get_vaddr(var->immed ? var->immed_val.str : 
			   var->get_name(var->root, var->parent));
    }
  else
    min = revm_get_vaddr(world.curjob->curcmd->param[1]);
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, (min));
}


/* Graph the binary */
int		cmd_graph(void)
{
  container_t	*cntnr;
  int		fd;
  char		buf[BUFSIZ];
  u_int		min;
  u_int		max;
  char		*dotfile;
  u_int		maxdepth;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);
  
  hash_init(&dumped, "dumped_containers", mjrHashVerySmall, ASPECT_TYPE_UNKNOW);
  
  /* Some preliminary checks */
  if (!world.mjr_session.cur->analysed)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
		 "Control flow section not found"
		 " : use analyse command", -1);
  
  /* Select use depending on number of arguments */
  switch (world.curjob->curcmd->argc)
    {
    case 0:
      
      dotfile = revm_get_dotfile_name(NULL, "object");
      
      revm_open_dot_file(dotfile, &fd);

      snprintf(buf, sizeof(buf),"strict digraph prof {\n ratio=fill;node [style=\"filled\"];\n");
      write(fd,buf,strlen(buf));
      
      printf(" [*] Dumping %d functions\n\n", 
	     world.mjr_session.cur->funchash.elmnbr);
      
      cntnr = mjr_get_container_by_vaddr(world.mjr_session.cur, 
					 world.mjr_session.cur->obj->hdr->e_entry, 
					 ASPECT_TYPE_FUNC);

      //revm_graph_function(cntnr,fd,CONTAINER_LINK_IN,0, 0, 1);
      revm_graph_function(cntnr,fd,CONTAINER_LINK_OUT,0, 0, 1);
      
      write(fd,"}\n",2);
      close(fd);

      revm_graph_compile_graphic(dotfile);
 
      break;

    case 2:

    	if (strcmp("func",world.curjob->curcmd->param[0]) == 0)
  	  {
	    /* resolve the block vaddr */
  	    min = revm_get_min_param();
	    cntnr = hash_get(&world.mjr_session.cur->funchash,
			     _vaddr2str(min));
	    if (!cntnr)
	      PROFILER_ERR(__FILE__,__FUNCTION__,__LINE__,
			   "Function not found",-1);
	    
	    dotfile = revm_get_dotfile_name(_vaddr2str(min),"function");
	    revm_open_dot_file(dotfile, &fd);
	    
  	    snprintf(buf, sizeof(buf),"strict digraph prof {\n ratio=fill;node [style=\"filled\"];\n");
  	    write(fd,buf,strlen(buf));

  	    revm_graph_function(cntnr,fd,CONTAINER_LINK_IN,1, 0, 1);
  	    revm_graph_function(cntnr,fd,CONTAINER_LINK_OUT,1, 0, 1);

  	    write(fd,"}\n",2);
  	    close(fd);

  	    revm_graph_compile_graphic(dotfile);

	    }
    	else if (strcmp("bloc",world.curjob->curcmd->param[0]) == 0)
  	  {
	    min = revm_get_min_param();
  	    max = revm_get_next_function_vaddr(min);
	    
	    cntnr = hash_get(&world.mjr_session.cur->blkhash,
			     _vaddr2str(min));

	    if (!cntnr)
	      PROFILER_ERR(__FILE__,__FUNCTION__,__LINE__,
			   "Function not found",-1);

  	    printf(" [D] MIN:%x MAX:%x\n",min,max);

	    dotfile = revm_get_dotfile_name(_vaddr2str(min),"block");
	    revm_open_dot_file(dotfile, &fd);

  	    snprintf(buf, sizeof(buf),"strict digraph prof {\n");
  	    write(fd,buf,strlen(buf));

	    revm_graph_legend(fd, "DEFAULT", min, max);

	    /* FIXME: the function addr interval is not always well guessed
	       which leads to truncated control flow graphs */
	    revm_graph_blocks(cntnr,fd,min,max,0,0,1);

	    write(fd,"}\n",2);
	    close(fd);

	    revm_graph_compile_graphic(dotfile);

  	    revm_output(" [*] Dump function blocks\n\n");
  	  }
	else
	  revm_output(" [!] Invalid syntax: help graph\n");
	break;

	/* FIXME: graph <func|bloc> <sym|func> <in|out> */
    case 3:
      break;
	
      /* FIXME: graph <func|bloc> <sym|func> <in|out|all> <maxdepth> */
    case 4:
      break;

    default:
      {
    	revm_output(" [!] INVALID SYNTAX \n");
	revm_output(HLP_GRAPH);
      }
    
  }

  hash_empty("dumped_containers");

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}
