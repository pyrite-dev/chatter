#ifndef __CR_H__
#define __CR_H__

#include <crConfig.h>

#ifndef __CR_PRIVATE_H__
typedef void Cr_VM;
#endif

enum CR_STATUS {
	CR_OK = 0,
	CR_ERROR
};

/* they are in kilowords (which is unsigned int) */
#define CR_MEM_1M (1 * 1024)   /* 4MiB */
#define CR_MEM_2M (2 * 1024)   /* 8MiB */
#define CR_MEM_4M (4 * 1024)   /* 16MiB */
#define CR_MEM_8M (8 * 1024)   /* 32MiB */
#define CR_MEM_16M (16 * 1024) /* 64MiB */

#define CR_MEM_STD CR_MEM_4M

/* vm.c */
Cr_VM* Cr_CreateVM(long mem);
void   Cr_DeleteVM(Cr_VM* vm);
int    Cr_Eval(Cr_VM* vm, const char* script);

#endif
