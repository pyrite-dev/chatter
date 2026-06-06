#include <cr.h>
#include <stdio.h>

int main(int argc, char** argv) {
	Cr_Interp* interp;
	FILE*	   f;
	char*	   src;
	int	   sz;

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

	interp = Cr_CreateInterp();
	Cr_Eval(interp, src);
	Cr_DeleteInterp(interp);

	free(src);
}
