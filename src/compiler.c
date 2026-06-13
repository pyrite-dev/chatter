#include <crPrivate.h>
#include <cr.h>

static int compile(Cr_VM* vm, Cr_AST* ast);

static void compile_section(Cr_VM* vm, Cr_AST* ast, Cr_Section* section) {
	switch(ast->type) {
	case CR_P_MESSAGE:
	case CR_P_PROGRAM:
	case CR_P_GROUP:
	case CR_P_ASSIGN:
	{
		CR_SIZE_T i;
		int	  op = 0;

		for(i = 0; i < Cr_ArrayLength(ast->children); i++) {
			Cr_AST* c = ast->children[i];

			compile_section(vm, c, section);
		}

		switch(ast->type) {
		case CR_P_ASSIGN:
			op = CR_VM_SETVAR;
		case CR_P_MESSAGE:
			if(op == 0) op = CR_VM_CALL;
			{
				Cr_Cell	      c;
				unsigned long n = Cr_Hash(ast->token, Cr_Length(ast->token));
				int	      l = Cr_ArrayLength(ast->children);

				c.u32  = 0;
				c.i.op = op;
				c.i.a1 = (l >> 16) & 0xff;
				c.i.a2 = (l >> 8) & 0xff;
				c.i.a3 = (l >> 0) & 0xff;
				Cr_ArrayPut(section->value, c);

				c.u32 = Cr_BigU32(vm, (n >> 32) & 0xffffffff);
				Cr_ArrayPut(section->value, c);

				c.u32 = Cr_BigU32(vm, (n >> 0) & 0xffffffff);
				Cr_ArrayPut(section->value, c);

				break;
			}
		}

		break;
	}
	case CR_P_ITEM:
	{
		Cr_Cell c;

		if(ast->sub == CR_L_NUMBER) {
			CR_SIZE_T i;

			c.u32  = 0;
			c.i.op = CR_VM_INT;

			for(i = 0; ast->token[i] != 0; i++) {
				if(ast->token[i] == '.') {
					c.i.op = CR_VM_FLOAT;
					break;
				}
			}

			Cr_ArrayPut(section->value, c);

			if(c.i.op == CR_VM_INT) c.s32 = Cr_BigS32(vm, CR_ATOI(ast->token));
			if(c.i.op == CR_VM_FLOAT) c.f32 = Cr_BigF32(vm, CR_ATOF(ast->token));
			Cr_ArrayPut(section->value, c);
		} else if(ast->sub == CR_L_IDENT) {
			unsigned long n = Cr_Hash(ast->token, Cr_Length(ast->token));

			c.u32  = 0;
			c.i.op = CR_VM_VAR;
			Cr_ArrayPut(section->value, c);

			c.u32 = Cr_BigU32(vm, (n >> 32) & 0xffffffff);
			Cr_ArrayPut(section->value, c);

			c.u32 = Cr_BigU32(vm, (n >> 0) & 0xffffffff);
			Cr_ArrayPut(section->value, c);
		}

		break;
	}
	case CR_P_BLOCK:
	case CR_P_ARRAY:
	case CR_P_BYTE_ARRAY:
	{
		Cr_Cell	  c;
		int	  n;
		CR_SIZE_T i;

		c.u32 = 0;
		switch(ast->type) {
		case CR_P_BLOCK:
			c.i.op = CR_VM_BLOCK;
			n      = compile(vm, ast);
			break;
		case CR_P_ARRAY:
			if(c.i.op == 0) c.i.op = CR_VM_ARR;
		case CR_P_BYTE_ARRAY:
			if(c.i.op == 0) c.i.op = CR_VM_BYTEARR;

			n = Cr_ArrayLength(ast->children);

			for(i = 0; i < Cr_ArrayLength(ast->children); i++) {
				Cr_AST* a = ast->children[i];

				compile_section(vm, a, section);
			}
		}
		c.i.a1 = (n >> 16) & 0xff;
		c.i.a2 = (n >> 8) & 0xff;
		c.i.a3 = (n >> 0) & 0xff;
		Cr_ArrayPut(section->value, c);

		break;
	}
	case CR_P_LOCAL:
	{
		Cr_Cell c;
		int	i;

		for(i = 0; i < Cr_ArrayLength(ast->children); i++) {
			unsigned long n = Cr_Hash(ast->children[i]->token, Cr_Length(ast->children[i]->token));

			c.u32  = 0;
			c.i.op = CR_VM_LOCAL;
			Cr_ArrayPut(section->value, c);

			c.u32 = Cr_BigU32(vm, (n >> 32) & 0xffffffff);
			Cr_ArrayPut(section->value, c);

			c.u32 = Cr_BigU32(vm, (n >> 0) & 0xffffffff);
			Cr_ArrayPut(section->value, c);
		}

		break;
	}
	}
}

static int compile(Cr_VM* vm, Cr_AST* ast) {
	int	   i;
	Cr_Section s;
	Cr_Cell	   c;

	s.key	= vm->section_seq++;
	s.value = CR_NULL;

	for(i = 0; i < Cr_ArrayLength(ast->children); i++) {
		Cr_AST* c = ast->children[i];

		compile_section(vm, c, &s);
	}

	c.u32  = 0;
	c.i.op = CR_VM_RET;
	Cr_ArrayPut(s.value, c);

	Cr_HashMapPutStruct(vm->sections, s);

	return s.key;
}

void Cr_Compile(Cr_VM* vm, Cr_AST* ast) {
	unsigned long i;
	int	      seq = vm->section_seq;

	compile(vm, ast);

	for(i = seq; i < vm->section_seq; i++) {
		Cr_Section* s = Cr_HashMapGet(vm->sections, i);

		Cr_Debug("compiler: %d: %d cells\n", s->key, Cr_ArrayLength(s->value));
	}
}
