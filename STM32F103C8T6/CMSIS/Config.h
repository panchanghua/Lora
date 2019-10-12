/**************************************************
 * Copyright 2004-2005 IAR Systems. All rights reserved.
 *
 * $Revision: 21621 $
 **************************************************/

#ifndef CONFIG_H

#include "DriverConfig.h"

// Maximum number of arguments passed to the program.
#ifndef MAX_ARGS
#define MAX_ARGS 7
#endif

// Maximum combined size of arguments, including a trailing null for each argument.
#ifndef MAX_ARG_SIZE
#define MAX_ARG_SIZE 64
#endif

// Minimum required read buffer size to get a reasonable download performance.
// Note that the read buffer is dynamically allocated from the remaining space
// in RAM, so the minimum check must be done at runtime.
#ifndef REQUIRED_READ_BUFFER_SIZE
#if TINY_FLASHLOADER == 0
#define REQUIRED_READ_BUFFER_SIZE 128
#else
#define REQUIRED_READ_BUFFER_SIZE 32
#endif
#endif

// Maximum read buffer size. 16 Kbytes will work well in most cases.
#ifndef MAX_READ_BUFFER_SIZE
#define MAX_READ_BUFFER_SIZE 16384
#endif

//
// Stuff below can be used when debugging the flash loader
// as a standalone application.
//

// Used when debugging the flash loader as a normal standalone application.
//#define DEBUG 1

// Specify the binary input file used when debugging.
// The file format of the file must be IAR simple-code.
#ifndef DEBUG_FILE
#define DEBUG_FILE "C:\\ew\\flashbug\\Debug\\Exe\\flashtest.sim"
#endif

// The argument passing mechanism is not available when debugging a
// flash loader as a standalone application. To be able to test flash
// loader arguments the DEBUG_ARGS can be set to the argument string
// needed during debugging.
#ifndef DEBUG_ARGS
#define DEBUG_ARGS ""
// #define DEBUG_ARGS "--base 0x12340000"
#endif

#endif /* CONFIG_H */

