#ifndef __CR_H__
#define __CR_H__

#include <crConfig.h>

typedef struct Cr_Interp Cr_Interp;
typedef struct Cr_Token Cr_Token;

struct Cr_Interp {
};

struct Cr_Token {
	int type;
	char token[CR_TOKSZ];
};

/* core.c */
Cr_Interp* Cr_CreateInterp(void);
void Cr_DeleteInterp(Cr_Interp* interp);
void Cr_Eval(Cr_Interp* interp, const char* script);

/* lexer.c */
Cr_Token* Cr_Lex(const char* str);

/* mem.c */
void* Cr_Alloc(int size);
void Cr_Free(void* ptr);
void Cr_Copy(void* dst, const void* src, int size);

/* array.c */
#define Cr_ArrayPut(x, y) {(x) = Cr_ArrayGrow((x), sizeof(*x)); (x)[Cr_ArrayLength((x))] = (y);}
#define Cr_ArrayFree(x) {Cr_ArrayDestroy((x)); (x) = CR_NULL;}

void* Cr_ArrayGrow(void* array, int size);
int Cr_ArrayLength(void* array);
void Cr_ArrayDestroy(void* array); /* do not use this, use Cr_ArrayFree instead */

#endif
