#include "cg_scoreboard.h"

CG_RegisterScoreboardDvars_t CG_RegisterScoreboardDvars_o = 0x0;
dvar_s* cg_showServerInfo = 0x0;

dvar_s* CG_RegisterScoreboardDvars()
{
	cg_showServerInfo = Dvar_RegisterInt("cg_showServerInfo", 0, 0, 1, 0x1, "Show the server name and IP address in the scoreboard backdrop");
	return CG_RegisterScoreboardDvars_o();
}

const char* CG_GetServerName(const char* name)
{
	if (cg_showServerInfo->current.enabled)
		return name;

	return "";
}

void* retn_CG_DrawBackdropServerInfo = (void*)0x00890E2D;
void __declspec(naked) mfh_CG_DrawBackdropServerInfo()
{
	_asm
	{
		movss [esp + 28h], xmm0
		push eax
		call CG_GetServerName
		add esp, 4
		mov [esp+0x14], eax

		jmp retn_CG_DrawBackdropServerInfo;
	}
}
