#ifndef __CR_H__
#define __CR_H__

#include <crConfig.h>

typedef struct Cr_Interp Cr_Interp;
typedef struct Cr_AST Cr_AST;

struct Cr_Interp {
	Cr_AST* ast;
};

struct Cr_AST {
	Cr_AST* parent;
	Cr_AST** children;
};

/* core.c */
Cr_Interp* Cr_CreateInterp(void);
void Cr_DeleteInterp(Cr_Interp* interp);
void Cr_Eval(Cr_Interp* interp, const char* script);

/* ast.c */
void Cr_ASTConstruct(Cr_Interp* interp, Cr_AST* parent, const char* string);

/* array.c */
#define Cr_ArrayPut(x, y) {(x) = Cr_ArrayGrow((x), sizeof(*x)); (x)[Cr_ArrayLength((x))] = (y);}
#define Cr_ArrayFree(x) {Cr_ArrayDestroy((x)); (x) = CR_NULL;}

void* Cr_ArrayGrow(void* array, int size);
int Cr_ArrayLength(void* array);
void Cr_ArrayDestroy(void* array); /* do not use this, use Cr_ArrayFree instead */

#endif
