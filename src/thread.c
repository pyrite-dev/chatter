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
