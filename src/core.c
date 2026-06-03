#include <cr.h>

Cr_Interp* Cr_CreateInterp(void) {
	Cr_Interp* interp = Cr_Alloc(sizeof(*interp));

	return interp;
}

void Cr_DeleteInterp(Cr_Interp* interp) {
	Cr_Free(interp);
}

#include <stdio.h>

void Cr_Eval(Cr_Interp* interp, const char* script) {
	struct Cr_Token* t;
	int		 n = 0;

	while((t = Cr_Lex(script + n)) != CR_NULL) {
		printf("%d [%s]\n", t->type, t->token);
		n += Cr_Length(t->token);
	}
}
