#include <crPrivate.h>
#include <cr.h>

static Cr_Object* allocObj(Cr_VM* vm) {
	Cr_Object* obj = Cr_Alloc(sizeof(*obj));

	obj->vm = vm;

	obj->ref++;

	Cr_ArrayPut(vm->objects, obj);

	return obj;
}

Cr_Object* Cr_NewObj(Cr_VM* vm) {
	Cr_Object* obj = allocObj(vm);

	obj->class_obj = Cr_GetClass(vm, "Object");

	return obj;
}

Cr_Object* Cr_NewClassObj(Cr_VM* vm, Cr_Object* superclass_obj, const char* name) {
	Cr_Object* obj = allocObj(vm);

	if(Cr_EqualString(name, "Class")) Cr_SetClass(vm, "Class", obj);

	obj->class_obj	    = Cr_GetClass(vm, "Class");
	obj->superclass_obj = superclass_obj;

	return obj;
}

Cr_Object* Cr_NewIntObj(Cr_VM* vm, int v) {
	Cr_Object* obj = allocObj(vm);

	obj->class_obj = Cr_GetClass(vm, "Integer");
	obj->raw.s32   = v;

	return obj;
}

Cr_Object* Cr_NewFloatObj(Cr_VM* vm, float v) {
	Cr_Object* obj = allocObj(vm);

	obj->class_obj = Cr_GetClass(vm, "Float");
	obj->raw.f32   = v;

	return obj;
}

Cr_Object* Cr_NewBlockObj(Cr_VM* vm, unsigned long block) {
	Cr_Object* obj = allocObj(vm);

	obj->class_obj	 = Cr_GetClass(vm, "Block");
	obj->raw.section = block;

	return obj;
}

void Cr_DeleteObj(Cr_Object* obj) {
	Cr_ArrayDeleteMatch(obj->vm->objects, obj);

	Cr_Free(obj);
}
