#include <cr.h>

typedef struct section section_t;

struct section {
	unsigned long name;
	Cr_Cell*      cells;
};

static int compile(Cr_VM* vm, Cr_AST* ast, section_t** sections);

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
		Cr_Cell c;

		if(ast->sub == CR_L_NUMBER) {
			int i;

			c.u32  = 0;
			c.i.op = CR_VM_INT;

			for(i = 0; ast->token[i] != 0; i++) {
				if(ast->token[i] == '.') {
					c.i.op = CR_VM_FLOAT;
					break;
				}
			}

			Cr_ArrayPut(section->cells, c);

			if(c.i.op == CR_VM_INT) c.s32 = Cr_BigS32(vm, CR_ATOI(ast->token));
			if(c.i.op == CR_VM_FLOAT) c.f32 = Cr_BigF32(vm, CR_ATOF(ast->token));
			Cr_ArrayPut(section->cells, c);
		} else if(ast->sub == CR_L_IDENT) {
			unsigned long n = Cr_Hash(ast->token, Cr_Length(ast->token));

			c.u32  = 0;
			c.i.op = CR_VM_VAR;
			Cr_ArrayPut(section->cells, c);

			c.u32 = Cr_BigU32(vm, (n >> 32) & 0xffffffff);
			Cr_ArrayPut(section->cells, c);

			c.u32 = Cr_BigU32(vm, (n >> 0) & 0xffffffff);
			Cr_ArrayPut(section->cells, c);
		}

		break;
	}
	case CR_P_BLOCK:
	{
		Cr_Cell c;
		int	n = compile(vm, ast, sections);

		c.u32  = 0;
		c.i.op = CR_VM_BLOCK;
		c.i.a1 = (n >> 16) & 0xff;
		c.i.a2 = (n >> 8) & 0xff;
		c.i.a3 = (n >> 0) & 0xff;
		Cr_ArrayPut(section->cells, c);

		break;
	}
	}
}

static int compile(Cr_VM* vm, Cr_AST* ast, section_t** sections) {
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

	return s.name;
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
