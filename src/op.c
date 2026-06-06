#include <cr.h>

int Cr_IsUnary(const char* op) {
	int i;

	for(i = 0; op[i] != 0; i++) {
		if(i == 0 && CR_IS_ALPHA(op[i])) continue;
		if(i > 0 && CR_IS_ALPHANUM(op[i])) continue;
		return 0;
	}

	return 1;
}

int Cr_IsBinary(const char* op) {
	int i;

	for(i = 0; op[i] != 0; i++) {
		if(!CR_IS_SYMBOL(op[i])) return 0;
	}

	return 1;
}

int Cr_IsKeyword(const char* op) {
	int i;

	for(i = 0; op[i] != 0; i++) {
		if(i == 0 && CR_IS_ALPHA(op[i])) continue;
		if(i > 0 && CR_IS_ALPHANUM(op[i])) continue;
		if(i == (Cr_Length(op) - 1) && op[i] == ':') continue;
		return 0;
	}

	return op[Cr_Length(op) - 1] == ':';
}

void Cr_SortMsgRecv(Cr_AST* ast) {
	int i;

	if(ast->type == CR_P_MESSAGE) {
		Cr_AST* receiver;

		if(Cr_ArrayLength(ast->children) < 1) return;

		/* unary */
		receiver = ast->children[0];
		for(i = 1; i < Cr_ArrayLength(ast->children); i++) {
			if(Cr_ArrayLength(ast->children) <= 2) break;
			if(Cr_IsUnary(ast->children[i]->token)) {
				Cr_AST* old = receiver;
				Cr_AST* c   = ast->children[i];
				int	j;

				receiver	 = CR_NEW_AST;
				receiver->type	 = CR_P_MESSAGE;
				receiver->parent = old->parent;

				Cr_ArrayPut(receiver->children, old);
				Cr_ArrayPut(receiver->children, c);

				Cr_ArrayDeleteMatch(receiver->parent->children, c);
				for(j = 0; j < Cr_ArrayLength(receiver->parent->children); j++) {
					if(receiver->parent->children[j] == old) {
						receiver->parent->children[j] = receiver;
						break;
					}
				}

				old->parent = receiver;
				c->parent   = receiver;

				i--;
			}
		}

		/* binary */
		receiver = ast->children[0];
		for(i = 1; i < Cr_ArrayLength(ast->children); i += 2) {
			if(Cr_ArrayLength(ast->children) <= 3) break;

			if(Cr_IsBinary(ast->children[i]->token)) {
				Cr_AST* old = receiver;
				Cr_AST* c   = ast->children[i];
				Cr_AST* c2  = ast->children[i + 1];
				int	j;

				receiver	 = CR_NEW_AST;
				receiver->type	 = CR_P_MESSAGE;
				receiver->parent = old->parent;

				Cr_ArrayPut(receiver->children, old);
				Cr_ArrayPut(receiver->children, c);
				Cr_ArrayPut(receiver->children, c2);

				Cr_ArrayDeleteMatch(receiver->parent->children, c);
				Cr_ArrayDeleteMatch(receiver->parent->children, c2);
				for(j = 0; j < Cr_ArrayLength(receiver->parent->children); j++) {
					if(receiver->parent->children[j] == old) {
						receiver->parent->children[j] = receiver;
						break;
					}
				}

				old->parent = receiver;
				c->parent   = receiver;
				c2->parent  = receiver;

				i -= 2;
			}
		}
	}

	for(i = 0; i < Cr_ArrayLength(ast->children); i++) {
		Cr_SortMsgRecv(ast->children[i]);
	}
}
