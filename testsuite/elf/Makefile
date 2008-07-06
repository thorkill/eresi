##
## Makefile for librevm in ERESI
##
## Started on  Fri Nov  2 16:32:29 2001 jfv
## Updated on  Wed Nov 28 12:57:38 2001 jfv
##

include ../../config.h

all			: all32 all64

all32			: 
			$(MAKE) -C ctors_hijack all32
			$(MAKE) -C dtors_hijack all32
			$(MAKE) -C extract_raw_data all32
			$(MAKE) -C etrel_inject all32
			$(MAKE) -C hash_retreive all32
			$(MAKE) -C got_hijack all32
			$(MAKE) -C sct_index_injection all32
			$(MAKE) -C sct_fillbss_injection all32
			$(MAKE) -C sct_top_injection all32
			$(MAKE) -C sct_unmapped_injection all32
			$(MAKE) -C sht_stripping all32
			$(MAKE) -C sht_rebuild all32
			$(MAKE) -C symtab_extend all32

all64			: 
			$(MAKE) -C ctors_hijack all64
			$(MAKE) -C dtors_hijack all64
			$(MAKE) -C extract_raw_data all64
			$(MAKE) -C etrel_inject all64
			$(MAKE) -C hash_retreive all64
			$(MAKE) -C got_hijack all64
			$(MAKE) -C sct_index_injection all64
			$(MAKE) -C sct_fillbss_injection all64
			$(MAKE) -C sct_top_injection all64
			$(MAKE) -C sct_unmapped_injection all64
			$(MAKE) -C sht_stripping all64
			$(MAKE) -C sht_rebuild all64
			$(MAKE) -C symtab_extend all64

clean		       :
			$(MAKE) -C ctors_hijack clean
			$(MAKE) -C dtors_hijack clean
			$(MAKE) -C extract_raw_data clean
			$(MAKE) -C etrel_inject clean
			$(MAKE) -C hash_retreive clean
			$(MAKE) -C got_hijack clean
			$(MAKE) -C sct_index_injection clean
			$(MAKE) -C sct_fillbss_injection clean
			$(MAKE) -C sct_top_injection clean
			$(MAKE) -C sct_unmapped_injection clean
			$(MAKE) -C sht_stripping clean
			$(MAKE) -C sht_rebuild clean
			$(MAKE) -C symtab_extend clean

fclean		       : clean
			$(MAKE) -C ctors_hijack fclean
			$(MAKE) -C dtors_hijack fclean
			$(MAKE) -C extract_raw_data fclean
			$(MAKE) -C etrel_inject fclean
			$(MAKE) -C hash_retreive fclean
			$(MAKE) -C got_hijack fclean
			$(MAKE) -C sct_index_injection fclean
			$(MAKE) -C sct_fillbss_injection fclean
			$(MAKE) -C sct_top_injection fclean
			$(MAKE) -C sct_unmapped_injection fclean
			$(MAKE) -C sht_stripping fclean
			$(MAKE) -C sht_rebuild fclean
			$(MAKE) -C symtab_extend fclean