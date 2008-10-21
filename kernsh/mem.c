/*
** @file mem.c
** @ingroup kernsh
** $Id: mem.c,v 1.14 2008-02-16 12:32:26 thor Exp $
**
*/
#include "kernsh.h"
#include "libkernsh.h"

/* Handle the sys_call_table */
int		cmd_sct()
{
  int		ret;
  int		index;
  listent_t     *actual;
  list_t	*h;
  libkernshsyscall_t *sct;
  char		buff[BUFSIZ];

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  XALLOC(__FILE__, __FUNCTION__, __LINE__, h, sizeof(list_t), -1);
  elist_init(h, "cmd_sct_list", ASPECT_TYPE_UNKNOW);
  ret = kernsh_sct(h);

  memset(buff, '\0', sizeof(buff));
  snprintf(buff, sizeof(buff), 
	   "%s\n\n",
	   revm_colorfieldstr("[+] SYS_CALL_TABLE"));
  revm_output(buff);

  for (index = 0, actual = h->head; 
       index < h->elmnbr; 
       index++, actual = actual->next)
    {
      sct = (libkernshsyscall_t *) actual->data;
      memset(buff, '\0', sizeof(buff));
      snprintf(buff, sizeof(buff),
	       "%s %-40s %s %s\n", 
	       revm_colornumber("id:%-10u", (unsigned int)index),
	       revm_colortypestr_fmt("%s", sct->name),
	       revm_colorstr("@"),
	       revm_coloraddress(XFMT, (eresi_Addr) sct->addr));
      revm_output(buff);
      revm_endline();
      
    }
  
  revm_output("\n");
  elist_destroy(h);

  if (ret)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Cannot get syscalltable", -1);

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}

/* Handle the idt */
int		cmd_idt()
{
  int		ret;
  int		index;
  listent_t     *actual;
  list_t	*h;
  libkernshint_t *dint;
  char		buff[BUFSIZ];

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  XALLOC(__FILE__, __FUNCTION__, __LINE__, h, sizeof(list_t), -1);
  elist_init(h, "cmd_idt_list", ASPECT_TYPE_UNKNOW);
  ret = kernsh_idt(h);
        
  memset(buff, '\0', sizeof(buff));
  snprintf(buff, sizeof(buff), 
	   "%s\n\n",
	   revm_colorfieldstr("[+] IDT"));
  revm_output(buff);
  
  for (index = 0, actual = h->head; 
       index < h->elmnbr; 
       index++, actual = actual->next)
    {
      dint = (libkernshint_t *) actual->data;

      snprintf(buff, sizeof(buff),
	       "%s %-40s %s %s\n",
	       revm_colornumber("id:%-10u", (unsigned int)index),
	       revm_colortypestr_fmt("%s", dint->name),
	       revm_colorstr("@"),
	       revm_coloraddress(XFMT, (eresi_Addr) dint->addr));
      revm_output(buff);
      revm_endline();
    }

  revm_output("\n");
  elist_destroy(h);

  if (ret)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Cannot get idt", -1);

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}

/* Handle the gdt */
int		cmd_gdt()
{
  int		ret;
  int		index;
  int		i;
  listent_t     *actual;
  list_t	*h;
  libkernshsgdt_t *sgdt;
  char		buff[BUFSIZ];

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  XALLOC(__FILE__, __FUNCTION__, __LINE__, h, sizeof(list_t), -1);

  elist_init(h, "cmd_gdt_list", ASPECT_TYPE_UNKNOW);

  ret = kernsh_gdt(h);
        
  memset(buff, '\0', sizeof(buff));
  snprintf(buff, sizeof(buff), 
	   "%s\n\n",
	   revm_colorfieldstr("[+] GDT"));
  revm_output(buff);
  
  for (i = 0, index = 0, actual = h->head; 
       index < h->elmnbr; 
       i+=8, index++, actual = actual->next)
    {
      sgdt = (libkernshsgdt_t *) actual->data;
      
      snprintf(buff, sizeof(buff),
	       "%s%s %s %s\n",
	       revm_coloraddress("%.8lX", (eresi_Addr) sgdt->deb),
	       revm_coloraddress("%.8lX", (eresi_Addr) sgdt->fin),
	       revm_colorstr("@"),
	       revm_coloraddress(XFMT, (eresi_Addr) sgdt->addr));

      revm_output(buff);
      
      revm_endline();
    } 
	     
  revm_output("\n");
  elist_destroy(h);

  if (ret)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Cannot get gdt", -1);

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}

