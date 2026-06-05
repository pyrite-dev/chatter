#include <cr.h>

Cr_AST* Cr_Parse(Cr_AST* parent, const char* script) {
	Cr_Token*  t;
	int	   n  = 0;
	Cr_Token** ts = CR_NULL;

	while((t = Cr_Lex(script + n)) != CR_NULL) {
		int l = Cr_ArrayLength(ts);

		Cr_ArrayPut(ts, t);

		if(ts[l]->type == CR_PERIOD) {
			int i;
			int bad	   = 0;
			int assign = 0;

			Cr_Debug("parser: saw period\n");

			for(i = 0; i < Cr_ArrayLength(ts) - 1; i++) {
				if(ts[i]->type == CR_ASSIGN) {
					int j;

					Cr_Debug("parser: assign\n");
					assign = 1;

					for(j = 0; j < i; j++) {
						if(ts[j]->type == CR_IDENT || ts[j]->type == CR_SEPARATOR) continue;

						Cr_Debug("parser: illegal assign pos\n");
						bad = 1;

						break;
					}

					break;
				}
			}

			if(bad) break;

			if(!assign) {
				Cr_Debug("parser: receiver-message\n");
			}

			Cr_ArrayFree(ts);
		} else if(l > 1 && ts[0]->type == CR_BAR && ts[l]->type == CR_BAR) {
			Cr_Debug("parser: local var\n");

			Cr_ArrayFree(ts);
		}

		n += Cr_Length(t->token);
	}

	Cr_Debug("parser: missed %d tokens\n", Cr_ArrayLength(ts));

	Cr_ArrayFree(ts);
}
