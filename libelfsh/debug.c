/*
 ** debug.c for elfsh
 **
 ** Started on  Thu Apr  3 21:36:55 2003 mayhem

*/
#include "libelfsh.h"



/* Debug : print the section list */
int		elfsh_print_sectlist(elfshobj_t *obj, char *label)
{
	elfshsect_t	*actual;
	u_int		index;
	u_char	*data;
	char		*sctname;

	ELFSH_PROFILE_IN(__FILE__, __FUNCTION__, __LINE__);

	printf(" [SCTLIST][%s]\n", label);
	for (index = 0, actual = obj->sectlist;
			actual != NULL;
			index++, actual = actual->next)
	{
		sctname = elfsh_get_section_name(obj, actual);
		if (sctname == NULL)
			sctname = "UNK";
		data = elfsh_get_raw(actual);
		if (data == (u_char *)NULL)
			data = (u_char *)"\xFF\xFF\xFF";
		printf(" [%03u:%03u] %-15s HDRNAM: %-15s BYTES[%02X %02X %02X] P(%8p) "
				"A(%8p) N(%8p) SCTIDX(%03u) HDRFOFF:%010u SZ:%010u VADDR:%08X \n",
				index, actual->index, 
				(actual->name != NULL ? actual->name : "UNK"),
				sctname,
				(u_int) data[0],
				(u_int) data[1],
				(u_int) data[2],
				actual->prev,
				actual,
				actual->next,
				actual->index,
				(u_int) actual->shdr->sh_offset,
				(u_int) actual->shdr->sh_size,
				(u_int) actual->shdr->sh_addr);
	}
	puts(" [EOL]\n");
	fflush(stdout);
	ELFSH_PROFILE_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}
