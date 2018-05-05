#pragma once

enum scriptInstance_t
{
	SCRIPTINSTANCE_SERVER = 0x0,
	SCRIPTINSTANCE_CLIENT = 0x1,
	SCRIPT_INSTANCE_MAX = 0x2,
};

struct scr_entref_t
{
	unsigned __int16 entnum;
	unsigned __int16 classnum;
	unsigned __int16 client;
};

int Scr_IsInOpcodeMemory(scriptInstance_t inst, const char *pos);