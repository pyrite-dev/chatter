#include <cr.h>

struct rule {
	char token[CR_TOKSZ];
	int  match;
};

#define MATCH(x) ((Cr_Length(str) >= strlen((x))) && Cr_Equal(str, (x), strlen((x))))
#define IF_MATCH(x, y) \
	if(t == CR_NULL && MATCH((x))) { \
		t = Cr_Alloc(sizeof(*t)); \
\
		t->type = (y); \
		Cr_Copy(t->token, (x), Cr_Length((x))); \
	}

Cr_Token* Cr_Lex(const char* str) {
	int	  i;
	Cr_Token* t = CR_NULL;

	if(strlen(str) == 0) return t;

	if(t == CR_NULL && (str[0] == ' ' || str[0] == '\t' || str[0] == '\r' || str[0] == '\n')) {
		t = Cr_Alloc(sizeof(*t));

		for(i = 0; str[i] != 0; i++) {
			if((str[0] == ' ' || str[0] == '\t') && str[i] != ' ' && str[i] != '\t') break;
			if((str[0] == '\r' || str[0] == '\n') && str[i] != '\r' && str[i] != '\n') break;
		}

		if(str[0] == ' ' || str[0] == '\t') {
			t->type = CR_SPACE;
		} else {
			t->type = CR_NEWLINE;
		}
		Cr_Copy(t->token, str, i);
	}

	IF_MATCH(":=", CR_ASSIGN);
	IF_MATCH("|", CR_LOCAL);
	IF_MATCH(".", CR_PERIOD);

	if(t == CR_NULL && str[0] == '$' && str[1] != '\r' && str[1] != '\n') {
		t = Cr_Alloc(sizeof(*t));

		t->type = CR_CHAR;
		Cr_Copy(t->token, str, 2);
	}

	if(t == CR_NULL && ((str[0] == '-' && CR_NUMBER(str[1])) || CR_NUMBER(str[0]))) {
		int dot	  = 0;
		int radix = 0;
		int exp	  = 0;

		t = Cr_Alloc(sizeof(*t));

		for(i = 1; str[i] != 0; i++) {
			if(dot == 0 && radix == 0 && str[i] == '.') {
				dot++;
				continue;
			}
			if(exp == 0 && radix == 0 && str[i] == 'e') {
				exp++;
				continue;
			}
			if(dot == 0 && radix == 0 && str[i] == 'r') {
				radix++;
				continue;
			}
			if(CR_NUMBER(str[i])) continue;
			break;
		}

		t->type = CR_NUMBER;
		Cr_Copy(t->token, str, i);
	}

	if(t == CR_NULL && str[0] == '\'') {
		int q = 0;

		t = Cr_Alloc(sizeof(*t));

		for(i = 0; str[i] != 0; i++) {
			if(str[i] == '\'') {
				q = !q;

				if(!q) {
					i++;
					break;
				}
			}
		}

		t->type = CR_STRING;
		Cr_Copy(t->token, str, i);
	}

	if(t == CR_NULL && CR_ALPHA(str[0])) {
		t = Cr_Alloc(sizeof(*t));

		for(i = 1; str[i] != 0; i++) {
			if(CR_ALPHASYM(str[i])) continue;
			if(CR_NUMBER(str[i])) continue;
			break;
		}

		t->type = CR_IDENT;
		Cr_Copy(t->token, str, i);
	}

	if(t == CR_NULL && CR_SYMBOL(str[0])) {
		t = Cr_Alloc(sizeof(*t));

		t->type = CR_IDENT;
		Cr_Copy(t->token, str, 1);
	}

	return t;
}
