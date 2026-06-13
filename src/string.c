#include <crPrivate.h>
#include <cr.h>

void Cr_Escape(char* dst, const char* src) {
	dst[0] = 0;

	Cr_EscapeConcat(dst, src);
}

void Cr_EscapeConcat(char* dst, const char* src) {
	int  i;
	char buf[2];

	buf[1] = 0;

	for(i = 0; src[i] != 0; i++) {
		if(src[i] == '\r') {
			Cr_Concat(dst, "\\r");
		} else if(src[i] == '\n') {
			Cr_Concat(dst, "\\n");
		} else {
			buf[0] = src[i];

			Cr_Concat(dst, buf);
		}
	}
}

int Cr_Length(const char* ptr) {
	int i;

	for(i = 0; ptr[i] != 0; i++);

	return i;
}

void Cr_Concat(char* dst, const char* src) {
	int i;
	int s = Cr_Length(dst);

	for(i = 0; src[i] != 0; i++) {
		dst[s + i] = src[i];
	}
	dst[s + i] = 0;
}

int Cr_EqualString(const char* a, const char* b) {
	if(Cr_Length(a) != Cr_Length(b)) return 0;

	return Cr_Equal(a, b, Cr_Length(a));
}
