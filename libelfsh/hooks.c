/*
** hooks.c for libelfsh (The ELF shell library)
**
** All the function pointers for all provided
** techniques.
**
** Started Jan 11 2004 02:57:03 mayhem
**
*/
#include "libelfsh.h"



hash_t	interp_hash;
hash_t	vector_hash;

u_char	elfsh_ostype[5] = {
  ELFOSABI_LINUX,		
  ELFOSABI_FREEBSD,	
  ELFOSABI_NETBSD,		
  ELFOSABI_OPENBSD,	
  ELFOSABI_SOLARIS,	
};


/* To be moved in libaspect.h */
typedef struct	s_elfshvector
{
  void		*hook;
  void		*register_func;
  void		*default_func;
  u_int		*arraydims;
  u_int		arraysz;
}		elfshvector_t;



/* Project each dimension and write the desired function pointer */
static void	 elfsh_project_vectdim(elfshvector_t *vect, u_int *dim, u_int dimsz, elfsh_Addr fct)
{
  elfsh_Addr	*tmp;
  u_int		idx;

  tmp = vect->hook;
  for (idx = 0; idx < dimsz; idx++)
    {
      //printf("+ Projecting dimension %u (dimsz = %u) \n", idx, dimsz);
      //fflush(stdout);
      tmp += dim[idx];
      //printf("= Dereferencing ptr %08X \n", (elfsh_Addr) tmp);
      if (idx + 1 < dimsz)
	tmp  = (elfsh_Addr *) *tmp;
      //printf("- Projected dimension %u (curtmp = %08X) \n", idx, (elfsh_Addr) tmp);
    }

  *tmp = (elfsh_Addr) fct;
}


/* Project each dimension and get the requested function pointer */
void*		elfsh_project_coords(elfshvector_t *vect, u_int *dim, u_int dimsz)
{
  elfsh_Addr	*tmp;
  u_int		idx;

  tmp = vect->hook;
  for (idx = 0; idx < dimsz; idx++)
    {
      //printf("+ Projecting dimension %u (dimsz = %u) \n", idx, dimsz);
      //fflush(stdout);
      tmp += dim[idx];
      //printf("= Dereferencing ptr %08X \n", (elfsh_Addr) tmp);
      tmp  = (elfsh_Addr *) *tmp;
      //printf("- Projected dimension %u (curtmp = %08X) \n", idx, (elfsh_Addr) tmp);
    }
  return (tmp);
}


/* Allocate recursively the hook array */
int		elfsh_recursive_vectalloc(elfsh_Addr *tab, u_int *dims, u_int depth, u_int dimsz)
{
  u_int		idx;

  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);

  //  printf("Recursing vectalloc depth %u \n", depth);
  //  fflush(stdout);

  if (depth == dimsz)
    ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
  for (idx = 0; idx < dims[depth - 1]; idx++)
    {
      XALLOC(tab[idx], dims[depth] * sizeof(elfsh_Addr), -1);
      elfsh_recursive_vectalloc((elfsh_Addr *) tab[idx], dims, depth + 1, dimsz);
    }
  ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}


/* Register a new vector. A vector is an multidimentional array of hooks */
int		elfsh_register_vector(char	*name, 
				      void	*registerfunc, 
				      void	*defaultfunc,
				      u_int	*dimensions, 
				      u_int	dimsz)
{
  elfshvector_t	*vector;
  elfsh_Addr	*ptr;

  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);
  if (!registerfunc || !defaultfunc || !dimsz || !dimensions)
    ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Invalid NULL parameters", -1);

  XALLOC(vector, sizeof(elfshvector_t), -1);  
  XALLOC(ptr, dimensions[0] * sizeof(elfsh_Addr), -1);
  vector->hook = ptr;
  if (dimsz > 1)
    elfsh_recursive_vectalloc((elfsh_Addr *) vector->hook, dimensions, 1, dimsz);

  vector->arraysz       = dimsz;
  vector->arraydims     = dimensions;
  vector->register_func = registerfunc;
  vector->default_func  = defaultfunc;
  hash_add(&vector_hash, name, vector);

  //  printf("Added vector with name %s \n", name);

  ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}


								
/* Default hooks handlers */ 
int	elfsh_default_plthandler(elfshobj_t *null, 
				 elfsh_Sym  *null2, 
				 elfsh_Addr null3)
{
  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);
  ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, 
		    "Unsupported Arch, ELF type, or OS", -1);
}

/* Used for encoding a random PLT entry */
/* Do nothing by default, this is not fatal */
int	elfsh_default_encodeplthandler(elfshobj_t *file, 
				       elfshsect_t *sect, 
				       elfsh_Addr diff)
{
  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);
  ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, 
		    "Unsupported Arch, ELF type, or OS", -1);
}

/* Used for encoding of the first PLT entry */
int	elfsh_default_encodeplt1handler(elfshobj_t *file, 
					elfshsect_t *sect,
					elfshsect_t *sect2,
					elfsh_Addr diff)
{
  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);
  ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, 
		    "Unsupported Arch, ELF type, or OS", -1);
}

int	elfsh_default_relhandler(elfshsect_t *null, 
				 elfsh_Rel  * null2, 
				 elfsh_Addr * null3, 
				 elfsh_Addr   null4,
				 elfshsect_t *null5)
{
  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);
  ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, 
		    "Unsupported Arch, ELF type, or OS", -1);
}

int	elfsh_default_cflowhandler(elfshobj_t   *null,
				   char		*nulls,
				   elfsh_Sym    *null2,
				   elfsh_Addr	null3)
{
  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);
  ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, 
		    "Unsupported Arch, ELF type, or OS", -1);
}


int     elfsh_default_breakhandler(elfshobj_t   *null,
				   elfshbp_t	*null3)
{
  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);
  ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, 
		    "Unsupported Arch, ELF type, or OS", -1);
}


int	elfsh_default_extplthandler(elfshsect_t *o, elfshsect_t *d, 
				    elfshsect_t *t, elfshsect_t *f)
{
  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);
  ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, 
		    "Unsupported Arch, ELF type, or OS", -1);
}

/* Used on architectures where altplt hijack is not required, thus induce no fatal error */
/* DO NOT use this as a default handler, unless you know exactly what you are doing */
int	elfsh_void_altplthandler(elfshobj_t *null, 
				 elfsh_Sym  *null2, 
				 elfsh_Addr null3)
{
  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);
  ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}





/* Registration handlers */
int	elfsh_register_altplthook(u_char archtype, 
				  u_char objtype, 
				  u_char ostype, 
				  void	 *fct)
{
  elfshvector_t	*altplt;
  u_int		*dim;

  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);
  
  altplt = hash_get(&vector_hash, ELFSH_HOOK_ALTPLT);
  if (archtype >= ELFSH_ARCHNUM)
    ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Invalid Architecture type", -1);
  if (objtype >= ELFSH_TYPENUM)
    ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Invalid Object type", -1);
  if (ostype >= ELFSH_OSNUM)
    ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Invalid Operating System type", -1);

  dim = alloca(sizeof(u_int) * 4);
  dim[0] = archtype;
  dim[1] = objtype;
  dim[2] = ostype;
  elfsh_project_vectdim(altplt, dim, 3, (elfsh_Addr) fct);
  //  puts("----------------- ALTPLT ----------------------");
  ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}


/* Register an EXTPLT handler */
int		elfsh_register_extplthook(u_char archtype, 
					  u_char objtype, 
					  u_char ostype, 
					  void *fct)
{
  elfshvector_t	*extplt;
  u_int		*dim;

  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);
  
  extplt = hash_get(&vector_hash, ELFSH_HOOK_EXTPLT);
  if (archtype >= ELFSH_ARCHNUM)
    ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Invalid Architecture type", -1);
  if (objtype >= ELFSH_TYPENUM)
    ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Invalid Object type", -1);
  if (ostype >= ELFSH_OSNUM)
    ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Invalid Operating System type", -1);

  dim = alloca(sizeof(u_int) * 4);
  dim[0] = archtype;
  dim[1] = objtype;
  dim[2] = ostype;
  elfsh_project_vectdim(extplt, dim, 3, (elfsh_Addr) fct);

  //puts("----------------- EXTPLT ----------------------");
  //hook = extplt->hook;
  //hook[archtype][objtype][ostype] = (elfsh_Addr) fct;

  ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}


/* Register an PLT handler */
int		elfsh_register_plthook(u_char archtype, u_char objtype, u_char ostype, 
				       void *fct)
{
  elfshvector_t	*plt;
  u_int		*dim;

  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);

  plt = hash_get(&vector_hash, ELFSH_HOOK_PLT);
  if (archtype >= ELFSH_ARCHNUM)
    ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Invalid Architecture type", -1);
  if (objtype >= ELFSH_TYPENUM)
    ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__,
		      "Invalid Object type", -1);
  if (ostype >= ELFSH_OSNUM)
    ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Invalid Operating System type", -1);

  dim    = alloca(sizeof(u_int) * 4);
  dim[0] = archtype;
  dim[1] = objtype;
  dim[2] = ostype;
  elfsh_project_vectdim(plt, dim, 3, (elfsh_Addr) fct);

  //puts("----------------- PLT ----------------------");
  //hook = plt->hook;
  //hook[archtype][objtype][ostype] = (elfsh_Addr) fct;

  ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}

