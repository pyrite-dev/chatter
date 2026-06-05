#include <cr.h>

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

	if(t == CR_NULL && CR_IS_SEPARATOR(str[0])) {
		t = Cr_Alloc(sizeof(*t));

		for(i = 0; str[i] != 0; i++) {
			if(!CR_IS_SEPARATOR(str[i])) break;
		}

		t->type = CR_SEPARATOR;
		Cr_Copy(t->token, str, i);
	}

	IF_MATCH(":=", CR_ASSIGN);
	IF_MATCH("|", CR_BAR);
	IF_MATCH(".", CR_PERIOD);
	IF_MATCH("#[", CR_BYTE_ARRAY_BEGIN);
	IF_MATCH("[", CR_BLOCK_BEGIN);
	IF_MATCH("]", CR_BLOCK_END);
	IF_MATCH("#(", CR_ARRAY_BEGIN);
	IF_MATCH("(", CR_PAR_BEGIN);
	IF_MATCH(")", CR_PAR_END);

	/* NOTE: appearantly some implementation allows newline after $... */
	if(t == CR_NULL && str[0] == '$' && str[1] != '\r' && str[1] != '\n') {
		t = Cr_Alloc(sizeof(*t));

		t->type = CR_CHAR;
		Cr_Copy(t->token, str, 2);
	}

	if(t == CR_NULL && ((str[0] == '-' && CR_IS_NUMBER(str[1])) || CR_IS_NUMBER(str[0]))) {
		int dot	  = 0;
		int radix = 0;
		int exp	  = 0;

		t = Cr_Alloc(sizeof(*t));

		for(i = 1; str[i] != 0; i++) {
			if(dot == 0 && radix == 0 && str[i] == '.' && !CR_IS_SEPARATOR(str[i + 1])) {
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
			if(CR_IS_NUMBER(str[i])) continue;
			break;
		}

		t->type = CR_NUMBER;
		Cr_Copy(t->token, str, i);
	}

	if(t == CR_NULL && (str[0] == '\'' || str[0] == '"')) {
		int q = 0;

		t = Cr_Alloc(sizeof(*t));

		for(i = 0; str[i] != 0; i++) {
			if(str[i] == str[0]) {
				q = !q;

				if(!q) {
					i++;
					break;
				}
			}
		}

		t->type = str[0] == '\'' ? CR_STRING : CR_COMMENT;
		Cr_Copy(t->token, str, i);
	}

	if(t == CR_NULL && str[0] == '#' && (CR_CAN_BE_FIRST(str[1]) || str[1] == '\'')) {
		int q = 0;

		t = Cr_Alloc(sizeof(*t));

		for(i = 1; str[i] != 0; i++) {
			if(str[1] == '\'' && str[i] == '\'') {
				q = !q;

				if(!q) {
					i++;
					break;
				}
			} else if(str[1] != '\'' && !(CR_IS_ALPHASYM(str[i]) || CR_IS_NUMBER(str[i]))) {
				break;
			}
		}

		t->type = CR_SYMBOL;
		Cr_Copy(t->token, str, i);
	}

	if(t == CR_NULL && CR_CAN_BE_FIRST(str[0])) {
		t = Cr_Alloc(sizeof(*t));

		for(i = 1; str[i] != 0; i++) {
			if(CR_IS_ALPHASYM(str[i])) continue;
			if(CR_IS_NUMBER(str[i])) continue;
			break;
		}

		t->type = CR_IDENT;
		Cr_Copy(t->token, str, i);
	}

	if(t == CR_NULL && str[0] == ':' && CR_CAN_BE_FIRST(str[1])) {
		t = Cr_Alloc(sizeof(*t));

		for(i = 2; str[i] != 0; i++) {
			if(CR_IS_ALPHASYM(str[i])) continue;
			if(CR_IS_NUMBER(str[i])) continue;
			break;
		}

		t->type = CR_BLOCK_ARG;
		Cr_Copy(t->token, str, i);
	}

	if(t == CR_NULL && CR_IS_SYMBOL(str[0])) {
		t = Cr_Alloc(sizeof(*t));

		t->type = CR_IDENT;
		Cr_Copy(t->token, str, 1);
	}

	if(t == CR_NULL) {
		Cr_Debug("lexer: error token\n");
	} else {
		Cr_Debug("lexer: %d [%s]\n", t->type, t->token);
	}

	return t;
}
