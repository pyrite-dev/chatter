#include <cr.h>

#ifdef DEBUG
#include <stdarg.h>
#include <stdio.h>

void Cr_Debug(const char* fmt, ...) {
	va_list va;

	va_start(va, fmt);
	vfprintf(stderr, fmt, va);
	va_end(va);
}

static int* debugAST(int n, int* last, Cr_AST* ast) {
	char	    bar[1024];
	char	    bar2[1024];
	char	    info[1024];
	int	    i;
	const char* g[] = {
	    "",
	    "program",
	    "block",
	    "group",
	    "assign",
	    "local",
	    "message",
	    "item",
	    "array",
	    "byte-array",
	    "block-argument"};

	bar[0]	= 0;
	bar2[0] = 0;
	info[0] = 0;

	if(ast->type == CR_P_ITEM || ast->type == CR_P_MESSAGE || ast->type == CR_P_ASSIGN) {
		Cr_Concat(info, " [");
		Cr_EscapeConcat(info, ast->token);
		Cr_Concat(info, "]");
	}

	for(int i = 0; i < n; i++) {
		if(i == (n - 1)) {
			Cr_Concat(bar, " +-");
		} else if(last[i + 1]) {
			Cr_Concat(bar, "   ");
			Cr_Concat(bar2, "   ");
		} else {
			Cr_Concat(bar, " | ");
			Cr_Concat(bar2, " | ");
		}
	}

	Cr_Debug("parser: %s%s%s\n", bar, g[ast->type], info);
	for(i = 0; i < Cr_ArrayLength(ast->children); i++) {
		last[n + 1] = i == (Cr_ArrayLength(ast->children) - 1);
		debugAST(n + 1, last, ast->children[i]);
	}

	if(n > 0 && last[n] && Cr_ArrayLength(ast->children) == 0) {
		Cr_Debug("parser: %s\n", bar2);
	}
}

void Cr_DebugAST(Cr_AST* root) {
	int n[512];

	n[0] = 0;

	debugAST(0, n, root);
}

static char hex[] = "0123456789ABCDEF";

static void printHex(unsigned char* n, int l) {
	int i;

	for(i = 0; i < l; i++) {
		Cr_Debug("%s%c%c", (i > 0) ? " " : "", hex[(n[i] >> 4) & 0xf], hex[(n[i] >> 0) & 0xf]);
	}
}

static void hexU32(char* out, unsigned int in, int z) {
	int  i;
	char s[2];

	s[1] = 0;

	if(z) Cr_Concat(out, "0x");

	for(i = 0; i < 8; i++) {
		int n = (in >> 28) & 0xf;

		in = in << 4;

		if(z && n == 0) continue;

		z = 0;

		s[0] = hex[n];

		Cr_Concat(out, s);
	}

	if(z) Cr_Concat(out, "0");
}

void Cr_DebugCells(Cr_VM* vm, Cr_Cell* cells, long length) {
	long i;

	for(i = 0; i < length; i++) {
		int	 args	= 0;
		int	 mcells = 0;
		int	 j;
		int	 alen[4];
		int	 s[4];
		Cr_Cell* c	  = &cells[i];
		int	 consumed = 2;
		char	 inst[128];
		int	 z;

		inst[0] = 0;

		alen[0] = 4;
		s[0]	= 1;
		alen[1] = 4;
		s[1]	= 1;
		alen[2] = 4;
		s[2]	= 1;
		alen[3] = 4;
		s[3]	= 1;

		Cr_Debug("compiler: ");

		if(c->i.op == CR_VM_NOP) {
			Cr_Concat(inst, "nop");
		} else if(c->i.op == CR_VM_RET) {
			Cr_Concat(inst, "ret");
		} else if(c->i.op == CR_VM_CALL) {
			args   = 1;
			mcells = 2;
			s[0]   = 0;

			Cr_Concat(inst, "call ");

			hexU32(inst, c->i.a1, 1);
			Cr_Concat(inst, ", ");

			hexU32(inst, Cr_BigU32(vm, cells[i + 1].u32), 1);
			hexU32(inst, Cr_BigU32(vm, cells[i + 2].u32), 0);
		}

		printHex(c->d, 1);

		for(j = 0; j < args; j++) {
			Cr_Debug("  ");
			printHex(c->d + j + 1, 1);

			consumed += 4;
		}

		for(j = 0; j < mcells; j++) {
			Cr_Debug("%s ", (j == 0 || s[j - 1]) ? " " : "");
			printHex(cells[i + j + 1].d + (4 - alen[j]), alen[j]);

			consumed++;
			if(j == 0 || s[j - 1]) consumed++;
			consumed += alen[j] * 2 + (alen[j] - 1);
		}
		i += mcells;

		for(j = 0; j < (48 - consumed); j++) Cr_Debug(" ");

		Cr_Debug("%s\n", inst);
	}
}
#endif