/* Register an ENCODEPLT handler */
int	elfsh_register_encodeplthook(u_char archtype, u_char objtype, 
				     u_char ostype, void *fct)
{
  elfshvector_t	*encodeplt;
  u_int		*dim;

  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);

  encodeplt = hash_get(&vector_hash, ELFSH_HOOK_ENCODEPLT);

  if (archtype >= ELFSH_ARCHNUM)
    ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Invalid Architecture type", -1);
  if (objtype >= ELFSH_TYPENUM)
    ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__,
		      "Invalid Object type", -1);
  if (ostype >= ELFSH_OSNUM)
    ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Invalid Operating System type", -1);

  dim = alloca(sizeof(u_int) * 4);
  dim[0] = archtype;
  dim[1] = objtype;
  dim[2] = ostype;
  elfsh_project_vectdim(encodeplt, dim, 3, (elfsh_Addr) fct);

  //puts("----------------- ENCODEPLT ----------------------");
  //hook = encodeplt->hook;
  //hook[archtype][objtype][ostype] = (elfsh_Addr) fct;

  ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}

/* Register an ENCODEPLT1 handler */
int	elfsh_register_encodeplt1hook(u_char archtype, u_char objtype, 
				      u_char ostype, void *fct)
{
  elfshvector_t	*encodeplt1;
  u_int		*dim;

  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);

  encodeplt1 = hash_get(&vector_hash, ELFSH_HOOK_ENCODEPLT1);

  if (archtype >= ELFSH_ARCHNUM)
    ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Invalid Architecture type", -1);
  if (objtype >= ELFSH_TYPENUM)
    ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__,
		      "Invalid Object type", -1);
  if (ostype >= ELFSH_OSNUM)
    ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Invalid Operating System type", -1);

  //hook = encodeplt1->hook;
  //hook[archtype][objtype][ostype] = (elfsh_Addr) fct;
  //puts("----------------- ENCODEPLT1 ----------------------");

  dim = alloca(sizeof(u_int) * 4);
  dim[0] = archtype;
  dim[1] = objtype;
  dim[2] = ostype;
  elfsh_project_vectdim(encodeplt1, dim, 3, (elfsh_Addr) fct);

  ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}



/* Register an ET_REL injection handler */
int	elfsh_register_relhook(u_char archtype, u_char objtype, u_char ostype,
			       void *fct)
{
  elfshvector_t	*rel;
  u_int		*dim;

  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);
  rel = hash_get(&vector_hash, ELFSH_HOOK_REL);
  
  if (archtype >= ELFSH_ARCHNUM)
    ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Invalid Architecture type", -1);
  if (objtype >= ELFSH_TYPENUM)
    ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Invalid Object type", -1);
  if (ostype >= ELFSH_OSNUM)
    ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Invalid Operating System type", -1);

  dim = alloca(sizeof(u_int) * 4);
  dim[0] = archtype;
  dim[1] = objtype;
  dim[2] = ostype;
  elfsh_project_vectdim(rel, dim, 3, (elfsh_Addr) fct);
  //puts("----------------- REL ----------------------");

  //hook = rel->hook;
  //hook[archtype][objtype][ostype] = (elfsh_Addr) fct;

  ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}


/* Register a control flow redirection handler */
int	elfsh_register_cflowhook(u_char archtype, u_char objtype, 
				 u_char ostype, void *fct)
{
  elfshvector_t	*cflow;
  u_int		*dim;

  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);
  cflow = hash_get(&vector_hash, ELFSH_HOOK_CFLOW);

  if (archtype >= ELFSH_ARCHNUM)
    ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Invalid Architecture type", -1);
  if (objtype >= ELFSH_TYPENUM)
    ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Invalid Object type", -1);
  if (ostype >= ELFSH_OSNUM)
    ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Invalid Operating System type", -1);

  dim = alloca(sizeof(u_int) * 4);
  dim[0] = archtype;
  dim[1] = objtype;
  dim[2] = ostype;
  elfsh_project_vectdim(cflow, dim, 3, (elfsh_Addr) fct);
  //puts("----------------- CFLOW ----------------------");

  //hook = cflow->hook;
  //hook[archtype][objtype][ostype] = (elfsh_Addr) fct;

  ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}



/* Register a breakpoint redirection handler */
int	elfsh_register_breakhook(u_char archtype, u_char objtype, 
				 u_char ostype, void *fct)
{
  elfshvector_t	*breakp;
  u_int		*dim;

  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);  
  breakp = hash_get(&vector_hash, ELFSH_HOOK_BREAK);

  if (archtype >= ELFSH_ARCHNUM)
    ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Invalid Architecture type", -1);
  if (objtype >= ELFSH_TYPENUM)
    ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Invalid Object type", -1);
  if (ostype >= ELFSH_OSNUM)
    ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "Invalid Operating System type", -1);

  dim = alloca(sizeof(u_int) * 4);
  dim[0] = archtype;
  dim[1] = objtype;
  dim[2] = ostype;
  elfsh_project_vectdim(breakp, dim, 3, (elfsh_Addr) fct);
  //puts("----------------- BREAK ----------------------");

  //hook = breakp->hook;
  //hook[archtype][objtype][ostype] = (elfsh_Addr) fct;

  ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}




/* Initialize hook hash table */
int		elfsh_init_vectors()
{
  u_int		*dims;

  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__); 
  hash_init(&vector_hash, 11);

  /* All hooks have the same dimensions here */
  XALLOC(dims, 4 * sizeof(u_int), -1);
  dims[0] = ELFSH_ARCHNUM;
  dims[1] = ELFSH_TYPENUM;
  dims[2] = ELFSH_OSNUM;
  dims[3] = 0;

  elfsh_register_vector(ELFSH_HOOK_ALTPLT, 
			elfsh_register_altplthook, 
			elfsh_default_plthandler,
			dims, 3);
  elfsh_register_vector(ELFSH_HOOK_PLT, 
			elfsh_register_plthook, 
			elfsh_default_plthandler,
			dims, 3);
  elfsh_register_vector(ELFSH_HOOK_REL, 
			elfsh_register_relhook, 
			elfsh_default_relhandler,
			dims, 3);
  elfsh_register_vector(ELFSH_HOOK_CFLOW, 
			elfsh_register_cflowhook, 
			elfsh_default_cflowhandler, 
			dims, 3);
  elfsh_register_vector(ELFSH_HOOK_BREAK, 
			elfsh_register_breakhook, 
			elfsh_default_breakhandler, 
			dims, 3);
  elfsh_register_vector(ELFSH_HOOK_EXTPLT, 
			elfsh_register_extplthook, 
			elfsh_default_extplthandler, 
			dims, 3);  
  elfsh_register_vector(ELFSH_HOOK_ENCODEPLT, 
			elfsh_register_encodeplthook, 
			elfsh_default_encodeplthandler, 
			dims, 3);  
  elfsh_register_vector(ELFSH_HOOK_ENCODEPLT1, 
			elfsh_register_encodeplt1hook, 
			elfsh_default_encodeplt1handler, 
			dims, 3);  

  ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}






