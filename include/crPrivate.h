#ifndef __CR_PRIVATE_H__
#define __CR_PRIVATE_H__

#include <crConfig.h>

typedef struct Cr_InstructionCell Cr_InstructionCell;
typedef union Cr_Cell		  Cr_Cell;
typedef struct Cr_Section	  Cr_Section;
typedef struct Cr_Object	  Cr_Object;
typedef struct Cr_ObjectKV	  Cr_ObjectKV;
typedef struct Cr_VM		  Cr_VM;
typedef struct Cr_CellPosition	  Cr_CellPosition;
typedef struct Cr_Thread	  Cr_Thread;
typedef struct Cr_Token		  Cr_Token;
typedef struct Cr_AST		  Cr_AST;

#define CR_NEW_HASHMAP(x, y) \
	struct x { \
		y unsigned char used; \
		void*		chain; \
		union x##_Union { \
			void*	   ptr; \
			void**	   pptr; \
			struct x*  casted; \
			struct x** pcasted; \
		} temp; \
	}

enum CR_VM_OP {
	CR_VM_NOP = 0,
	CR_VM_RET,
	CR_VM_CALL,
	CR_VM_INT,
	CR_VM_FLOAT,
	CR_VM_ARR,
	CR_VM_BYTEARR,
	CR_VM_VAR,
	CR_VM_BLOCK,
	CR_VM_LOCAL,
	CR_VM_SETVAR
};

struct Cr_InstructionCell {
	unsigned char op;
	unsigned char a1;
	unsigned char a2;
	unsigned char a3;
};

union Cr_Cell {
	Cr_InstructionCell i;
	unsigned char	   d[4];
	unsigned int	   u32;
	int		   s32;
	float		   f32;
};

CR_NEW_HASHMAP(Cr_Section, unsigned long key; Cr_Cell * value;);

struct Cr_Object {
	Cr_VM* vm;

	Cr_Object* class_obj;
	Cr_Object* superclass_obj;

	unsigned long ref;

	union Cr_Object_Raw {
		float	      f32;
		int	      s32;
		unsigned long section;
	} raw;
};

CR_NEW_HASHMAP(Cr_ObjectKV, unsigned long key; Cr_Object * value;);

struct Cr_VM {
	int big;

	Cr_Section* sections;
	int	    section_seq;

	Cr_Thread** threads;

	Cr_ObjectKV* classes;

	Cr_Object** objects;
};

struct Cr_CellPosition {
	unsigned long sp; /* section ptr */
	unsigned long ip; /* instruction ptr */
};

struct Cr_Thread {
	Cr_VM* vm;

	Cr_Thread* parent;
	Cr_Thread* wait;

	int		 dead;
	Cr_CellPosition* running;

	Cr_Object** stack;
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
	CR_P_LOCAL,
	CR_P_MESSAGE,
	CR_P_ITEM,
	CR_P_ARRAY,
	CR_P_BYTE_ARRAY,
	CR_P_BLOCK_ARG
};

struct Cr_AST {
	int	 type;
	int	 sub;
	char	 token[CR_TOKSZ + 1];
	Cr_AST*	 parent;
	Cr_AST** children;
};

#define CR_NEW_AST Cr_Alloc(sizeof(Cr_AST))

#define CR_IS_ALPHA(x) (('a' <= (x) && (x) <= 'z') || ('A' <= (x) && (x) <= 'Z'))
#define CR_IS_NUMBER(x) ('0' <= (x) && (x) <= '9')
#define CR_IS_ALPHANUM(x) (CR_IS_ALPHA((x)) || CR_IS_NUMBER((x)))
#define CR_IS_SYMBOL(x) ((x) == '+' || (x) == '-' || (x) == '*' || (x) == '/' || (x) == '!' || (x) == '#' || (x) == '$' || (x) == '%' || (x) == '&' || (x) == '=' || (x) == '^' || (x) == '~' || (x) == '\\' || (x) == '@' || (x) == ':' || (x) == ',' || (x) == '<' || (x) == '>' || (x) == '?' || (x) == '_' || (x) == '\\')
#define CR_IS_ALPHASYM(x) (CR_IS_ALPHA((x)) || CR_IS_SYMBOL((x)))
#define CR_IS_SEPARATOR(x) ((x) == ' ' || (x) == '\t' || (x) == '\r' || (x) == '\n')
#define CR_CAN_BE_FIRST(x) (CR_IS_ALPHA((x)) || (x) == '_')

#define CR_OFFSETOF(var, field) ((char*)&(var)->field - (char*)(var))

/* vm.c */
int	   Cr_Step(Cr_VM* vm);
void	   Cr_GetArgs(Cr_Cell* cell, int* n8, int* n32);
Cr_Object* Cr_GetClass(Cr_VM* vm, const char* name);
void	   Cr_SetClass(Cr_VM* vm, const char* name, Cr_Object* obj);

