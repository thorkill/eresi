struct kma_struct 
{
	unsigned long ( * kmalloc )( unsigned int, int );
	int size;
	int flags;
	unsigned long mem;
};

/* This function is called by syscall() */
void kmalloc( struct kma_struct * k )
{
  /* kma_struct.kmalloc contains the address of kmalloc symbol */
  /* kma_struct.size contains the size to alloc (first arg) */
  /* kma_struct.flags contains the flags (twict arg ) */
  /* kma_struct.mem contains the return address of kmalloc */
  k->mem = ( unsigned long )k->kmalloc( k->size, k->flags );
}