/* Handle kernel symbol */
int		cmd_ksym()
{
  char          *param;
  char		buff[BUFSIZ];
  eresi_Addr	addr;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);
  param = world.curjob->curcmd->param[0];

  if (param)
    {
      if (kernsh_get_addr_by_name(param, &addr, strlen(param)))
	{
	  revm_setvar_int("_", -1);
	  PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		       "Cannot get symbol", -1);
	}
      memset(buff, '\0', sizeof(buff));
      snprintf(buff, sizeof(buff), 
	       "%s %s %s %s %s\n\n",
	       revm_colorfieldstr("SYMBOL"),
	       revm_colorstr(param),
	       revm_colorfieldstr("is"),
	       revm_colorstr("@"),
	       revm_coloraddress(XFMT, (eresi_Addr) addr));
      revm_output(buff);
      revm_setvar_long("_", addr);
    }

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}

int		cmd_kmodule_relink()
{
  int		ret;
  char		*param, *param2, *param3;
  char		buff[BUFSIZ];

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  memset(buff, '\0', sizeof(buff));

  param = world.curjob->curcmd->param[0];
  param2 = world.curjob->curcmd->param[1];
  param3 = world.curjob->curcmd->param[2];

  if (param && param2)
    {
      ret = kernsh_relink_module(param, param2, param3);
      if (ret == 0)
	{
	  if (param3 == NULL)
	    param3 = param;
	  snprintf(buff, sizeof(buff), 
		   "Module %s and %s is linked in %s\n\n",
		   revm_colorstr(param),
		   revm_colorstr(param2),
		   revm_colorstr(param3));
	  revm_output(buff);
	}
    }

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, ret);
}

int		cmd_kmodule_infect()
{
  int		ret;
  char		*param, *param2, *param3;
  char		buff[BUFSIZ];

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  memset(buff, '\0', sizeof(buff));

  param = world.curjob->curcmd->param[0];
  param2 = world.curjob->curcmd->param[1];
  param3 = world.curjob->curcmd->param[2];

  if (param && param2 && param3)
    {
      ret = kernsh_infect_module(param, param2, param3);
      if (ret == 0)
	{
	  snprintf(buff, sizeof(buff), 
		   "%s have been replaced by %s in %s\n\n",
		   revm_colorstr(param),
		   revm_colorstr(param2),
		   revm_colorstr(param3));
	  revm_output(buff);
	}
    }

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, ret);
}

int		cmd_kmodule_load()
{
  int		ret;
  char		*filename;
  char		buff[BUFSIZ];

  ret = -1;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  filename = world.curjob->curcmd->param[0];

  if (filename)
    {
      ret = kernsh_kload_module(filename);
      if (ret == 0)
	{
	  snprintf(buff, sizeof(buff), 
		   "Module %s is loaded\n\n",
		   revm_colorstr(filename));
	  revm_output(buff);
	}
    }

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, ret);
}

int		cmd_kmodule_unload()
{
  int		ret;
  char		*filename;
  char		buff[BUFSIZ];

  ret = -1;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  filename = world.curjob->curcmd->param[0];

  if (filename)
    {
      ret = kernsh_kunload_module(filename);
      if (ret == 0)
	{
	  snprintf(buff, sizeof(buff), 
		   "Module %s is unloaded\n\n",
		   revm_colorstr(filename));
	  revm_output(buff);
	}
    }

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, ret);
}

