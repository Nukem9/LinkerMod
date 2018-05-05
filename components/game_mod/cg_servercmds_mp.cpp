#include "stdafx.h"
#include "cg_servercmds_mp.h"

void __declspec(naked) mfh_CG_DeployServerCommand(void)
{
	static auto Sys_GetValue = (void* (__cdecl*)(int valueIndex))(0x0067D4F0);
	static const DWORD dwUnhandledCase = 0x0078E76F;
	static const DWORD dwHandledCase = 0x0078E81A;

	_asm
	{
		// push ecx (immediately before this func is called)
		// auto popped shortly after the parent function is called, or by our return case

		pushad		// save registers, just to be safe

		// returns 0 if the case was successfully handled
		push eax	// cmd
		call CG_DeployServerCommand_HandleCase

		add esp, 4
		cmp eax, 0
		popad // restore the registers (just to be safe - doesn't affect flags)
		jz HANDLED_CASE

		// UNHANDLED_CASE

		// Inlined Cmd_Args
		push 4
		call Sys_GetValue
		add esp, 4

		jmp dwUnhandledCase

	HANDLED_CASE:
		// Remove the value that was pushed before this function was called
		//  we don't need it anymore
		add esp, 4

		jmp dwHandledCase
	}
}

// [CLIENTSIDE]: Handle any custom commands from the server
int __cdecl CG_DeployServerCommand_HandleCase(int cmd)
{
	Com_Printf(0, "[SERVER COMMAND]: %d\n", cmd);
	return 1;
}
