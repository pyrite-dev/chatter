#ifndef __CR_H__
#define __CR_H__

#include <crConfig.h>

typedef struct Cr_VM		  Cr_VM;
typedef struct Cr_InstructionCell Cr_InstructionCell;
typedef union Cr_Cell		  Cr_Cell;
typedef struct Cr_Object	  Cr_Object;
typedef struct Cr_Token		  Cr_Token;
typedef struct Cr_AST		  Cr_AST;

struct Cr_VM {
	Cr_Cell* mem;
	long	 memsize;

	struct Cr_Object** stack;
};

struct Cr_InstructionCell {
	unsigned char op;
	unsigned char a1;
	unsigned char a2;
	unsigned char a3;
};

union Cr_Cell {
	Cr_InstructionCell i;
	unsigned int	   u32;
};

struct Cr_Object {
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

struct Cr_Token {
	int  type;
	char token[CR_TOKSZ + 1];
};

enum CR_PARSER_TOKEN {
	CR_P_PROGRAM = 1,
	CR_P_BLOCK,
	CR_P_GROUP,
	CR_P_ASSIGN,
	CR_P_MESSAGE,
	CR_P_ITEM,
	CR_P_ARRAY,
	CR_P_BYTE_ARRAY,
	CR_P_BLOCK_ARG
};

struct Cr_AST {
	int	 type;
	char	 token[CR_TOKSZ + 1];
	Cr_AST*	 parent;
	Cr_AST** children;
};

#define CR_NEW_AST Cr_Alloc(sizeof(Cr_AST))

enum CR_STATUS {
	CR_OK = 0,
	CR_ERROR
};

/* they are in kilowords (which is unsigned int) */
#define CR_MEM_1M (1 * 1024)   /* 4MiB */
#define CR_MEM_2M (2 * 1024)   /* 8MiB */
#define CR_MEM_4M (4 * 1024)   /* 16MiB */
#define CR_MEM_8M (8 * 1024)   /* 32MiB */
#define CR_MEM_16M (16 * 1024) /* 64MiB */

#define CR_MEM_STD CR_MEM_4M

#define CR_IS_ALPHA(x) (('a' <= (x) && (x) <= 'z') || ('A' <= (x) && (x) <= 'Z'))
#define CR_IS_NUMBER(x) ('0' <= (x) && (x) <= '9')
#define CR_IS_ALPHANUM(x) (CR_IS_ALPHA((x)) || CR_IS_NUMBER((x)))
#define CR_IS_SYMBOL(x) ((x) == '+' || (x) == '-' || (x) == '*' || (x) == '/' || (x) == '!' || (x) == '#' || (x) == '$' || (x) == '%' || (x) == '&' || (x) == '=' || (x) == '^' || (x) == '~' || (x) == '\\' || (x) == '@' || (x) == ':' || (x) == ',' || (x) == '<' || (x) == '>' || (x) == '?' || (x) == '_' || (x) == '\\')
#define CR_IS_ALPHASYM(x) (CR_IS_ALPHA((x)) || CR_IS_SYMBOL((x)))
#define CR_IS_SEPARATOR(x) ((x) == ' ' || (x) == '\t' || (x) == '\r' || (x) == '\n')
#define CR_CAN_BE_FIRST(x) (CR_IS_ALPHA((x)) || (x) == '_')

/* vm.c */
Cr_VM* Cr_CreateVM(long mem);
void   Cr_DeleteVM(Cr_VM* vm);
int    Cr_Eval(Cr_VM* vm, const char* script);

/* lexer.c */
#define CR_LEX_ERROR ((void*)1)

Cr_Token* Cr_Lex(const char* str);

/* parser.c */
Cr_AST* Cr_Parse(const char* str);
void	Cr_DeleteAST(Cr_AST* root);

/* mem.c */
void* Cr_Alloc(long size);
void  Cr_Free(void* ptr);
void  Cr_Copy(void* dst, const void* src, long size);
int   Cr_Equal(const void* a, const void* b, long size);

/* string.c */
void Cr_Escape(char* dst, const char* src);
void Cr_EscapeConcat(char* dst, const char* src);
int  Cr_Length(const char* ptr);
void Cr_Concat(char* dst, const char* src);

/* op.c */
int Cr_IsUnary(const char* op);
int Cr_IsBinary(const char* op);
int Cr_IsKeyword(const char* op);
int Cr_IsReceiver(const char* op);
int Cr_SortAndCleanMsgRecv(Cr_AST* ast);

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
