#ifndef MEMTEST_PCI_H
 #define MEMTEST_PCI_H

#define PCI_CONF_TYPE_NONE 0
#define PCI_CONF_TYPE_1    1
#define PCI_CONF_TYPE_2    2

#define PCI_CONF1_ADDRESS(bus, dev, fn, reg) (0x80000000 | (bus << 16) | (dev << 11) | (fn << 8) | (reg & ~3))
#define PCI_CONF2_ADDRESS(dev, reg)	     (unsigned short)(0xC000 | (dev << 8) | reg)


int pci_conf_read(unsigned bus, unsigned dev, unsigned fn, unsigned reg, 
		  unsigned len, unsigned int *value);
int pci_conf_write(unsigned bus, unsigned dev, unsigned fn, unsigned reg, 
		   unsigned len, unsigned int value);
int pci_init(void);

/*
 * Under PCI, each device has 256 bytes of configuration address space,
 * of which the first 64 bytes are standardized as follows:
 */
#define PCI_VENDOR_ID		0x00	/* 16 bits */
#define PCI_DEVICE_ID		0x02	/* 16 bits */
#define PCI_COMMAND		0x04	/* 16 bits */
#define  PCI_COMMAND_IO		0x1	/* Enable response in I/O space */
#define  PCI_COMMAND_MEMORY	0x2	/* Enable response in Memory space */
#define  PCI_COMMAND_MASTER	0x4	/* Enable bus mastering */
#define  PCI_COMMAND_SPECIAL	0x8	/* Enable response to special cycles */
#define  PCI_COMMAND_INVALIDATE	0x10	/* Use memory write and invalidate */
#define  PCI_COMMAND_VGA_PALETTE 0x20	/* Enable palette snooping */
#define  PCI_COMMAND_PARITY	0x40	/* Enable parity checking */
#define  PCI_COMMAND_WAIT 	0x80	/* Enable address/data stepping */
#define  PCI_COMMAND_SERR	0x100	/* Enable SERR */
#define  PCI_COMMAND_FAST_BACK	0x200	/* Enable back-to-back writes */

#define PCI_STATUS		0x06	/* 16 bits */
#define  PCI_STATUS_CAP_LIST	0x10	/* Support Capability List */
#define  PCI_STATUS_66MHZ	0x20	/* Support 66 Mhz PCI 2.1 bus */
#define  PCI_STATUS_UDF		0x40	/* Support User Definable Features [obsolete] */
#define  PCI_STATUS_FAST_BACK	0x80	/* Accept fast-back to back */
#define  PCI_STATUS_PARITY	0x100	/* Detected parity error */
#define  PCI_STATUS_DEVSEL_MASK	0x600	/* DEVSEL timing */
#define  PCI_STATUS_DEVSEL_FAST	0x000	
#define  PCI_STATUS_DEVSEL_MEDIUM 0x200
#define  PCI_STATUS_DEVSEL_SLOW 0x400
#define  PCI_STATUS_SIG_TARGET_ABORT 0x800 /* Set on target abort */
#define  PCI_STATUS_REC_TARGET_ABORT 0x1000 /* Master ack of " */
#define  PCI_STATUS_REC_MASTER_ABORT 0x2000 /* Set on master abort */
#define  PCI_STATUS_SIG_SYSTEM_ERROR 0x4000 /* Set when we drive SERR */
#define  PCI_STATUS_DETECTED_PARITY 0x8000 /* Set on parity error */

#define PCI_CLASS_REVISION	0x08	/* High 24 bits are class, low 8
					   revision */
#define PCI_REVISION_ID         0x08    /* Revision ID */
#define PCI_CLASS_PROG          0x09    /* Reg. Level Programming Interface */
#define PCI_CLASS_DEVICE        0x0a    /* Device class */

#define PCI_CACHE_LINE_SIZE	0x0c	/* 8 bits */
#define PCI_LATENCY_TIMER	0x0d	/* 8 bits */
#define PCI_HEADER_TYPE		0x0e	/* 8 bits */
#define  PCI_HEADER_TYPE_NORMAL	0
#define  PCI_HEADER_TYPE_BRIDGE 1
#define  PCI_HEADER_TYPE_CARDBUS 2

