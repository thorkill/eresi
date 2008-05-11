/**
 * @file sht_rebuild.c
 * @ingroup libelfsh
 ** sht_rebuild.c for elfsh
 ** 
 ** !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! 
 ** This code is functional as it, but it is WORK IN PROGRESS 
 ** !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! 
 ** 
 ** Started on  Tue Mar 26 19:07:23 2002 jfv
 ** $Id: sht_rebuild.c,v 1.24 2008-02-16 12:32:27 thor Exp $
 */
#include "libelfsh.h"
#include "libasm.h"



/** 
 * @brief This function insert a SHT entry without shifting the address space
 * I.E. it does truncate some sections for creating new ones.
 *
 * This is necessary when we do SHT reconstruction or when we unmerge
 * the PLT section from the TEXT section on the MIPS architecture.
 *
 * @param file
 * @param phdr_index
 * @param shdr
 * @param name
 * @return
 */
int	elfsh_merge_shtentry(elfshobj_t *file, 
			     int phdr_index, 
			     elfsh_Shdr shdr, 
			     char	*name)
{
  elfshsect_t	*sect;
  int         index;
  int         namelen;
  int         len;
  int         off;
  char        buff[BUFSIZ];
  void        *data;
  
  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);
  
  namelen = strlen(name);
  
  printf("Insertion section header *%s* \n", name);
  
  for (index = 0; index < file->hdr->e_shnum; index++)
    {
      /* Used to allow a 3 bytes padding for the current section */
      len = file->sht[index].sh_size - shdr.sh_size;
      
      printf("SHT rebuild: len = *%d* \n", len);
      
      if (len < 0)
        len = -len;
      
      /* The new section perfectly match an existing one (with an allowed padding of 3 bytes) */
      if (file->sht[index].sh_offset == shdr.sh_offset && len < 4)
        {
          /* In the rare case where both names have equal lenght, we can optimize a bit */
          data = elfsh_get_raw(file->secthash[ELFSH_SECTION_SHSTRTAB]);
          len = (file->sht[index].sh_name ? strlen(data + file->sht[index].sh_name) : 0);
          if (len == namelen)
            {
              printf("SHT rebuild case 1.1 \n");
              memcpy(data + file->sht[index].sh_name, name, len);
            }
	  
          /* The off variable is used just to clean the code . This case is the common one . */
          else
            {
              printf("SHT rebuild case 1.2 \n");
              off = file->sht[index].sh_name + len + 1;
              memmove(data + file->sht[index].sh_name, 
                      data + off, 
                      file->secthash[ELFSH_SECTION_SHSTRTAB]->shdr->sh_size - off);
              file->sht[index].sh_size -= len + 1;
              elfsh_update_nameidx(file, file->sht[index].sh_name, len + 1);
              file->sht[index].sh_name = elfsh_insert_in_shstrtab(file, name);
            }
          break;
        }
      
      /* The new header defines a section matching the beginning area of an existing section */
      else if (file->sht[index].sh_offset == shdr.sh_offset)
        {
          printf("SHT rebuild case 2 \n");
          XREALLOC(__FILE__, __FUNCTION__, __LINE__,file->sht, 
                   file->sht, (file->hdr->e_shnum + 1) * file->hdr->e_shentsize, -1);
          memmove(file->sht + index + 1, 
                  file->sht + index, 
                  file->hdr->e_shentsize * (file->hdr->e_shnum - index));
	  
          file->sht[index] = shdr;
          if (strcmp(ELFSH_SECTION_NAME_BSS, name))
            {
              file->sht[index + 1].sh_size -= (shdr.sh_size);
              file->sht[index + 1].sh_offset += (shdr.sh_size + 1);
            }
          file->sht[index + 1].sh_addr += (file->sht[index + 1].sh_addr ? shdr.sh_size : 0);
	  
          if (file->hdr->e_shstrndx >= index)
            file->hdr->e_shstrndx++;
          file->secthash[ELFSH_SECTION_SHSTRTAB]->shdr = file->sht + file->hdr->e_shstrndx;
          file->sht[index].sh_name = elfsh_insert_in_shstrtab(file, name);
	  
          XALLOC(__FILE__, __FUNCTION__, __LINE__,sect, sizeof(*sect), -1);
          if (elfsh_add_section(file, sect, index, NULL, ELFSH_SHIFTING_ABSENT) < 0)
            PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
                         "Unable to add section", -1);
	  
          file->hdr->e_shnum++;
          elfsh_update_linkidx(file, index, 1);
          break;
        }
      
      
      /* The new header defines a section matching the end area of an existing section */
      else if (file->sht[index].sh_offset + file->sht[index].sh_size == shdr.sh_offset + shdr.sh_size)
        {
          printf("SHT rebuild case 3 \n");
          XREALLOC(__FILE__, __FUNCTION__, __LINE__,
                   file->sht, file->sht, (file->hdr->e_shnum + 1) * file->hdr->e_shentsize, -1);
          memmove(file->sht + index + 2, file->sht + index + 1, 
                  file->hdr->e_shentsize * (file->hdr->e_shnum - index - 1));
	  
          file->sht[index + 1] = shdr;
          file->sht[index].sh_size -= shdr.sh_size;
	  
          file->hdr->e_shstrndx++;
          file->secthash[ELFSH_SECTION_SHSTRTAB]->shdr = file->sht + file->hdr->e_shstrndx;
          file->sht[index + 1].sh_name = elfsh_insert_in_shstrtab(file, name);
	  
          XALLOC(__FILE__, __FUNCTION__, __LINE__,sect, sizeof(*sect), -1);
          if (elfsh_add_section(file, sect, index, NULL, ELFSH_SHIFTING_ABSENT) < 0)
            PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
                         "Unable to add section", -1);
	  
          file->hdr->e_shnum++;
          elfsh_update_linkidx(file, index + 1, 1);
          break;
        }


      /* The new header defines a section strictly included in an already existing section */
      else if (file->sht[index].sh_offset < shdr.sh_offset && 
               file->sht[index].sh_offset + file->sht[index].sh_size > shdr.sh_offset + shdr.sh_size)
        {
          printf("SHT rebuild case 4 \n");

          XREALLOC(__FILE__, __FUNCTION__, __LINE__,
                   file->sht, file->sht, (file->hdr->e_shnum + 2) * file->hdr->e_shentsize, -1);
          memmove(file->sht + index + 3, 
                  file->sht + index + 1, 
                  file->hdr->e_shentsize * (file->hdr->e_shnum - index - 1));

          file->sht[index + 1] = shdr;
          file->sht[index + 2] = elfsh_create_shdr(0, file->sht[index].sh_type,
                                                   file->sht[index].sh_flags,
                                                   shdr.sh_addr + shdr.sh_size,
                                                   shdr.sh_offset + shdr.sh_size,
                                                   file->sht[index].sh_size - 
                                                   (shdr.sh_offset - file->sht[index].sh_offset) - 
                                                   shdr.sh_size,
                                                   0, 0, 0, 0);
          file->sht[index].sh_size = shdr.sh_offset - file->sht[index].sh_offset;

          file->hdr->e_shstrndx += 2;
          file->secthash[ELFSH_SECTION_SHSTRTAB]->shdr = file->sht + file->hdr->e_shstrndx;

          file->sht[index + 1].sh_name = elfsh_insert_in_shstrtab(file, name);
          snprintf(buff, sizeof(buff), "%s_"UFMT, 
                   (file->sht[index + 2].sh_addr ? 
                    ELFSH_SECTION_NAME_MAPPED : ELFSH_SECTION_NAME_UNMAPPED),
                   file->sht[index + 2].sh_offset);
          file->sht[index + 2].sh_name = elfsh_insert_in_shstrtab(file, buff);

          XALLOC(__FILE__, __FUNCTION__, __LINE__,sect, sizeof(*sect), -1);
          if (elfsh_add_section(file, sect, index, 
                                NULL, ELFSH_SHIFTING_ABSENT) < 0)
            PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
                         "Unable to add section", -1);

          XALLOC(__FILE__, __FUNCTION__, __LINE__,sect, sizeof(*sect), -1);
          if (elfsh_add_section(file, sect, index + 2, 
                                NULL, ELFSH_SHIFTING_ABSENT) < 0)
            PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
                         "Unable to add section", -1);

          file->hdr->e_shnum += 2;
          elfsh_update_linkidx(file, index, 1);
          break;
        }
    }       

  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}

