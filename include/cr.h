#ifndef __CR_H__
#define __CR_H__

#include <crConfig.h>

typedef struct Cr_Interp Cr_Interp;
typedef struct Cr_Token	 Cr_Token;
typedef struct Cr_AST	 Cr_AST;

struct Cr_Interp {
};

enum CR_LEXER_TOKEN {
	CR_L_IDENT = 1,
	CR_L_NUMBER,
	CR_L_STRING,
	CR_L_CHAR,
	CR_L_SYMBOL,

	/* ignorable */
	CR_L_COMMENT,
	CR_L_SEPARATOR,

	/* control */
	CR_L_ASSIGN,
	CR_L_BAR,
	CR_L_PERIOD,
	CR_L_SEMICOLON,
	CR_L_BLOCK_BEGIN,
	CR_L_BLOCK_END,
	CR_L_BLOCK_ARG,
	CR_L_ARRAY_BEGIN,      /* NOTE: this ends with CR_L_BLOCK_END */
	CR_L_BYTE_ARRAY_BEGIN, /* NOTE: this ends with CR_L_PAR_END */
	CR_L_PAR_BEGIN,
	CR_L_PAR_END
};

enum CR_PARSER_TOKEN {
	CR_P_PROGRAM = 1,
	CR_P_BLOCK,
	CR_P_GROUP,
	CR_P_ASSIGN,
	CR_P_MESSAGE,
	CR_P_ITEM,
	CR_P_BLOCK_ARG
};

struct Cr_Token {
	int  type;
	char token[CR_TOKSZ + 1];
};

struct Cr_AST {
	int	 type;
	char	 token[CR_TOKSZ + 1];
	Cr_AST*	 parent;
	Cr_AST** children;
};

#define CR_IS_ALPHA(x) (('a' <= (x) && (x) <= 'z') || ('A' <= (x) && (x) <= 'Z'))
#define CR_IS_NUMBER(x) ('0' <= (x) && (x) <= '9')
#define CR_IS_ALPHANUM(x) (CR_IS_ALPHA((x)) || CR_IS_NUMBER((x)))
#define CR_IS_SYMBOL(x) ((x) == '+' || (x) == '-' || (x) == '*' || (x) == '/' || (x) == '!' || (x) == '#' || (x) == '$' || (x) == '%' || (x) == '&' || (x) == '=' || (x) == '^' || (x) == '~' || (x) == '\\' || (x) == '@' || (x) == ':' || (x) == ',' || (x) == '<' || (x) == '>' || (x) == '?' || (x) == '_' || (x) == '\\')
#define CR_IS_ALPHASYM(x) (CR_IS_ALPHA((x)) || CR_IS_SYMBOL((x)))
#define CR_IS_SEPARATOR(x) ((x) == ' ' || (x) == '\t' || (x) == '\r' || (x) == '\n')
#define CR_CAN_BE_FIRST(x) (CR_IS_ALPHA((x)) || (x) == '_')

#define CR_NEW_AST Cr_Alloc(sizeof(Cr_AST))

/* core.c */
Cr_Interp* Cr_CreateInterp(void);
void	   Cr_DeleteInterp(Cr_Interp* interp);
void	   Cr_Eval(Cr_Interp* interp, const char* script);

/* lexer.c */
#define CR_LEX_ERROR ((void*)1)

Cr_Token* Cr_Lex(const char* str);

/* parser.c */
Cr_AST* Cr_Parse(const char* str);
void	Cr_DeleteAST(Cr_AST* root);

/* mem.c */
void* Cr_Alloc(int size);
void  Cr_Free(void* ptr);
void  Cr_Copy(void* dst, const void* src, int size);
int   Cr_Equal(const void* a, const void* b, int size);

/* string.c */
void Cr_Escape(char* dst, const char* src);
void Cr_EscapeConcat(char* dst, const char* src);
int  Cr_Length(const char* ptr);
void Cr_Concat(char* dst, const char* src);

/* op.c */
int  Cr_IsUnary(const char* op);
int  Cr_IsBinary(const char* op);
int  Cr_IsKeyword(const char* op);
int  Cr_IsReceiver(const char* op);
void Cr_SortAndCleanMsgRecv(Cr_AST* ast);

/* debug.c */
#ifdef DEBUG
void Cr_Debug(const char* fmt, ...);
void Cr_DebugAST(Cr_AST* root);
#else
#define Cr_Debug(x, ...)
#define Cr_DebugAST(root)
#endif

/* array.c */
#define Cr_ArrayPut(x, y) \
	{ \
		(x)			     = Cr_ArrayGrow((x), sizeof(*x)); \
		(x)[Cr_ArrayLength((x)) - 1] = (y); \
	}
#define Cr_ArrayFree(x) \
	{ \
		Cr_ArrayDestroy((x)); \
		(x) = CR_NULL; \
	}
#define Cr_ArrayDelete(x, i) \
	{ \
		(x) = Cr_ArrayShrink((x), (i)); \
	}
#define Cr_ArrayDeleteMatch(x, e) \
	{ \
		(x) = Cr_ArrayShrinkMatch((x), &(e)); \
	}

void* Cr_ArrayGrow(void* array, int size);
int   Cr_ArrayLength(void* array);
void  Cr_ArrayDestroy(void* array);		       /* do not use this, use Cr_ArrayFree instead */
void* Cr_ArrayShrink(void* array, int index);	       /* do not use this, use Cr_ArrayDelete instead */
void* Cr_ArrayShrinkMatch(void* array, void* element); /* do not use this, use Cr_ArrayDeleteMatch instead */

#endif
