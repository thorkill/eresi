struct kfr_struct 
{
  unsigned long ( * kfree )( const void * );
  const void * obj;
};

void
kfree( struct kfr_struct * k )
{
  k->kfree( k->obj );
}
