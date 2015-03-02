typedef void (__cdecl PROC_VID_RESTART_COMPELTE)(void);
typedef void (__cdecl PROC_SV_KILLSERVER)(void);

static PROC_VID_RESTART_COMPELTE* pCL_Vid_Restart_Complete_f = (PROC_VID_RESTART_COMPELTE*)0x005D2F00; //Initialized to the original address as for safety
static PROC_SV_KILLSERVER* SV_KillServer_f = (PROC_SV_KILLSERVER*)0x0087CF70;

void __cdecl CL_Vid_Restart_Complete_f();