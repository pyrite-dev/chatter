#include <cr.h>

typedef struct section section_t;

struct section {
	unsigned long name;
	Cr_Cell*      cells;
};

static void compile(Cr_VM* vm, Cr_AST* ast, section_t** sections);

static void compile_section(Cr_VM* vm, Cr_AST* ast, section_t** sections, section_t* section) {
	switch(ast->type) {
	case CR_P_MESSAGE:
	case CR_P_PROGRAM:
	case CR_P_GROUP:
	case CR_P_ASSIGN:
	{
		int i;

		for(i = 0; i < Cr_ArrayLength(ast->children); i++) {
			Cr_AST* c = ast->children[i];

			compile_section(vm, c, sections, section);
		}

		if(ast->type == CR_P_MESSAGE) {
			Cr_Cell	      c;
			unsigned long n = Cr_Hash(ast->token, Cr_Length(ast->token));

			c.u32  = 0;
			c.i.op = CR_VM_CALL;
			c.i.a1 = Cr_ArrayLength(ast->children);
			Cr_ArrayPut(section->cells, c);

			c.u32 = Cr_BigU32(vm, (n >> 32) & 0xffffffff);
			Cr_ArrayPut(section->cells, c);

			c.u32 = Cr_BigU32(vm, (n >> 0) & 0xffffffff);
			Cr_ArrayPut(section->cells, c);
		}

		break;
	}
	case CR_P_ITEM:
	{
		break;
	}
	case CR_P_BLOCK:
	{
		compile(vm, ast, sections);
		break;
	}
	}
}

static void compile(Cr_VM* vm, Cr_AST* ast, section_t** sections) {
	int	  i;
	section_t s;
	Cr_Cell	  c;

	s.name	= vm->section_seq++;
	s.cells = CR_NULL;

	for(i = 0; i < Cr_ArrayLength(ast->children); i++) {
		Cr_AST* c = ast->children[i];

		compile_section(vm, c, sections, &s);
	}

	c.u32  = 0;
	c.i.op = CR_VM_RET;
	Cr_ArrayPut(s.cells, c);

	Cr_ArrayPut(*sections, s);
}

void Cr_Compile(Cr_VM* vm, Cr_AST* ast) {
	section_t* sections = CR_NULL;
	int	   i;
	int	   seq = 0;

	compile(vm, ast, &sections);

	for(i = 0; i < Cr_ArrayLength(sections); i++) {
		Cr_Debug("compiler: %d: %d cells\n", sections[i].name, Cr_ArrayLength(sections[i].cells));
		Cr_DebugCells(vm, sections[i].cells, Cr_ArrayLength(sections[i].cells));
		Cr_Debug("compiler:\n");

		Cr_ArrayFree(sections[i].cells);
	}
	Cr_ArrayFree(sections);
}
