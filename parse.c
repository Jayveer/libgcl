#include "libgcl.h"

void GCL_InitArgStack() 
{
	gcl_work.argstack_p = gcl_work.argbuffer;
}

void GCL_InitCommandLineBuffer() 
{
	gcl_work.commandline_p = gcl_work.commandlines;
}

void GCL_ParseInit()
{
	void GCL_InitArgStack(); 
	void GCL_InitCommandLineBuffer(); 
}