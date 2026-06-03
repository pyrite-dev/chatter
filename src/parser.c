#include <cr.h>

Cr_AST* Cr_Parse(const char* script){
	Cr_Token* t;
	int		 n = 0;
	Cr_Token** ts = CR_NULL;

	while((t = Cr_Lex(script + n)) != CR_NULL) {
		Cr_ArrayPut(ts, t);

		n += Cr_Length(t->token);
	}

	Cr_ArrayFree(ts);
}