/** 
 * INTERNAL FUNCTION : Rebuild an initial SHT and 
 * create some sections based on the pht entries 
 *
 * @param file
 * @param num
 * @return
 */
static int elfsh_init_sht(elfshobj_t *file, u_int num)
{
  elfsh_Shdr	shdr;
  elfshsect_t	*sect;
  char		name[256];
  unsigned int	index,idx;
  void	  	*data;
  unsigned int	nnames,lnames,dnames;
  unsigned int	tlsnames,ehnames,snames;
  long		type, total, section_offset, base_addr = 0, dyn_addr = 0, dyn_size = 0;
  int		flags;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  /* count how many sections we will have */
  for (total = idx = 0; idx < num; idx++)
    {
      switch (file->pht[idx].p_type) 
        {
	case PT_NOTE:
	case PT_LOAD:
	case PT_DYNAMIC:
	case PT_INTERP:
#if !defined(sgi) && !defined(__NetBSD__)
	case PT_TLS:
	case PT_GNU_EH_FRAME:
          //case PT_GNU_STACK:
#endif
	  total++;
	  break;
        }
    }
  total++;
  file->hdr->e_shnum     = 100;//total+1; /* total, for the shstrtab */
  file->hdr->e_shentsize = sizeof(elfsh_Shdr);

  /* insert the sht at the end of the file */
  file->hdr->e_shoff     = file->fstat.st_size;
  section_offset         = 0;
  
  /* Create the initial SHT */ 
  XALLOC(__FILE__, __FUNCTION__, __LINE__, file->sht, 
	 file->hdr->e_shentsize * (total + 8), -1);

  printf("SHT @ 0x%p\n", file->sht);
  snames = ehnames = tlsnames = nnames = lnames = dnames = 0;
  
  /* rebuild the sht based on the pht entries */
  for(idx = 0, index = 0; index < num; index++)
    {
      /* setup the flags of this section */
      flags = SHF_ALLOC;
      flags |= (elfsh_segment_is_writable(file->pht+index)?SHF_WRITE:0);
      flags |= (elfsh_segment_is_executable(file->pht+index)?SHF_EXECINSTR:0);
      
      switch(file->pht[index].p_type)
        {
	case PT_INTERP:
	  /* .interp */
	  snprintf(name, sizeof(name), ELFSH_SECTION_NAME_INTERP);
	  type = SHT_PROGBITS;
	  break;
	case PT_LOAD:
	  /* XXX: not working */
	  if(file->pht[index].p_memsz < file->pht[index].p_filesz)
	    { /* .bss */
	      snprintf(name, sizeof(name), ELFSH_SECTION_NAME_BSS);
	      type = SHT_NOBITS;
	    }
	  else
	    {
	      if(file->pht[index].p_vaddr >= 0xFFFF0000)
		{ /* used for retrieve information from core files */
		  snprintf(name, sizeof(name), ".kernel_info%d", lnames++);
		  type=SHT_PROGBITS;
		  break;
		}
	      /* .text */
	      if(file->pht[index].p_vaddr >= 0x8000000 && 
		 elfsh_segment_is_executable(file->pht + index)) 
		{
		  snprintf(name, sizeof(name), ELFSH_SECTION_NAME_TEXT);
		  base_addr = file->pht[index].p_vaddr;
		}
	      /* .data */
	      else if(file->pht[index].p_vaddr >= 0x8000000 && 
		      elfsh_segment_is_writable(file->pht + index))
		snprintf(name, sizeof(name), ELFSH_SECTION_NAME_DATA);

	      type = SHT_PROGBITS;
	    }
	  break;
	case PT_NOTE:
	  /* .note */
	  if(file->coretype==ELFSH_CORE_LINUX)
	    {
	      snprintf(name, sizeof(name), ".note%d", nnames++);
	      type = SHT_NOTE;
	    }
	  else
	    {
	      snprintf(name, sizeof(name), ".note-ABI.tag");
	      type=SHT_NOTE;
	    }
	  break;
#if !defined(sgi) && !defined(__NetBSD__)
	case PT_TLS:
	  /* .gnu.tls */
	  snprintf(name, sizeof(name), ".tls%d", tlsnames++);
	  type = SHT_PROGBITS;
	  break;
#endif
	case PT_DYNAMIC:
	  /* .dynamic */
	  snprintf(name, sizeof(name), ELFSH_SECTION_NAME_DYNAMIC);
	  type = SHT_DYNAMIC;
	  break;
#if !defined(sgi)
	case PT_GNU_EH_FRAME:
	  snprintf(name, sizeof(name), ".eh_frame%d", ehnames++);
	  type = SHT_PROGBITS;
	  break;
#endif
	default: 
	  continue;
        }
      
      /* create the new shdr based on the address from the pht entry */
      file->sht[idx] = elfsh_create_shdr(0, type, flags, 
                                         file->pht[index].p_vaddr, 
					 file->pht[index].p_offset,
                                         file->pht[index].p_filesz ? 
					 file->pht[index].p_memsz:0, 0, 0, 
                                         file->pht[index].p_align,
					 sizeof(elfsh_Shdr));
      if(type==SHT_DYNAMIC) 
	{
	  dyn_addr = file->pht[index].p_vaddr;
	  dyn_size = file->pht[index].p_filesz;
	}
      if(file->pht[index].p_filesz) 
	{ 
	  /* duplicate the data */
	  XSEEK(file->fd, file->pht[index].p_offset, SEEK_SET, -1);
	  XALLOC(__FILE__, __FUNCTION__, __LINE__, data, 
		 file->pht[index].p_filesz + 1, -1);
	  XREAD(file->fd, data, file->pht[index].p_filesz, -1);
	} 
      else 
        { 
          data = NULL;
        }
      
      XALLOC(__FILE__, __FUNCTION__, __LINE__, sect, sizeof(elfshsect_t), -1);
      sect->name=strdup(name); 
      printf("IDX[%d]: %s\n", idx, sect->name);
      if (elfsh_add_section(file, sect, idx++, data, ELFSH_SHIFTING_ABSENT)<0)
        PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
                     "Unable to add section", -1);
      
      /* the section_offset must be the biggest offset from all sht entries */
      if(file->pht[index].p_offset)
        if((file->pht[index].p_offset+file->pht[index].p_filesz)>section_offset)
          section_offset = file->pht[index].p_offset + file->pht[index].p_filesz;
    } 

