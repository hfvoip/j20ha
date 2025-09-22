/**
 * @file sections.ld
 * @brief General linker control file.
 * @copyright @parblock
 * Copyright (c) 2023 Semiconductor Components Industries, LLC (d/b/a
 * onsemi), All Rights Reserved
 *
 * This code is the property of onsemi and may not be redistributed
 * in any form without prior written permission from onsemi.
 * The terms of use and warranty for this code are covered by contractual
 * agreements between onsemi and the licensee.
 *
 * This is Reusable Code.
 * @endparblock
 * @cond NO_DOXYGEN
 */

/* Total CSS DRAM in the system is split into two sections:
 *  Stack:  The stack area is defined here so that if it over-runs then it will
 *          cause a hard fault.
 *  Usable: The rest of the RAM is available for normal use with statics and
 *          heap.
 */ 

/** @brief DRAM ROM status reserved */
_DRAM_ROMStatus_Reserved_size = 0x10;

/** @brief DRAM base address */ 
_DRAM_Total_Base = 0x2000C000 + _DRAM_ROMStatus_Reserved_size;

/** @brief DRAM total size is defined as 144K */ 
_DRAM_Total_Size = 0x24000 - _DRAM_ROMStatus_Reserved_size;

/** @brief Wakeup address is at the end of DRAM minus 32 */
_DRAM_Wakeup_Reserved_Size = 0x20;

/** @brief CPU special registers back-up size */
_DRAM_Core_Reserved_Size = 0x40;

/** @brief DRAM base address */ 
_DRAM_Total_Top = _DRAM_Total_Base + _DRAM_Total_Size;

/** @brief Available DRAM size is the total minus that reserved for wakeup data */
_DRAM_Available_Size = _DRAM_Total_Size - _DRAM_Wakeup_Reserved_Size - _DRAM_Core_Reserved_Size;

/** @brief Define the stack size as a constant 3K here */
_DRAM_Stack_Size = 3072;

/** @brief Define the stack area to be at the top of RAM */
_DRAM_Stack_Base = _DRAM_Total_Top - _DRAM_Stack_Size - _DRAM_Wakeup_Reserved_Size - _DRAM_Core_Reserved_Size;

_DRAM_Base = _DRAM_Total_Base;

/**@brief The DRAM size will be defined as available minus stack */
_DRAM_Size = _DRAM_Available_Size - _DRAM_Stack_Size;

/** @brief The MRAM Base address */
_MRAM_Base = 0x00200000;

/** @brief The MRAM Top address */
_MRAM_Top  = 0x003FFFFF;

/** Define the symbols required to place the application correctly for loading */
_BL_Recovery_KB                 = 7;
_BL_Storage_RSVD_KB             = 1;
_BL_Bootloader_KB               = 20;
_MRAM_Bond_List_KB              = 4;
_MRAM_RSVD_KB                   = 4;
_BL_Application_KB              = 2048 - _BL_Recovery_KB - (_BL_Bootloader_KB * 2) - _MRAM_Bond_List_KB - _MRAM_RSVD_KB;

_BL_Recovery_Size               = 1024 * _BL_Recovery_KB;
_BL_Bootloader_Size             = 1024 * _BL_Bootloader_KB;
_BL_Storage_RSVD_Size           = 1024 * _BL_Storage_RSVD_KB;
_BL_Storage_RSVD_Inode_Size     = 296;
_BL_Applicaton_Size             = 1024 * _BL_Application_KB;
_MRAM_Bond_List_Size            = 1024 * _MRAM_Bond_List_KB;
_MRAM_RSVD_Size                 = 1024 * _MRAM_RSVD_KB;

_MRAM_Bond_List_Address         = _MRAM_Top - _MRAM_Bond_List_Size - _MRAM_RSVD_Size + 1;
_MRAM_RSVD_Address              = _MRAM_Top - _MRAM_RSVD_Size + 1;
_BL_Recovery_Address            = _MRAM_Base;
_BL_Storage_RSVD_Address        = _BL_Recovery_Address + _BL_Recovery_Size;
_BL_Storage_RSVD_Data_Address   = _BL_Storage_RSVD_Address + _BL_Storage_RSVD_Inode_Size;
_BL_Bootloader_Address          = _BL_Storage_RSVD_Address + _BL_Storage_RSVD_Size;
_BL_Bootloader_Download_Address = _BL_Bootloader_Address + _BL_Bootloader_Size;
_BL_Application_Address         = _BL_Bootloader_Download_Address + _BL_Bootloader_Size;

/*
 * Define the memory map
 *
 * Note that this uses the stack definitions from startup.S to control the
 * layout of the sections as described above.
 */
