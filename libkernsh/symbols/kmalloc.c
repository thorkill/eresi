struct kma_struct 
{
  unsigned long ( * kmalloc )( unsigned int, int );
  int size;
  int flags;
  unsigned long mem;
};

void
kmalloc( struct kma_struct * k )
{
  k->mem = ( unsigned long )k->kmalloc( k->size, k->flags );
}