#if 0
  /* SHSTRTAB */
  /* Insert the section header string table (.shstrtab) */
  shdr = elfsh_create_shdr(0, SHT_STRTAB, 0, 0, section_offset, 
                           0, 0, 0, 0, sizeof(elfsh_Shdr));

  file->sht[idx] = shdr;
  
  XALLOC(__FILE__, __FUNCTION__, __LINE__,sect, sizeof(elfshsect_t), -1);
  sect->name = strdup(ELFSH_SECTION_NAME_SHSTRTAB);

  if (elfsh_add_section(file, sect, idx, NULL, ELFSH_SHIFTING_ABSENT) < 0)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
                 "Unable to add section", -1);
  
  file->secthash[ELFSH_SECTION_SHSTRTAB] = sect;

  file->hdr->e_shstrndx = idx;
  file->hdr->e_shnum = idx+1;
    
  section_offset += sizeof(elfsh_Shdr);

  /* insert the names of the sections in the .shstrtab */
  for (idx= 0, sect = file->sectlist; sect; sect=sect->next, idx++) 
    {
      sect->shdr->sh_name = elfsh_insert_in_shstrtab(file, sect->name);
      printf("name[%d][%d]:%s @ 0x%08x\n", idx, sect->shdr->sh_name, 
	     sect->name, sect->shdr->sh_addr);
      section_offset += strlen(sect->name)+1;
    }
