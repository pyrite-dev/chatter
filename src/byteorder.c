#include <crPrivate.h>
#include <cr.h>

unsigned int Cr_BigU32(Cr_VM* vm, unsigned int n) {
	unsigned int r = 0;
	int	     i;

	if(vm->big) return n;

	for(i = 0; i < 4; i++) {
		r = r << 8;
		r = r | (n >> (i * 8)) & 0xff;
	}

	return r;
}

int Cr_BigS32(Cr_VM* vm, int n) {
	unsigned int r = Cr_BigU32(vm, *(unsigned int*)&n);

	return *(int*)&r;
}

float Cr_BigF32(Cr_VM* vm, float n) {
	float	       r;
	unsigned char* f = (unsigned char*)&n;
	unsigned char* t = (unsigned char*)&r;
	int	       i;

	if(vm->big) return n;

	for(i = 0; i < 4; i++) {
		t[4 - i - 1] = f[i];
	}

	return r;
}
