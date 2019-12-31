#include "libgcl.h"

int GCL_VariableVersion;

GCL_SCRIPT current_script;
GCL_SCRIPT current_script2;

GCL_STRING_RESOURCE current_strres;
GCL_STRING_RESOURCE current_strres2;

int gcl_thread_id;

GCL_ARGS gcl_null_args;

GCL_COMMANDDEF *commdef;


typedef struct
{
	unsigned int offset; 
} GCL_PROC_TABLE;

void GCL_ResetCommList()
{
	commdef = 0;
}

int GCL_AddCommMulti(GCL_COMMANDDEF *def) 
{
	def->next = commdef;
	commdef = def;
	return 0;
}

int GCL_DelCommMulti(GCL_COMMANDDEF *def) 
{
	GCL_COMMANDDEF *p, *prev;
	
	while (commdef != def) 
	{
		if (commdef->next == 0) 
			return -1;

		commdef = commdef->next;
	}

	return 0;
}

GCL_COMMANDLIST *FindCommand(int id)
{
	GCL_COMMANDDEF *def; 
	def = commdef;

	while (def->next != 0) 
	{
		GCL_COMMANDLIST *cl = def->commlist;
		for (int i = 0; i < def->n_commlist; i++) 
		{
			if (cl[i].id == id)
				return &cl[i];
		}
		def = def->next;
	}

	return 0;
}

int GCL_Command(char *ptr)
{
	//todo
	int id; 
	GCL_COMMANDLIST *cl;
	int ret;
	char *p; 
	char *next; 
	int ofs; 

	p = ptr;
	id = GCL_GetStrCode(ptr);
	cl = FindCommand(id);

	p += 3;
	next = GCL_GetShortSize(p, &ret);

	return 0;
}

void GCL_SetProcSelectNo(int no) 
{
	gcl_work.proc_select_no = no;
}

char *set_proc_table(char *proc_table, int *proc_num) 
{
	GCL_PROC_TABLE *pt = (GCL_PROC_TABLE*)proc_table; 
	int n = 0;

	while (pt->offset != -1) 
	{
		n++;
		pt++;
	}

	*proc_num = n;
	return (char*)pt;
}

char *get_proc_block(int id, int *local_arg_num)
{
	//todo
	GCL_PROC_TABLE *pt;
	int offset;
	char *res; 

	pt = (GCL_PROC_TABLE*)get_script()->proc_table;

	int prev = id - 1;
	offset = pt[prev].offset;

	if (id & 16) 
	{
		while (offset != gcl_work.proc_select_no) 
		{
			prev++;
			offset = pt[prev].offset;
		}
	}
	*local_arg_num = offset & 4;
	return offset + get_script()->proc_body;
}

int GCL_ExecProc(int proc_id, GCL_ARGS *arg)
{
	//todo
	int result; 
	//if (sub_B0F80())
		//return sub_BoF80();

	int save_id = proc_id; 

	char *p; 
	int size;
	int local; 

	p = get_proc_block(proc_id, &result);
	local = result;
	p = GCL_GetBlockSize(p, &size);

	return GCL_ExecProcBlock(p, arg, local);
}

int GCL_Proc(char *p)
{
	//todo
	int argbuf[16];
	int i = 0, id; 
	GCL_ARGS arg; 

	id = GCL_GetShort(p);
	p += 2;
	int type, value;
	
	do 
	{ 
		p = GCL_GetNextValue(p, &type, &value);
		if (type != 0) 
		{
			argbuf[i] = value;
			i++;
		}
	} while (type != 0);

	arg.argc = i;
	arg.argv = argbuf;

	id = GCL_ExecProc(id, &arg);

	return 0;
}

unsigned int rand_seed = 0x0C;

void my_srand(unsigned int seed)
{
	rand_seed = seed;
}

int my_rand() 
{
	rand_seed = (rand_seed * 0x7d2b89dd) + 0xcf9;
	return rand_seed;
}

