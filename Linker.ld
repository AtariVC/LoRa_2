
/*-----------------------------------------------------------------------------------
#	  Generated by Cortex-Builder extension fo Visual Studio code
#	  Author: Semyon Ivanov
#	  e-mail: agreement90@mail.ru 
#	  repo: https://github.com/7bnx/Cortex-Builder
/*---------------------------------------------------------------------------------*/


/*---------------------------------------------------------------------------------*/
/*	Entry Point*/
/*---------------------------------------------------------------------------------*/
ENTRY(Reset_Handler)
/*---------------------------------------------------------------------------------*/
/*	End of Entry Point*/
/*---------------------------------------------------------------------------------*/


/*---------------------------------------------------------------------------------*/
/*	Defines*/
/*---------------------------------------------------------------------------------*/
STARTRAM = 0x20000000;
STARTFLASH = 0x08000000;
ENDRAM = 0x2000C000;
LENGTHRAM = 48K;
LENGTHFLASH = 256K;
HEAPSIZE = 0;
STACKSIZE = 100;
/*---------------------------------------------------------------------------------*/
/*	End of Defines*/
/*---------------------------------------------------------------------------------*/


/*---------------------------------------------------------------------------------*/
/*	Memory Area*/
/*---------------------------------------------------------------------------------*/
MEMORY
{
  RAM (xrw):	ORIGIN = STARTRAM, LENGTH = LENGTHRAM
  FLASH (rx):	ORIGIN = STARTFLASH, LENGTH = LENGTHFLASH
}
/*---------------------------------------------------------------------------------*/
/*	End of Memory Areas*/
/*---------------------------------------------------------------------------------*/


