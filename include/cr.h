#ifndef __CR_H__
#define __CR_H__

#include <crConfig.h>

typedef struct Cr_Interp Cr_Interp;
typedef struct Cr_Token	 Cr_Token;

struct Cr_Interp {
};

enum CR_TOKEN {
	CR_IDENT = 0,
	CR_NUMBER,
	CR_STRING,
	CR_CHAR,
	CR_SYMBOL,

	/* ignorable */
	CR_COMMENT,
	CR_SEPARATOR,

	/* control */
	CR_ASSIGN,
	CR_BAR,
	CR_PERIOD,
	CR_BLOCK_BEGIN,
	CR_BLOCK_END,
	CR_BLOCK_ARG,
	CR_ARRAY_BEGIN, /* NOTE: this ends with CR_BLOCK_END */
	CR_BYTE_ARRAY_BEGIN, /* NOTE: this ends with CR_PAR_END */
	CR_PAR_BEGIN,
	CR_PAR_END
};

struct Cr_Token {
	int  type;
	char token[CR_TOKSZ];
};

#define CR_IS_ALPHA(x) (('a' <= (x) && (x) <= 'z') || ('A' <= (x) && (x) <= 'Z'))
#define CR_IS_NUMBER(x) ('0' <= (x) && (x) <= '9')
#define CR_IS_SYMBOL(x) ((x) == '+' || (x) == '-' || (x) == '*' || (x) == '/' || (x) == '!' || (x) == '#' || (x) == '$' || (x) == '%' || (x) == '&' || (x) == '=' || (x) == '^' || (x) == '~' || (x) == '\\' || (x) == '@' || (x) == ':' || (x) == ',' || (x) == '<' || (x) == '>' || (x) == '?' || (x) == '_' || (x) == '\\')
#define CR_IS_SEPARATOR(x) ((x) == ' ' || (x) == '\t' || (x) == '\r' || (x) == '\n')
#define CR_IS_ALPHASYM(x) (CR_IS_ALPHA((x)) || CR_IS_SYMBOL((x)))
#define CR_CAN_BE_FIRST(x) (CR_IS_ALPHA((x)) || (x) == '_')

/* core.c */
Cr_Interp* Cr_CreateInterp(void);
void	   Cr_DeleteInterp(Cr_Interp* interp);
void	   Cr_Eval(Cr_Interp* interp, const char* script);

/* lexer.c */
Cr_Token* Cr_Lex(const char* str);

/* mem.c */
void* Cr_Alloc(int size);
void  Cr_Free(void* ptr);
void  Cr_Copy(void* dst, const void* src, int size);
int   Cr_Length(const char* ptr);
int   Cr_Equal(const void* a, const void* b, int size);

/* array.c */
#define Cr_ArrayPut(x, y) \
	{ \
		(x)			 = Cr_ArrayGrow((x), sizeof(*x)); \
		(x)[Cr_ArrayLength((x))] = (y); \
	}
#define Cr_ArrayFree(x) \
	{ \
		Cr_ArrayDestroy((x)); \
		(x) = CR_NULL; \
	}

void* Cr_ArrayGrow(void* array, int size);
int   Cr_ArrayLength(void* array);
void  Cr_ArrayDestroy(void* array); /* do not use this, use Cr_ArrayFree instead */

#endif
