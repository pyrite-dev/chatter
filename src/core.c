#include <cr.h>

Cr_Interp* Cr_CreateInterp(void) {
	Cr_Interp* interp = Cr_Alloc(sizeof(*interp));

	return interp;
}

void Cr_DeleteInterp(Cr_Interp* interp) {
	Cr_Free(interp);
}

void Cr_Eval(Cr_Interp* interp, const char* script) {
	Cr_AST* parent = Cr_Alloc(sizeof(*parent));

	Cr_Debug("core: %p\n", Cr_Parse(parent, script));
}
