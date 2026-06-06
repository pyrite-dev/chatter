#include <cr.h>

void Cr_Escape(char* dst, const char* src) {
}

int Cr_Length(const char* ptr) {
	int i;

	for(i = 0; ptr[i] != 0; i++);

	return i;
}
