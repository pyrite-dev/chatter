#include <cr.h>

Cr_AST* Cr_Parse(Cr_AST* parent, const char* script) {
	Cr_Token*  t;
	int	   n  = 0;
	Cr_Token** ts = CR_NULL;

	while((t = Cr_Lex(script + n)) != CR_NULL) {
		int l = Cr_ArrayLength(ts);

		Cr_ArrayPut(ts, t);

		if(ts[l]->type == CR_PERIOD) {
			Cr_Debug("parser: saw period\n");

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