MEMORY
{
    /* The ROM is defined to be the bottom 24K of memory */
    ROM  (r)            : ORIGIN = 0x00000000, LENGTH = 18K   

    /* Define the bootloader MRAM as the bottom section of MRAM */
    BL_Recovery_MRAM (xrw)      : ORIGIN = _BL_Recovery_Address, LENGTH = _BL_Recovery_Size

    /* Define the info storage as the bottom section of MRAM */
    BL_STORAGE_RSVD_MRAM (xrw) : ORIGIN = _BL_Storage_RSVD_Address, LENGTH = _BL_Storage_RSVD_Size

    /* Define the bootloader MRAM section of MRAM */
    BL_MRAM (xrw)      : ORIGIN = _BL_Bootloader_Address, LENGTH = _BL_Bootloader_Size

    /* Define the Application section of MRAM */
    MRAM (xrw)         : ORIGIN = _BL_Application_Address, LENGTH = _BL_Applicaton_Size

    /* Reserve 4k for Bluetooth bond information */
    MRAM_BOND_RSVD (xrw)   : ORIGIN = _MRAM_Bond_List_Address, LENGTH = _MRAM_Bond_List_Size

    /* The ROM requires the last 4K of data MRAM for DEU transfer space */
    MRAM_RSVD (xrw)    : ORIGIN = _MRAM_RSVD_Address, LENGTH = _MRAM_RSVD_Size

    /* Application available DRAM */
    DRAM (xrw)          : ORIGIN = _DRAM_Base, LENGTH = _DRAM_Size

    /* The stack */
    DRAM_STACK (xrw)    : ORIGIN = _DRAM_Stack_Base, LENGTH = _DRAM_Stack_Size
    
    /* Reserved area for CPU registers */
    DRAM_CORE_RSVD (xrw)    : ORIGIN = (_DRAM_Total_Top - _DRAM_Wakeup_Reserved_Size - _DRAM_Core_Reserved_Size), LENGTH = _DRAM_Core_Reserved_Size
 
    /* Reserved area for wakeup elements */
    DRAM_WAKEUP_RSVD (xrw)    : ORIGIN = (_DRAM_Total_Top - _DRAM_Wakeup_Reserved_Size), LENGTH = _DRAM_Wakeup_Reserved_Size    
    
    /* Baseband specific DRAM */
    DRAM_BB (xrw)       : ORIGIN = 0x20000000, LENGTH = 48K

	/* DSP PRAM area for DSP Core 0 */
	PRAM_DSP_0 (xrw) : ORIGIN = 0x20060000, LENGTH = 80K
  
	/* DSP PRAM area for DSP Core 1 */  
	PRAM_DSP_1 (xrw) : ORIGIN = 0x20080000, LENGTH = 80K 	

	/* DSP DRAM area for DSP Core 0 ARAM0-3 */
	DRAM_DSP_0 (xrw) : ORIGIN = 0x20038000, LENGTH = 64K

	/* DSP DRAM area for DSP Core 1 ARAM4-7*/
	DRAM_DSP_1 (xrw) : ORIGIN = 0x20048000, LENGTH = 64K
	
	/* DSP PRAM Emulated area for DSP Core 0 */
	PRAM_EM_DSP_0 (xrw) : ORIGIN = 0x20090000, LENGTH = 80K

	/* DSP PRAM Emulated area for DSP Core 1 */
	PRAM_EM_DSP_1 (xrw) : ORIGIN = 0x200C0000, LENGTH = 80K	

	/* DSP DRAM Emulated area for DSP Cores 0, 1 */
	DRAM_EM_DSP_1_2 (xrw) : ORIGIN = 0x21000000, LENGTH = 160K	
	
	/* DSP DRAM 8 Emulated, Real address of DRAM 8 is 0x20034000*/
	DRAM_DSP0_CM33 (xrw) : ORIGIN = 0x21808000, LENGTH = 16K
	
	/* DSP DRAM 7 Emulated, Real address of DRAM 7 is 0x20030000*/
	DRAM_DSP1_CM33 (xrw) : ORIGIN = 0x2180C000, LENGTH = 16K
}

/** @brief The cmsis library needs to know the relocated address of the stack */
PROVIDE(__stack_limit = ORIGIN(DRAM_STACK));
PROVIDE(__stack = ORIGIN(DRAM_STACK) + LENGTH(DRAM_STACK));
PROVIDE(__Wakeup_addr = ORIGIN(DRAM_WAKEUP_RSVD));
PROVIDE(__CoreRsvd_addr = ORIGIN(DRAM_CORE_RSVD));

/* Define the heap to run from the end of the static data to the top of RAM
 */
PROVIDE (__Heap_Begin__ = __noinit_end__);
PROVIDE (__Heap_Limit__ = ORIGIN(DRAM) + LENGTH(DRAM));

/* The entry point is informative, for debuggers and simulators,
 * since the Cortex-M vector points to it anyway.
 */
ENTRY(Reset_Handler)

/* As is the VTOR register, we refer to it in startup documentation
 */
__VTOR = 0xE000ED08;

/* ----------------------------------------------------------------------------
 * Section definitions
 * ------------------------------------------------------------------------- */
