#include "libgcl.h"

void *GCL_Parse_if(char *top)
{
    //todo
    return 0;
}

int command_if(char *top) {
    //todo
	return 0;
}

int command_switch(char *top)
{
    //todo
	return 0;
}

int GCL_Command_eval(char *top)
{
    //todo
	return 0;
}

int GCL_Command_return(char *top) 
{
    //todo
    return 0;
}

int GCL_Command_print(char *top) 
{
    //todo	
	return 0;
}

int GCL_Command_call(char *top)
{
    //todo
    return 0;
}

GCL_COMMANDLIST commlist[6] = {
	{ 0xD86,    command_if          },
	{ 0xA65DB5, command_switch      },
	{ 0x34648C, GCL_Command_eval    },
	{ 0x3311EC, GCL_Command_call    },
	{ 0x8BE398, GCL_Command_return  },
	{ 0x3AB23B, GCL_Command_print   }
};

GCL_COMMANDDEF builtin_commands = { 0, 6, commlist };

void GCL_InitBasicCommands()
{
	GCL_AddCommMulti(&builtin_commands);
}