int		cmd_kmem_info()
{
  char		buff[BUFSIZ];
  char		*data;
  int		val;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  data = (char *)config_get_data(LIBKERNSH_CONFIG_DEVICE);
  snprintf(buff, sizeof(buff), 
	   "DEVICE : %s\n",
	   data);
  revm_output(buff);

  data = (char *)config_get_data(LIBKERNSH_CONFIG_MODE);
  snprintf(buff, sizeof(buff), 
	   "FLAGS : %s\n",
	   data);
  revm_output(buff);

  switch(libkernshworld.mem)
    {
    case LIBKERNSH_MEM_MODE :
      snprintf(buff, sizeof(buff), 
	       "MODE : DYNAMIC\n");
      break;
    case LIBKERNSH_STATIC_MODE :
      snprintf(buff, sizeof(buff), 
	       "MODE : STATIC\n");
      break;
    }
  revm_output(buff);

  data = (char *)config_get_data(LIBKERNSH_CONFIG_SYSTEMMAP);
  snprintf(buff, sizeof(buff), 
	   "SYSTEMMAP : %s\n",
	   data);
  revm_output(buff);

  val = (int)config_get_data(LIBKERNSH_CONFIG_MMAP);
  switch (val)
    {
    case 0 :
      snprintf(buff, sizeof(buff), 
	       "MMAP : OFF\n");
      break;
    case 1 :
      snprintf(buff, sizeof(buff), 
	       "MMAP : ON\n");
      break;
    }
  revm_output(buff);

  val = (int)config_get_data(LIBKERNSH_CONFIG_MMAP_SIZE);
  snprintf(buff, sizeof(buff), 
	   "MMAP_SIZE : 0x%lx\n",
	   (unsigned long)val);
  revm_output(buff);

  val = (int)config_get_data(LIBKERNSH_CONFIG_KERNEL_START);
  snprintf(buff, sizeof(buff), 
	   "KERNEL_START : 0x%lx\n",
	   (unsigned long)val);
  revm_output(buff);

  val = (int)config_get_data(LIBKERNSH_CONFIG_KERNEL_END);
  snprintf(buff, sizeof(buff), 
	   "KERNEL_END : 0x%lx\n",
	   (unsigned long)val);
  revm_output(buff);

  data = (char *)config_get_data(LIBKERNSH_CONFIG_KERNEL);
  snprintf(buff, sizeof(buff), 
	   "KERNEL : %s\n",
	   data);
  revm_output(buff);

  val = (int)config_get_data(LIBKERNSH_CONFIG_USE_KERNEL);
  switch (val)
    {
    case 0 :
      snprintf(buff, sizeof(buff), 
	       "USE_KERNEL : OFF\n");
      break;
    case 1 :
      snprintf(buff, sizeof(buff), 
	       "USE_KERNEL : ON\n");
      break;
    }
  revm_output(buff);

  
  val = (int)config_get_data(LIBKERNSH_CONFIG_ALLOC);
  switch (val)
    {
    case 0 :
      snprintf(buff, sizeof(buff), 
	       "ALLOC : CONTIGUOUS\n");
      break;
    case 1 :
      snprintf(buff, sizeof(buff), 
	       "ALLOC : NO CONTIGUOUS\n");
      break;
    }
  revm_output(buff);
  
  val = (int)config_get_data(LIBKERNSH_CONFIG_NB_SYSCALLS);
  snprintf(buff, sizeof(buff), 
	   "NB_SYSCALLS : %d\n",
	   val);
  revm_output(buff);
  
  val = (int)config_get_data(LIBKERNSH_CONFIG_NIL_SYSCALL);
  snprintf(buff, sizeof(buff), 
	   "NIL_SYSCALL : %d\n",
	   val);
  revm_output(buff);

  val = (int)config_get_data(LIBKERNSH_CONFIG_USEVM);
  switch (val)
    {
    case 0 :
      snprintf(buff, sizeof(buff), 
	       "USE_VM : OFF\n");
      break;
    case 1 :
      snprintf(buff, sizeof(buff), 
	       "USE_VM : ON\n");
      break;
    }
  revm_output(buff);

  val = (int)config_get_data(LIBKERNSH_CONFIG_VIRTM);
  switch (val)
    {
    case LIBKERNSH_KERNEL_MODE:
      snprintf(buff, sizeof(buff), 
	       "VIRTM : PROC\n");
      break;
    case LIBKERNSH_USER_MODE :
      snprintf(buff, sizeof(buff), 
	       "VIRTM : SYSCALL\n");
      break;
    }
  revm_output(buff);

  val = (int)config_get_data(LIBKERNSH_CONFIG_VIRTM_NIL_SYSCALL);
  snprintf(buff, sizeof(buff), 
	   "VIRTM_NIL_SYSCALL : %d\n",
	   val);
  revm_output(buff);

  val = (int)config_get_data(LIBKERNSH_CONFIG_HASH);
  switch (val)
    {
    case LIBKERNSH_HASH_MD5:
      snprintf(buff, sizeof(buff), 
	       "HASH : MD5\n");
      break;
    case LIBKERNSH_HASH_SHA1 :
      snprintf(buff, sizeof(buff), 
	       "HASH : SHA1\n");
      break;
    }
  revm_output(buff);

  val = (int)config_get_data(LIBKERNSH_CONFIG_VMA);
  switch (val)
    {
    case LIBKERNSH_VMA_USERLAND:
      snprintf(buff, sizeof(buff), 
	       "VMA : USERLAND\n");
      break;
    case LIBKERNSH_VMA_KERNELLAND :
      snprintf(buff, sizeof(buff), 
	       "VMA : KERNELLAND\n");
      break;
    }
  revm_output(buff);

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}

