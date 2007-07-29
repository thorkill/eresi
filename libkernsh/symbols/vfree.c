struct vfr_struct 
{
	unsigned long ( * vfree )( void * );
	void * obj;
};

/* This function is called by syscall() */
void vfree( struct vfr_struct * v )
{
  /* vfr_struct.vfree contains the address of vfree symbol */
  /* vfr_struct.obj contains the address to be free */
  v->vfree( v->obj );
}
