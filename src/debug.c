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

	for(int i = 0; i < n; i++) {
		if(i == (n - 1)) {
			strcat(bar, " +-");
		} else if(last[i + 1]) {
			strcat(bar, "   ");
			strcat(bar2, "   ");
		} else {
			strcat(bar, " | ");
			strcat(bar2, " | ");
		}
	}

	Cr_Debug("parser: %s%s\n", bar, g[ast->type]);
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
