#include <crPrivate.h>
#include <cr.h>

Cr_Object* Cr_NewObj(Cr_VM* vm) {
	Cr_Object* obj = Cr_Alloc(sizeof(*obj));

	obj->meta = Cr_GetClass(vm, "Object");

	return obj;
}

Cr_Object* Cr_NewClassObj(Cr_VM* vm, const char* name) {
	Cr_Object* obj = Cr_Alloc(sizeof(*obj));

	if(Cr_EqualString(name, "Metaclass")) {
		Cr_SetClass(vm, "Metaclass", obj);
	}

	obj->meta = Cr_GetClass(vm, "Metaclass");

	return obj;
}

void Cr_DeleteObj(Cr_Object* obj) {
	Cr_Free(obj);
}