/* Initialize ALTPLT hijack and ET_REL injection handlers in their hook */
void	elfsh_setup_hooks()
{
  u_int	i, j, k;
  static int done = 0;

  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);
  
  /* Check if already initialized */
  if (done)
    ELFSH_PROFILE_OUT(__FILE__, __FUNCTION__, __LINE__);

  elfsh_init_vectors();

  /* Initialize hooks to void */
  /* Those hooks are all of the same dimensions */
  for (i = 0; i < ELFSH_ARCHNUM; i++)
    for (j = 0; j < ELFSH_TYPENUM; j++)
      for (k = 0; k < ELFSH_OSNUM; k++)
	{
	  elfsh_register_altplthook(i, j, k, elfsh_default_plthandler);
	  elfsh_register_plthook(i, j, k, elfsh_default_plthandler); 
	  elfsh_register_relhook(i, j, k, elfsh_default_relhandler);
	  elfsh_register_cflowhook(i, j, k, elfsh_default_cflowhandler);
	  elfsh_register_breakhook(i, j, k, elfsh_default_breakhandler);
	  elfsh_register_extplthook(i, j, k, elfsh_default_extplthandler);
	  elfsh_register_encodeplthook(i, j, k, elfsh_default_encodeplthandler);
	  elfsh_register_encodeplt1hook(i, j, k, elfsh_default_encodeplt1handler);
	}

  /***************************************/
  /****** PLT hijacking handlers *********/
  /**************************************/

  /* Usual PLT targets for ET_EXEC/i386 */
  elfsh_register_plthook(ELFSH_ARCH_IA32, ELFSH_TYPE_EXEC, 
			 ELFSH_OS_LINUX, elfsh_hijack_plt_ia32);
  elfsh_register_plthook(ELFSH_ARCH_IA32, ELFSH_TYPE_EXEC, 
			 ELFSH_OS_FREEBSD, elfsh_hijack_plt_ia32);
  elfsh_register_plthook(ELFSH_ARCH_IA32, ELFSH_TYPE_EXEC, 
			 ELFSH_OS_NETBSD, elfsh_hijack_plt_ia32);
  elfsh_register_plthook(ELFSH_ARCH_IA32, ELFSH_TYPE_EXEC, 
			 ELFSH_OS_OPENBSD, elfsh_hijack_plt_ia32);
  elfsh_register_plthook(ELFSH_ARCH_IA32, ELFSH_TYPE_EXEC, 
			 ELFSH_OS_SOLARIS, elfsh_hijack_plt_ia32);
  
  /* Usual PLT targets for ET_EXEC/Sparc32 */
  elfsh_register_plthook(ELFSH_ARCH_SPARC32, ELFSH_TYPE_EXEC, 
			 ELFSH_OS_LINUX, elfsh_hijack_plt_sparc32);
  elfsh_register_plthook(ELFSH_ARCH_SPARC32, ELFSH_TYPE_EXEC, 
			 ELFSH_OS_FREEBSD, elfsh_hijack_plt_sparc32);
  elfsh_register_plthook(ELFSH_ARCH_SPARC32, ELFSH_TYPE_EXEC, 
			 ELFSH_OS_NETBSD, elfsh_hijack_plt_sparc32);
  elfsh_register_plthook(ELFSH_ARCH_SPARC32, ELFSH_TYPE_EXEC, 
			 ELFSH_OS_OPENBSD, elfsh_hijack_plt_sparc32);
  elfsh_register_plthook(ELFSH_ARCH_SPARC32, ELFSH_TYPE_EXEC, 
			 ELFSH_OS_SOLARIS, elfsh_hijack_plt_sparc32);

  /* Usual PLT targets for ET_EXEC/Sparc64 */
  elfsh_register_plthook(ELFSH_ARCH_SPARC64, ELFSH_TYPE_EXEC, 
			 ELFSH_OS_LINUX, elfsh_hijack_plt_sparc64);
  elfsh_register_plthook(ELFSH_ARCH_SPARC64, ELFSH_TYPE_EXEC, 
			 ELFSH_OS_FREEBSD, elfsh_hijack_plt_sparc64);
  elfsh_register_plthook(ELFSH_ARCH_SPARC64, ELFSH_TYPE_EXEC, 
			 ELFSH_OS_NETBSD, elfsh_hijack_plt_sparc64);
  elfsh_register_plthook(ELFSH_ARCH_SPARC64, ELFSH_TYPE_EXEC, 
			 ELFSH_OS_OPENBSD, elfsh_hijack_plt_sparc64);
  elfsh_register_plthook(ELFSH_ARCH_SPARC64, ELFSH_TYPE_EXEC, 
			 ELFSH_OS_SOLARIS, elfsh_hijack_plt_sparc64);

  /* Usual PLT targets for ET_EXEC/IA64 */
  elfsh_register_plthook(ELFSH_ARCH_IA64, ELFSH_TYPE_EXEC, 
			 ELFSH_OS_LINUX, elfsh_hijack_plt_ia64);
  elfsh_register_plthook(ELFSH_ARCH_IA64, ELFSH_TYPE_EXEC, 
			 ELFSH_OS_FREEBSD, elfsh_hijack_plt_ia64);
  elfsh_register_plthook(ELFSH_ARCH_IA64, ELFSH_TYPE_EXEC, 
			 ELFSH_OS_NETBSD, elfsh_hijack_plt_ia64);
  elfsh_register_plthook(ELFSH_ARCH_IA64, ELFSH_TYPE_EXEC, 
			 ELFSH_OS_OPENBSD, elfsh_hijack_plt_ia64);
  elfsh_register_plthook(ELFSH_ARCH_IA64, ELFSH_TYPE_EXEC, 
			 ELFSH_OS_SOLARIS, elfsh_hijack_plt_ia64);

  /* Usual PLT targets for ET_EXEC/Alpha64 */
  elfsh_register_plthook(ELFSH_ARCH_ALPHA64, ELFSH_TYPE_EXEC, 
			 ELFSH_OS_LINUX, elfsh_hijack_plt_alpha64);
  elfsh_register_plthook(ELFSH_ARCH_ALPHA64, ELFSH_TYPE_EXEC, 
			 ELFSH_OS_FREEBSD, elfsh_hijack_plt_alpha64);
  elfsh_register_plthook(ELFSH_ARCH_ALPHA64, ELFSH_TYPE_EXEC, 
			 ELFSH_OS_NETBSD, elfsh_hijack_plt_alpha64);
  elfsh_register_plthook(ELFSH_ARCH_ALPHA64, ELFSH_TYPE_EXEC, 
			 ELFSH_OS_OPENBSD, elfsh_hijack_plt_alpha64);
  elfsh_register_plthook(ELFSH_ARCH_ALPHA64, ELFSH_TYPE_EXEC, 
			 ELFSH_OS_SOLARIS, elfsh_hijack_plt_alpha64);

  /* Usual PLT targets for ET_EXEC/MIPS32 */
  elfsh_register_plthook(ELFSH_ARCH_MIPS32, ELFSH_TYPE_EXEC, 
			 ELFSH_OS_LINUX, elfsh_hijack_plt_mips32);
  elfsh_register_plthook(ELFSH_ARCH_MIPS32, ELFSH_TYPE_EXEC, 
			 ELFSH_OS_FREEBSD, elfsh_hijack_plt_mips32);
  elfsh_register_plthook(ELFSH_ARCH_MIPS32, ELFSH_TYPE_EXEC, 
			 ELFSH_OS_NETBSD, elfsh_hijack_plt_mips32);
  elfsh_register_plthook(ELFSH_ARCH_MIPS32, ELFSH_TYPE_EXEC, 
			 ELFSH_OS_OPENBSD, elfsh_hijack_plt_mips32);
  elfsh_register_plthook(ELFSH_ARCH_MIPS32, ELFSH_TYPE_EXEC, 
			 ELFSH_OS_SOLARIS, elfsh_hijack_plt_mips32);

  /* Usual PLT targets for ET_EXEC/MIPS64 */
  elfsh_register_plthook(ELFSH_ARCH_MIPS64, ELFSH_TYPE_EXEC, 
			 ELFSH_OS_LINUX, elfsh_hijack_plt_mips64);
  elfsh_register_plthook(ELFSH_ARCH_MIPS64, ELFSH_TYPE_EXEC, 
			 ELFSH_OS_FREEBSD, elfsh_hijack_plt_mips64);
  elfsh_register_plthook(ELFSH_ARCH_MIPS64, ELFSH_TYPE_EXEC, 
			 ELFSH_OS_NETBSD, elfsh_hijack_plt_mips64);
  elfsh_register_plthook(ELFSH_ARCH_MIPS64, ELFSH_TYPE_EXEC, 
			 ELFSH_OS_OPENBSD, elfsh_hijack_plt_mips64);
  elfsh_register_plthook(ELFSH_ARCH_MIPS64, ELFSH_TYPE_EXEC, 
			 ELFSH_OS_SOLARIS, elfsh_hijack_plt_mips64);

  /***************************************/
  /****** ALTPLT hijacking handlers ******/
  /**************************************/

  /* Usual ALTPLT targets for ET_EXEC/i386 */
  elfsh_register_altplthook(ELFSH_ARCH_IA32, ELFSH_TYPE_EXEC, 
			    ELFSH_OS_LINUX, elfsh_void_altplthandler);
  elfsh_register_altplthook(ELFSH_ARCH_IA32, ELFSH_TYPE_EXEC, 
			    ELFSH_OS_FREEBSD, elfsh_void_altplthandler);
  elfsh_register_altplthook(ELFSH_ARCH_IA32, ELFSH_TYPE_EXEC, 
			    ELFSH_OS_NETBSD, elfsh_void_altplthandler);
  elfsh_register_altplthook(ELFSH_ARCH_IA32, ELFSH_TYPE_EXEC, 
			    ELFSH_OS_OPENBSD, elfsh_void_altplthandler);
  elfsh_register_altplthook(ELFSH_ARCH_IA32, ELFSH_TYPE_EXEC, 
			    ELFSH_OS_SOLARIS, elfsh_void_altplthandler);
  
  /* Usual ALTPLT targets for ET_EXEC/Sparc32 */
  elfsh_register_altplthook(ELFSH_ARCH_SPARC32, ELFSH_TYPE_EXEC, 
			    ELFSH_OS_LINUX, elfsh_hijack_altplt_sparc32);
  elfsh_register_altplthook(ELFSH_ARCH_SPARC32, ELFSH_TYPE_EXEC, 
			    ELFSH_OS_FREEBSD, elfsh_hijack_altplt_sparc32);
  elfsh_register_altplthook(ELFSH_ARCH_SPARC32, ELFSH_TYPE_EXEC, 
			    ELFSH_OS_NETBSD, elfsh_hijack_altplt_sparc32);
  elfsh_register_altplthook(ELFSH_ARCH_SPARC32, ELFSH_TYPE_EXEC, 
			    ELFSH_OS_OPENBSD, elfsh_hijack_altplt_sparc32);
  elfsh_register_altplthook(ELFSH_ARCH_SPARC32, ELFSH_TYPE_EXEC, 
			    ELFSH_OS_SOLARIS, elfsh_hijack_altplt_sparc32);
  
  /* Usual ALTPLT targets for ET_EXEC/Sparc64 */
  elfsh_register_altplthook(ELFSH_ARCH_SPARC64, ELFSH_TYPE_EXEC, 
			    ELFSH_OS_LINUX, elfsh_hijack_altplt_sparc64);
  elfsh_register_altplthook(ELFSH_ARCH_SPARC64, ELFSH_TYPE_EXEC, 
			    ELFSH_OS_FREEBSD, elfsh_hijack_altplt_sparc64);
  elfsh_register_altplthook(ELFSH_ARCH_SPARC64, ELFSH_TYPE_EXEC, 
			    ELFSH_OS_NETBSD, elfsh_hijack_altplt_sparc64);
  elfsh_register_altplthook(ELFSH_ARCH_SPARC64, ELFSH_TYPE_EXEC, 
			    ELFSH_OS_OPENBSD, elfsh_hijack_altplt_sparc64);
  elfsh_register_altplthook(ELFSH_ARCH_SPARC64, ELFSH_TYPE_EXEC, 
			    ELFSH_OS_SOLARIS, elfsh_hijack_altplt_sparc64);
  
  /* Usual ALTPLT targets for ET_EXEC/IA64 */
  elfsh_register_altplthook(ELFSH_ARCH_IA64, ELFSH_TYPE_EXEC, 
			    ELFSH_OS_LINUX, elfsh_hijack_altplt_ia64);
  elfsh_register_altplthook(ELFSH_ARCH_IA64, ELFSH_TYPE_EXEC, 
			    ELFSH_OS_FREEBSD, elfsh_hijack_altplt_ia64);
  elfsh_register_altplthook(ELFSH_ARCH_IA64, ELFSH_TYPE_EXEC, 
			    ELFSH_OS_NETBSD, elfsh_hijack_altplt_ia64);
  elfsh_register_altplthook(ELFSH_ARCH_IA64, ELFSH_TYPE_EXEC, 
			    ELFSH_OS_OPENBSD, elfsh_hijack_altplt_ia64);
  elfsh_register_altplthook(ELFSH_ARCH_IA64, ELFSH_TYPE_EXEC, 
			    ELFSH_OS_SOLARIS, elfsh_hijack_altplt_ia64);
  
  /* Usual ALTPLT targets for ET_EXEC/Alpha64 */
  elfsh_register_altplthook(ELFSH_ARCH_ALPHA64, ELFSH_TYPE_EXEC, 
			    ELFSH_OS_LINUX, elfsh_hijack_altplt_alpha64);
  elfsh_register_altplthook(ELFSH_ARCH_ALPHA64, ELFSH_TYPE_EXEC, 
			    ELFSH_OS_FREEBSD, elfsh_hijack_altplt_alpha64);
  elfsh_register_altplthook(ELFSH_ARCH_ALPHA64, ELFSH_TYPE_EXEC, 
			    ELFSH_OS_NETBSD, elfsh_hijack_altplt_alpha64);
  elfsh_register_altplthook(ELFSH_ARCH_ALPHA64, ELFSH_TYPE_EXEC, 
			    ELFSH_OS_OPENBSD, elfsh_hijack_altplt_alpha64);
  elfsh_register_altplthook(ELFSH_ARCH_ALPHA64, ELFSH_TYPE_EXEC, 
			    ELFSH_OS_SOLARIS, elfsh_hijack_altplt_alpha64);
  
  /* Usual ALTPLT targets for ET_EXEC/MIPS32 */
  elfsh_register_altplthook(ELFSH_ARCH_MIPS32, ELFSH_TYPE_EXEC, 
			    ELFSH_OS_LINUX, elfsh_hijack_altplt_mips32);
  elfsh_register_altplthook(ELFSH_ARCH_MIPS32, ELFSH_TYPE_EXEC, 
			    ELFSH_OS_FREEBSD, elfsh_hijack_altplt_mips32);
  elfsh_register_altplthook(ELFSH_ARCH_MIPS32, ELFSH_TYPE_EXEC, 
			    ELFSH_OS_NETBSD, elfsh_hijack_altplt_mips32);
  elfsh_register_altplthook(ELFSH_ARCH_MIPS32, ELFSH_TYPE_EXEC, 
			    ELFSH_OS_OPENBSD, elfsh_hijack_altplt_mips32);
  elfsh_register_altplthook(ELFSH_ARCH_MIPS32, ELFSH_TYPE_EXEC, 
			    ELFSH_OS_SOLARIS, elfsh_hijack_altplt_mips32);
  
  /* Usual ALTPLT targets for ET_EXEC/MIPS64 */
  elfsh_register_altplthook(ELFSH_ARCH_MIPS64, ELFSH_TYPE_EXEC, 
			    ELFSH_OS_LINUX, elfsh_hijack_altplt_mips64);
  elfsh_register_altplthook(ELFSH_ARCH_MIPS64, ELFSH_TYPE_EXEC, 
			    ELFSH_OS_FREEBSD, elfsh_hijack_altplt_mips64);
  elfsh_register_altplthook(ELFSH_ARCH_MIPS64, ELFSH_TYPE_EXEC, 
			    ELFSH_OS_NETBSD, elfsh_hijack_altplt_mips64);
  elfsh_register_altplthook(ELFSH_ARCH_MIPS64, ELFSH_TYPE_EXEC, 
			    ELFSH_OS_OPENBSD, elfsh_hijack_altplt_mips64);
  elfsh_register_altplthook(ELFSH_ARCH_MIPS64, ELFSH_TYPE_EXEC, 
			    ELFSH_OS_SOLARIS, elfsh_hijack_altplt_mips64);
  
  /***********************************/
  /* Now register Relocation hooks */
  /***********************************/

  
  /* Usual REL targets for ET_EXEC/i386 */
  elfsh_register_relhook(ELFSH_ARCH_IA32, ELFSH_TYPE_EXEC, 
			 ELFSH_OS_LINUX, elfsh_relocate_ia32);
  elfsh_register_relhook(ELFSH_ARCH_IA32, ELFSH_TYPE_EXEC, 
			 ELFSH_OS_FREEBSD, elfsh_relocate_ia32);
  elfsh_register_relhook(ELFSH_ARCH_IA32, ELFSH_TYPE_EXEC, 
			 ELFSH_OS_NETBSD, elfsh_relocate_ia32);
  elfsh_register_relhook(ELFSH_ARCH_IA32, ELFSH_TYPE_EXEC, 
			 ELFSH_OS_OPENBSD, elfsh_relocate_ia32);
  elfsh_register_relhook(ELFSH_ARCH_IA32, ELFSH_TYPE_EXEC, 
			 ELFSH_OS_SOLARIS, elfsh_relocate_ia32);
  
  /* Usual REL targets for ET_EXEC/Sparc32 */
  elfsh_register_relhook(ELFSH_ARCH_SPARC32, ELFSH_TYPE_EXEC, 
			 ELFSH_OS_LINUX, elfsh_relocate_sparc32);
  elfsh_register_relhook(ELFSH_ARCH_SPARC32, ELFSH_TYPE_EXEC, 
			 ELFSH_OS_FREEBSD, elfsh_relocate_sparc32);
  elfsh_register_relhook(ELFSH_ARCH_SPARC32, ELFSH_TYPE_EXEC, 
			 ELFSH_OS_NETBSD, elfsh_relocate_sparc32);
  elfsh_register_relhook(ELFSH_ARCH_SPARC32, ELFSH_TYPE_EXEC, 
			 ELFSH_OS_OPENBSD, elfsh_relocate_sparc32);
  elfsh_register_relhook(ELFSH_ARCH_SPARC32, ELFSH_TYPE_EXEC, 
			 ELFSH_OS_SOLARIS, elfsh_relocate_sparc32);

  /* Usual REL targets for ET_EXEC/Sparc64 */
  elfsh_register_relhook(ELFSH_ARCH_SPARC64, ELFSH_TYPE_EXEC, 
			 ELFSH_OS_LINUX, elfsh_relocate_sparc64);
  elfsh_register_relhook(ELFSH_ARCH_SPARC64, ELFSH_TYPE_EXEC, 
			 ELFSH_OS_FREEBSD, elfsh_relocate_sparc64);
  elfsh_register_relhook(ELFSH_ARCH_SPARC64, ELFSH_TYPE_EXEC, 
			 ELFSH_OS_NETBSD, elfsh_relocate_sparc64);
  elfsh_register_relhook(ELFSH_ARCH_SPARC64, ELFSH_TYPE_EXEC, 
			 ELFSH_OS_OPENBSD, elfsh_relocate_sparc64);
  elfsh_register_relhook(ELFSH_ARCH_SPARC64, ELFSH_TYPE_EXEC, 
			 ELFSH_OS_SOLARIS, elfsh_relocate_sparc64);

  /* Usual REL targets for ET_EXEC/IA64 */
  elfsh_register_relhook(ELFSH_ARCH_IA64, ELFSH_TYPE_EXEC, 
			 ELFSH_OS_LINUX, elfsh_relocate_ia64);
  elfsh_register_relhook(ELFSH_ARCH_IA64, ELFSH_TYPE_EXEC, 
			 ELFSH_OS_FREEBSD, elfsh_relocate_ia64);
  elfsh_register_relhook(ELFSH_ARCH_IA64, ELFSH_TYPE_EXEC, 
			 ELFSH_OS_NETBSD, elfsh_relocate_ia64);
  elfsh_register_relhook(ELFSH_ARCH_IA64, ELFSH_TYPE_EXEC, 
			 ELFSH_OS_OPENBSD, elfsh_relocate_ia64);
  elfsh_register_relhook(ELFSH_ARCH_IA64, ELFSH_TYPE_EXEC, 
			 ELFSH_OS_SOLARIS, elfsh_relocate_ia64);

  /* Usual REL targets for ET_EXEC/Alpha64 */
  elfsh_register_relhook(ELFSH_ARCH_ALPHA64, ELFSH_TYPE_EXEC, 
			 ELFSH_OS_LINUX, elfsh_relocate_alpha64);
  elfsh_register_relhook(ELFSH_ARCH_ALPHA64, ELFSH_TYPE_EXEC, 
			 ELFSH_OS_FREEBSD, elfsh_relocate_alpha64);
  elfsh_register_relhook(ELFSH_ARCH_ALPHA64, ELFSH_TYPE_EXEC, 
			 ELFSH_OS_NETBSD, elfsh_relocate_alpha64);
  elfsh_register_relhook(ELFSH_ARCH_ALPHA64, ELFSH_TYPE_EXEC, 
			 ELFSH_OS_OPENBSD, elfsh_relocate_alpha64);
  elfsh_register_relhook(ELFSH_ARCH_ALPHA64, ELFSH_TYPE_EXEC, 
			 ELFSH_OS_SOLARIS, elfsh_relocate_alpha64);

  /* Usual REL targets for ET_EXEC/MIPS32 */
  elfsh_register_relhook(ELFSH_ARCH_MIPS32, ELFSH_TYPE_EXEC, 
			 ELFSH_OS_LINUX, elfsh_relocate_mips32);
  elfsh_register_relhook(ELFSH_ARCH_MIPS32, ELFSH_TYPE_EXEC, 
			 ELFSH_OS_FREEBSD, elfsh_relocate_mips32);
  elfsh_register_relhook(ELFSH_ARCH_MIPS32, ELFSH_TYPE_EXEC, 
			 ELFSH_OS_NETBSD, elfsh_relocate_mips32);
  elfsh_register_relhook(ELFSH_ARCH_MIPS32, ELFSH_TYPE_EXEC, 
			 ELFSH_OS_OPENBSD, elfsh_relocate_mips32);
  elfsh_register_relhook(ELFSH_ARCH_MIPS32, ELFSH_TYPE_EXEC, 
			 ELFSH_OS_SOLARIS, elfsh_relocate_mips32);

  /* Usual REL targets for ET_EXEC/MIPS64 */
  elfsh_register_relhook(ELFSH_ARCH_MIPS64, ELFSH_TYPE_EXEC, 
			 ELFSH_OS_LINUX, elfsh_relocate_mips64);
  elfsh_register_relhook(ELFSH_ARCH_MIPS64, ELFSH_TYPE_EXEC, 
			 ELFSH_OS_FREEBSD, elfsh_relocate_mips64);
  elfsh_register_relhook(ELFSH_ARCH_MIPS64, ELFSH_TYPE_EXEC, 
			 ELFSH_OS_NETBSD, elfsh_relocate_mips64);
  elfsh_register_relhook(ELFSH_ARCH_MIPS64, ELFSH_TYPE_EXEC, 
			 ELFSH_OS_OPENBSD, elfsh_relocate_mips64);
  elfsh_register_relhook(ELFSH_ARCH_MIPS64, ELFSH_TYPE_EXEC, 
			 ELFSH_OS_SOLARIS, elfsh_relocate_mips64);
  
  /***********************************/
  /* Now register Control flow hooks */
  /***********************************/
  
  /* Usual REL targets for ET_EXEC/IA32 */
  elfsh_register_cflowhook(ELFSH_ARCH_IA32, ELFSH_TYPE_EXEC, 
			   ELFSH_OS_LINUX, elfsh_cflow_ia32);
  elfsh_register_cflowhook(ELFSH_ARCH_IA32, ELFSH_TYPE_EXEC, 
			   ELFSH_OS_FREEBSD, elfsh_cflow_ia32);
  elfsh_register_cflowhook(ELFSH_ARCH_IA32, ELFSH_TYPE_EXEC, 
			   ELFSH_OS_NETBSD, elfsh_cflow_ia32);
  elfsh_register_cflowhook(ELFSH_ARCH_IA32, ELFSH_TYPE_EXEC, 
			   ELFSH_OS_OPENBSD, elfsh_cflow_ia32);
  elfsh_register_cflowhook(ELFSH_ARCH_IA32, ELFSH_TYPE_EXEC, 
			   ELFSH_OS_SOLARIS, elfsh_cflow_ia32);

  /* Usual CFLOW targets for ET_EXEC/IA64 */
  elfsh_register_cflowhook(ELFSH_ARCH_IA64, ELFSH_TYPE_EXEC, 
			   ELFSH_OS_LINUX, elfsh_cflow_ia64);
  elfsh_register_cflowhook(ELFSH_ARCH_IA64, ELFSH_TYPE_EXEC, 
			   ELFSH_OS_FREEBSD, elfsh_cflow_ia64);
  elfsh_register_cflowhook(ELFSH_ARCH_IA64, ELFSH_TYPE_EXEC, 
			   ELFSH_OS_NETBSD, elfsh_cflow_ia64);
  elfsh_register_cflowhook(ELFSH_ARCH_IA64, ELFSH_TYPE_EXEC, 
			   ELFSH_OS_OPENBSD, elfsh_cflow_ia64);
  elfsh_register_cflowhook(ELFSH_ARCH_IA64, ELFSH_TYPE_EXEC, 
			   ELFSH_OS_SOLARIS, elfsh_cflow_ia64);

  /* Usual CFLOW targets for ET_EXEC/SPARC32 */
  elfsh_register_cflowhook(ELFSH_ARCH_SPARC32, ELFSH_TYPE_EXEC, 
			   ELFSH_OS_LINUX, elfsh_cflow_sparc32);
  elfsh_register_cflowhook(ELFSH_ARCH_SPARC32, ELFSH_TYPE_EXEC, 
			   ELFSH_OS_FREEBSD, elfsh_cflow_sparc32);
  elfsh_register_cflowhook(ELFSH_ARCH_SPARC32, ELFSH_TYPE_EXEC, 
			   ELFSH_OS_NETBSD, elfsh_cflow_sparc32);
  elfsh_register_cflowhook(ELFSH_ARCH_SPARC32, ELFSH_TYPE_EXEC, 
			   ELFSH_OS_OPENBSD, elfsh_cflow_sparc32);
  elfsh_register_cflowhook(ELFSH_ARCH_SPARC32, ELFSH_TYPE_EXEC, 
			   ELFSH_OS_SOLARIS, elfsh_cflow_sparc32);

  /* Usual CFLOW targets for ET_EXEC/SPARC64 */
  elfsh_register_cflowhook(ELFSH_ARCH_SPARC64, ELFSH_TYPE_EXEC, 
			   ELFSH_OS_LINUX, elfsh_cflow_sparc64);
  elfsh_register_cflowhook(ELFSH_ARCH_SPARC64, ELFSH_TYPE_EXEC, 
			   ELFSH_OS_FREEBSD, elfsh_cflow_sparc64);
  elfsh_register_cflowhook(ELFSH_ARCH_SPARC64, ELFSH_TYPE_EXEC, 
			   ELFSH_OS_NETBSD, elfsh_cflow_sparc64);
  elfsh_register_cflowhook(ELFSH_ARCH_SPARC64, ELFSH_TYPE_EXEC, 
			   ELFSH_OS_OPENBSD, elfsh_cflow_sparc64);
  elfsh_register_cflowhook(ELFSH_ARCH_SPARC64, ELFSH_TYPE_EXEC, 
			   ELFSH_OS_SOLARIS, elfsh_cflow_sparc64);

  /* Usual CFLOW targets for ET_EXEC/ALPHA64 */
  elfsh_register_cflowhook(ELFSH_ARCH_ALPHA64, ELFSH_TYPE_EXEC, 
			   ELFSH_OS_LINUX, elfsh_cflow_alpha64);
  elfsh_register_cflowhook(ELFSH_ARCH_ALPHA64, ELFSH_TYPE_EXEC, 
			   ELFSH_OS_FREEBSD, elfsh_cflow_alpha64);
  elfsh_register_cflowhook(ELFSH_ARCH_ALPHA64, ELFSH_TYPE_EXEC, 
			   ELFSH_OS_NETBSD, elfsh_cflow_alpha64);
  elfsh_register_cflowhook(ELFSH_ARCH_ALPHA64, ELFSH_TYPE_EXEC, 
			   ELFSH_OS_OPENBSD, elfsh_cflow_alpha64);
  elfsh_register_cflowhook(ELFSH_ARCH_ALPHA64, ELFSH_TYPE_EXEC, 
			   ELFSH_OS_SOLARIS, elfsh_cflow_alpha64);

  /* Usual CFLOW targets for ET_EXEC/MIPS32 */
  elfsh_register_cflowhook(ELFSH_ARCH_MIPS32, ELFSH_TYPE_EXEC, 
			   ELFSH_OS_LINUX, elfsh_cflow_mips32);
  elfsh_register_cflowhook(ELFSH_ARCH_MIPS32, ELFSH_TYPE_EXEC, 
			   ELFSH_OS_FREEBSD, elfsh_cflow_mips32);
  elfsh_register_cflowhook(ELFSH_ARCH_MIPS32, ELFSH_TYPE_EXEC, 
			   ELFSH_OS_NETBSD, elfsh_cflow_mips32);
  elfsh_register_cflowhook(ELFSH_ARCH_MIPS32, ELFSH_TYPE_EXEC, 
			   ELFSH_OS_OPENBSD, elfsh_cflow_mips32);
  elfsh_register_cflowhook(ELFSH_ARCH_MIPS32, ELFSH_TYPE_EXEC, 
			   ELFSH_OS_SOLARIS, elfsh_cflow_mips32);

  /* Usual CFLOW targets for ET_EXEC/MIPS64 */
  elfsh_register_cflowhook(ELFSH_ARCH_MIPS64, ELFSH_TYPE_EXEC, 
			   ELFSH_OS_LINUX, elfsh_cflow_mips64);
  elfsh_register_cflowhook(ELFSH_ARCH_MIPS64, ELFSH_TYPE_EXEC, 
			   ELFSH_OS_FREEBSD, elfsh_cflow_mips64);
  elfsh_register_cflowhook(ELFSH_ARCH_MIPS64, ELFSH_TYPE_EXEC, 
			   ELFSH_OS_NETBSD, elfsh_cflow_mips64);
  elfsh_register_cflowhook(ELFSH_ARCH_MIPS64, ELFSH_TYPE_EXEC, 
			   ELFSH_OS_OPENBSD, elfsh_cflow_mips64);
  elfsh_register_cflowhook(ELFSH_ARCH_MIPS64, ELFSH_TYPE_EXEC, 
			   ELFSH_OS_SOLARIS, elfsh_cflow_mips64);
  elfsh_register_cflowhook(ELFSH_ARCH_IA32, ELFSH_TYPE_DYN, 
			   ELFSH_OS_LINUX, elfsh_cflow_ia32);

  /* Usual REL/PLT/ALTPLT targets for ET_DYN/x86 */
  elfsh_register_plthook(ELFSH_ARCH_IA32, ELFSH_TYPE_DYN, 
			 ELFSH_OS_BEOS, elfsh_hijack_plt_ia32);
  elfsh_register_plthook(ELFSH_ARCH_IA32, ELFSH_TYPE_DYN, 
			 ELFSH_OS_LINUX, elfsh_hijack_plt_ia32);
  elfsh_register_plthook(ELFSH_ARCH_IA32, ELFSH_TYPE_DYN, 
			 ELFSH_OS_FREEBSD, elfsh_hijack_plt_ia32);
  elfsh_register_plthook(ELFSH_ARCH_IA32, ELFSH_TYPE_DYN, 
			 ELFSH_OS_NETBSD, elfsh_hijack_plt_ia32);
  elfsh_register_plthook(ELFSH_ARCH_IA32, ELFSH_TYPE_DYN, 
			 ELFSH_OS_SOLARIS, elfsh_hijack_plt_ia32);
  elfsh_register_relhook(ELFSH_ARCH_IA32, ELFSH_TYPE_DYN,
			 ELFSH_OS_BEOS, elfsh_relocate_ia32);
  elfsh_register_altplthook(ELFSH_ARCH_IA32, ELFSH_TYPE_DYN, 
			    ELFSH_OS_LINUX, elfsh_void_altplthandler);
  elfsh_register_relhook(ELFSH_ARCH_IA32, ELFSH_TYPE_DYN, 
			 ELFSH_OS_LINUX, elfsh_relocate_ia32);
  elfsh_register_altplthook(ELFSH_ARCH_IA32, ELFSH_TYPE_DYN, 
			    ELFSH_OS_BEOS, elfsh_void_altplthandler);  


  /***********************************/
  /* Now register break points hooks */
  /***********************************/
  
  /* Usual breakpoint targets for ET_EXEC/IA32 */
  elfsh_register_breakhook(ELFSH_ARCH_IA32, ELFSH_TYPE_EXEC, 
			   ELFSH_OS_LINUX, elfsh_break_ia32);
  elfsh_register_breakhook(ELFSH_ARCH_IA32, ELFSH_TYPE_EXEC, 
			   ELFSH_OS_FREEBSD, elfsh_break_ia32);
  elfsh_register_breakhook(ELFSH_ARCH_IA32, ELFSH_TYPE_EXEC, 
			   ELFSH_OS_NETBSD, elfsh_break_ia32);
  elfsh_register_breakhook(ELFSH_ARCH_IA32, ELFSH_TYPE_EXEC, 
			   ELFSH_OS_OPENBSD, elfsh_break_ia32);
  elfsh_register_breakhook(ELFSH_ARCH_IA32, ELFSH_TYPE_EXEC, 
			   ELFSH_OS_SOLARIS, elfsh_break_ia32);
  elfsh_register_breakhook(ELFSH_ARCH_IA32, ELFSH_TYPE_DYN, 
			   ELFSH_OS_LINUX, elfsh_break_ia32);
  elfsh_register_breakhook(ELFSH_ARCH_IA32, ELFSH_TYPE_DYN, 
			   ELFSH_OS_FREEBSD, elfsh_break_ia32);
  elfsh_register_breakhook(ELFSH_ARCH_IA32, ELFSH_TYPE_DYN, 
			   ELFSH_OS_NETBSD, elfsh_break_ia32);
  elfsh_register_breakhook(ELFSH_ARCH_IA32, ELFSH_TYPE_DYN, 
			   ELFSH_OS_OPENBSD, elfsh_break_ia32);
  elfsh_register_breakhook(ELFSH_ARCH_IA32, ELFSH_TYPE_DYN, 
			   ELFSH_OS_SOLARIS, elfsh_break_ia32);

  /***********************************/
  /* Now register encodeplt points hooks */
  /***********************************/
  
  /* Usual ENCODEPLT breakpoint targets for ET_EXEC/IA32 */
  elfsh_register_encodeplthook(ELFSH_ARCH_IA32, ELFSH_TYPE_EXEC, 
			       ELFSH_OS_LINUX, elfsh_encodeplt_ia32);
  elfsh_register_encodeplthook(ELFSH_ARCH_IA32, ELFSH_TYPE_EXEC, 
			       ELFSH_OS_FREEBSD, elfsh_encodeplt_ia32);
  elfsh_register_encodeplthook(ELFSH_ARCH_IA32, ELFSH_TYPE_EXEC, 
			       ELFSH_OS_NETBSD, elfsh_encodeplt_ia32);
  elfsh_register_encodeplthook(ELFSH_ARCH_IA32, ELFSH_TYPE_EXEC, 
			       ELFSH_OS_OPENBSD, elfsh_encodeplt_ia32);
  elfsh_register_encodeplthook(ELFSH_ARCH_IA32, ELFSH_TYPE_EXEC, 
			       ELFSH_OS_SOLARIS, elfsh_encodeplt_ia32);
  elfsh_register_encodeplthook(ELFSH_ARCH_IA32, ELFSH_TYPE_DYN, 
			       ELFSH_OS_LINUX, elfsh_encodeplt_ia32);
  elfsh_register_encodeplthook(ELFSH_ARCH_IA32, ELFSH_TYPE_DYN, 
			       ELFSH_OS_FREEBSD, elfsh_encodeplt_ia32);
  elfsh_register_encodeplthook(ELFSH_ARCH_IA32, ELFSH_TYPE_DYN, 
			       ELFSH_OS_NETBSD, elfsh_encodeplt_ia32);
  elfsh_register_encodeplthook(ELFSH_ARCH_IA32, ELFSH_TYPE_DYN, 
			       ELFSH_OS_OPENBSD, elfsh_encodeplt_ia32);
  elfsh_register_encodeplthook(ELFSH_ARCH_IA32, ELFSH_TYPE_DYN, 
			       ELFSH_OS_SOLARIS, elfsh_encodeplt_ia32);

  /* Usual ENCODEPLT1 targets for ET_EXEC/IA32 */
  elfsh_register_encodeplt1hook(ELFSH_ARCH_IA32, ELFSH_TYPE_EXEC, 
				ELFSH_OS_LINUX, elfsh_encodeplt1_ia32);
  elfsh_register_encodeplt1hook(ELFSH_ARCH_IA32, ELFSH_TYPE_EXEC, 
				ELFSH_OS_FREEBSD, elfsh_encodeplt1_ia32);
  elfsh_register_encodeplt1hook(ELFSH_ARCH_IA32, ELFSH_TYPE_EXEC, 
				ELFSH_OS_NETBSD, elfsh_encodeplt1_ia32);
  elfsh_register_encodeplt1hook(ELFSH_ARCH_IA32, ELFSH_TYPE_EXEC, 
				ELFSH_OS_OPENBSD, elfsh_encodeplt1_ia32);
  elfsh_register_encodeplt1hook(ELFSH_ARCH_IA32, ELFSH_TYPE_EXEC, 
				ELFSH_OS_SOLARIS, elfsh_encodeplt1_ia32);
  elfsh_register_encodeplt1hook(ELFSH_ARCH_IA32, ELFSH_TYPE_DYN, 
				ELFSH_OS_LINUX, elfsh_encodeplt1_ia32);
  elfsh_register_encodeplt1hook(ELFSH_ARCH_IA32, ELFSH_TYPE_DYN, 
				ELFSH_OS_FREEBSD, elfsh_encodeplt1_ia32);
  elfsh_register_encodeplt1hook(ELFSH_ARCH_IA32, ELFSH_TYPE_DYN, 
				ELFSH_OS_NETBSD, elfsh_encodeplt1_ia32);
  elfsh_register_encodeplt1hook(ELFSH_ARCH_IA32, ELFSH_TYPE_DYN, 
				ELFSH_OS_OPENBSD, elfsh_encodeplt1_ia32);
  elfsh_register_encodeplt1hook(ELFSH_ARCH_IA32, ELFSH_TYPE_DYN, 
				ELFSH_OS_SOLARIS, elfsh_encodeplt1_ia32);

  /*** Now EXTPLT handlers ***/

  /* Usual EXTPLT targets for ET_EXEC/IA32 */
  elfsh_register_extplthook(ELFSH_ARCH_IA32, ELFSH_TYPE_EXEC, 
			    ELFSH_OS_LINUX, elfsh_extplt_ia32);
  elfsh_register_extplthook(ELFSH_ARCH_IA32, ELFSH_TYPE_EXEC, 
			    ELFSH_OS_FREEBSD, elfsh_extplt_ia32);
  elfsh_register_extplthook(ELFSH_ARCH_IA32, ELFSH_TYPE_EXEC, 
			    ELFSH_OS_NETBSD, elfsh_extplt_ia32);
  elfsh_register_extplthook(ELFSH_ARCH_IA32, ELFSH_TYPE_EXEC, 
			    ELFSH_OS_OPENBSD, elfsh_extplt_ia32);
  elfsh_register_extplthook(ELFSH_ARCH_IA32, ELFSH_TYPE_EXEC, 
			    ELFSH_OS_SOLARIS, elfsh_extplt_ia32);
  elfsh_register_extplthook(ELFSH_ARCH_IA32, ELFSH_TYPE_DYN, 
			    ELFSH_OS_LINUX, elfsh_extplt_ia32);
  elfsh_register_extplthook(ELFSH_ARCH_IA32, ELFSH_TYPE_DYN, 
			    ELFSH_OS_FREEBSD, elfsh_extplt_ia32);
  elfsh_register_extplthook(ELFSH_ARCH_IA32, ELFSH_TYPE_DYN, 
			    ELFSH_OS_NETBSD, elfsh_extplt_ia32);
  elfsh_register_extplthook(ELFSH_ARCH_IA32, ELFSH_TYPE_DYN, 
			    ELFSH_OS_OPENBSD, elfsh_extplt_ia32);
  elfsh_register_extplthook(ELFSH_ARCH_IA32, ELFSH_TYPE_DYN, 
			    ELFSH_OS_SOLARIS, elfsh_extplt_ia32);
  
  puts("Finished initialisations of vectorized hooks ");
  
  done++;
  ELFSH_PROFILE_OUT(__FILE__, __FUNCTION__, __LINE__);
}


