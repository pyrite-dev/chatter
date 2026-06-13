#include <cr.h>

struct arrayinfo {
	long esize;  /* element size */
	long length; /* length */
};

void* Cr_ArrayGrow(void* array, long size) {
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

void* Cr_ArrayGrowFrom(void* array, long index, long size) {
	long  i	  = 0;
	long  l	  = Cr_ArrayLength(array);
	void* old = CR_NULL;

	if(l > 0) {
		old = Cr_Alloc(l * size);

		Cr_Copy(old, array, l * size);
	}

	array = Cr_ArrayGrow(array, size);

	for(i = index; i < l; i++) {
		Cr_Copy(((unsigned char*)array) + l * (i + 1), ((unsigned char*)old) + l * i, l);
	}

	if(old != CR_NULL) Cr_Free(old);

	return array;
}

long Cr_ArrayLength(void* array) {
	struct arrayinfo* ai = array;

	if(array == CR_NULL) return 0;
	ai--;

	return ai->length;
}

void Cr_FreeArrayInternal(void* array) {
	struct arrayinfo* ai = array;

	if(array == CR_NULL) return;
	ai--;

	Cr_Free(ai);
}

void* Cr_ArrayDeleteInternal(void* array, long index) {
	struct arrayinfo* old = array;
	struct arrayinfo* ai;
	long		  i;
	long		  n = 0;

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

void* Cr_ArrayDeleteMatchInternal(void* array, void* element) {
	struct arrayinfo* ai = array;
	long		  esize;
	unsigned char*	  b = array;
	long		  i;

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
