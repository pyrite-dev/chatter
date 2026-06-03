#include <cr.h>

Cr_Interp* Cr_CreateInterp(void){
	Cr_Interp* interp = CR_MALLOC(sizeof(*interp));

	return interp;
}

void Cr_DeleteInterp(Cr_Interp* interp){
	CR_FREE(interp);
}

void Cr_Eval(Cr_Interp* interp, const char* script){
	interp->ast = CR_MALLOC(sizeof(*interp->ast));
	CR_MEMSET(interp->ast, 0, sizeof(*interp->ast));

	Cr_ASTConstruct(interp, interp->ast, script);
}