/* Call the relocation hook */
int		  elfsh_rel(elfshobj_t *file, elfshsect_t *s, elfsh_Rel *r, 
			    elfsh_Addr *l, elfsh_Addr a, elfshsect_t *m)
{
  u_char        archtype;
  u_char        elftype;
  u_char        ostype;
  int		ret;
  elfshvector_t	*rel;

  //elfsh_Addr	***hook;
  u_int		dim[3];

  int		(*fct)(elfshsect_t *n,
		       elfsh_Rel  *n2,
		       elfsh_Addr *n3,
		       elfsh_Addr  n4,
		       elfshsect_t *n5);

  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);

  rel = (elfshvector_t *) hash_get(&vector_hash, ELFSH_HOOK_REL);

  /* Fingerprint binary */
  archtype = elfsh_get_archtype(file);
  elftype  = elfsh_get_elftype(file);
  ostype   = elfsh_get_ostype(file);
  if (archtype == ELFSH_ARCH_ERROR ||
      elftype  == ELFSH_TYPE_ERROR ||
      ostype   == ELFSH_OS_ERROR)
    ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "RELOCATION handler unexistant for this ARCH/OS", -1);
  
  //hook = rel->hook;
  //fct = (void *) hook[archtype][elftype][ostype];

  dim[0] = archtype;
  dim[1] = elftype;
  dim[2] = ostype;
  fct    = elfsh_project_coords(rel, dim, 3);

  ret = fct(s, r, l, a, m);
  if (ret < 0)
    ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__,
		      "Relocation handler failed", (-1));
  ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}



