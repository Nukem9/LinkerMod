#include "stdafx.h"

// /dw/dwlogon_pc.cpp:426
void dwLogonSeAcquiredSteamTicket()
{
	*(DWORD *)0x10CF3DC = 5;
	*(bool *)0x010CF429 = true;
	*(DWORD *)0x10CF42C = Sys_Milliseconds();
}