#endif
  /* final round for the sht reconstruction 
     extract data from the .dynamic section 

     WORK IN PROGRESS.
  */
  elfsh_Dyn *dyn;
  eresi_Addr sect_addr;
  elfsh_Word sect_size, ent_size;

#if 1
  /* .dynstr */
  if((dyn = elfsh_get_dynamic_entry_by_type(file, DT_STRTAB)) != NULL)
    {
      sect_addr = dyn->d_un.d_ptr;
      dyn = elfsh_get_dynamic_entry_by_type(file, DT_STRSZ);
      sect_size = dyn->d_un.d_val;
      printf("@" XFMT " => SECT SIZE: %x %ld bytes\n", sect_addr - base_addr, sect_size, sect_size);
     
      shdr = elfsh_create_shdr(0, SHT_STRTAB, SHF_ALLOC, sect_addr, 
			       section_offset, sect_size,0,0,0,
			       0);

      //elfsh_insert_shdr(file, shdr, ELFSH_SECTION_LAST, ".dynstr", ELFSH_SHIFTING_ABSENT);
      file->sht[idx] = shdr;

      XALLOC(__FILE__, __FUNCTION__, __LINE__, sect, sizeof(elfshsect_t), -1);
      sect->name = strdup(".dynstr");
      
      XSEEK(file->fd, sect_addr - base_addr, SEEK_SET, -1);
      XALLOC(__FILE__, __FUNCTION__, __LINE__, data, sect_size + 1, -1);
      printf("READ: %d bytes\n", read(file->fd, data, sect_size));

      if(elfsh_add_section(file, sect, idx, data, 
			   ELFSH_SHIFTING_ABSENT)<0)
	PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
		     "Unable to add section", -1);
      
      file->secthash[ELFSH_SECTION_DYNSTR] = sect;
      
      section_offset += sect_size;
      idx++;
    }

#endif

  /* .dynsym */
  if((dyn = elfsh_get_dynamic_entry_by_type(file, DT_SYMTAB)) != NULL)
    {
      sect_addr = dyn->d_un.d_ptr;
      sect_size = elfsh_get_dynamic_entry_by_type(file, DT_STRTAB)->d_un.d_ptr - sect_addr;

      ent_size = elfsh_get_dynamic_entry_by_type(file, DT_SYMENT)->d_un.d_val;
            
      /* sh_link must point to the .dynstr section index */
      /* sh_info is the total number of entries in dynsym plus one */
      shdr = elfsh_create_shdr(0, SHT_DYNSYM, SHF_ALLOC, sect_addr, 
			       section_offset, sect_size, idx-1,
			       (sect_size / dyn->d_un.d_val) + 1, 4,
			       ent_size);

      //elfsh_insert_shdr(file, shdr, ELFSH_SECTION_LAST, ".dynsym", ELFSH_SHIFTING_ABSENT);
      file->sht[idx] = shdr;

      XALLOC(__FILE__, __FUNCTION__, __LINE__, sect, sizeof(elfshsect_t), -1);
      sect->name = strdup(".dynsym");
      
      XSEEK(file->fd, sect_addr - base_addr, SEEK_SET, -1);
      XALLOC(__FILE__, __FUNCTION__, __LINE__, data,
	     ((sect_size / dyn->d_un.d_val) * ent_size)+1, -1);
      XREAD(file->fd, data, (sect_size*6), -1);
      
      if(elfsh_add_section(file, sect, idx, data,ELFSH_SHIFTING_ABSENT)<0)
	PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
		     "Unable to add section", -1);

      //      elfsh_fixup_dynsymtab(sect);

      file->secthash[ELFSH_SECTION_DYNSYM] = sect;
      section_offset += sect_size;
      idx++;
    }
  
  /* .hash */