/* Call the relocation hook */
int             elfsh_cflow(elfshobj_t *file, char *name, elfsh_Sym *old, 
			    elfsh_Addr new)
{
  elfshvector_t	*cflow;
  u_char        archtype;
  u_char        elftype;
  u_char        ostype;
  int           ret;
  int		(*fct)(elfshobj_t *n, char *n2, elfsh_Sym *n3, elfsh_Addr n4);
  //elfsh_Addr	***hook;
  u_int		dim[3];

  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);
  cflow = (elfshvector_t *) hash_get(&vector_hash, ELFSH_HOOK_CFLOW);

  /* Fingerprint binary */
  archtype = elfsh_get_archtype(file);
  elftype = elfsh_get_elftype(file);
  ostype   = elfsh_get_ostype(file);
  if (archtype == ELFSH_ARCH_ERROR ||
      elftype == ELFSH_TYPE_ERROR ||
      ostype   == ELFSH_OS_ERROR)
    ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__,
                      "CFLOW handler unexistant for this ARCH/OS", -1);
  
  //hook = cflow->hook;
  //fct = (void *) hook[archtype][elftype][ostype];
  dim[0] = archtype;
  dim[1] = elftype;
  dim[2] = ostype;
  fct    = elfsh_project_coords(cflow, dim, 3);

  ret = fct(file, name, old, new);
  if (ret < 0)
    ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__,
                      "Control flow redirection handler failed", (-1));
  ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}


