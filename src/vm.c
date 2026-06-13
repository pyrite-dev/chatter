#include <crPrivate.h>
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
	unsigned long i;

	while(Cr_ArrayLength(vm->threads) > 0) Cr_DeleteThread(vm->threads[0]);
	Cr_FreeArray(vm->threads);

	for(i = 0; i < vm->section_seq; i++) {
		Cr_Section* s = Cr_HashMapGet(vm->sections, i);

		Cr_FreeArray(s->value);
	}
	Cr_FreeHashMap(vm->sections);

	Cr_Free(vm->mem);
	Cr_Free(vm);
}

int Cr_Eval(Cr_VM* vm, const char* script) {
	Cr_AST*	      ast = Cr_Parse(script);
	int	      st  = CR_OK;
	unsigned long seq = vm->section_seq;
	Cr_Thread*    th;

	if(ast == CR_NULL) return CR_ERROR;

	if(Cr_SortAndCleanMsgRecv(ast) == CR_OK) {
		Cr_DebugAST(ast);

		Cr_Compile(vm, ast);
	} else {
		st = CR_ERROR;
	}

	Cr_DeleteAST(ast);

	th = Cr_CreateThread(vm, seq);
	while(!th->dead) Cr_Step(vm);
	Cr_DeleteThread(th);

	return st;
}

void Cr_Step(Cr_VM* vm) {
	unsigned long i;

	for(i = 0; i < Cr_ArrayLength(vm->threads); i++) Cr_ThreadStep(vm->threads[i]);
}
