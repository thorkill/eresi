/*
** implicit.c for elfsh
** 
** Started on  Tue Feb 18 06:45:35 2003 mayhem
** Last update Sat Mar 15 08:11:18 2003 mayhem
*/
#include "elfsh.h"



/* Load the working files */
void				vm_load_cwfiles(char **argv)
{
  if (world.state.input == NULL)
    {
      cmd_help();
      exit (-1);
    }
  world.current = (world.state.output != NULL ? 
		   elfsh_map_obj(world.state.input) :
		   elfsh_load_obj(world.state.input));
  if (world.current == NULL)							
    {								
      perror(world.state.input);					
      exit(-1);						
    }								
  world.current->next = world.list;
  world.list = world.current;
  hash_add(&file_hash, world.current->name, (void *) world.current);
  if (!world.state.vm_quiet)								
    printf("\n [*] Object %s has been loaded (%s) \n\n", 
	   world.state.input, 
	   (world.state.output != NULL ? "O_RDWR" : "O_RDONLY"));
}




/* Unload (and save if asked) files */
int				vm_unload_cwfiles()
{
  if (world.state.output)
    switch (elfsh_save_obj(world.current, world.state.output))
      {
      case 0:
	if (!world.state.vm_quiet)
	  printf(" [*] Object %s saved successfully \n\n", world.state.output);
	break;
      default:
	fprintf(stderr, 
		" [*] Unable to save modified object in %s \n\n", 
		world.state.output);
	return (-1);
      }
  else
    {
      elfsh_unload_obj(world.current);
      if (!world.state.vm_quiet)
	printf(" [*] Object %s unloaded \n\n", world.state.input);
    }
  return (0);
}




/* Do all implicit operation when calling elfsh from a script */
int		vm_implicit(elfshcmd_t *actual, char **argv)
{

  /* If the requested command does not need a current pointer */
  if (actual && !actual->wflags)
    return (0);

  /* Load implicit current file */
  else if (world.state.vm_mode == ELFSH_VMSTATE_CMDLINE && 
	   world.current == NULL)
    {
      vm_load_cwfiles(argv);
      if (!world.current)
	{
	  cmd_help();
	  exit(-1);
	}
    }
  
  /* Do error in imode if current is NULL */
  else if (world.state.vm_mode != ELFSH_VMSTATE_CMDLINE && 
	   !world.current)
    {
      cmd_dolist();
      return (-1);
    }

  /* We need to set it here since the CURRENT object can change */
#if defined(USE_LIBASM)
  asm_set_resolve_handler(&world.proc, do_resolve, world.current);
#endif
  return (0);
}
