void decode_buffer(unsigned int seed, void *buffer, unsigned int size) {
	unsigned char *p = (unsigned char*)buffer;
	int i; 

	my_srand(seed);
	for (int i = 0; i < size; i++) 
	{
		int c = (my_rand() >> 0xF) & 0x00FF;
		p[i] ^= c;
	}
}

int check_thread() 
{
	return gcl_thread_id != -1 && gcl_thread_id == MTS_ThreadID();
}

GCL_SCRIPT *get_script() 
{
	return check_thread() ? &current_script2 : &current_script;
}

GCL_STRING_RESOURCE *get_string_resource() 
{
	return check_thread() ? &current_strres2 : &current_strres;
}

int GCL_LoadScript(char *datatop)
{
	GCL_SCRIPT *cur; 
	int num = 0;
	char *next; 

	cur = get_script();

	GCL_VariableVersion = GCL_GetLong(datatop);

	cur->proc_table = datatop + 4;
	next = set_proc_table(cur->proc_table, &cur->proc_num);

	next += 4;
	char *fres_top = next;
	GCL_STRING_RESOURCE *cfr = get_string_resource();
	unsigned int seed;

	cfr->block_top = fres_top;

	next += 4;
	cfr->resource_table = (int*)(fres_top + GCL_GetLong(next));

	next += 4;
	cfr->string_table = fres_top + GCL_GetLong(next);

	next += 4;
	cfr->font_data = fres_top + GCL_GetLong(next);

	next += 4;
	seed = GCL_GetULong(next);

	if (seed != 0) 
	{
		int size = cfr->font_data - cfr->string_table;

		if (size != 0) 
		{
			decode_buffer(rand_seed, cfr->string_table, size);
		}

		*(unsigned long*)next = 0;
	}

	next = fres_top + GCL_GetLong(fres_top); 

	cur->proc_body = next + 4;
	int firstScriptOffset = GCL_GetLong(next);
	cur->script_body = next + firstScriptOffset + 8;

	cfr = get_string_resource();

	//rest doesn't make sense to me as it exceeds the length of the file
	long scriptLength = GCL_GetLong(next + firstScriptOffset + 4);
	cfr->unknown0x18 = cur->script_body + scriptLength + 0x08;
	cfr->unknown0x14 = GCL_GetLong(cur->script_body + scriptLength + 0x04);
	cfr->unknown0x18 = next + firstScriptOffset + 0x10;

	
	return 0;
}

char *GCL_GetFontDataTop()
{
	GCL_STRING_RESOURCE *cfr = get_string_resource();
	return cfr->font_data + 4;
}

int _GCL_ExecBlockBody(char *top)
{
	//todo
	char *p;
	char *next;
	int size; 

	next = GCL_GetBlockSize(top, &size);

	//while size + next !=0 do the below

	if ((top[0] & 0xF0) == 0x60) 
	{
		size = GCL_Command(next);
	}

	if ((top[0] & 0xF0) == 0x70) 
	{
		size = GCL_Proc(next);
	}

	//END: 
	return 0;
}

int GCL_ExecBlockBody(char *top, GCL_ARGS *args, int local_num)
{
	//todo
	int res;
	void *org_stack; 

	org_stack = GCL_SetArgStack(args, local_num);

	if (top == 0) 
	{
		GCL_UnsetArgStack(org_stack);
		return 0;
	}

	res = _GCL_ExecBlockBody(top);

	GCL_UnsetArgStack(org_stack);

	return res;
}

int GCL_ExecBlock(char *top, GCL_ARGS *args)
{
	return GCL_ExecBlockBody(top, args, 0);
}

void GCL_ExecScript()
{
	char *datatop = get_script()->script_body;
	char *next;
	int size;

	next = GCL_GetBlockSize(datatop, &size);
	GCL_ExecBlock(next, &gcl_null_args);
}