#define PCI_BIST		0x0f	/* 8 bits */
#define PCI_BIST_CODE_MASK	0x0f	/* Return result */
#define PCI_BIST_START		0x40	/* 1 to start BIST, 2 secs or less */
#define PCI_BIST_CAPABLE	0x80	/* 1 if BIST capable */

/*
 * Base addresses specify locations in memory or I/O space.
 * Decoded size can be determined by writing a value of 
 * 0xffffffff to the register, and reading it back.  Only 
 * 1 bits are decoded.
 */
#define PCI_BASE_ADDRESS_0	0x10	/* 32 bits */
#define PCI_BASE_ADDRESS_1	0x14	/* 32 bits [htype 0,1 only] */
#define PCI_BASE_ADDRESS_2	0x18	/* 32 bits [htype 0 only] */
#define PCI_BASE_ADDRESS_3	0x1c	/* 32 bits */
#define PCI_BASE_ADDRESS_4	0x20	/* 32 bits */
#define PCI_BASE_ADDRESS_5	0x24	/* 32 bits */
#define  PCI_BASE_ADDRESS_SPACE	0x01	/* 0 = memory, 1 = I/O */
#define  PCI_BASE_ADDRESS_SPACE_IO 0x01
#define  PCI_BASE_ADDRESS_SPACE_MEMORY 0x00
#define  PCI_BASE_ADDRESS_MEM_TYPE_MASK 0x06
#define  PCI_BASE_ADDRESS_MEM_TYPE_32	0x00	/* 32 bit address */
#define  PCI_BASE_ADDRESS_MEM_TYPE_1M	0x02	/* Below 1M [obsolete] */
#define  PCI_BASE_ADDRESS_MEM_TYPE_64	0x04	/* 64 bit address */
#define  PCI_BASE_ADDRESS_MEM_PREFETCH	0x08	/* prefetchable? */
#define  PCI_BASE_ADDRESS_MEM_MASK	(~0x0fUL)
#define  PCI_BASE_ADDRESS_IO_MASK	(~0x03UL)
/* bit 1 is reserved if address_space = 1 */


/* Device classes and subclasses */
#define PCI_CLASS_NOT_DEFINED		0x0000
#define PCI_CLASS_NOT_DEFINED_VGA	0x0001

#define PCI_BASE_CLASS_BRIDGE		0x06
#define PCI_CLASS_BRIDGE_HOST		0x0600

#endif /* MEMTEST_PCI_H */

#ifndef uchar
#define uchar unsigned char
#endif
#ifndef ushort
#define ushort unsigned short
#endif
#ifndef ulong
#define ulong unsigned int
#endif


/*MOI MOI*/
typedef struct pci_info {
    ushort  vendor_id;              /* vendor id */
    ushort  device_id;              /* device id */
    ushort  command;                    /* bus number */
    ushort  status;                 /* device number on bus */
    uchar   revision;               /* revision id */
    uchar   class_api;              /* specific register interface type */
    uchar   class_sub;              /* specific device function */
    uchar   class_base;             /* device type (display vs network, etc) */
    uchar   line_size;              /* cache line size in 32 bit words */
    uchar   latency;                /* latency timer */
    uchar   header_type;            /* header type */
    uchar   bist;                   /* built-in self-test */


    union {
        struct {
	  //0x10
            ulong   base_registers[6];      /* base registers, viewed from host */
	    ulong   cardbus_cis;            /* CardBus CIS pointer */
	    

            ushort  subsystem_vendor_id;    /* subsystem (add-in card) vendor id */
            ushort  subsystem_id;           /* subsystem (add-in card) id */
	  //0x30
	    ulong   rom_base;               /* rom base address, viewed from host */
            ulong   rom_base_pci;           /* rom base addr, viewed from pci */
            ulong   rom_size;               /* rom size */

	  //0x3c
            uchar   interrupt_line;         /* interrupt line */
            uchar   interrupt_pin;          /* interrupt pin */

	  //0x3e
	    uchar   min_grant;              /* burst period @ 33 Mhz */
            uchar   max_latency;            /* how often PCI access needed */


        } h0;
        struct {
	  //0x10
            ulong   base_registers[2];      /* base registers, viewed from host */

            ulong   base_registers_pci[2];  /* base registers, viewed from pci */
            ulong   base_register_sizes[2]; /* size of what base regs point to */
            uchar   base_register_flags[2]; /* flags from base address fields */

            uchar   primary_bus;
            uchar   secondary_bus;
            uchar   subordinate_bus;
            uchar   secondary_latency;
            uchar   io_base;
            uchar   io_limit;
            ushort  secondary_status;
            ushort  memory_base;
            ushort  memory_limit;
            ushort  prefetchable_memory_base;
            ushort  prefetchable_memory_limit;
            ulong   prefetchable_memory_base_upper32;
            ulong   prefetchable_memory_limit_upper32;
	  //0x30
            ushort  io_base_upper16;
            ushort  io_limit_upper16;
            ulong   rom_base;               /* rom base address, viewed from host */
	    ulong   rom_base_pci;           /* rom base addr, viewed from pci */
	    
	  //0x3c
	    uchar   interrupt_line;         /* interrupt line */
            uchar   interrupt_pin;          /* interrupt pin */
            ushort  bridge_control;     
        } h1; 
    } u;
} pci_info;



