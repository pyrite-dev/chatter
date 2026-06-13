#include <crPrivate.h>
#include <cr.h>

#ifdef DEBUG
#define MAKE_VALGRIND_QUIET
#endif

#ifdef MAKE_VALGRIND_QUIET
#define N 1
#else
#define N 0
#endif

struct arrayinfo {
	CR_SIZE_T esize;  /* element size */
	CR_SIZE_T length; /* length */
};

void* Cr_ArrayGrow(void* array, CR_SIZE_T size) {
	struct arrayinfo* ai;
	if(array == CR_NULL) {
		ai	   = Cr_Alloc(sizeof(*ai) + size * (N + 1));
		ai->esize  = size;
		ai->length = 1;
	} else {
		struct arrayinfo* old = array;
		old--;

		ai = Cr_Alloc(sizeof(*ai) + old->esize * old->length + size * (N + 1));
		Cr_Copy(ai, old, sizeof(*ai) + old->esize * old->length);

		ai->length++;

		Cr_Free(old);
	}

	return ai + 1;
}

void* Cr_ArrayGrowFrom(void* array, CR_SIZE_T index, CR_SIZE_T size) {
	CR_SIZE_T i   = 0;
	CR_SIZE_T l   = Cr_ArrayLength(array);
	void*	  old = CR_NULL;

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

CR_SIZE_T Cr_ArrayLength(void* array) {
	struct arrayinfo* ai = array;

	if(array == CR_NULL) return 0;
	ai--;

	return ai->length;
}

void* Cr_ArrayDeleteInternal(void* array, CR_SIZE_T index) {
	struct arrayinfo* old = array;
	struct arrayinfo* ai;
	CR_SIZE_T	  i;
	CR_SIZE_T	  n = 0;

	if(array == CR_NULL) return CR_NULL;
	old--;

	ai = Cr_Alloc(sizeof(*ai) + (old->length - 1 + N) * old->esize);
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
	CR_SIZE_T	  esize;
	unsigned char*	  b = array;
	CR_SIZE_T	  i;

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

void Cr_FreeArrayInternal(void* array) {
	struct arrayinfo* ai = array;

	if(array == CR_NULL) return;
	ai--;

	Cr_Free(ai);
}
