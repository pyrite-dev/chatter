#include <cr.h>

void* Cr_Alloc(int size) {
	void* ptr = CR_MALLOC(size);

	CR_MEMSET(ptr, 0, size);

	return ptr;
}

void Cr_Free(void* ptr) {
	CR_FREE(ptr);
}

void Cr_Copy(void* dst, const void* src, int size) {
	CR_MEMCPY(dst, src, size);
}

int Cr_Length(const char* ptr) {
	int i;

	for(i = 0; ptr[i] != 0; i++);

	return i;
}

int Cr_Equal(const void* a, const void* b, int size) {
	int		     i;
	const unsigned char* ba = a;
	const unsigned char* bb = b;

	for(i = 0; i < size && ba[i] == bb[i]; i++);

	return i == size;
}