typedef struct _pci_info {
    ushort  vendor_id;              /* vendor id */
    ushort  device_id;              /* device id */
    uchar   bus;                    /* bus number */
    uchar   device;                 /* device number on bus */
    uchar   function;               /* function number in device */
    uchar   revision;               /* revision id */
    uchar   class_api;              /* specific register interface type */
    uchar   class_sub;              /* specific device function */
    uchar   class_base;             /* device type (display vs network, etc) */
    uchar   line_size;              /* cache line size in 32 bit words */
    uchar   latency;                /* latency timer */
    uchar   header_type;            /* header type */
    uchar   bist;                   /* built-in self-test */
    uchar   reserved;               /* filler, for alignment */
    union {
        struct {
            ulong   cardbus_cis;            /* CardBus CIS pointer */
            ushort  subsystem_id;           /* subsystem (add-in card) id */
            ushort  subsystem_vendor_id;    /* subsystem (add-in card) vendor id */
            ulong   rom_base;               /* rom base address, viewed from host */
            ulong   rom_base_pci;           /* rom base addr, viewed from pci */
            ulong   rom_size;               /* rom size */
            ulong   base_registers[6];      /* base registers, viewed from host */
            ulong   base_registers_pci[6];  /* base registers, viewed from pci */
            ulong   base_register_sizes[6]; /* size of what base regs point to */
            uchar   base_register_flags[6]; /* flags from base address fields */
            uchar   interrupt_line;         /* interrupt line */
            uchar   interrupt_pin;          /* interrupt pin */
            uchar   min_grant;              /* burst period @ 33 Mhz */
            uchar   max_latency;            /* how often PCI access needed */
        } _h0;
        struct {
            ulong   base_registers[2];      /* base registers, viewed from host */
            ulong   base_registers_pci[2];  /* base registers, viewed from pci */
            ulong   base_register_sizes[2]; /* size of what base regs point to */
            uchar   base_register_flags[2]; /* flags from base address fields */
            uchar   primary_bus;
            uchar   secondary_bus;
            uchar   subordinate_bus;
            uchar   secondary_latency;
            uchar   io_base;
            uchar   io_limit;
            ushort  secondary_status;
            ushort  memory_base;
            ushort  memory_limit;
            ushort  prefetchable_memory_base;
            ushort  prefetchable_memory_limit;
            ulong   prefetchable_memory_base_upper32;
            ulong   prefetchable_memory_limit_upper32;
            ushort  io_base_upper16;
            ushort  io_limit_upper16;
            ulong   rom_base;               /* rom base address, viewed from host */
            ulong   rom_base_pci;           /* rom base addr, viewed from pci */
            uchar   interrupt_line;         /* interrupt line */
            uchar   interrupt_pin;          /* interrupt pin */
            ushort  bridge_control;     
        } _h1; 
    } _u;
} _pci_info;




