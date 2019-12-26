char gcl_variable_buf[68224];

char *linkvarbuf 	= gcl_variable_buf;
char *sv_linkvarbuf = gcl_variable_buf + 0x5B40;
char *var_buf 		= gcl_variable_buf + 0xB680;
char *sv_var_buf 	= gcl_variable_buf + 0xDE80;

char local_var_buf[1024];


void GCL_InitVar() {
	memset(linkvarbuf, 0, 0x5B40);
	memset(var_buf, 0, 0x2800);
}

void GCL_InitClearVar() {
	memset(var_buf, 0x0, 0x2800);
	memset(linkvarbuf + 0x158, 0x0, 0x59E8);
	GCL_SaveVar(); 
}

void GCL_SaveVar() {
	memcpy(sv_linkvarbuf, linkvarbuf, 0x5B40);
	memcpy(sv_var_buf, var_buf, 0x2800);
}

void GCL_RestoreVar()
{
	memcpy(linkvarbuf, sv_linkvarbuf, 0x5B40);
	memcpy(var_buf, sv_var_buf, 0x2800);
}


void GCL_SaveLinkVar(void *ptr, int size)
{
	int diff = (linkvarbuf - ptr); 
	memcpy(sv_linkvarbuf + diff, ptr, size);
}

void *GCL_GetSavedLinkVar(void *ptr)
{
	int diff = (linkvarbuf - ptr); 
	return sv_linkvarbuf + diff;
}

void GCL_ResetLocalVar() {
	memset(local_var_buf, 0, 0x400);
}

void *GCL_GetLinkvarSaveAreaTop()
{
	return sv_linkvarbuf;
}

void *GCL_GetVarSaveAreaTop()
{
	return sv_var_buf;
}

void *GCL_GetLinkvarAreaTop()
{
	return linkvarbuf;
}

void *GCL_GetVarAreaTop()
{
	return var_buf;
}

void *GCL_GetLocalVarAreaTop()
{
	return local_var_buf;
}