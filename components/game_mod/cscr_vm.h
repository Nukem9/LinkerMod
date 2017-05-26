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

const char *Scr_GetString(unsigned int index, scriptInstance_t inst);
void Scr_AddBool(int value, scriptInstance_t inst);
void Scr_Error(const char *error, bool force_terminal);
void Scr_ObjectError(const char *error, scriptInstance_t inst);