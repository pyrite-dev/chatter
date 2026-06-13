#include <crPrivate.h>
#include <cr.h>

Cr_Thread* Cr_CreateThread(Cr_VM* vm, long section) {
	Cr_Thread*	 thread = Cr_Alloc(sizeof(*thread));
	Cr_ThreadRunning r;

	r.sp = section;
	r.ip = 0;

	thread->vm = vm;
	Cr_ArrayPut(thread->running, r);

	Cr_ArrayPut(vm->threads, thread);

	return thread;
}

void Cr_DeleteThread(Cr_Thread* thread) {
	Cr_ArrayDeleteMatch(thread->vm->threads, thread);

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

void Cr_ThreadStep(Cr_Thread* thread) {
	Cr_ThreadRunning* r;
	Cr_Section*	  s;
	Cr_Cell*	  c;
	int		  n8, n32;
	int		  i;

	if(thread->dead) return;

	if(!thread->dead && Cr_ArrayLength(thread->running) == 0) thread->dead = 1;

	if(thread->dead) return;

	r = &thread->running[Cr_ArrayLength(thread->running) - 1];
	s = Cr_HashMapGet(thread->vm->sections, r->sp);
	c = &s->value[r->ip];

	Cr_GetArgs(c, &n8, &n32);

	Cr_Debug("thread %p: %8s (0x%02X)", thread, (c->i.op >= (sizeof(inst) / sizeof(inst[0]))) ? "???" : inst[c->i.op], (int)c->i.op);

	Cr_Debug(" ");

	for(i = 0; i < 3; i++) {
		if(i < n8) {
			Cr_Debug(" 0x%02X", (int)c->d[i + 1]);
		} else {
			Cr_Debug("     ");
		}
	}

	Cr_Debug(" ");

	for(i = 0; i < n32; i++) {
		Cr_Cell* c2 = &s->value[r->ip + 1 + i];

		Cr_Debug(" 0x%02X 0x%02X 0x%02X 0x%02X", (int)c2->d[0], (int)c2->d[1], (int)c2->d[2], (int)c2->d[3]);
	}

	Cr_Debug("\n");

	if(c->i.op == CR_VM_RET) {
		unsigned long n = Cr_ArrayLength(thread->running) - 1;

		Cr_ArrayDelete(thread->running, n);

		return;
	}

	r->ip += n32 + 1;
}
