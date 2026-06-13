#ifndef __CR_CONFIG_H__
#define __CR_CONFIG_H__

/* Adjust them as you need */

#include <string.h>
#include <stdlib.h>

#define CR_CALLOC calloc
#define CR_FREE free

/* if your platform is missing calloc, define this */
/* #define CR_MALLOC malloc */

#define CR_ATOI atoi
#define CR_ATOF atof

#define CR_SIZE_T long
#define CR_USIZE_T unsigned long
#define CR_HASH_T unsigned long

#define CR_MEMSET memset
#define CR_MEMCPY memcpy

/* this is optional */
#define CR_MEMCMP memcmp

#define CR_NULL NULL

#define CR_BUFSZ 1024
#define CR_TOKSZ 256

#endif