#if 1
  if((dyn = elfsh_get_dynamic_entry_by_type(file, DT_HASH)))
    {
      elfsh_Word nbucket, nchain;
      sect_addr = dyn->d_un.d_ptr;

      XSEEK(file->fd, sect_addr - base_addr, SEEK_SET, -1);
      XREAD(file->fd, &nbucket, 4, -1);
      XREAD(file->fd, &nchain, 4, -1);

      sect_size = (2 + nbucket + nchain) * sizeof(elfsh_Word);
      shdr = elfsh_create_shdr(0, SHT_HASH, SHF_ALLOC, sect_addr, 
			       section_offset,sect_size,0,0,0,
			       sizeof(elfsh_Word));

      //elfsh_insert_shdr(file, shdr, ELFSH_SECTION_LAST, ".hash", ELFSH_SHIFTING_ABSENT);
      file->sht[idx] = shdr;

      XALLOC(__FILE__, __FUNCTION__, __LINE__, sect, sizeof(elfshsect_t), -1);
      sect->name = strdup(".hash");
      
      XSEEK(file->fd, sect_addr - base_addr, SEEK_SET, -1);
      XALLOC(__FILE__, __FUNCTION__, __LINE__, data, sect_size + 1, -1);
      XREAD(file->fd, data, sect_size, -1);
      
      if(elfsh_add_section(file, sect, idx, data,ELFSH_SHIFTING_ABSENT)<0)
	PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
		     "Unable to add section", -1);
      
      file->secthash[ELFSH_SECTION_HASH] = sect;
      
      section_offset += sect_size;
      idx++;
    }
#endif
#if 1
  /* .rel.dyn */
  if((dyn = elfsh_get_dynamic_entry_by_type(file, DT_REL)) != NULL)
    {
      sect_addr = dyn->d_un.d_ptr;
      sect_size = elfsh_get_dynamic_entry_by_type(file, DT_RELSZ)->d_un.d_val;
      ent_size  = elfsh_get_dynamic_entry_by_type(file, DT_RELENT)->d_un.d_val;
      
      shdr = elfsh_create_shdr(0, SHT_REL, SHF_ALLOC, sect_addr, 
			       section_offset,sect_size,idx-2,1,0,
			       ent_size);

      //elfsh_insert_shdr(file, shdr, ELFSH_SECTION_LAST, ".rel.dyn", ELFSH_SHIFTING_ABSENT);
      file->sht[idx] = shdr;

      XALLOC(__FILE__, __FUNCTION__, __LINE__, sect, sizeof(elfshsect_t), -1);
      sect->name = strdup(".rel.dyn");
      
      XSEEK(file->fd, sect_addr - base_addr, SEEK_SET, -1);
      XALLOC(__FILE__, __FUNCTION__, __LINE__, data, sect_size + 1, -1);
      XREAD(file->fd, data, sect_size, -1);
      
      if(elfsh_add_section(file, sect, idx, data, 
			   ELFSH_SHIFTING_ABSENT)<0)
	PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
		     "Unable to add section", -1);
      
      section_offset += sect_size;
      idx++;
    }
#endif
#if 1
  /* .rel.plt */
  if((dyn = elfsh_get_dynamic_entry_by_type(file, DT_JMPREL)) != NULL)
    {
      sect_addr = dyn->d_un.d_ptr;
      sect_size = elfsh_get_dynamic_entry_by_type(file, DT_PLTRELSZ)->d_un.d_val;
      //ent_size  = elfsh_get_dynamic_entry_by_type(file, DT_RELENT)->d_un.d_val;
      
      shdr = elfsh_create_shdr(0, SHT_REL, SHF_ALLOC, sect_addr, 
			       section_offset,sect_size,idx-3,1,0,
			       sizeof(elfsh_Rel));

      // elfsh_insert_shdr(file, shdr, ELFSH_SECTION_LAST, ".rel.plt", ELFSH_SHIFTING_ABSENT);
      file->sht[idx] = shdr;

      XALLOC(__FILE__, __FUNCTION__, __LINE__, sect, sizeof(elfshsect_t), -1);
      sect->name = strdup(".rel.plt");
      
      XSEEK(file->fd, sect_addr - base_addr, SEEK_SET, -1);
      XALLOC(__FILE__, __FUNCTION__, __LINE__, data, sect_size + 1, -1);
      XREAD(file->fd, data, sect_size, -1);
      
      if(elfsh_add_section(file, sect, idx, data, 
			   ELFSH_SHIFTING_ABSENT)<0)
	PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
		     "Unable to add section", -1);
      
      section_offset += sect_size;
      idx++;
    }
