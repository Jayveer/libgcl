#include "libgcl.h"

int scenerio_code; 
GCL_WORK_T gcl_work; 


int GCL_InitFunc(void *top, int id) 
{
	if (id == scenerio_code)
		GCL_LoadScript((char *)top);

	return 0;
}

void GCL_ChangeSenerioCode(int code)
{
	scenerio_code = code;
}

void GCL_Initialize()
{
	GCL_ParseInit();
	GCL_InitVar();
	GCL_ResetCommList();
	GCL_InitBasicCommands();
	//GV_LoaderSet(2, (GV_LOADFUNC)GCL_InitFunc, 0);
	GCL_ChangeSenerioCode(0x180720); 
}

void GCL_ResetSystem() 
{
	GCL_ResetLocalVar();
}