SECTIONS
{
    /* For Cortex-M devices, the beginning of the startup code is stored in
     * the .interrupt_vector section, which goes to MRAM 
     */
    __rom_start = DEFINED(__app_rom_start) ? __app_rom_start : ORIGIN(MRAM);
    __cert_size = DEFINED(__cert_size) ? __cert_size : 0;
    __image_size = __data_init__ + SIZEOF(.data) - __rom_start;
    .text __rom_start :
    {
        . = ALIGN(8);
        KEEP(*(.interrupt_vector)) 

        /* This section is here to store the startup code immediately after
         * the interrupt vectors, as required by the program ROM.
         */
        KEEP(*(.reset))

        /* Pre-initialization Code */
        . = ALIGN(8);
        PROVIDE_HIDDEN (__preinit_array_start__ = .);

        /* System initialization and the platform initialization (if present)
         * should be first */
        KEEP(*(.preinit_array_sysinit .preinit_array_sysinit.*))
        KEEP(*(.preinit_array_platform .preinit_array_platform.*))

        /* Pre-initialization functions (to be executed before C++ 
         * constructors are run) */
        KEEP(*(.preinit_array .preinit_array.*))

        PROVIDE_HIDDEN (__preinit_array_end__ = .);

        /* Initialization Code */
        . = ALIGN(8);
        PROVIDE_HIDDEN (__init_array_start__ = .);

        KEEP(*(SORT(.init_array.*)))
        KEEP(*(.init_array))

        PROVIDE_HIDDEN (__init_array_end__ = .);

        /* Destructor Code  to support C++ */
        . = ALIGN(8);
        PROVIDE_HIDDEN (__fini_array_start = .);

        KEEP(*(SORT(.fini_array.*)))
        KEEP(*(.fini_array))

        PROVIDE_HIDDEN (__fini_array_end = .);

        /* The program code is stored in the .text section, 
         * which goes to MRAM.
         */
        . = ALIGN(8);
   
        *(.text .text.*)            /* all remaining code */
        *(.rodata .rodata.*)        /* read-only data (constants) */
		*(.dsp .dsp.*)				/* dsp data for LPDSP32, may not exist */

        . = ALIGN(8);
        
    } >MRAM
    
        . = ALIGN(8);
    __data_init__ = .;
     
    /* This address is used by the startup code to initialize the .data
     * section.
     */

    /* The initialised data section.
     *
     * The program executes knowing that the data is in the RAM
     * but the loader puts the initial values in the MRAM (inidata).
     * It is one task of the startup to copy the initial values from 
     * MRAM to RAM.
     */
    .data  : AT ( __data_init__ )
    {
        . = ALIGN(4);

        /* This is used by the startup code to initialize the .data section */
        __data_start__ = . ;
        *(.data_begin .data_begin.*)
        *(.wakeup_section)
        *(.data .data.*)
        *(.data_end .data_end.*)
        . = ALIGN(4);

        /* This is used by the startup code to initialize the .data section */
        __data_end__ = . ;

    } >DRAM
      
    /*
     * The uninitialized data section. NOLOAD is used to avoid
     * the "section `.bss' type changed to PROGBITS" warning
     */
    .bss (NOLOAD) :
    {
        . = ALIGN(4);
        __bss_start__ = .;
        *(.bss_begin .bss_begin.*)

        *(.bss .bss.*)
        *(COMMON)
        
        *(.bss_end .bss_end.*)
        . = ALIGN(4);
        __bss_end__ = .;
    } >DRAM
    
    .noinit (NOLOAD) :
    {
        . = ALIGN(4);
        __noinit_start__ = .;
        
        *(.noinit .noinit.*) 
        
         . = ALIGN(4) ;
        __noinit_end__ = .;
    } > DRAM
    
    /* Check if there is enough space to allocate the main stack */
    ._stack (NOLOAD) :
    {
        . = ALIGN(4);
        
        . = . + _DRAM_Stack_Size;

        . = ALIGN(4);
    } >DRAM_STACK

    /* Provide a fixed location for wakeup address section
     */
     .wakeupsection :
     {
        . = ALIGN(4);
        app_wakeup_adrs = .; /* Place wakeuparray app_wakeup_adrs at current address, ie first address of DRAM_WAKEUP_RSVD */
        . = ALIGN(4);
     } >DRAM_WAKEUP_RSVD

     /* Put the shared buffers onto the DSP0 RAM
	 */
	.dsp0sharedConfig :
	{
		 . = ALIGN(4);
	     KEEP(*(.dsp0sharedConfig))
	}>DRAM_DSP0_CM33
	.dsp0shared :
	{
		 . = ALIGN(4);
	     KEEP(*(.dsp0shared))
	}>DRAM_DSP0_CM33
	
	/* Put the shared buffers onto the DSP1 RAM
	 */
	.dsp1shared :
	{    
		 . = ALIGN(4);
	     KEEP(*(.dsp1shared)) 
	}>DRAM_DSP1_CM33
}