/* Call the PLT hook */
int             elfsh_plt(elfshobj_t *file, elfsh_Sym *s, elfsh_Addr new)
{
  elfshvector_t	*plt;
  u_char        archtype;
  u_char        elftype;
  u_char        ostype;
  int           ret;
  int		(*fct)(elfshobj_t *f, elfsh_Sym *s, elfsh_Addr a);
  //elfsh_Addr	***hook;
  u_int		dim[3];

  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);
  plt = (elfshvector_t *) hash_get(&vector_hash, ELFSH_HOOK_PLT);

  /* Fingerprint binary */
  archtype = elfsh_get_archtype(file);
  elftype = elfsh_get_elftype(file);
  ostype = elfsh_get_ostype(file);
  if (archtype == ELFSH_ARCH_ERROR ||
      elftype  == ELFSH_TYPE_ERROR ||
      ostype   == ELFSH_OS_ERROR)
    ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__,
                      "PLT handler unexistant for this ARCH/OS", -1);

  //hook = plt->hook;
  //fct  = (void *) hook[archtype][elftype][ostype];
  dim[0] = archtype;
  dim[1] = elftype;
  dim[2] = ostype;
  fct    = elfsh_project_coords(plt, dim, 3);

  ret  = fct(file, s, new);
  if (ret < 0)
    ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__,
                      "PLT redirection handler failed", (-1));
  ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, ret);
}



