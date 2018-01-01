/* 
	Editor: http://www.visualmicro.com
			visual micro and the arduino ide ignore this code during compilation. this code is automatically maintained by visualmicro, manual changes to this file will be overwritten
			the contents of the Visual Micro sketch sub folder can be deleted prior to publishing a project
			all non-arduino files created by visual micro and all visual studio project or solution files can be freely deleted and are not required to compile a sketch (do not delete your own code!).
			note: debugger breakpoints are stored in '.sln' or '.asln' files, knowledge of last uploaded breakpoints is stored in the upload.vmps.xml file. Both files are required to continue a previous debug session without needing to compile and upload again
	
	Hardware: Arduino/Genuino Mega w/ ATmega2560 (Mega 2560), Platform=avr, Package=arduino
*/

#if defined(_VMICRO_INTELLISENSE)

#ifndef _VSARDUINO_H_
#define _VSARDUINO_H_
#define __AVR_ATmega2560__
#define F_CPU 16000000L
#define ARDUINO 10801
#define ARDUINO_AVR_MEGA2560
#define ARDUINO_ARCH_AVR
#define __cplusplus 201103L
#define __AVR__
#define __inline__
#define __asm__(...)
#define __extension__
#define __inline__
#define __volatile__
#define GCC_VERSION 40902

#define __cplusplus 201103L
#undef __cplusplus
#define __cplusplus 201103L

#define volatile(va_arg) 
#define _CONST
#define __builtin_va_start
#define __builtin_va_end
#define __attribute__(...)
#define NOINLINE __attribute__((noinline))
#define prog_void
#define PGM_VOID_P int


#ifndef __builtin_constant_p
	#define __builtin_constant_p __attribute__((__const__))
#endif
#ifndef __builtin_strlen
	#define __builtin_strlen  __attribute__((__const__))
#endif


#define NEW_H
typedef void *__builtin_va_list;
//extern "C" void __cxa_pure_virtual() {;}

typedef int div_t;
typedef int ldiv_t;


typedef void *__builtin_va_list;
//extern "C" void __cxa_pure_virtual() {;}



#include <Arduino.h>
#include <pins_arduino.h> 
#undef F
#define F(string_literal) ((const PROGMEM char *)(string_literal))
#undef PSTR
#define PSTR(string_literal) ((const PROGMEM char *)(string_literal))


#define pgm_read_byte(address_short) uint8_t() 
#define pgm_read_word(address_short) uint16_t() 
#define pgm_read_dword(address_short) uint32_t()
#define pgm_read_float(address_short) float()
#define pgm_read_ptr(address_short)   short()

#include "Device2.ino"
#endif
#endif