#endif
#if 1
  /* .got */
  if((dyn = elfsh_get_dynamic_entry_by_type(file, DT_PLTGOT)) != NULL)
    {
      sect_addr = dyn->d_un.d_ptr;
      sect_size = sect_addr - (dyn_addr + dyn_size); //elfsh_get_dynamic_entry_by_type(file, DT_PLTRELSZ)->d_un.d_val;
      //ent_size  = elfsh_get_dynamic_entry_by_type(file, DT_RELENT)->d_un.d_val;
      sect_addr -= sect_size;
      
      shdr = elfsh_create_shdr(0, SHT_PROGBITS, SHF_ALLOC|SHF_WRITE, sect_addr, 
			       section_offset,sect_size,0,0,4,
			       4);

      // elfsh_insert_shdr(file, shdr, ELFSH_SECTION_LAST, ".rel.plt", ELFSH_SHIFTING_ABSENT);
      file->sht[idx] = shdr;

      XALLOC(__FILE__, __FUNCTION__, __LINE__, sect, sizeof(elfshsect_t), -1);
      sect->name = strdup(".got");
      
      printf("OFFSET: " AFMT " (size: " DFMT ")\n", sect_addr, (dyn_size));
      XSEEK(file->fd, sect_addr - (base_addr + 0x1000), SEEK_SET, -1);
      XALLOC(__FILE__, __FUNCTION__, __LINE__, data, sect_size + 1, -1);
      XREAD(file->fd, data, sect_size, -1);
      
      if(elfsh_add_section(file, sect, idx, data, 
			   ELFSH_SHIFTING_ABSENT)<0)
	PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
		     "Unable to add section", -1);
      
      section_offset += sect_size;
      idx++;
    }
#endif
#if 1
  /* .init */
  if((dyn = elfsh_get_dynamic_entry_by_type(file, DT_INIT)) != NULL)
    {
      asm_instr instr;
      asm_processor proc;
      u_char temp[1024];
      //int curr;

      sect_addr = dyn->d_un.d_ptr;

      switch(elfsh_get_arch(file->hdr))
	{
	case EM_SPARC32PLUS:
	case EM_SPARC:
	case EM_SPARCV9:
	  asm_init_arch(&proc, ASM_PROC_SPARC);
	  break;
	case EM_386:
	  asm_init_arch(&proc, ASM_PROC_IA32);
	  break;
	case EM_MIPS:
	case EM_MIPS_RS3_LE:
	case EM_MIPS_X:
	  asm_init_arch(&proc, ASM_PROC_MIPS);
	  break;
	}
      
      XSEEK(file->fd, sect_addr - base_addr, SEEK_SET, -1);
      XREAD(file->fd, temp, 1024, -1);
      
      //elfsh_read_raw(file, sect_addr - base_addr, data, 1024);

      sect_size = 0;
      while(sect_size < 1024)
	{
	  if(asm_read_instr(&instr, temp + sect_size, 1024 - sect_size, &proc) > 0)
	    {
	      if(instr.instr==ASM_RET)
		{
		  //		  printf("RET RET RET: %d\n", sect_size);
		  sect_size += asm_instr_len(&instr);
		  break;
		}
	    }
	  sect_size += asm_instr_len(&instr);
	}

      shdr = elfsh_create_shdr(0, SHT_PROGBITS, SHF_ALLOC|SHF_EXECINSTR, sect_addr, 
			       section_offset,sect_size,0,0,4, 4);

      // elfsh_insert_shdr(file, shdr, ELFSH_SECTION_LAST, ".rel.plt", ELFSH_SHIFTING_ABSENT);
      file->sht[idx] = shdr;

      XALLOC(__FILE__, __FUNCTION__, __LINE__, sect, sizeof(elfshsect_t), -1);
      sect->name = strdup(".init");
      
      XSEEK(file->fd, sect_addr - base_addr, SEEK_SET, -1);
      XALLOC(__FILE__, __FUNCTION__, __LINE__, data, sect_size + 1, -1);
      XREAD(file->fd, data, sect_size, -1);
      
      if(elfsh_add_section(file, sect, idx, data, 
			   ELFSH_SHIFTING_ABSENT)<0)
	PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
		     "Unable to add section", -1);
      
      section_offset += sect_size;
      idx++;
    }
