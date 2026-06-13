#include <crPrivate.h>
#include <cr.h>

Cr_VM* Cr_CreateVM(void) {
	Cr_VM*	       vm = Cr_Alloc(sizeof(*vm));
	unsigned short n  = 1;
	Cr_Object*     class_c;
	Cr_Object*     obj_c;
	Cr_Object*     mag_c;
	Cr_Object*     char_c;
	Cr_Object*     num_c;
	Cr_Object*     int_c;
	Cr_Object*     float_c;
	Cr_Object*     bool_c;
	Cr_Object*     true_c;
	Cr_Object*     false_c;
	Cr_Object*     block_c;

	if((*(unsigned char*)&n) == 0) vm->big = 1;

	class_c = Cr_NewClassObj(vm, CR_NULL, "Class");

	obj_c			= Cr_NewClassObj(vm, CR_NULL, "Object");
	class_c->superclass_obj = obj_c;

	mag_c = Cr_NewClassObj(vm, obj_c, "Magnitude");

	char_c = Cr_NewClassObj(vm, mag_c, "Character");

	num_c = Cr_NewClassObj(vm, mag_c, "Number");

	int_c = Cr_NewClassObj(vm, num_c, "Integer");

	float_c = Cr_NewClassObj(vm, num_c, "Float");

	bool_c = Cr_NewClassObj(vm, obj_c, "Boolean");

	true_c = Cr_NewClassObj(vm, bool_c, "True");

	false_c = Cr_NewClassObj(vm, bool_c, "False");

	block_c = Cr_NewClassObj(vm, obj_c, "Block");

	return vm;
}

void Cr_DeleteVM(Cr_VM* vm) {
	unsigned long i;

	while(Cr_ArrayLength(vm->threads) > 0) Cr_DeleteThread(vm->threads[0]);
	Cr_FreeArray(vm->threads);

	Cr_FreeHashMap(vm->classes);

	while(Cr_ArrayLength(vm->objects) > 0) Cr_DeleteObj(vm->objects[0]);
	Cr_FreeArray(vm->objects);

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
	int	      s;

	if(ast == CR_NULL) return CR_ERROR;

	if(Cr_SortAndCleanMsgRecv(ast) == CR_OK) {
		Cr_DebugAST(ast);

		Cr_Compile(vm, ast);
	} else {
		st = CR_ERROR;
	}

	Cr_DeleteAST(ast);

	if(st != CR_ERROR) {
		th = Cr_CreateThread(vm, CR_NULL, seq);
		while(!th->dead && (s = Cr_Step(vm)) != CR_ERROR);
		Cr_DeleteThread(th);

		if(s == CR_ERROR) st = CR_ERROR;
	}

	return st;
}

int Cr_Step(Cr_VM* vm) {
	unsigned long i;
	int	      s;

	for(i = 0; i < Cr_ArrayLength(vm->threads); i++) {
		if((s = Cr_ThreadStep(vm->threads[i])) == CR_ERROR) {
			return CR_ERROR;
		}
	}

	return CR_OK;
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
