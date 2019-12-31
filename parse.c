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
	//todo
	int type, tag;
	unsigned char *p; 

	if (*top == 0xC0) 
	{
		//todo
		return 0;
	}

	if ((*top & 0xF0) == 0) 
	{

		*type_p = r8;
		switch (*type_p) 
		{
		case 0:
			break;
		case 1:
			break;
		}
	}

	return 0;
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

void GCL_SetArgTop(char *top) {
	gcl_work.next_str_ptr = top;
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

void GCL_ParseInit()
{
	GCL_InitArgStack(); 
	GCL_InitCommandLineBuffer(); 
}