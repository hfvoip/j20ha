/**
 * @file sections_ram.ld
 * @brief General linker control file.
 * @copyright @parblock
 * Copyright (c) 2022 Semiconductor Components Industries, LLC (d/b/a
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
 *  Code:   The area used for program code
 */ 

/** @brief DRAM base address */ 
_DRAM_Total_Base = 0x20020000;

/** @brief DRAM total size is defined as 64K for CM33 */
_DRAM_Total_Size = 0x10000;

/** @brief DRAM base address */ 
_DRAM_Total_Top = _DRAM_Total_Base + _DRAM_Total_Size;

/** @brief Available DRAM size */
_DRAM_Available_Size = _DRAM_Total_Size;

/** @brief Define the stack size as a constant 2K here */
_DRAM_Stack_Size = 2048;

/** @brief Define the stack area to be at the top of RAM */
_DRAM_Stack_Base = _DRAM_Total_Top - _DRAM_Stack_Size;

/** @brief The application usable DRAM */
_DRAM_Base = _DRAM_Total_Base;

/** @brief The DRAM size will be defined as available minus stack */
_DRAM_Size = _DRAM_Available_Size - _DRAM_Stack_Size;

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
  
    /* 2048K of code MRAM is available */
    MRAM (xrw)         : ORIGIN = 0x00200000, LENGTH = 2040K

    /* Reserve 4k for Bluetooth bond information */
    MRAM_BOND_RSVD (xrw)    : ORIGIN = 0x003FE000, LENGTH = 4K
        
    /* The ROM requires the last 4K of data MRAM for DEU transfer space */
    MRAM_RSVD (xrw)    : ORIGIN = 0x003FF000, LENGTH = 4K
    
    /* Application available DRAM */
    DRAM (xrw)          : ORIGIN = _DRAM_Base, LENGTH = _DRAM_Size
    
    DRAM_P (xrw)        : ORIGIN = 0x10000, LENGTH = 80K
    
    /* The stack */
    DRAM_STACK (xrw)    : ORIGIN = _DRAM_Stack_Base, LENGTH = _DRAM_Stack_Size
    
    /* Baseband specific DRAM */
    DRAM_BB (xrw)       : ORIGIN = 0x20000000, LENGTH = 48K

	/* DSP PRAM area for DSP Core 0 */
	PRAM_DSP_0 (xrw) : ORIGIN = 0x20060000, LENGTH = 80K
  
	/* DSP PRAM area for DSP Core 1 */  
	PRAM_DSP_1 (xrw) : ORIGIN = 0x20080000, LENGTH = 80K

	/* DSP DRAM area for DSP Core 0 ARAM0-3 */
	DRAM_DSP_0 (xrw) : ORIGIN = 0x20038000, LENGTH = 64K

	/* DSP DRAM area for DSP Core 1 ARAM4-7 */
	DRAM_DSP_1 (xrw) : ORIGIN = 0x20048000, LENGTH = 64K

	/* DSP PRAM Emulated area for DSP Core 0 */
	PRAM_EM_DSP_0 (xrw) : ORIGIN = 0x20090000, LENGTH = 80K

	/* DSP PRAM Emulated area for DSP Core 1 */
	PRAM_EM_DSP_1 (xrw) : ORIGIN = 0x200C0000, LENGTH = 80K

	/* DSP DRAM Emulated area for DSP Cores 0, 1 */
	DRAM_EM_DSP_1_2 (xrw) : ORIGIN = 0x21000000, LENGTH = 160K	
	
	/* DSP DRAM 8 Emulated, Real address of DRAM 8 is 0x20034000 */
	DRAM_DSP0_CM33 (xrw) : ORIGIN = 0x21808000, LENGTH = 16K
	
	/* DSP DRAM 7 Emulated, Real address of DRAM 7 is 0x20030000 */
	DRAM_DSP1_CM33 (xrw) : ORIGIN = 0x2180C000, LENGTH = 16K
}

/** @brief The cmsis library needs to know the relocated address of the stack */
PROVIDE(__stack_limit = ORIGIN(DRAM_STACK));
PROVIDE(__stack = ORIGIN(DRAM_STACK) + LENGTH(DRAM_STACK));

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
     * the .interrupt_vector section, which goes to DRAM 
     */
    __rom_start = DEFINED(__app_rom_start) ? __app_rom_start : ORIGIN(MRAM);
    __cert_size = DEFINED(__cert_size) ? __cert_size : 0;
    __image_size = __data_init__ + SIZEOF(.data) - __rom_start;
    .text :
    {
        . = ALIGN(4);
        KEEP(*(.interrupt_vector)) 

        /* This section is here to store the startup code immediately after
         * the interrupt vectors, as required by the program ROM.
         */
        KEEP(*(.reset))

        /* Pre-initialization Code */
        . = ALIGN(4);
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
        . = ALIGN(4);
        PROVIDE_HIDDEN (__init_array_start__ = .);

        KEEP(*(SORT(.init_array.*)))
        KEEP(*(.init_array))

        PROVIDE_HIDDEN (__init_array_end__ = .);

        /* The program code is stored in the .text section, 
         * which goes to DRAM.
         */
        . = ALIGN(4);
   
        *(.text .text.*)            /* all remaining code */
        *(.rodata .rodata.*)        /* read-only data (constants) */
		*(.dsp .dsp.*)				/* dsp data for LPDSP32, may not exist */

        . = ALIGN(4);
        
    } >DRAM_P
    
        . = ALIGN(4);
    __data_init__ = .;
       
    /* This address is used by the startup code to initialize the .data
     * section.
     */

    /* The initialised data section.
     *
     * The program executes knowing that the data is in the RAM
     * but the loader puts the initial values in the DRAM (inidata).
     * It is one task of the startup to copy the initial values from 
     * DRAM to RAM.
     */
    .data  : AT ( __data_init__ )
    {
        . = ALIGN(4);

        /* This is used by the startup code to initialize the .data section */
        __data_start__ = . ;
        *(.data_begin .data_begin.*)
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
