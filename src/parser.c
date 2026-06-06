#include <cr.h>

#define NEW_AST Cr_Alloc(sizeof(Cr_AST))

Cr_AST* Cr_Parse(Cr_AST* parent, const char* script) {
	Cr_Token*  t;
	int	   n	    = 0;
	Cr_Token** ts	    = CR_NULL;
	Cr_Token** fl	    = CR_NULL; /* free list */
	Cr_AST*	   top	    = NEW_AST;
	Cr_AST*	   current  = top;
	int	   skip_sep = 0;
	int	   bad	    = 0;
	int	   i;

	top->type = CR_P_PROGRAM;

	while((t = Cr_Lex(script + n)) != CR_NULL) {
		int l	     = Cr_ArrayLength(ts);
		int consumed = 1;
		int j;

		Cr_ArrayPut(ts, t);
		Cr_ArrayPut(fl, t);

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
			current		= NEW_AST;
			current->type	= CR_P_ASSIGN;
			current->parent = parent;

			Cr_ArrayPut(parent->children, current);

			skip_sep = 1;

			if(bad) break;
		} else if(ts[l]->type == CR_L_PERIOD) {
			current = current->parent;
		} else if(ts[l]->type == CR_L_BLOCK_BEGIN) {
			Cr_AST* parent;

			parent		= current;
			current		= NEW_AST;
			current->type	= CR_P_BLOCK;
			current->parent = parent;

			Cr_ArrayPut(parent->children, current);
		} else if(ts[l]->type == CR_L_BLOCK_END) {
			current = current->parent;
		} else if(l > 1 && ts[0]->type == CR_L_BAR && ts[l]->type == CR_L_BAR) {
			Cr_Debug("parser: local var\n");

			skip_sep = 1;
		} else if((current->type == CR_P_PROGRAM || current->type == CR_P_ASSIGN || current->type == CR_P_MESSAGE) && ts[0]->type != CR_L_BAR) {
			if(current->type == CR_P_PROGRAM) {
				Cr_AST* parent;

				parent		= current;
				current		= NEW_AST;
				current->type	= CR_P_MESSAGE;
				current->parent = parent;

				Cr_ArrayPut(parent->children, current);
			}

			for(i = 0; i <= l; i++) {
				Cr_AST* new;

				if(ts[i]->type == CR_L_SEPARATOR) continue;

				new	    = NEW_AST;
				new->type   = CR_P_ITEM;
				new->parent = current;

				Cr_ArrayPut(current->children, new);
			}
		} else {
			consumed = 0;
		}

		if(consumed) {
			Cr_ArrayFree(ts);
		}

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
		free(fl[i]);
	}
	Cr_ArrayFree(fl);

	if(bad) {
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

	free(root);
}
