#include <cr.h>

#define MATCH(x) ((Cr_Length(str) >= Cr_Length((x))) && Cr_Equal(str, (x), Cr_Length((x))))
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

	if(Cr_Length(str) == 0) return t;

	if(t == CR_NULL && CR_IS_SEPARATOR(str[0])) {
		for(i = 0; str[i] != 0; i++) {
			if(!CR_IS_SEPARATOR(str[i])) break;
		}

		if(i >= CR_TOKSZ) goto err;

		t = Cr_Alloc(sizeof(*t));

		t->type = CR_L_SEPARATOR;
		Cr_Copy(t->token, str, i);
	}

	IF_MATCH(":=", CR_L_ASSIGN);
	IF_MATCH("|", CR_L_BAR);
	IF_MATCH(".", CR_L_PERIOD);
	IF_MATCH(";", CR_L_SEMICOLON);
	IF_MATCH("#[", CR_L_BYTE_ARRAY_BEGIN);
	IF_MATCH("[", CR_L_BLOCK_BEGIN);
	IF_MATCH("]", CR_L_BLOCK_END);
	IF_MATCH("#(", CR_L_ARRAY_BEGIN);
	IF_MATCH("(", CR_L_PAR_BEGIN);
	IF_MATCH(")", CR_L_PAR_END);

	/* NOTE: appearantly some implementation allows newline after $... */
	if(t == CR_NULL && str[0] == '$' && str[1] != '\r' && str[1] != '\n') {
		t = Cr_Alloc(sizeof(*t));

		t->type = CR_L_CHAR;
		Cr_Copy(t->token, str, 2);
	}

	if(t == CR_NULL && ((str[0] == '-' && CR_IS_NUMBER(str[1])) || CR_IS_NUMBER(str[0]))) {
		int dot	  = 0;
		int radix = 0;
		int exp	  = 0;

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

		if(i >= CR_TOKSZ) goto err;

		t = Cr_Alloc(sizeof(*t));

		t->type = CR_L_NUMBER;
		Cr_Copy(t->token, str, i);
	}

	if(t == CR_NULL && (str[0] == '\'' || str[0] == '"')) {
		int q = 0;

		for(i = 0; str[i] != 0; i++) {
			if(str[i] == str[0]) {
				q = !q;

				if(!q) {
					i++;
					break;
				}
			}
		}

		if(i >= CR_TOKSZ) goto err;

		t = Cr_Alloc(sizeof(*t));

		t->type = str[0] == '\'' ? CR_L_STRING : CR_L_COMMENT;
		Cr_Copy(t->token, str, i);
	}

	if(t == CR_NULL && str[0] == '#' && (CR_CAN_BE_FIRST(str[1]) || str[1] == '\'')) {
		int q = 0;

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

		if(i >= CR_TOKSZ) goto err;

		t = Cr_Alloc(sizeof(*t));

		t->type = CR_L_SYMBOL;
		Cr_Copy(t->token, str, i);
	}

	if(t == CR_NULL && CR_CAN_BE_FIRST(str[0])) {
		for(i = 1; str[i] != 0; i++) {
			if(CR_IS_ALPHASYM(str[i])) continue;
			if(CR_IS_NUMBER(str[i])) continue;
			break;
		}

		if(i >= CR_TOKSZ) goto err;

		t = Cr_Alloc(sizeof(*t));

		t->type = CR_L_IDENT;
		Cr_Copy(t->token, str, i);
	}

	if(t == CR_NULL && str[0] == ':' && CR_CAN_BE_FIRST(str[1])) {
		for(i = 2; str[i] != 0; i++) {
			if(CR_IS_ALPHASYM(str[i])) continue;
			if(CR_IS_NUMBER(str[i])) continue;
			break;
		}

		if(i >= CR_TOKSZ) goto err;

		t = Cr_Alloc(sizeof(*t));

		t->type = CR_L_BLOCK_ARG;
		Cr_Copy(t->token, str, i);

		Cr_Debug("%s\n", t->token);
	}

	if(t == CR_NULL && CR_IS_SYMBOL(str[0])) {
		t = Cr_Alloc(sizeof(*t));

		t->type = CR_L_IDENT;
		Cr_Copy(t->token, str, 1);
	}

err:;
	if(t == CR_NULL) {
		t = CR_LEX_ERROR;
		Cr_Debug("lexer: error token\n");
	} else {
		Cr_Debug("lexer: %d [%s]\n", t->type, t->token);
	}

	return t;
}