/* Call the ENCODEPLT hook */
int             elfsh_encodeplt(elfshobj_t *file, elfshsect_t *plt, 
				elfsh_Addr diff, u_int off)
{
  elfshvector_t	*encodeplt;
  u_char        archtype;
  u_char        elftype;
  u_char        ostype;
  int           ret;
  int		(*fct)(elfshobj_t *f, elfshsect_t *s, elfsh_Addr a, u_int off);
  //elfsh_Addr	***hook;
  u_int		dim[3];

  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);
  encodeplt = (elfshvector_t *) hash_get(&vector_hash, ELFSH_HOOK_ENCODEPLT);

  /* Fingerprint binary */
  archtype = elfsh_get_archtype(file);
  elftype = elfsh_get_elftype(file);
  ostype = elfsh_get_ostype(file);
  if (archtype == ELFSH_ARCH_ERROR ||
      elftype  == ELFSH_TYPE_ERROR ||
      ostype   == ELFSH_OS_ERROR)
    ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__,
                      "ENCODEPLT handler unexistant for this ARCH/OS", -1);

  //hook = encodeplt->hook;
  //fct  = (void *) hook[archtype][elftype][ostype];
  dim[0] = archtype;
  dim[1] = elftype;
  dim[2] = ostype;
  fct    = elfsh_project_coords(encodeplt, dim, 3);

  ret  = fct(file, plt, diff, off);
  if (ret < 0)
    ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__,
                      "PLT encoding handler failed", (-1));
  ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, ret);
}



/* Call the ENCODEPLT1 hook */
int             elfsh_encodeplt1(elfshobj_t *file, elfshsect_t *plt, 
				 elfshsect_t *extplt, elfsh_Addr diff)
{
  elfshvector_t	*encodeplt1;
  u_char        archtype;
  u_char        elftype;
  u_char        ostype;
  int           ret;
  int		(*fct)(elfshobj_t *f, elfshsect_t *s, elfshsect_t *s2,
		       elfsh_Addr a);
  //elfsh_Addr	***hook;
  u_int		dim[3];

  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);
  encodeplt1 = (elfshvector_t *) hash_get(&vector_hash, ELFSH_HOOK_ENCODEPLT1);

  /* Fingerprint binary */
  archtype = elfsh_get_archtype(file);
  elftype = elfsh_get_elftype(file);
  ostype = elfsh_get_ostype(file);
  if (archtype == ELFSH_ARCH_ERROR ||
      elftype  == ELFSH_TYPE_ERROR ||
      ostype   == ELFSH_OS_ERROR)
    ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__,
                      "ENCODEPLT1 handler unexistant for this ARCH/OS", -1);

  //hook = encodeplt1->hook;
  //fct  = (void *) hook[archtype][elftype][ostype];
  dim[0] = archtype;
  dim[1] = elftype;
  dim[2] = ostype;
  fct    = elfsh_project_coords(encodeplt1, dim, 3);

  ret  = fct(file, plt, extplt, diff);
  if (ret < 0)
    ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__,
                      "PLT1 encoding handler failed", (-1));
  ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, ret);
}



/* Call the ALTPLT hook */
int             elfsh_altplt(elfshobj_t *file, elfsh_Sym *s, elfsh_Addr new)
{
  elfshvector_t	*altplt;
  u_char        archtype;
  u_char        elftype;
  u_char        ostype;
  int           ret;
  int		(*fct)(elfshobj_t *file, elfsh_Sym *s, elfsh_Addr a);
  //elfsh_Addr	***hook;
  u_int		dim[3];

  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);
  altplt = (elfshvector_t *) hash_get(&vector_hash, ELFSH_HOOK_ALTPLT);
 
  /* Fingerprint binary */
  archtype = elfsh_get_archtype(file);
  elftype = elfsh_get_elftype(file);
  ostype = elfsh_get_ostype(file);
  if (archtype == ELFSH_ARCH_ERROR ||
      elftype  == ELFSH_TYPE_ERROR ||
      ostype   == ELFSH_OS_ERROR)
    ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__,
                      "ALTPLT handler unexistant for this ARCH/OS", -1);

  dim[0] = archtype;
  dim[1] = elftype;
  dim[2] = ostype;
  fct    = elfsh_project_coords(altplt, dim, 3);

  //hook = altplt->hook;
  //fct  = (void *) hook[archtype][elftype][ostype];
  ret  = fct(file, s, new);
  if (ret < 0)
    ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__,
                      "ALTPLT redirection handler failed", (-1));
  ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, ret);
}