/* ---
    offsets in PCI configuration space to the elements of the predefined
    header common to all header types
--- */
 
#define PCI_vendor_id           0x00        /* (2 byte) vendor id */
#define PCI_device_id           0x02        /* (2 byte) device id */
#define PCI_command             0x04        /* (2 byte) command */
#define PCI_status              0x06        /* (2 byte) status */
#define PCI_revision            0x08        /* (1 byte) revision id */
#define PCI_class_api           0x09        /* (1 byte) specific register interface type */
#define PCI_class_sub           0x0a        /* (1 byte) specific device function */
#define PCI_class_base          0x0b        /* (1 byte) device type (display vs network, etc) */
#define PCI_line_size           0x0c        /* (1 byte) cache line size in 32 bit words */
#define PCI_latency             0x0d        /* (1 byte) latency timer */
#define PCI_header_type         0x0e        /* (1 byte) header type */
#define PCI_bist                0x0f        /* (1 byte) built-in self-test */



/* ---
    offsets in PCI configuration space to the elements of the predefined
    header common to header types 0x00 and 0x01
--- */
#define PCI_base_registers      0x10        /* base registers (size varies) */
#define PCI_interrupt_line      0x3c        /* (1 byte) interrupt line */
#define PCI_interrupt_pin       0x3d        /* (1 byte) interrupt pin */




/* ---
    offsets in PCI configuration space to the elements of header type 0x00
--- */

#define PCI_cardbus_cis         0x28        /* (4 bytes) CardBus CIS (Card Information Structure) pointer (see PCMCIA v2.10) */
#define PCI_subsystem_vendor_id 0x2c        /* (2 bytes) subsystem (add-in card) vendor id */
#define PCI_subsystem_id        0x2e        /* (2 bytes) subsystem (add-in card) id */
#define PCI_rom_base            0x30        /* (4 bytes) expansion rom base address */
#define PCI_min_grant           0x3e        /* (1 byte) burst period @ 33 Mhz */
#define PCI_max_latency         0x3f        /* (1 byte) how often PCI access needed */


/* ---
    offsets in PCI configuration space to the elements of header type 0x01 (PCI-to-PCI bridge)
--- */

#define PCI_primary_bus                             0x18
#define PCI_secondary_bus                           0x19
#define PCI_subordinate_bus                         0x1A
#define PCI_secondary_latency                       0x1B    
#define PCI_io_base                                 0x1C
#define PCI_io_limit                                0x1D
#define PCI_secondary_status                        0x1E
#define PCI_memory_base                             0x20
#define PCI_memory_limit                            0x22
#define PCI_prefetchable_memory_base                0x24
#define PCI_prefetchable_memory_limit               0x26
#define PCI_prefetchable_memory_base_upper32        0x28
#define PCI_prefetchable_memory_limit_upper32       0x2C
#define PCI_io_base_upper16                         0x30
#define PCI_io_limit_upper16                        0x32
#define PCI_bridge_rom_base                         0x38
#define PCI_bridge_control                          0x3E                  



/* ---
    values for the class_base field in the common header
--- */

#define PCI_early                   0x00    /* built before class codes defined */
#define PCI_mass_storage            0x01    /* mass storage_controller */
#define PCI_network                 0x02    /* network controller */
#define PCI_display                 0x03    /* display controller */
#define PCI_multimedia              0x04    /* multimedia device */
#define PCI_memory                  0x05    /* memory controller */
#define PCI_bridge                  0x06    /* bridge controller */
#define PCI_simple_communications   0x07    /* simple communications controller */
#define PCI_base_peripheral         0x08    /* base system peripherals */
#define PCI_input                   0x09    /* input devices */
#define PCI_docking_station         0x0a    /* docking stations */
#define PCI_processor               0x0b    /* processors */
#define PCI_serial_bus              0x0c    /* serial_bus_controller */

#define PCI_undefined               0xFF    /* not in any defined class */



struct pci_desc 
{
  unsigned int id;
  unsigned char name[130];
  struct pci_desc* list;
};

typedef struct pci_desc pci_desc;


