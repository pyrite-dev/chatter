#include <cr.h>
#include <stdio.h>

CR_NEW_HASHMAP(map,
	       char* key;
	       int   value;);

int main(int argc, char** argv) {
	Cr_VM* vm;
	FILE*  f;
	char*  src;
	int    sz;

	struct map* h = NULL;
	char*	    k;
	int	    v;

	k = "Hello";
	v = 123;

	Cr_HashMapPut(h, k, v);

	printf("%d\n", Cr_HashMapGet(h, k)->value);

	return 0;

	if(argc != 2 || (f = fopen(argv[1], "rb")) == NULL) {
		fprintf(stderr, "Usage: %s input\n", argv[0]);
		return 1;
	}

	fseek(f, 0, SEEK_END);
	sz = ftell(f);
	fseek(f, 0, SEEK_SET);

	src = malloc(sz + 1);
	fread(src, 1, sz, f);
	fclose(f);

	src[sz] = 0;

	vm = Cr_CreateVM(CR_MEM_STD);
	Cr_Eval(vm, src);
	Cr_DeleteVM(vm);

	free(src);
}