/* thread.c */
Cr_Thread* Cr_CreateThread(Cr_VM* vm, Cr_Thread* parent, long section);
void	   Cr_DeleteThread(Cr_Thread* thread);
int	   Cr_ThreadStep(Cr_Thread* thread);

/* compiler.c */
void Cr_Compile(Cr_VM* vm, Cr_AST* ast);

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

/* byteorder.c */
unsigned int Cr_BigU32(Cr_VM* vm, unsigned int n);
int	     Cr_BigS32(Cr_VM* vm, int n);
float	     Cr_BigF32(Cr_VM* vm, float n);

/* string.c */
void Cr_Escape(char* dst, const char* src);
void Cr_EscapeConcat(char* dst, const char* src);
int  Cr_Length(const char* ptr);
void Cr_Concat(char* dst, const char* src);
int  Cr_EqualString(const char* a, const char* b);

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

/* hash.c */
unsigned long Cr_Hash(const void* input, long length);

/* array.c */
#define Cr_ArrayPut(x, y) \
	{ \
		(x)			     = Cr_ArrayGrow((x), sizeof(*x)); \
		(x)[Cr_ArrayLength((x)) - 1] = (y); \
	}
#define Cr_ArrayInsert(x, y, z) \
	{ \
		(x)	 = Cr_ArrayGrowFrom((x), (y), sizeof(*x)); \
		(x)[(y)] = (z); \
	}
#define Cr_ArrayDelete(x, i) \
	{ \
		(x) = Cr_ArrayDeleteInternal((x), (i)); \
	}
#define Cr_ArrayDeleteMatch(x, e) \
	{ \
		(x) = Cr_ArrayDeleteMatchInternal((x), &(e)); \
	}
#define Cr_FreeArray(x) \
	{ \
		Cr_FreeArrayInternal((x)); \
		(x) = CR_NULL; \
	}

void* Cr_ArrayGrow(void* array, long size);		    /* do not use this */
void* Cr_ArrayGrowFrom(void* array, long index, long size); /* do not use this */
long  Cr_ArrayLength(void* array);
void* Cr_ArrayDeleteInternal(void* array, long index);	       /* do not use this, use Cr_ArrayDelete instead */
void* Cr_ArrayDeleteMatchInternal(void* array, void* element); /* do not use this, use Cr_ArrayDeleteMatch instead */
void  Cr_FreeArrayInternal(void* array);		       /* do not use this, use Cr_FreeArray instead */

/* hashmap.c */
#define Cr_HashMapGet(x, y) ( \
    (x)->temp.ptr = Cr_HashMapGetInternal((x), sizeof(*(x)), &(y), CR_OFFSETOF((x), key), sizeof((x)->key), CR_OFFSETOF((x), used), CR_OFFSETOF((x), chain)), \
    (x)->temp.casted)
#define Cr_HashMapPut(x, y, z) \
	{ \
		(x) = Cr_HashMapPutInternal((x), sizeof(*(x)), &(y), CR_OFFSETOF((x), key), sizeof((x)->key), &(z), CR_OFFSETOF((x), value), sizeof((x)->value), CR_OFFSETOF((x), used), sizeof((x)->used), CR_OFFSETOF((x), chain)); \
	}
#define Cr_HashMapPutStruct(x, y) \
	{ \
		Cr_HashMapPut((x), (y).key, (y).value); \
	}
#define Cr_HashMapGetAll(x) ( \
    (x)->temp.ptr = Cr_HashMapGetAllInternal((x), sizeof(*(x)), CR_OFFSETOF((x), used), CR_OFFSETOF((x), chain)), \
    (x)->temp.casted) /* Cr_Free this */
#define Cr_HashMapLength(x) ( \
    Cr_HashMapLengthInternal((x), sizeof(*(x)), CR_OFFSETOF((x), used), CR_OFFSETOF((x), chain)))
#define Cr_FreeHashMap(x) \
	{ \
		Cr_FreeHashMapInternal((x)); \
		(x) = CR_NULL; \
	}

void* Cr_HashMapGetInternal(void* hashmap, long size, const void* key, long kstart, long ksize, long ustart, long cstart);							   /* do not use this, use Cr_HashMapGet instead */
void* Cr_HashMapPutInternal(void* hashmap, long size, const void* key, long kstart, long ksize, const void* value, long vstart, long vsize, long ustart, long usize, long cstart); /* do not use this, use Cr_HashMapPut instead */
long  Cr_HashMapLengthInternal(void* hashmap, long size, long ustart, long cstart);												   /* do not use this, use Cr_HashMapLength instead */
void* Cr_HashMapGetAllInternal(void* hashmap, long size, long ustart, long cstart);												   /* do not use this, use Cr_HashMapGetAll instead */
void  Cr_FreeHashMapInternal(void* hashmap);																	   /* do not use this, use Cr_FreeHashMap instead */

#endif
