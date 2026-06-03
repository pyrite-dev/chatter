#include <cr.h>

Cr_Token* Cr_Lex(const char* str){
	Cr_Token* t;
	
	t = Cr_Alloc(sizeof(*t));
	Cr_Copy(t->token, str, strlen(str));

	return t;
}