/*---------------------------------------------------------------------------------*/
/*	Output Sections*/
/*---------------------------------------------------------------------------------*/
SECTIONS
{
/*---------------------------------------------------------------------------------*/
/*	1. ISR section*/
/*---------------------------------------------------------------------------------*/
  .isr_vector :
  {
    . = ALIGN(4);
    KEEP(*(.isr_vector))
    . = ALIGN(4);
  } >FLASH
/*---------------------------------------------------------------------------------*/
/*	End of ISR Section*/
/*---------------------------------------------------------------------------------*/


/*---------------------------------------------------------------------------------*/
/*	2. Text section*/
/*---------------------------------------------------------------------------------*/
  .text :
  {
    . = ALIGN(4);
    *(.text)
    *(.text*)
    *(.glue_7)
    *(.glue_7t)
    *(.eh_frame)

    KEEP(*(.init))
    KEEP(*(.fini))

    . = ALIGN(4);
    _etext = .;
  } >FLASH
/*---------------------------------------------------------------------------------*/
/*	End of Text Section*/
/*---------------------------------------------------------------------------------*/


/*---------------------------------------------------------------------------------*/
/*	3. rodata section*/
/*---------------------------------------------------------------------------------*/
  .rodata :
  {
    . = ALIGN(4);
    *(.rodata)
    *(.rodata*)
    . = ALIGN(4);
  } >FLASH
/*---------------------------------------------------------------------------------*/
/*	End of rodata Section*/
/*---------------------------------------------------------------------------------*/


/*---------------------------------------------------------------------------------*/
/*	4. ARMEXTAB section*/
/*---------------------------------------------------------------------------------*/
  .ARM.extab :
  {
    *(.ARM.extab* .gnu.linkonce.armextab.*)
  } >FLASH
/*---------------------------------------------------------------------------------*/
/*	End of ARMEXTAB Section*/
/*---------------------------------------------------------------------------------*/


/*---------------------------------------------------------------------------------*/
/*	5. ARM section*/
/*---------------------------------------------------------------------------------*/
  .ARM :
  {
    __exidx_start = .;
    *(.ARM.exidx*)
    __exidx_end = .;
  } >FLASH
/*---------------------------------------------------------------------------------*/
/*	End of ARM Section*/
/*---------------------------------------------------------------------------------*/


/*---------------------------------------------------------------------------------*/
/*	6. preinit_array section*/
/*---------------------------------------------------------------------------------*/
  .preinit_array :
  {
    PROVIDE_HIDDEN (__preinit_array_start = .);
    KEEP (*(.preinit_array*))
    PROVIDE_HIDDEN (__preinit_array_end = .);
  } >FLASH
/*---------------------------------------------------------------------------------*/
/*	End of preinit_array Section*/
/*---------------------------------------------------------------------------------*/


/*---------------------------------------------------------------------------------*/
/*	7. init_array section*/
/*---------------------------------------------------------------------------------*/
  .init_array :
  {
    PROVIDE_HIDDEN (__init_array_start = .);
    KEEP (*(SORT(.init_array.*)))
    KEEP (*(.init_array*))
    PROVIDE_HIDDEN (__init_array_end = .);
  } >FLASH
/*---------------------------------------------------------------------------------*/
/*	End of init_array Section*/
/*---------------------------------------------------------------------------------*/


/*---------------------------------------------------------------------------------*/
/*	8. fini_array section*/
/*---------------------------------------------------------------------------------*/
  .fini_array :
  {
    PROVIDE_HIDDEN (__fini_array_start = .);
    KEEP (*(SORT(.fini_array.*)))
    KEEP (*(.fini_array*))
    PROVIDE_HIDDEN (__fini_array_end = .);
  } >FLASH
/*---------------------------------------------------------------------------------*/
/*	End of fini_array Section*/
/*---------------------------------------------------------------------------------*/


/*---------------------------------------------------------------------------------*/
/*	9. data section*/
/*---------------------------------------------------------------------------------*/
  _sidata = LOADADDR(.data);
  .data :
  {
    . = ALIGN(4);
    _sdata = .;
    *(.data)
    *(.data*)
    . = ALIGN(4);
    _edata = .;
  } >RAM AT> FLASH
/*---------------------------------------------------------------------------------*/
/*	End of data Section*/
/*---------------------------------------------------------------------------------*/


/*---------------------------------------------------------------------------------*/
/*	10. bss section*/
/*---------------------------------------------------------------------------------*/
  . = ALIGN(4);
  .bss :
  {
    _sbss = .;
    __bss_start__ = _sbss;
    *(.bss)
    *(.bss*)
    *(COMMON)

    . = ALIGN(4);
    _ebss = .;
    __bss_end__ = _ebss;
  } >RAM
/*---------------------------------------------------------------------------------*/
/*	End of bss Section*/
/*---------------------------------------------------------------------------------*/


/*---------------------------------------------------------------------------------*/
/*	11. User Heap section*/
/*---------------------------------------------------------------------------------*/
  ._user_heap_stack :
  {
    . = ALIGN(8);
    PROVIDE ( end = . );
    PROVIDE ( _end = . );
    . = . + HEAPSIZE;
    . = . + STACKSIZE;
    . = ALIGN(8);
  } >RAM
/*---------------------------------------------------------------------------------*/
/*	End of User Heap Section*/
/*---------------------------------------------------------------------------------*/


/*---------------------------------------------------------------------------------*/
/*	12. DISCARD section*/
/*---------------------------------------------------------------------------------*/
  /DISCARD/ :
  {
    libc.a ( * )
    libm.a ( * )
    libgcc.a ( * )
  }
/*---------------------------------------------------------------------------------*/
/*	End of DISCARD Section*/
/*---------------------------------------------------------------------------------*/


/*---------------------------------------------------------------------------------*/
/*	13. ARM Attributes section*/
/*---------------------------------------------------------------------------------*/
  .ARM.attributes 0 :
  {
    *(.ARM.attributes)
  }
/*---------------------------------------------------------------------------------*/
/*	End of ARM Attributes Section*/
/*---------------------------------------------------------------------------------*/
}
/*---------------------------------------------------------------------------------*/
/*	End of Output Sections*/
/*---------------------------------------------------------------------------------*/