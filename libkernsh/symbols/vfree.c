struct vfr_struct 
{
  unsigned long ( * vfree )( void * );
  void * obj;
};

void
vfree( struct vfr_struct * v )
{
  v->vfree( v->obj );
}
