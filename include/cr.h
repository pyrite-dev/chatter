#ifndef __CR_H__
#define __CR_H__

#include <crConfig.h>

typedef struct Cr_Interp Cr_Interp;
typedef struct Cr_AST Cr_AST;

struct Cr_Interp {
	Cr_AST* ast;
};

struct Cr_AST {
};

/* core.c */
Cr_Interp* Cr_CreateInterp(void);
void Cr_DeleteInterp(Cr_Interp* interp);
void Cr_Eval(Cr_Interp* interp, const char* script);

/* ast.c */
Cr_AST* Cr_ASTConstruct(Cr_Interp* interp, const char* string);

#endif
