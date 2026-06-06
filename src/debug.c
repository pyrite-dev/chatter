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
	    "message",
	    "item"};

	bar[0]	= 0;
	bar2[0] = 0;
	info[0] = 0;

	if(ast->type == CR_P_ITEM) {
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
#endif
