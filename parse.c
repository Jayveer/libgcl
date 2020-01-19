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

	if (tag) {
		int size;
		char *next = top;
		*type_p = tag;
		switch (tag) 
		{
		case 0x10:
			break;
		case 0x20:
			p = (unsigned char *)GCL_GetVar(next, type_p, value_p);
			break;
		case 0x30:
			next = GCL_GetBlockSize(next, &size);
			*value_p = GCL_Expr(next);
			p = (unsigned char*)next + size;
		case 0x40:
			int temp = (*p & 0x0F);

			if (temp == 0x0F) 
			{
				p++;
				temp += *p;
			}

			*value_p = GCL_GetArgs(temp);
			*type_p = 9;
			p++;
			break;
		case 0x50:
			next = GCL_GetBlockSize(next, &size);
			*type_p = (*next << 16) | 0x50;
			*value_p = (int)next + 4;
			p = (unsigned char*)next + size;
			break;
		case 0x80:
			next = GCL_GetBlockSize(next, &size);
			*value_p = (int)next;
			p = (unsigned char*)next + size;
			break;
		case 0x90:
			*value_p = GCL_GetLocalArgs((*p &0x0F) << 2);
			*type_p = 9;
			p++;
			break;
		case 0xC0:
			*type_p = 9;
			*value_p = ((*p & 0x3F) << 8) - 1;
			p++;
			break;
		}
	}

	if (tag == 0) 
	{
		type = *top;
		*type_p = type;
		char* next = top + 1;
		int size;

		switch (type) 
		{
		case 0x0:
			p = 0;
			break;
		case 0x1:
			*value_p = GCL_GetUShort(next);
			p = (unsigned char *)next + 2;
			break;
		case 0x2:
		case 0x3:
		case 0x4:
			*value_p = GCL_GetByte(next);
			p = (unsigned char *)next + 1;
		case 0x6:
			*value_p = GCL_GetStrCode(next);
			p = (unsigned char *)next + 3;
			break;
		case 0x7:
			size = GCL_GetByte(next);
			next++;
			*value_p = (int)next;
			p = (unsigned char*)next + size;
		case 0x8:
			*value_p = GCL_GetShort(next);
			p = (unsigned char *)next + 2;
			break;
		case 0x5:
		case 0xB:
		case 0xC:
			break;
		case 0x9:
		case 0xA:
		case 0xD:
			*value_p = GCL_GetULong(next);
			p = (unsigned char *)next + 4;
			break;
		case 0xE:
			int temp = GCL_GetUShort(next);
			p = (unsigned char *)next + 2;
			*value_p = (temp & 0x16) ? (int)GCL_GetStringResource2(temp) : (int)GCL_GetStringResource(temp);
			*type_p = 7;
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

	GCL_ARGS args = *(GCL_ARGS*)(gcl_work.argstack_p - 1);
	return args.argv[argno];
}

int GCL_GetLocalArgs(int argno)
{
	return *(gcl_work.argstack_p - argno - 1);
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