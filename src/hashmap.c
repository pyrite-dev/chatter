#include <crPrivate.h>
#include <cr.h>

#define INITIAL_CAPACITY 8

struct hashmapinfo {
	unsigned long  capacity;
	long	       length;
	unsigned long* indices;
};

void* Cr_HashMapGetInternal(void* hashmap, long size, const void* key, long kstart, long ksize, long ustart, long cstart) {
	struct hashmapinfo* hi = hashmap;
	unsigned char*	    mem;
	long		    ind;
	const void*	    n = CR_NULL;

	if(hashmap == CR_NULL) return CR_NULL;
	hi--;

	mem = hashmap;

	ind = Cr_Hash(key, ksize) % hi->capacity;

	mem = &mem[size * ind];

	if(!mem[ustart]) return CR_NULL;

	while(mem[ustart] && !Cr_Equal(mem + kstart, key, ksize)) {
		if(Cr_Equal(mem + cstart, &n, sizeof(n))) {
			return CR_NULL;
		}

		mem = *(unsigned char**)&mem[cstart];
	}

	return mem;
}

void* Cr_HashMapPutInternal(void* hashmap, long size, const void* key, long kstart, long ksize, const void* value, long vstart, long vsize, long ustart, long usize, long cstart) {
	struct hashmapinfo* hi;
	unsigned char*	    mem;
	unsigned char*	    mem2;
	unsigned long	    ind;
	long		    i, j;
	const void*	    n = CR_NULL;

	if(hashmap == CR_NULL) {
		hi = Cr_Alloc(sizeof(*hi) + size * INITIAL_CAPACITY);

		hi->capacity = INITIAL_CAPACITY;
		hi->length   = 1;
	} else {
		struct hashmapinfo* old = hashmap;
		old--;

		if((double)(old->length + 1) / old->capacity > 0.75) {
			hi = Cr_Alloc(sizeof(*hi) + size * old->capacity * 2);

			hi->capacity = old->capacity * 2;
			hi->length   = old->length;
			hi->indices  = CR_NULL;

			for(i = 0; i < Cr_ArrayLength(old->indices); i++) {
				mem = (unsigned char*)(old + 1);
				mem = &mem[size * old->indices[i]];

				do {
					ind = Cr_Hash(&mem[kstart], ksize) % hi->capacity;

					mem2 = (unsigned char*)(hi + 1);
					mem2 = &mem2[size * ind];

					while(mem2[ustart]) {
						if(Cr_Equal(mem2 + cstart, &n, sizeof(n))) {
							void* ptr = Cr_Alloc(size);

							Cr_Copy(&mem2[cstart], &ptr, sizeof(ptr));

							mem2 = *(unsigned char**)&mem2[cstart];

							break;
						}

						mem2 = *(unsigned char**)&mem2[cstart];
					}

					Cr_Debug("from %2d: ", old->indices[i]);
					for(j = 0; j < size; j++) Cr_Debug("%02x ", mem[j]);
					Cr_Debug("\n");

					Cr_Copy(mem2, mem, size);
					Cr_Copy(mem2 + cstart, &n, sizeof(n));

					Cr_Debug("  to %2d: ", ind);
					for(j = 0; j < size; j++) Cr_Debug("%02x ", mem2[j]);
					Cr_Debug("\n");

					Cr_Debug("\n");

					Cr_ArrayDeleteMatch(hi->indices, ind);
					Cr_ArrayPut(hi->indices, ind);

					Cr_Copy(&mem, mem + cstart, sizeof(mem));
				} while(mem != CR_NULL);
			}

			Cr_FreeArray(old->indices);
			Cr_Free(old);
		} else {
			hi = hashmap;
			hi--;
		}

		hi->length++;
	}

	mem = (unsigned char*)(hi + 1);

	ind = Cr_Hash(key, ksize) % hi->capacity;

	Cr_ArrayDeleteMatch(hi->indices, ind);
	Cr_ArrayPut(hi->indices, ind);

	mem = &mem[size * ind];

	while(mem[ustart] && !Cr_Equal(mem + kstart, key, ksize)) {
		if(Cr_Equal(mem + cstart, &n, sizeof(n))) {
			void* ptr = Cr_Alloc(size);

			Cr_Copy(&mem[cstart], &ptr, sizeof(ptr));

			mem = *(unsigned char**)&mem[cstart];
			break;
		}

		mem = *(unsigned char**)&mem[cstart];
	}

	Cr_Copy(mem + kstart, key, ksize);
	Cr_Copy(mem + vstart, value, vsize);

	for(i = 0; i < usize; i++) mem[ustart + i] = 0xff;

	return hi + 1;
}

long Cr_HashMapLengthInternal(void* hashmap, long size, long ustart, long cstart) {
	struct hashmapinfo* hi	= hashmap;
	unsigned char*	    mem = hashmap;
	long		    i	= 0;
	long		    l	= 0;
	const void*	    n	= CR_NULL;

	if(hashmap == CR_NULL) return 0;
	hi--;

	for(i = 0; i < hi->length; i++) {
		while(mem[ustart]) {
			l++;

			if(Cr_Equal(mem + cstart, &n, sizeof(n))) {
				break;
			}

			mem = *(unsigned char**)&mem[cstart];
		}

		mem += size;
	}

	return l;
}

void* Cr_HashMapGetAllInternal(void* hashmap, long size, long ustart, long cstart) {
	struct hashmapinfo* hi	= hashmap;
	unsigned char*	    r	= CR_NULL;
	unsigned char*	    mem = hashmap;
	long		    i	= 0;
	long		    l	= 0;
	const void*	    n	= CR_NULL;

	if(hashmap == CR_NULL) return CR_NULL;
	if((l = Cr_HashMapLengthInternal(hashmap, size, ustart, cstart)) == 0) return CR_NULL;

	r = Cr_Alloc(l * size);
	l = 0;

	hi--;

	for(i = 0; i < hi->length; i++) {
		while(mem[ustart]) {
			Cr_Copy(&r[(l++) * size], mem, size);

			if(Cr_Equal(mem + cstart, &n, sizeof(n))) {
				break;
			}

			mem = *(unsigned char**)&mem[cstart];
		}

		mem += size;
	}

	return r;
}

void Cr_FreeHashMapInternal(void* hashmap) {
	struct hashmapinfo* hi = hashmap;

	if(hashmap == CR_NULL) return;
	hi--;

	Cr_FreeArray(hi->indices);
	Cr_Free(hi);
}
