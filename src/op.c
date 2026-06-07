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

int Cr_IsReceiver(const char* op) {
	return !(Cr_IsBinary(op) || Cr_IsKeyword(op));
}

int Cr_SortAndCleanMsgRecv(Cr_AST* ast) {
	int i;

	for(i = 0; i < Cr_ArrayLength(ast->children); i++) {
		if(ast->children[i]->type == CR_P_MESSAGE && Cr_ArrayLength(ast->children[i]->children) == 0) {
			Cr_ArrayDelete(ast->children, i);
			i--;
		} else {
			if(Cr_SortAndCleanMsgRecv(ast->children[i]) == CR_ERROR) {
				return CR_ERROR;
			}
		}
	}

	if(ast->type == CR_P_MESSAGE || ast->type == CR_P_ASSIGN) {
		Cr_AST* old;
		Cr_AST* receiver;
		int	m;
		int	cond = 0;
		int	f    = 1;

		if(Cr_ArrayLength(ast->children) < 1) return CR_OK;

		/* unary */
		for(i = 1; i < Cr_ArrayLength(ast->children); i++) {
			if(Cr_ArrayLength(ast->children) <= 1) break;

			receiver = ast->children[i - 1];
			if(receiver->type != CR_P_BLOCK && receiver->type != CR_P_GROUP && receiver->type != CR_P_MESSAGE && !Cr_IsReceiver(receiver->token)) continue;
			if(Cr_IsUnary(ast->children[i]->token)) {
				Cr_AST* c = ast->children[i];

				old = receiver;

				receiver       = CR_NEW_AST;
				receiver->type = CR_P_MESSAGE;
				Cr_Copy(receiver->token, c->token, Cr_Length(c->token));
				receiver->parent = ast;

				Cr_ArrayPut(receiver->children, old);

				ast->children[i - 1] = receiver;

				Cr_ArrayDeleteMatch(receiver->parent->children, c);

				Cr_DeleteAST(c);

				i--;
			}
		}

		/* binary/keyword */
		for(m = 0; m < 2; m++) {
			for(i = 1; i < Cr_ArrayLength(ast->children); i += 2) {
				if(Cr_ArrayLength(ast->children) <= 2) break;

				receiver = ast->children[i - 1];
				if(receiver->type != CR_P_BLOCK && receiver->type != CR_P_GROUP && receiver->type != CR_P_MESSAGE && !Cr_IsReceiver(receiver->token)) continue;
				if((m == 0 ? Cr_IsBinary : Cr_IsKeyword)(ast->children[i]->token)) {
					Cr_AST* c = ast->children[i];
					Cr_AST* a = ast->children[i + 1];
					int	n = 0;

					old = receiver;

					if(m == 0 || f) {
						receiver = CR_NEW_AST;

						n = 1;
						if(m == 1) f = 0;
					}
					receiver->type = CR_P_MESSAGE;
					if(m == 0) {
						Cr_Copy(receiver->token, c->token, Cr_Length(c->token));

					} else {
						Cr_Concat(receiver->token, c->token);
					}
					receiver->parent = ast;

					if(n) Cr_ArrayPut(receiver->children, old);

					Cr_ArrayPut(receiver->children, a);

					if(n) ast->children[i - 1] = receiver;

					Cr_ArrayDeleteMatch(receiver->parent->children, c);
					Cr_ArrayDeleteMatch(receiver->parent->children, a);

					Cr_DeleteAST(c);

					i -= 2;
				}
			}
		}

		cond = cond || ast->children[0]->type == CR_P_BLOCK;
		cond = cond || ast->children[0]->type == CR_P_GROUP;
		cond = cond || ast->children[0]->type == CR_P_MESSAGE;
		cond = cond || ast->children[0]->type == CR_P_ITEM;
		cond = cond || ast->children[0]->type == CR_P_ARRAY;
		cond = cond || ast->children[0]->type == CR_P_BYTE_ARRAY;
		cond = cond && ast->type == CR_P_MESSAGE && Cr_ArrayLength(ast->children) == 1;

		if(cond) {
			old = ast->children[0];
			Cr_Copy(ast, old, sizeof(*ast));
			Cr_Free(old);
		}
	}

	return CR_OK;
}
