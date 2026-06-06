#include <cr.h>

Cr_AST* Cr_Parse(const char* script) {
	Cr_Token*  t;
	int	   n	    = 0;
	Cr_Token** ts	    = CR_NULL;
	Cr_Token** fl	    = CR_NULL; /* free list */
	Cr_AST*	   top	    = CR_NEW_AST;
	Cr_AST*	   current  = top;
	int	   skip_sep = 0;
	int	   bad	    = 0;
	int	   i;

	top->type = CR_P_PROGRAM;

	while((t = Cr_Lex(script + n)) != CR_NULL) {
		int l	     = Cr_ArrayLength(ts);
		int consumed = 1;
		int j;

		if(t == CR_LEX_ERROR) {
			bad = 1;
			break;
		}

		Cr_ArrayPut(fl, t);

		if(t->type == CR_L_COMMENT) {
			goto skip;
		}

		Cr_ArrayPut(ts, t);

		if(skip_sep && ts[l]->type != CR_L_SEPARATOR) {
			skip_sep = 0;
		}

		if(skip_sep) {
		} else if(ts[l]->type == CR_L_ASSIGN) {
			Cr_Token* lt = CR_NULL;
			Cr_AST*	  parent;

			Cr_Debug("parser: assign\n");

			if(current->type == CR_P_MESSAGE) {
				Cr_AST* d = current;

				current = current->parent;

				for(i = 0; i < Cr_ArrayLength(current->children); i++) {
					if(current->children[i] == d) {
						Cr_ArrayDelete(current->children, i);
						break;
					}
				}

				Cr_DeleteAST(d);
			}

			parent		= current;
			current		= CR_NEW_AST;
			current->type	= CR_P_ASSIGN;
			current->parent = parent;

			Cr_ArrayPut(parent->children, current);

			skip_sep = 1;

			if(bad) break;
		} else if(ts[l]->type == CR_L_PERIOD) {
			current = current->parent;
		} else if(ts[l]->type == CR_L_SEMICOLON) {
			Cr_AST* parent;
			Cr_AST* new_ast;

			current = current->parent;
			if(current == CR_NULL) goto skip;

			parent		= current;
			current		= CR_NEW_AST;
			current->type	= CR_P_MESSAGE;
			current->parent = parent;

			Cr_ArrayPut(parent->children, current);

			new_ast	      = CR_NEW_AST;
			new_ast->type = CR_P_ITEM;
			Cr_Copy(new_ast->token, ";", 1);
			new_ast->parent = current;

			Cr_ArrayPut(current->children, new_ast);
		} else if(ts[l]->type == CR_L_BLOCK_BEGIN) {
			Cr_AST* parent;

			parent		= current;
			current		= CR_NEW_AST;
			current->type	= CR_P_BLOCK;
			current->parent = parent;

			Cr_ArrayPut(parent->children, current);
		} else if(ts[l]->type == CR_L_PAR_BEGIN) {
			Cr_AST* parent;

			parent		= current;
			current		= CR_NEW_AST;
			current->type	= CR_P_GROUP;
			current->parent = parent;

			Cr_ArrayPut(parent->children, current);
		} else if(ts[l]->type == CR_L_BLOCK_END || ts[l]->type == CR_L_PAR_END) {
			while(current->parent != CR_NULL && current->type != (ts[l]->type == CR_L_BLOCK_END ? CR_P_BLOCK : CR_P_GROUP)) current = current->parent;

			current = current->parent;
		} else if(l > 1 && ts[0]->type == CR_L_BAR && ts[l]->type == CR_L_BAR) {
			Cr_Debug("parser: local var\n");

			skip_sep = 1;
		} else if((current->type == CR_P_PROGRAM || current->type == CR_P_BLOCK || current->type == CR_P_GROUP || current->type == CR_P_ASSIGN || current->type == CR_P_MESSAGE) && ts[l]->type != CR_L_SEPARATOR && ts[0]->type != CR_L_BAR) {
			Cr_AST* parent = current;

			if(current->type == CR_P_BLOCK && Cr_ArrayLength(current->children) == 0) {
				if(ts[l]->token[0] == ':') goto skip_arg;
			}

			if(current->type == CR_P_PROGRAM || current->type == CR_P_BLOCK || current->type == CR_P_GROUP) {
				current		= CR_NEW_AST;
				current->type	= CR_P_MESSAGE;
				current->parent = parent;

				Cr_ArrayPut(parent->children, current);
			}

			if(parent != current) {
				for(i = 0; i < Cr_ArrayLength(parent->children); i++) {
					if(parent->children[i]->type != CR_P_MESSAGE) {
						parent->children[i]->parent = current;
						Cr_ArrayPut(current->children, parent->children[i]);

						Cr_ArrayDelete(parent->children, i);

						i--;
					}
				}
			}

			for(i = 0; i <= l; i++) {
				Cr_AST* new_ast;

				if(ts[i]->type == CR_L_SEPARATOR) continue;
				if(ts[i]->type == CR_L_BAR) continue;

				new_ast	      = CR_NEW_AST;
				new_ast->type = CR_P_ITEM;
				Cr_Copy(new_ast->token, ts[i]->token, Cr_Length(ts[i]->token));
				new_ast->parent = current;

				Cr_ArrayPut(current->children, new_ast);
			}
		skip_arg:;
		} else {
			consumed = 0;
		}

		if(consumed) {
			Cr_ArrayFree(ts);
		}

	skip:;
		n += Cr_Length(t->token);

		if(current == CR_NULL) {
			Cr_Debug("parser: out of tree\n");
			bad = 1;
			break;
		}
	}

	Cr_Debug("parser: missed %d tokens\n", Cr_ArrayLength(ts));

	Cr_ArrayFree(ts);

	for(i = 0; i < Cr_ArrayLength(fl); i++) {
		Cr_Free(fl[i]);
	}
	Cr_ArrayFree(fl);

	if(bad) {
		Cr_Debug("parser: error\n");
		Cr_DeleteAST(top);
		top = CR_NULL;
	}

	return top;
}

void Cr_DeleteAST(Cr_AST* root) {
	int i;

	for(i = 0; i < Cr_ArrayLength(root->children); i++) {
		Cr_DeleteAST(root->children[i]);
	}
	Cr_ArrayFree(root->children);

	Cr_Free(root);
}
