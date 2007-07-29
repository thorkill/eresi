struct kfr_struct 
{
	unsigned long ( * kfree )( const void * );
	const void * obj;
};

/* This function is called by syscall() */
void kfree( struct kfr_struct * k )
{
  /* kfr_struct.kfree contains the address of kfree symbol */
  /* kfr_struct.obj contains the address to be free */
  k->kfree( k->obj );
}
