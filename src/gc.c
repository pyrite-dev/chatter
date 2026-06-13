#include <crPrivate.h>
#include <cr.h>

void Cr_GC(Cr_VM* vm) {
	int	  n = 0;
	CR_SIZE_T i;

	for(i = 0; i < Cr_ArrayLength(vm->objects); i++) {
		if(vm->objects[i]->ref == 0) {
			n++;

			Cr_DeleteObj(vm->objects[i]);

			i--;
			continue;
		}
	}

	if(n > 0) Cr_Debug("gc: cleaned %d object(s)\n", n);
}