/* Call the EXTPLT hook */
int             elfsh_extplt(elfshsect_t *extplt, elfshsect_t *altgot, 
			     elfshsect_t *dynsym, elfshsect_t *relplt)
{
  elfshvector_t	*vextplt;
  u_char        archtype;
  u_char        elftype;
  u_char        ostype;
  int           ret;
  int		(*fct)(elfshsect_t *extplt, elfshsect_t *altgot,
		       elfshsect_t *dynsym, elfshsect_t *relplt);
  //elfsh_Addr	***hook;
  u_int		dim[3];

  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);
  vextplt = (elfshvector_t *) hash_get(&vector_hash, ELFSH_HOOK_EXTPLT);

  /* Fingerprint binary */
  archtype = elfsh_get_archtype(extplt->parent);
  elftype = elfsh_get_elftype(extplt->parent);
  ostype = elfsh_get_ostype(extplt->parent);
  if (archtype == ELFSH_ARCH_ERROR ||
      elftype  == ELFSH_TYPE_ERROR ||
      ostype   == ELFSH_OS_ERROR)
    ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__,
                      "EXTPLT handler unexistant for this ARCH/OS", -1);

  //hook = vextplt->hook;
  //fct  = (void *) hook[archtype][elftype][ostype];
  dim[0] = archtype;
  dim[1] = elftype;
  dim[2] = ostype;
  fct    = elfsh_project_coords(vextplt, dim, 3);
  ret  = fct(extplt, altgot, dynsym, relplt);
  if (ret < 0)
    ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__,
                      "EXTPLT redirection handler failed", (-1));
  ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, ret);
}




/* Call the breakpoint hook */
int		  elfsh_setbreak(elfshobj_t *file, elfshbp_t *bp)
{
  elfshvector_t	*breakh;
  u_char        archtype;
  u_char        elftype;
  u_char        ostype;
  int		ret;
  int		(*fct)(elfshobj_t *file, elfshbp_t *bp);
  //elfsh_Addr	***hook;
  u_int		dim[3];

  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);
  breakh = (elfshvector_t *) hash_get(&vector_hash, ELFSH_HOOK_BREAK);

  /* Fingerprint binary */
  archtype = elfsh_get_archtype(file);
  elftype = elfsh_get_elftype(file);
  ostype = elfsh_get_ostype(file);
  if (archtype == ELFSH_ARCH_ERROR ||
      elftype  == ELFSH_TYPE_ERROR ||
      ostype   == ELFSH_OS_ERROR)
    ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, 
		      "SETBREAK handler unexistant for this ARCH/OS", -1);
  
  dim[0] = archtype;
  dim[1] = elftype;
  dim[2] = ostype;
  fct    = elfsh_project_coords(breakh, dim, 3);

  //hook = breakh->hook;
  //fct  = (void *) hook[archtype][elftype][ostype];

  ret  = fct(file, bp);
  if (ret < 0)
    ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__,
		      "Breakpoint handler failed", (-1));
  ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}




/* Translate ELF architecture type into ELFsh architecture type */
u_char		elfsh_get_archtype(elfshobj_t *file)
{
  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);

  if (file == NULL)
   ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__,
    "Invalid file argument!", (ELFSH_ARCH_ERROR));

  switch (elfsh_get_arch(file->hdr))
    {
    case EM_386:
      ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, (ELFSH_ARCH_IA32));
    case EM_SPARC:
    case EM_SPARC32PLUS:
      ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, (ELFSH_ARCH_SPARC32));
    case EM_SPARCV9:
      ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, (ELFSH_ARCH_SPARC64));
    case EM_PARISC:
      ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, (ELFSH_ARCH_PARISC));
    case EM_IA_64:
      ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, (ELFSH_ARCH_IA64));
    case EM_PPC:
      ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, (ELFSH_ARCH_PPC32));
    case EM_PPC64:
      ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, (ELFSH_ARCH_PPC64));
    case EM_SVX:
      ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, (ELFSH_ARCH_CRAY));
    case EM_ALPHA:
#if EM_ALPHA != EM_ALPHA_EXP
    case EM_ALPHA_EXP:
#endif
      ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, (ELFSH_ARCH_ALPHA64));
    case EM_MIPS:
    case EM_MIPS_RS3_LE:
      ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, (ELFSH_ARCH_MIPS32));
    case EM_ARM:
      ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, (ELFSH_ARCH_ARM));
    default:
      ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, (ELFSH_ARCH_ERROR));
    }
}


/* Translate ELF object type into ELFsh object type */
u_char		elfsh_get_elftype(elfshobj_t *file)
{
  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);

  switch (elfsh_get_objtype(file->hdr))
    {
    case ET_EXEC:
      ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, (ELFSH_TYPE_EXEC));
    case ET_DYN:
      ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, (ELFSH_TYPE_DYN));
    default:
      ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, (ELFSH_TYPE_ERROR));
    }
}

      
/* Retreive ELFsh OS type from ELF header */
u_char  elfsh_get_real_ostype(elfshobj_t *file)
{
  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);

  switch (file->hdr->e_ident[EI_OSABI])
    {
    case ELFOSABI_LINUX:
      ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, (ELFSH_OS_LINUX));
    case ELFOSABI_FREEBSD:
      ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, (ELFSH_OS_FREEBSD));
    case ELFOSABI_NETBSD:
      ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, (ELFSH_OS_NETBSD));
    case ELFOSABI_OPENBSD:
      ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, (ELFSH_OS_OPENBSD));
    case ELFOSABI_SOLARIS:
      ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, (ELFSH_OS_SOLARIS));
    case 0:
      ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
    default:
      ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, (ELFSH_OS_ERROR));
    }
}


/* For now, always return the type of a userland process */
/* Need to be extended for the kernel memory type */
u_char		elfsh_get_hosttype(elfshobj_t *file)
{
  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);
  ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, (E2DBG_HOST_PROC));
}


/* Fill up ELFsh Operating System type */
u_char		elfsh_get_ostype(elfshobj_t *file)
{
  u_char	*res;
  char		*interp;
  char		*end;
  char		r;

  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);

  /* First try */
  r = elfsh_get_real_ostype(file);
  switch (r)
    {
      /* Information not present */
    case 0:
      break;
    default:
      ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, (r));
    }
  
  /* Smart intermediate way, fingerprint by .interp section */
  /* Not aware of any other software using this technique */
  if (elfsh_get_interp(file))
    {
      if (!interp_hash.size)
	{
	  hash_init(&interp_hash, 10);
	  hash_add(&interp_hash, 
		   "/lib/ld-linux.so", 
		   &elfsh_ostype[ELFSH_OS_LINUX]);
	  hash_add(&interp_hash, 
		   "/usr/libexec/ld-elf.so", 
		   &elfsh_ostype[ELFSH_OS_FREEBSD]);
	  hash_add(&interp_hash, 
		   "/usr/libexec/ld-eld_so", 
		   &elfsh_ostype[ELFSH_OS_NETBSD]);
	  hash_add(&interp_hash, 
		   "??????????????", 
		   &elfsh_ostype[ELFSH_OS_OPENBSD]);
	  hash_add(&interp_hash, 
		   "/usr/lib/ld.so", 
		   &elfsh_ostype[ELFSH_OS_SOLARIS]);
	  
	}


      /* Only useful for objects with .interp section */
      /* e.g. : ET_EXEC, and some special ET_DYN like libc */
      interp = (char *) elfsh_get_raw(file->secthash[ELFSH_SECTION_INTERP]);
      end = strstr(".so", interp);
      if (end)
	*(end + 3) = 0x00;
      res  = hash_get(&interp_hash, interp);
      if (res)
	{
	  file->hdr->e_ident[EI_OSABI] = *res;
	  ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, (elfsh_get_real_ostype(file)));
	}
    }

  /* Fatalist */
#if defined(__linux__)
    ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, ELFSH_OS_LINUX);
#elif defined(__FreeBSD__)
    ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, ELFSH_OS_FREEBSD);
#elif defined(__NetBSD__)
    ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, ELFSH_OS_NETBSD);
#elif defined(__OpenBSD__)
    ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, ELFSH_OS_OPENBSD);
#elif defined(sun)
    ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, ELFSH_OS_SOLARIS);
#elif defined(__BEOS__)
    ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, ELFSH_OS_BEOS);
#else
    ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, ELFSH_OS_ERROR);
#endif
}

/* Return the page size */
int       elfsh_get_pagesize(elfshobj_t *file)
{
  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);

  switch (elfsh_get_archtype(file))
    {
    case ELFSH_ARCH_IA32:
    case ELFSH_ARCH_MIPS32:
      ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, 4096);
    case ELFSH_ARCH_SPARC32:
    case ELFSH_ARCH_SPARC64:
    case ELFSH_ARCH_PARISC:
    case ELFSH_ARCH_IA64:
    case ELFSH_ARCH_ALPHA64:
      ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, 8192);
    case ELFSH_ARCH_PPC32:
    case ELFSH_ARCH_PPC64:
    case ELFSH_ARCH_CRAY:
    case ELFSH_ARCH_ERROR:
    default:
      ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, 
			"Unsupported architecture", 0);
    }  
}

/* Return the page size */
u_int       elfsh_get_breaksize(elfshobj_t *file)
{
  ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);
  
  switch (elfsh_get_archtype(file))
    {
    case ELFSH_ARCH_IA32:
      ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, 1);
    case ELFSH_ARCH_MIPS32:
    case ELFSH_ARCH_SPARC32:
    case ELFSH_ARCH_SPARC64:
    case ELFSH_ARCH_PARISC:
    case ELFSH_ARCH_ALPHA64:
    case ELFSH_ARCH_PPC32:
    case ELFSH_ARCH_PPC64:
      ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, 4);
    case ELFSH_ARCH_IA64:
    case ELFSH_ARCH_CRAY:
    case ELFSH_ARCH_ERROR:
    default:
      ELFSH_PROFILE_ERR(__FILE__, __FUNCTION__, __LINE__, 
			"Unsupported architecture", 0);
    }  
}

