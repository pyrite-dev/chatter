#include <crPrivate.h>
#include <cr.h>

Cr_VM* Cr_CreateVM(void) {
	Cr_VM*	       vm = Cr_Alloc(sizeof(*vm));
	unsigned short n  = 1;
	Cr_Object*     mc;
	Cr_Object*     oc;

	if((*(unsigned char*)&n) == 0) vm->big = 1;

	mc = Cr_NewClassObj(vm, "Metaclass");
	oc = Cr_NewClassObj(vm, "Object");

	return vm;
}

void Cr_DeleteVM(Cr_VM* vm) {
	unsigned long i;
	Cr_ObjectKV*  cs = Cr_HashMapGetAll(vm->classes);

	while(Cr_ArrayLength(vm->threads) > 0) Cr_DeleteThread(vm->threads[0]);
	Cr_FreeArray(vm->threads);

	for(i = 0; i < Cr_HashMapLength(vm->classes); i++) {
		Cr_DeleteObj(cs[i].value);
	}
	Cr_Free(cs);
	Cr_FreeHashMap(vm->classes);

	for(i = 0; i < vm->section_seq; i++) {
		Cr_Section* s = Cr_HashMapGet(vm->sections, i);

		Cr_FreeArray(s->value);
	}
	Cr_FreeHashMap(vm->sections);

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

	th = Cr_CreateThread(vm, CR_NULL, seq);
	while(!th->dead) Cr_Step(vm);
	Cr_DeleteThread(th);

	return st;
}

void Cr_Step(Cr_VM* vm) {
	unsigned long i;

	for(i = 0; i < Cr_ArrayLength(vm->threads); i++) Cr_ThreadStep(vm->threads[i]);
}

void Cr_GetArgs(Cr_Cell* cell, int* n8, int* n32) {
	*n8  = 0;
	*n32 = 0;

	if(cell->i.op == CR_VM_CALL) {
		*n8  = 3;
		*n32 = 2;
	} else if(cell->i.op == CR_VM_INT || cell->i.op == CR_VM_FLOAT) {
		*n32 = 1;
	} else if(cell->i.op == CR_VM_VAR || cell->i.op == CR_VM_LOCAL || cell->i.op == CR_VM_SETVAR) {
		*n32 = 2;
	} else if(cell->i.op == CR_VM_BLOCK || cell->i.op == CR_VM_ARR || cell->i.op == CR_VM_BYTEARR) {
		*n8 = 3;
	}
}

Cr_Object* Cr_GetClass(Cr_VM* vm, const char* name) {
	unsigned long h = Cr_Hash(name, Cr_Length(name));
	Cr_ObjectKV*  c = Cr_HashMapGet(vm->classes, h);

	return c == CR_NULL ? CR_NULL : c->value;
}

void Cr_SetClass(Cr_VM* vm, const char* name, Cr_Object* obj) {
	unsigned long h = Cr_Hash(name, Cr_Length(name));

	Cr_HashMapPut(vm->classes, h, obj);
}
