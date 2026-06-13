#include <crPrivate.h>
#include <cr.h>

void* Cr_Alloc(CR_SIZE_T size) {
#ifdef CR_CALLOC
	return CR_CALLOC(1, size);
#else
	void* ptr = CR_MALLOC(size);

	CR_MEMSET(ptr, 0, size);

	return ptr;
#endif
}

void Cr_Free(void* ptr) {
	CR_FREE(ptr);
}

void Cr_Copy(void* dst, const void* src, CR_SIZE_T size) {
	CR_MEMCPY(dst, src, size);
}

int Cr_Equal(const void* a, const void* b, CR_SIZE_T size) {
#ifdef CR_MEMCMP
	return memcmp(a, b, size) == 0;
#else
	CR_SIZE_T	     i;
	const unsigned char* ba = a;
	const unsigned char* bb = b;

	for(i = 0; i < size && ba[i] == bb[i]; i++);

	return i == size;
#endif
}
