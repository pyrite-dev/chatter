#include <crPrivate.h>
#include <cr.h>

Cr_Thread* Cr_CreateThread(Cr_VM* vm, Cr_Thread* parent, long section) {
	Cr_Thread*	thread = Cr_Alloc(sizeof(*thread));
	Cr_CellPosition r;

	r.sp = section;
	r.ip = 0;

	thread->vm     = vm;
	thread->parent = parent;

	Cr_ArrayPut(thread->running, r);

	Cr_ArrayPut(vm->threads, thread);

	return thread;
}

void Cr_DeleteThread(Cr_Thread* thread) {
	Cr_ArrayDeleteMatch(thread->vm->threads, thread);

	Cr_FreeArray(thread->stack);
	Cr_FreeArray(thread->running);
	Cr_Free(thread);
}

static const char* inst[] = {
    "nop",
    "ret",
    "call",
    "int",
    "float",
    "arr",
    "bytearr",
    "var",
    "block",
    "local",
    "setvar"};

int Cr_ThreadStep(Cr_Thread* thread) {
	Cr_CellPosition* r;
	Cr_Section*	 s;
	Cr_Cell*	 c;
	int		 n8, n32;
	unsigned int	 i;
	unsigned int	 d24 = 0;
	unsigned long	 d64 = 0;
	Cr_Object*	 obj = CR_NULL;

	if(thread->dead) return CR_OK;
	if(!thread->dead && Cr_ArrayLength(thread->running) == 0) thread->dead = 1;
	if(thread->dead) return CR_OK;

	if(thread->wait != CR_NULL && !thread->wait->dead) return CR_OK;
	if(thread->wait != CR_NULL) {
		Cr_DeleteThread(thread->wait);

		thread->wait = CR_NULL;
	}

	r = &thread->running[Cr_ArrayLength(thread->running) - 1];
	s = Cr_HashMapGet(thread->vm->sections, r->sp);
	c = &s->value[r->ip];

	Cr_GetArgs(c, &n8, &n32);

	Cr_Debug("thread %p: %8s: %8s ", thread, "dump", (c->i.op >= (sizeof(inst) / sizeof(inst[0]))) ? "???" : inst[c->i.op]);

	for(i = 0; i < n8; i++) {
		d24 = d24 << 8;
		d24 = d24 | c->d[i + 1];
	}

	for(i = 0; i < n32; i++) {
		Cr_Cell* c2 = &s->value[r->ip + 1 + i];

		d64 = d64 << 8;
		d64 = d64 | c2->d[0];
		d64 = d64 << 8;
		d64 = d64 | c2->d[1];
		d64 = d64 << 8;
		d64 = d64 | c2->d[2];
		d64 = d64 << 8;
		d64 = d64 | c2->d[3];
	}

	if(n8 > 0) {
		Cr_Debug("0x%06X", d24);
	} else {
		Cr_Debug("          ");
	}
	if(n8 > 0 && n32 > 0) Cr_Debug(", ");
	if(n32 == 1) {
		Cr_Debug("0x%08X", d64);
	} else if(n32 == 2) {
		Cr_Debug("0x%016X", d64);
	}

	Cr_Debug("\n");

	switch(c->i.op) {
	case CR_VM_RET:
	{
		unsigned long n = Cr_ArrayLength(thread->running) - 1;

		Cr_ArrayDelete(thread->running, n);

		return CR_OK;
	}
	case CR_VM_CALL:
	{
		Cr_Object** args = Cr_Alloc(sizeof(*args) * d24);

		for(i = 0; i < d24; i++) {
			unsigned int n = Cr_ArrayLength(thread->stack) - 1;

			if(n < 0) {
				Cr_Debug("thread %p: %8s: stack underflow\n", thread, "error");

				Cr_Free(args);

				return CR_ERROR;
			}

			args[d24 - i - 1] = thread->stack[n];

			Cr_ArrayDelete(thread->stack, n);
		}

		Cr_Debug("thread %p: %8s: consume %d, method %lu\n", thread, "call", d24, d64);

		Cr_Free(args);

		break;
	}
	case CR_VM_INT:
	{
		unsigned int u32 = d64;

		obj = Cr_NewIntObj(thread->vm, *(int*)&u32);

		break;
	}
	case CR_VM_FLOAT:
	{
		unsigned int u32 = d64;

		obj = Cr_NewFloatObj(thread->vm, *(float*)&u32);

		break;
	}
	case CR_VM_ARR:
	{
		break;
	}
	case CR_VM_BYTEARR:
	{
		break;
	}
	case CR_VM_VAR:
	{
		break;
	}
	case CR_VM_BLOCK:
	{
		obj = Cr_NewBlockObj(thread->vm, d64);

		break;
	}
	case CR_VM_LOCAL:
	{
		break;
	}
	case CR_VM_SETVAR:
	{
		break;
	}
	}

	if(obj != CR_NULL) {
		Cr_ArrayPut(thread->stack, obj);
	}

	r->ip += n32 + 1;

	return CR_OK;
}
