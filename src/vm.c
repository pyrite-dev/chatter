#include <cr.h>

Cr_VM* Cr_CreateVM(long mem) {
	Cr_VM*	       vm = Cr_Alloc(sizeof(*vm));
	unsigned short n  = 1;

	if((*(unsigned char*)&n) == 0) vm->big = 1;

	vm->mem	    = Cr_Alloc(sizeof(*vm->mem) * 1024 * mem);
	vm->memsize = mem * 1024;

	return vm;
}

void Cr_DeleteVM(Cr_VM* vm) {
	Cr_Free(vm->mem);
	Cr_Free(vm);
}

int Cr_Eval(Cr_VM* vm, const char* script) {
	Cr_AST* ast = Cr_Parse(script);
	int	st  = CR_OK;

	if(ast == CR_NULL) return CR_ERROR;

	if(Cr_SortAndCleanMsgRecv(ast) == CR_OK) {
		Cr_DebugAST(ast);

		Cr_Compile(vm, ast);
	} else {
		st = CR_ERROR;
	}

	Cr_DeleteAST(ast);

	return st;
}