int		cmd_kmem_read()
{
  int		ret, len;
  char		*new_buff;
  eresi_Addr	addr;
  revmlist_t	*actual;
  char		buff[BUFSIZ];

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  ret = len = 0;
  addr = 0;

  actual = world.curjob->curcmd->disasm + 0;

  if (actual)
    {
      kernsh_addrlen(actual, &addr, &len);

      XALLOC(__FILE__, __FUNCTION__, __LINE__,
	     new_buff,
	     len,
	     -1);

      memset(new_buff, '\0', len);
      
      memset(buff, '\0', sizeof(buff));
      snprintf(buff, sizeof(buff),
	       "Reading kernel memory %s %s strlen(%s)\n\n", 
	       revm_colorstr("@"),
	       revm_coloraddress(XFMT, (eresi_Addr) addr),
	       revm_colornumber("%u", len));
      revm_output(buff);

      ret = elfsh_readmema(libkernshworld.root, addr, new_buff, len);
      
      kernsh_hexdump((unsigned char *)new_buff, len, addr);

      XFREE(__FILE__, __FUNCTION__, __LINE__, new_buff);
    }

  if (ret != len)
    {
      PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
		   "Impossible to read mem",
		   -1);
    }

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}

int		cmd_kmem_write()
{
  revmexpr_t	*e1;
  revmexpr_t    *e2;
  revmobj_t     *o1;
  revmobj_t     *o2;
  void          *dat;
  int           ret, size;
  eresi_Addr	addr;
  char		buff[BUFSIZ];

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  /* Sanity checks */
  if (world.curjob->curcmd->param[0] == NULL ||
      world.curjob->curcmd->param[1] == NULL)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
                      "Needs 2 parameters", -1);

  if (IS_VADDR(world.curjob->curcmd->param[0]))
    {
      if (sscanf(world.curjob->curcmd->param[0] + 2, AFMT, &addr) != 1)
	PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
		     "Invalid virtual address requested",
		     -1);
    }
  else
    {
      e1 = revm_lookup_param(world.curjob->curcmd->param[0], 1);
      o1 = e1->value;
      switch (o1->otype->type)
	{
	case ASPECT_TYPE_LONG:
	case ASPECT_TYPE_CADDR:
	case ASPECT_TYPE_DADDR:
	  addr = (o1->immed ? o1->immed_val.ent : o1->get_obj(o1->parent));
	      break;
	      
	case ASPECT_TYPE_INT:
	  addr = (o1->immed ? o1->immed_val.word : o1->get_obj(o1->parent));
	  break;
	}
    }

  e2 = revm_lookup_param(world.curjob->curcmd->param[1], 1); 

  o2 = e2->value;

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
  size = o2->size;

  if (size <= 0)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
                      "Source offset too big", -1);

  printf("WRITE @ 0x%lx %d %d %s\n", 
	 (unsigned long)addr,
	 size,
	 strlen(world.curjob->curcmd->param[1]),
	 world.curjob->curcmd->param[1]);

  memset(buff, '\0', sizeof(buff));
  snprintf(buff, sizeof(buff),
	   "Writing into kernel memory %s %s strlen(%s)\n\n", 
	   revm_colorstr("@"),
	   revm_coloraddress(XFMT, (eresi_Addr) addr),
	   revm_colornumber("%u", size));
  revm_output(buff);

  kernsh_hexdump((unsigned char *)dat, size, (unsigned int)dat);
  ret = elfsh_writemem(libkernshworld.root, addr, dat, size);

  if (ret != size)
    {
      PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
		   "Impossible to write mem",
		   -1);
    }

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}

int		cmd_kmem_disasm()
{
  int		ret, len;
  eresi_Addr	addr;
  char		*new_buff;
  revmlist_t	*actual;
  char		buff[BUFSIZ];

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  ret = len = 0;
  addr = 0;

  actual = world.curjob->curcmd->disasm + 0;

  if (actual)
    {
      kernsh_addrlen(actual, &addr, &len);

      XALLOC(__FILE__, __FUNCTION__, __LINE__,
	     new_buff,
	     len,
	     -1);

      memset(new_buff, '\0', len);

      ret = elfsh_readmema(libkernshworld.root, addr, new_buff, len);

      memset(buff, '\0', sizeof(buff));
      snprintf(buff, sizeof(buff),
	       "Disassembling kernel memory %s %s strlen(%s)\n\n", 
	       revm_colorstr("@"),
	       revm_coloraddress(XFMT, (eresi_Addr) addr),
	       revm_colornumber("%u", len));
      revm_output(buff);

      kernsh_disasm(new_buff, len, addr);

      XFREE(__FILE__, __FUNCTION__, __LINE__, new_buff);
    }

  if (ret != len)
    {
      PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
		   "Impossible to read mem",
		   -1);
    }

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}
