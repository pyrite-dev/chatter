#include <crPrivate.h>
#include <cr.h>

CR_HASH_T Cr_Hash(const void* input, CR_SIZE_T length) {
	CR_HASH_T	     hash = 5381;
	const unsigned char* in	  = input;
	CR_SIZE_T	     i;

	for(i = 0; i < length; i++) hash = ((hash << 5) + hash) + in[i];

	return hash;
}