#endif
#if 1
  /* .fini */
  if((dyn = elfsh_get_dynamic_entry_by_type(file, DT_FINI)) != NULL)
    {
      asm_instr instr;
      asm_processor proc;
      u_char temp[1024];

      sect_addr = dyn->d_un.d_ptr;

      switch(elfsh_get_arch(file->hdr))
	{
	case EM_SPARC32PLUS:
	case EM_SPARC:
	case EM_SPARCV9:
	  asm_init_arch(&proc, ASM_PROC_SPARC);
	  break;
	case EM_386:
	  asm_init_arch(&proc, ASM_PROC_IA32);
	  break;
	case EM_MIPS:
	  asm_init_arch(&proc, ASM_PROC_MIPS);
	  break;
	}
      
      XSEEK(file->fd, sect_addr - base_addr, SEEK_SET, -1);
      XREAD(file->fd, temp, 1024, -1);
      
      sect_size = 0;
      while(sect_size < 1024)
	{
	  if(asm_read_instr(&instr, temp + sect_size, 1024 - sect_size, &proc) > 0)
	    {
	      if(instr.instr==ASM_RET)
		{
		  sect_size += asm_instr_len(&instr);
		  break;
		}
	    }
	  sect_size += asm_instr_len(&instr);
	}

      shdr = elfsh_create_shdr(0, SHT_PROGBITS, SHF_ALLOC|SHF_EXECINSTR, sect_addr, 
			       section_offset,sect_size,0,0,4, 4);

      // elfsh_insert_shdr(file, shdr, ELFSH_SECTION_LAST, ".rel.plt", ELFSH_SHIFTING_ABSENT);
      file->sht[idx] = shdr;

      XALLOC(__FILE__, __FUNCTION__, __LINE__, sect, sizeof(elfshsect_t), -1);
      sect->name = strdup(".fini");
      
      XSEEK(file->fd, sect_addr - base_addr, SEEK_SET, -1);
      XALLOC(__FILE__, __FUNCTION__, __LINE__, data, sect_size + 1, -1);
      XREAD(file->fd, data, sect_size, -1);
      
      if(elfsh_add_section(file, sect, idx, data, 
			   ELFSH_SHIFTING_ABSENT)<0)
	PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
		     "Unable to add section", -1);
      
      section_offset += sect_size;
      idx++;
    }
#endif

#if 1
  /* SHSTRTAB */
  /* Insert the section header string table (.shstrtab) */
  shdr = elfsh_create_shdr(0, SHT_STRTAB, 0, 0, section_offset, 
                           0, 0, 0, 0, sizeof(elfsh_Shdr));

  file->sht[idx] = shdr;
  
  XALLOC(__FILE__, __FUNCTION__, __LINE__,sect, sizeof(elfshsect_t), -1);
  sect->name = strdup(ELFSH_SECTION_NAME_SHSTRTAB);

  if (elfsh_add_section(file, sect, idx, NULL, ELFSH_SHIFTING_ABSENT) < 0)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
                 "Unable to add section", -1);
  
  file->secthash[ELFSH_SECTION_SHSTRTAB] = sect;

  file->hdr->e_shstrndx = idx;
  file->hdr->e_shnum = idx+1;
    
  section_offset += sizeof(elfsh_Shdr);

  /* insert the names of the sections in the .shstrtab */
  for (idx= 0, sect = file->sectlist; sect; sect=sect->next, idx++) 
    {
      sect->shdr->sh_name = elfsh_insert_in_shstrtab(file, sect->name);
      printf("name[%d][%d]:%s @ 0x%08x\n", idx, sect->shdr->sh_name, 
	     sect->name, sect->shdr->sh_addr);
      section_offset += strlen(sect->name) + 1;
    }
#endif
  //elfsh_sort_sht(file);

  /* sync the sht entries and name */
  elfsh_sync_sectnames(file); 
  elfsh_sync_sht(file); 

  //  elfsh_fixup_symtab(file, &total);

  /*  elfsh_Sym newent;
  int symsz;
  elfshsect_t *symtab = elfsh_get_symtab(file, &symsz);

  for(idx = 0; idx != (file->hdr->e_phnum * file->hdr->e_phentsize); idx++)
    {
      if(file->pht[idx].p_type == PT_DYNAMIC)
	{
	  newent = elfsh_create_symbol(file->pht[idx].p_vaddr,
				       0, STT_OBJECT,0,0,symsz);
	  elfsh_insert_symbol(symtab, &newent, "_DYNAMIC");
	}
	}*/
    PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}

#if 0
/**
 * First round : Read PHT and guess .bss, .interp, .dynamic and .notes bounds 
 */
