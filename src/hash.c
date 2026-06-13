#include <crPrivate.h>
#include <cr.h>

unsigned long Cr_Hash(const void* input, long length) {
	unsigned long	     hash = 5381;
	const unsigned char* in	  = input;
	long		     i;

	for(i = 0; i < length; i++) hash = ((hash << 5) + hash) + in[i];

	return hash;
}
