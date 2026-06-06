#include <cr.h>

struct arrayinfo {
	int esize;  /* element size */
	int length; /* length */
};

void* Cr_ArrayGrow(void* array, int size) {
	struct arrayinfo* ai;
	if(array == CR_NULL) {
		ai	   = Cr_Alloc(sizeof(*ai) + size);
		ai->esize  = size;
		ai->length = 1;
	} else {
		struct arrayinfo* old = array;
		old--;

		ai = Cr_Alloc(sizeof(*ai) + old->esize * old->length + size);
		Cr_Copy(ai, old, sizeof(*ai) + old->esize * old->length);

		ai->length++;

		Cr_Free(old);
	}

	return ai + 1;
}

int Cr_ArrayLength(void* array) {
	struct arrayinfo* ai = array;

	if(array == CR_NULL) return 0;
	ai--;

	return ai->length;
}

void Cr_ArrayDestroy(void* array) {
	struct arrayinfo* ai = array;

	if(array == CR_NULL) return;
	ai--;

	Cr_Free(ai);
}

void* Cr_ArrayShrink(void* array, int index) {
	struct arrayinfo* old = array;
	struct arrayinfo* ai;
	int		  i;
	int		  n = 0;

	if(array == CR_NULL) return CR_NULL;
	old--;

	ai = Cr_Alloc(sizeof(*ai) + (old->length - 1) * old->esize);
	Cr_Copy(ai, old, sizeof(*ai));

	for(i = 0; i < ai->length; i++) {
		if(i != index) {
			Cr_Copy((unsigned char*)(ai + 1) + n * ai->esize, (unsigned char*)(old + 1) + i * ai->esize, ai->esize);
			n++;
		}
	}

	ai->length = n;

	Cr_Free(old);

	return ai + 1;
}

void* Cr_ArrayShrinkMatch(void* array, void* element) {
	struct arrayinfo* ai = array;
	int		  esize;
	unsigned char*	  b = array;
	int		  i;

	if(array == CR_NULL) return CR_NULL;
	ai--;

	esize = ai->esize;

	for(i = 0; i < Cr_ArrayLength(array); i++) {
		if(Cr_Equal(b, element, esize)) {
			Cr_ArrayDelete(array, i);
			i = -1;
			b = array;
			continue;
		}

		b += esize;
	}

	return array;
}
