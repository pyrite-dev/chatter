#include <cr.h>

struct arrayinfo {
	int esize; /* element size */
	int length; /* length */
};

void* Cr_ArrayGrow(void* array, int size){
	struct arrayinfo* ai;
	if(array == CR_NULL){
		ai = CR_MALLOC(sizeof(*ai) + size);
		ai->esize = size;
		ai->length = 1;
	}else{
		struct arrayinfo* old = array;
		old--;

		ai = CR_MALLOC(sizeof(*ai) + old->esize * old->length + size);
		CR_MEMCPY(ai, old, sizeof(*ai) + old->esize * old->length);

		CR_FREE(ai);
	}

	return ai + 1;
}

int Cr_ArrayLength(void* array){
	struct arrayinfo* ai = array;

	if(array == CR_NULL) return 1;
	ai--;

	return ai->length;
}

void Cr_ArrayFreeBody(void* array){
	struct arrayinfo* ai = array;

	ai--;

	free(ai);
}
