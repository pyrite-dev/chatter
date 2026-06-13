#ifndef __CR_H__
#define __CR_H__

#include <crConfig.h>

#ifndef __CR_PRIVATE_H__
typedef void Cr_Object;
typedef void Cr_Thread;
typedef void Cr_VM;
#endif

enum CR_STATUS {
	CR_OK = 0,
	CR_ERROR
};

typedef int (*Cr_Method)(Cr_VM* vm, Cr_Thread* thread, int argc, Cr_Object** argv);

/* vm.c */
Cr_VM* Cr_CreateVM(void);
void   Cr_DeleteVM(Cr_VM* vm);
int    Cr_Eval(Cr_VM* vm, const char* script);

/* object.c */
Cr_Object* Cr_NewObj(Cr_VM* vm);
Cr_Object* Cr_NewClassObj(Cr_VM* vm, Cr_Object* superclass_obj, const char* name);
Cr_Object* Cr_NewIntObj(Cr_VM* vm, int v);
Cr_Object* Cr_NewFloatObj(Cr_VM* vm, float v);
Cr_Object* Cr_NewBlockObj(Cr_VM* vm, unsigned long block);
void	   Cr_NewMethod(Cr_VM* vm, const char* name, void* userdata, Cr_Method method);
void	   Cr_DeleteObj(Cr_Object* obj);

#endif
