#include "libgcl.h"

char *GCL_GetBlockSize(char *top, int *size)
{
	unsigned char *p;
	unsigned int code;

	p = (unsigned char*)top;
	code = *p & 0xF;
	p++;

	switch (code) 
	{
	case 0xD:
		*size = *p;
		p++;
		break;
	case 0xE:
		*size = GCL_GetShort((char*)p);
		p += 2;
		break;
	case 0xF:
		*size = GCL_Get3Bytes((char*)p);
		p += 3;
		break;
	default:
		*size = code;
	}

	return (char *)p;
}

char *GCL_GetShortSize(char *top, int *size)
{
	unsigned char *p; /* Line: 59 */
	p = (unsigned char*)top;

	*size = *top;

	if (size < 0) 
	{
		*size = GCL_GetShort(top);
		p++;
	}

	p++;
	return (char*)p;

}

char *GCL_GetNextValue(char *top, int *type_p, int *value_p)
{
	int type, tag; 
	unsigned char *p = (unsigned char*)top; 

	tag = *p & 0xF0;

	if (tag == 0) 
	{
		type = *top;
		*type_p = type;
		char* next = top + 1;

		switch (type) 
		{
		case 0:
			next = 0;
			break;
		case 1:
			break;
		case 6:
			*value_p = GCL_GetStrCode(next);
			p = (unsigned char *)next + 3;
			break;
		}
	}

	return (char*)p;
}

void GCL_InitArgStack() 
{
	gcl_work.argstack_p = gcl_work.argbuffer;
}

void *GCL_SetArgStack(GCL_ARGS *args, int local_args_num) 
{
	int *org;
	org = gcl_work.argstack_p;

	if (!args)
		return 0;

	gcl_work.argstack_p[local_args_num] = (int)args;
	gcl_work.argstack_p++;
	return org;
}


void GCL_UnsetArgStack(void *stack) 
{
	if (stack != 0)
		gcl_work.argstack_p = (int*)stack;
}

int GCL_GetArgs(int argno)
{
	if (argno == 0)
		return gcl_work.status;


	return 0;
}

void GCL_InitCommandLineBuffer() 
{
	gcl_work.commandline_p = gcl_work.commandlines;
}

void GCL_SetCommandLine(char *argtop)
{
	*gcl_work.commandline_p = argtop;
	gcl_work.commandline_p += 4;
}

void GCL_UnsetCommandLine()
{
	gcl_work.commandline_p -= 4;
}

void GCL_SetArgTop(char *top) {
	gcl_work.next_str_ptr = top;
}

int GCL_GetInt(char *ptr)
{
	int type, value; 
	
	if (ptr != 0)
		ptr = GCL_GetNextValue(ptr, &type, &value);

	gcl_work.next_str_ptr = ptr;
	return value;
}

char *GCL_NextStr() {

	if (gcl_work.next_str_ptr == 0)
		return 0;

	if (*gcl_work.next_str_ptr == 0)
		return 0;

	if (*gcl_work.next_str_ptr & 0xF0 == 0x50)
		return 0;

	return gcl_work.next_str_ptr;
}

int GCL_GetNextInt() {
	char* p= GCL_NextStr();
	return GCL_GetInt(p);
}

void GCL_ParseInit()
{
	GCL_InitArgStack(); 
	GCL_InitCommandLineBuffer(); 
}