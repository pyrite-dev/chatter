#include <cr.h>

void* Cr_Alloc(int size){
	void* ptr = CR_MALLOC(size);

	CR_MEMSET(ptr, 0, size);

	return ptr;
}

void Cr_Free(void* ptr){
	CR_FREE(ptr);
}

void Cr_Copy(void* dst, const void* src, int size){
	CR_MEMCPY(dst, src, size);
}

