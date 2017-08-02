#include "stdafx.h"

// /server_mp/sv_main_mp.cpp:542
client_t *SV_FindClientByAddress(netadr_t from, int qport)
{
	return ((client_t *(__cdecl *)(netadr_t, int))0x005BD110)(from, qport);
}

// /server_mp/sv_main_mp.cpp:???
void LiveStats_DecompressStats(char *statsDestBuffer, int numCompressedPackets)
{
	((void(__cdecl *)(char *, int))0x0047F300)(statsDestBuffer, numCompressedPackets);
}

// /server_mp/sv_main_mp.cpp:???
bool SV_DoWeHaveAllStatsPackets(client_t *cl, int numCompressedPackets)
{
	if (numCompressedPackets <= 0)
		return true;

	for (int i = 0; *(__int64 *)((int)cl + 0x3EE60) & (1i64 << i);)
	{
		if (++i >= numCompressedPackets)
			return true;
	}

	return false;
}

// /server_mp/sv_main_mp.cpp:???
void SV_ReceiveStats(netadr_t addr, msg_t *msg)
{
	int qport = MSG_ReadShort(msg);

	client_t *cl = SV_FindClientByAddress(addr, qport);

	if (!cl)
	{
		Com_PrintWarning(
			15,
			"Received stats packet from unknown remote client %u.%u.%u.%u\n",
			addr.ip[0],
			addr.ip[1],
			addr.ip[2],
			addr.ip[3]);
		return;
	}

	int packetNum = MSG_ReadByte(msg);
	int numCompressedPackets = MSG_ReadByte(msg);

	// Don't do anything if we already have the full stats
	if (SV_DoWeHaveAllStatsPackets(cl, numCompressedPackets))
		return;

	// Packet index sanity check
	if (packetNum < 0 || packetNum >= 1)
	{
		Com_PrintWarning(15, "Invalid stat packet %i of stats data\n", packetNum);
		return;
	}

	Com_Printf(15, "Received packet %i of %i of stats data\n", packetNum, numCompressedPackets);

	int start = 1156 * packetNum;
	int size = 1024 - start;

	if (size > 1156)
		size = 1156;

	ASSERT(size > 0);
	ASSERT_MSG_VA(start + size <= (1 * 1024), "Start is %i, size is %i", start, size);

	MSG_ReadData(msg, (void *)(start + (int)cl + 0x3E9D5), size);

	*(__int64 *)((int)cl + 0x3EE60) |= 1i64 << packetNum;	// statPacketsReceived
	*(DWORD *)((int)cl + 0x11450) = *(DWORD *)0x0286D014;	// lastPacketTime
	*(DWORD *)((int)cl + 0x3EE58) = 0;						// statsSentIndex
	*(DWORD *)((int)cl + 0x3EE5C) = 0;						// statsModified

	memset((void *)((int)cl + 0x3EDD5), 0, 128);			// modifiedStatBytes

	// We updated the flags this time, so check again
	if (SV_DoWeHaveAllStatsPackets(cl, numCompressedPackets))
	{
		LiveStats_DecompressStats((char *)((int)cl + 0x3E9D5), numCompressedPackets);

		// These bitflags indicate we have all packets
		*(__int64 *)((int)cl + 0x3EE60) |= 1;
	}

	__int64 neededStatBits = ~*(__int64 *)((int)cl + 0x3EE60) & ((1i64 << numCompressedPackets) - 1);
	NET_OutOfBandPrint(NS_CLIENT1, addr, va("statresponse %lld", neededStatBits));
}

// /server_mp/sv_main_mp.cpp:2086
void SV_CheckTimeouts()
{
	((void(__cdecl *)())0x004F5810)();
}

// /server_mp/sv_main_mp.cpp:2264
void SV_PostFrame(bool FrameTimeout)
{
	RadiantRemoteUpdate();								//
	// ((void(__cdecl *)(int))0x00000000)(0);			// Scr_UpdateDebugger
	((void(__cdecl *)())0x004BC500)();					// SV_ProcessPendingSaves
	SV_CheckTimeouts();									//
	((void(__cdecl *)(bool))0x00465890)(FrameTimeout);	// SV_SendClientMessages
	((void(__cdecl *)())0x00601CC0)();					// ????
}

// Allow the server to preload maps (per the default behavior)
// Should always be enabled except when doing ChangeLevel()
int sv_allowMapPreload = 1;

int __cdecl SV_UseMapPreloading(void)
{
	int preload = sv_allowMapPreload;

	// Re-enable preloading for the next check
	if (!preload)
		sv_allowMapPreload = true;

	return preload;
}

void __declspec(naked) mfh_PartyHost_StartMatch(void)
{
	static const DWORD dwRetn_Default = 0x005A3107;
	static const DWORD dwRetn_Skip = 0x005A3122;

	_asm
	{
		push ecx
		push edi
		call SV_UseMapPreloading
		pop edi
		pop ecx

		cmp eax, 0
		jz PRELOAD_SKIP

	// Default preload behavior
		mov ecx, useSvMapPreloading
		cmp byte ptr[ecx + 18h], 0
		jmp dwRetn_Default

	PRELOAD_SKIP:
		jmp dwRetn_Skip
	}
}

void __declspec(naked) mfh_SV_SpawnServer(void)
{
	static const DWORD dwRetn_Default = 0x0050F074;
	static const DWORD dwRetn_Skip = 0x0050F07B;

	_asm
	{
		push ecx
		push edi
		call SV_UseMapPreloading
		pop edi
		pop ecx

		cmp eax, 0
		jz PRELOAD_SKIP

	// Default preload behavior
		mov eax, useSvMapPreloading
		cmp     byte ptr[eax + 18h], 0
		jmp dwRetn_Default

	PRELOAD_SKIP:
		jmp dwRetn_Skip
	}
}
