#include <cr.h>

Cr_Interp* Cr_CreateInterp(void){
	Cr_Interp* interp = CR_MALLOC(sizeof(*interp));

	return interp;
}

void Cr_DeleteInterp(Cr_Interp* interp){
	free(interp);
}

void Cr_Eval(Cr_Interp* interp, const char* script){
	Cr_ASTConstruct(interp, script);
}