static void	sht_first_round(elfshobj_t *file, u_int num)
{
  /*
    elfsh_Phdr	*dyn;
    elfsh_Shdr	shdr;
  */
  u_int		index;
  int		flags;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  for (index = 0; index < num; index++)
    {
      flags = SHF_ALLOC;
      flags |= (elfsh_segment_is_writable(file->pht + index)    ? SHF_WRITE     : 0);
      flags |= (elfsh_segment_is_executable(file->pht + index)  ? SHF_EXECINSTR : 0);
      switch (file->pht[index].p_type)
        {

          /* The dynamic segment */

	case PT_DYNAMIC:
	  dyn = file->pht + index;
	  shdr = elfsh_create_section_header(0, SHT_DYNAMIC, flags , file->pht[index].p_vaddr,
					     file->pht[index].p_offset, file->pht[index].p_filesz, 
					     0, 0, 0, 0);
	  elfsh_merge_shtentry(file, index, shdr, ELFSH_SECTION_NAME_DYNAMIC);
	  break;


	  /* The interpretor segment */

	case PT_INTERP:
	  shdr = elfsh_create_section_header(0, SHT_PROGBITS, flags, file->pht[index].p_vaddr,
					     file->pht[index].p_offset, file->pht[index].p_filesz, 
					     0, 0, 0, 0);
	  elfsh_merge_shtentry(file, index, shdr, ELFSH_SECTION_NAME_INTERP);
	  break;


	  /* This is the auxiliary information */

	case PT_NOTE:
	  shdr = elfsh_create_section_header(0, SHT_NOTE, flags, file->pht[index].p_vaddr,
					     file->pht[index].p_offset, file->pht[index].p_filesz, 
					     0, 0, 0, 0);
	  elfsh_merge_shtentry(file, index, shdr, ELFSH_SECTION_NAME_NOTES_ABI);
	  break;


	  /* Rebuild the .bss */

	case PT_LOAD:
	  if (file->pht[index].p_memsz != file->pht[index].p_filesz)
	    {
	      shdr = elfsh_create_section_header(0, SHT_NOBITS, flags, 
						 file->pht[index].p_vaddr + file->pht[index].p_filesz, 
						 file->pht[index].p_offset + file->pht[index].p_filesz,
						 file->pht[index].p_memsz - file->pht[index].p_filesz,
						 0, 0, 0, 0);
	      elfsh_merge_shtentry(file, index, shdr, ELFSH_SECTION_NAME_BSS);
	    }
	  break;


	default:
	  break;
        }
    }    

  PROFILER_OUT(__FILE__, __FUNCTION__, __LINE__);
}


/**
 * INTERNAL FUNCTION: guess .text, .data, .init, .fini, and .bss bounds 
 *
 * @param file
 * @param num
 * @return
 */
static void	sht_second_round(elfshobj_t *file, u_int num)
{
  /* elfsh_Phdr	*dyn; */
  elfsh_Shdr	shdr;
  u_int		index;
  int		flags;
  char		*name;
  char		i;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  /* Do not remove if first_round() is entirely working with section paddings */
  PROFILER_OUT(__FILE__, __FUNCTION__, __LINE__);

  for (index = 0; index < num; index++)
    {
      flags = SHF_ALLOC;
      flags |= (elfsh_segment_is_writable(file->pht + index)  ? SHF_WRITE     : 0);
      flags |= (elfsh_segment_is_executable(file->pht + index)? SHF_EXECINSTR : 0);
      switch (file->pht[index].p_type)
        {
	case PT_LOAD:
	  shdr = elfsh_create_shdr(0, SHT_PROGBITS, flags, 
				   file->pht[index].p_vaddr, 
				   file->pht[index].p_offset, 
				   file->pht[index].p_filesz, 
				   0, 0, 0, 0);
	  i = INTERVAL(file->pht[index].p_vaddr, file->hdr->e_entry, 
		       file->pht[index].p_vaddr + file->pht[index].p_filesz);
	  name = (i ? ELFSH_SECTION_NAME_TEXT : ELFSH_SECTION_NAME_DATA);
	  printf("MERGIN SHDR second round!\n");
	  elfsh_merge_shtentry(file, index, shdr, name);
	default:
	  break;
        }
    }
}
#endif

/**
 * Recreate the section header table examining the program header table 
 *
 * @param file
 * @return
 */
int		elfsh_rebuild_sht(elfshobj_t *file)
{
  int		num;

  PROFILER_IN(__FILE__, __FUNCTION__, __LINE__);

  /* Some preliminary stuffs */
  if (!elfsh_get_pht(file, &num) || elfsh_init_sht(file, num) < 0)
    PROFILER_ERR(__FILE__, __FUNCTION__, __LINE__,
                 "Unable to get PHT or to init SHT", -1);

  //  sht_first_round(file, num);
  //  sht_second_round(file, num);
 
  file->shtrb = 1;
  PROFILER_ROUT(__FILE__, __FUNCTION__, __LINE__, 0);
}
