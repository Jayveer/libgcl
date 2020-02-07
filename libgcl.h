#pragma once

typedef struct
{
	char *proc_table; 
	int proc_num; 
	char *proc_body;
	char *script_body; 
} GCL_SCRIPT;


typedef struct
{
	char *block_top;
	int *resource_table; 
	char *string_table;
	char *font_data;
	char *unknown0x10;
	char *unknown0x14;
	char *unknown0x18;
} GCL_STRING_RESOURCE;


typedef struct
{
	GCL_SCRIPT script;
	GCL_STRING_RESOURCE res; 
} GCL_ENVIRONMENT;


typedef struct
{
	char *next_str_ptr; 
	int status;
	int proc_select_no;
	char **commandline_p; 
	char *commandlines[24];
	int *argstack_p; 
	int argbuffer[32];
	int block_nest;
	int *call_stack_p;
	int call_stack[64]; 

} GCL_WORK_T;


extern GCL_WORK_T gcl_work;

typedef int GCL_COMMANDFUNC(char *); 


typedef struct
{
	int id; 
	GCL_COMMANDFUNC *func; 
} GCL_COMMANDLIST;


typedef struct _gcl_commanddef
{
	struct _gcl_commanddef *next; 
	int n_commlist; 
	GCL_COMMANDLIST *commlist;
} GCL_COMMANDDEF;


typedef struct
{
	unsigned short argc;
	int *argv; 
} GCL_ARGS;


typedef struct _gcl_var_ref {
	unsigned char code[4];
	short unsigned int max;
	short unsigned int offset;
} GCL_VAR_REF;


static inline int GCL_GetLong(char *ptr)
{
	unsigned char *p; 
	p = (unsigned char *)ptr;
	return (signed long)(p[0] << 24) | (p[1] << 16) | (p[2] << 8) | (p[3]);
}

static inline unsigned int GCL_GetULong(char *ptr) 
{
	unsigned char *p; 
	p = (unsigned char *)ptr;
	return (p[0] << 24) | (p[1] << 16) | (p[2] << 8) | (p[3]);
}

static inline unsigned int GCL_GetVarCode(char *ptr)
{
	unsigned char *p;
	p = (unsigned char *)ptr;
	return (unsigned long)(p[0] << 24) | (p[1] << 16) | (p[2] << 8) | (p[3]);
}

static inline int GCL_Get3Bytes(char *ptr)
{
    unsigned char *p; /* Line: 238 */
	p = (unsigned char *)ptr;
	return (signed long)(p[0] << 16) | (p[1] << 8) | (p[2] << 0); //original for ps3
}

static inline int GCL_GetShort(char *ptr)
{
	unsigned char *p; 
	p = (unsigned char *)ptr;
	return (signed short)((p[0] << 8) | (p[1]));
}

static inline unsigned int GCL_GetUShort(char *ptr)
{
	unsigned char *p;
	p = (unsigned char *)ptr;
	return (p[0] << 8) | (p[1]);
}

static inline unsigned char GCL_GetByte(char *ptr)
{
	return (unsigned char)*ptr; 
}

static inline int GCL_GetStrCode(char *ptr)
{
	unsigned char *p; /* Line: 283 */
	p = (unsigned char *)ptr;
	return (unsigned long)(p[0] << 16) | (p[1] << 8) | (p[2] << 0);
}

extern GCL_STRING_RESOURCE *get_string_resource();
extern GCL_SCRIPT *get_script();

//gcl_init.c
void GCL_Initialize();

//expr.c
int GCL_Expr(char *data);

//parse.c
char *GCL_NextStr();
void GCL_ParseInit();
void GCL_ResetLocalVar();
int GCL_GetArgs(int argno);
void GCL_UnsetCommandLine();
void GCL_SetArgTop(char *top);
int GCL_GetLocalArgs(int argno);
void GCL_UnsetArgStack(void *stack);
void GCL_SetCommandLine(char *argtop);
void GCL_SetLocalArgs(int argno, int value);
char *GCL_GetShortSize(char *top, int *size);
char *GCL_GetBlockSize(char *top, int *size);
void *GCL_SetArgStack(GCL_ARGS *args, int local_args_num);
char *GCL_GetNextValue(char *top, int *type_p, int *value_p);


//variable.c
extern char local_var_buf[1024];

void GCL_SaveVar();
void GCL_InitVar();
int GCL_GetNextInt();
char *GCL_SetVar(char *top, int value);
char *GCL_GetVar(char *top, int *type_p, int *value_p);

//command.c
void GCL_ExecScript();
void GCL_ResetCommList();
int GCL_LoadScript(char *datatop);
char *GCL_GetStringResource(int id);
char *GCL_GetStringResource2(int id);
int GCL_AddCommMulti(GCL_COMMANDDEF *def);
int GCL_ExecBlockBody(char *top, GCL_ARGS *args, int local_num);
int GCL_ExecProcBlock(char *top, GCL_ARGS *args, int local_num);



//basic.c
void GCL_InitBasicCommands();