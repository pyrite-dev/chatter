#include <cr.h>

#ifdef DEBUG
#include <stdarg.h>
#include <stdio.h>

void Cr_Debug(const char* fmt, ...){
	va_list va;

	va_start(va, fmt);
	vfprintf(stderr, fmt, va);
	va_end(va);
}
#endif
