struct vma_struct 
{
	unsigned long ( * vmalloc )( unsigned int );
	int size;
	unsigned long mem;
};

/* This function is called by syscall() */
void vmalloc( struct vma_struct * v )
{
  /* vma_struct.vmalloc contains the address of vmalloc symbol */
  /* vma_struct.size contains the size to alloc (first arg) */
  /* vma_struct.mem contains the return address of vmalloc */
  v->mem = ( unsigned long )v->vmalloc( v->size );
}
