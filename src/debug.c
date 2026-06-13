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

static void hexU32(char* out, unsigned int in, int* z, int* p) {
	int  i;
	char s[2];
	int  _z = 1;

	if(z == CR_NULL) z = &_z;

	s[1] = 0;

	if(p == CR_NULL || *p) Cr_Concat(out, "0x");

	for(i = 0; i < 8; i++) {
		int n = (in >> 28) & 0xf;

		in = in << 4;

		if(*z && n == 0) continue;

		*z = 0;

		s[0] = hex[n];

		Cr_Concat(out, s);
	}

	if(*z) Cr_Concat(out, "0");

	if(p != CR_NULL) *p = 0;
}

void Cr_DebugCells(Cr_VM* vm, Cr_Cell* cells, long length) {
	long i;

	for(i = 0; i < length; i++) {
		int	 args	= 0;
		int	 mcells = 0;
		int	 j;
		int	 alen[4];
		int	 aspace[4];
		int	 s[4];
		Cr_Cell* c	  = &cells[i];
		int	 consumed = 2;
		char	 inst[128];
		int	 z;

		inst[0] = 0;

		for(j = 0; j < sizeof(alen) / sizeof(alen[0]); j++) {
			alen[j] = aspace[j] = 4;
			s[j]		    = 1;
		}

		Cr_Debug("compiler: ");

		if(c->i.op == CR_VM_NOP) {
			Cr_Concat(inst, "nop");
		} else if(c->i.op == CR_VM_RET) {
			Cr_Concat(inst, "ret");
		} else if(c->i.op == CR_VM_CALL) {
			int z = 1, p = 1;

			args   = 1;
			mcells = 2;
			s[0]   = 0;

			Cr_Concat(inst, "call ");

			hexU32(inst, c->i.a1, CR_NULL, CR_NULL);
			Cr_Concat(inst, ", ");

			if(cells[i + 1].u32) hexU32(inst, Cr_BigU32(vm, cells[i + 1].u32), &z, &p);
			hexU32(inst, Cr_BigU32(vm, cells[i + 2].u32), &z, &p);
		} else if(c->i.op == CR_VM_INT || c->i.op == CR_VM_FLOAT) {
			char s[16];

			s[0] = 0;

			mcells = 1;

			switch(c->i.op) {
			case CR_VM_INT:
				Cr_Concat(s, "int ");
				break;
			case CR_VM_FLOAT:
				Cr_Concat(s, "float ");
				break;
			}

			Cr_Concat(inst, s);

			hexU32(inst, Cr_BigU32(vm, cells[i + 1].u32), CR_NULL, CR_NULL);
		} else if(c->i.op == CR_VM_VAR) {
			int z = 1, p = 1;

			mcells = 2;
			s[0]   = 0;

			Cr_Concat(inst, "var ");

			if(cells[i + 1].u32) hexU32(inst, Cr_BigU32(vm, cells[i + 1].u32), &z, &p);
			hexU32(inst, Cr_BigU32(vm, cells[i + 2].u32), &z, &p);
		} else if(c->i.op == CR_VM_BLOCK) {
			int z = 1, p = 1;

			args	  = 3;
			aspace[0] = aspace[1] = aspace[2] = 0;

			Cr_Concat(inst, "block ");

			hexU32(inst, ((int)c->i.a1 << 16) | ((int)c->i.a2 << 8) | ((int)c->i.a3 << 0), CR_NULL, CR_NULL);
		}

		printHex(c->d, 1);

		for(j = 0; j < args; j++) {
			Cr_Debug("%s ", (j == 0 || aspace[j]) ? " " : "");
			printHex(c->d + j + 1, 1);

			if(j == 0 || aspace[j]) consumed++;
			consumed += 3;
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
