struct vma_struct 
{
  unsigned long ( * vmalloc )( unsigned int );
  int size;
  unsigned long mem;
};

void
vmalloc( struct vma_struct * v )
{
  v->mem = ( unsigned long )v->vmalloc( v->size );
}
