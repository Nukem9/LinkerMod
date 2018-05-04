#include "stdafx.h"
#pragma pack(1)

static HINSTANCE hLThis = 0;
static HINSTANCE hL = 0;
static FARPROC p[181] = {0};

BOOL WINAPI Winmm_Init(HINSTANCE hInst, DWORD reason)
{
	if (reason != DLL_PROCESS_ATTACH)
		return FALSE;

	hLThis = hInst;
	hL = LoadLibraryA("C:\\Windows\\System32\\winmm.dll");
	if (!hL)
		return FALSE;

	p[0] = GetProcAddress(hL, "CloseDriver");
	p[1] = GetProcAddress(hL, "DefDriverProc");
	p[2] = GetProcAddress(hL, "DriverCallback");
	p[3] = GetProcAddress(hL, "DrvGetModuleHandle");
	p[4] = GetProcAddress(hL, "GetDriverModuleHandle");
	p[5] = GetProcAddress(hL, "OpenDriver");
	p[6] = GetProcAddress(hL, "PlaySound");
	p[7] = GetProcAddress(hL, "PlaySoundA");
	p[8] = GetProcAddress(hL, "PlaySoundW");
	p[9] = GetProcAddress(hL, "SendDriverMessage");
	p[10] = GetProcAddress(hL, "WOWAppExit");
	p[11] = GetProcAddress(hL, "auxGetDevCapsA");
	p[12] = GetProcAddress(hL, "auxGetDevCapsW");
	p[13] = GetProcAddress(hL, "auxGetNumDevs");
	p[14] = GetProcAddress(hL, "auxGetVolume");
	p[15] = GetProcAddress(hL, "auxOutMessage");
	p[16] = GetProcAddress(hL, "auxSetVolume");
	p[17] = GetProcAddress(hL, "joyConfigChanged");
	p[18] = GetProcAddress(hL, "joyGetDevCapsA");
	p[19] = GetProcAddress(hL, "joyGetDevCapsW");
	p[20] = GetProcAddress(hL, "joyGetNumDevs");
	p[21] = GetProcAddress(hL, "joyGetPos");
	p[22] = GetProcAddress(hL, "joyGetPosEx");
	p[23] = GetProcAddress(hL, "joyGetThreshold");
	p[24] = GetProcAddress(hL, "joyReleaseCapture");
	p[25] = GetProcAddress(hL, "joySetCapture");
	p[26] = GetProcAddress(hL, "joySetThreshold");
	p[27] = GetProcAddress(hL, "mciDriverNotify");
	p[28] = GetProcAddress(hL, "mciDriverYield");
	p[29] = GetProcAddress(hL, "mciExecute");
	p[30] = GetProcAddress(hL, "mciFreeCommandResource");
	p[31] = GetProcAddress(hL, "mciGetCreatorTask");
	p[32] = GetProcAddress(hL, "mciGetDeviceIDA");
	p[33] = GetProcAddress(hL, "mciGetDeviceIDFromElementIDA");
	p[34] = GetProcAddress(hL, "mciGetDeviceIDFromElementIDW");
	p[35] = GetProcAddress(hL, "mciGetDeviceIDW");
	p[36] = GetProcAddress(hL, "mciGetDriverData");
	p[37] = GetProcAddress(hL, "mciGetErrorStringA");
	p[38] = GetProcAddress(hL, "mciGetErrorStringW");
	p[39] = GetProcAddress(hL, "mciGetYieldProc");
	p[40] = GetProcAddress(hL, "mciLoadCommandResource");
	p[41] = GetProcAddress(hL, "mciSendCommandA");
	p[42] = GetProcAddress(hL, "mciSendCommandW");
	p[43] = GetProcAddress(hL, "mciSendStringA");
	p[44] = GetProcAddress(hL, "mciSendStringW");
	p[45] = GetProcAddress(hL, "mciSetDriverData");
	p[46] = GetProcAddress(hL, "mciSetYieldProc");
	p[47] = GetProcAddress(hL, "midiConnect");
	p[48] = GetProcAddress(hL, "midiDisconnect");
	p[49] = GetProcAddress(hL, "midiInAddBuffer");
	p[50] = GetProcAddress(hL, "midiInClose");
	p[51] = GetProcAddress(hL, "midiInGetDevCapsA");
	p[52] = GetProcAddress(hL, "midiInGetDevCapsW");
	p[53] = GetProcAddress(hL, "midiInGetErrorTextA");
	p[54] = GetProcAddress(hL, "midiInGetErrorTextW");
	p[55] = GetProcAddress(hL, "midiInGetID");
	p[56] = GetProcAddress(hL, "midiInGetNumDevs");
	p[57] = GetProcAddress(hL, "midiInMessage");
	p[58] = GetProcAddress(hL, "midiInOpen");
	p[59] = GetProcAddress(hL, "midiInPrepareHeader");
	p[60] = GetProcAddress(hL, "midiInReset");
	p[61] = GetProcAddress(hL, "midiInStart");
	p[62] = GetProcAddress(hL, "midiInStop");
	p[63] = GetProcAddress(hL, "midiInUnprepareHeader");
	p[64] = GetProcAddress(hL, "midiOutCacheDrumPatches");
	p[65] = GetProcAddress(hL, "midiOutCachePatches");
	p[66] = GetProcAddress(hL, "midiOutClose");
	p[67] = GetProcAddress(hL, "midiOutGetDevCapsA");
	p[68] = GetProcAddress(hL, "midiOutGetDevCapsW");
	p[69] = GetProcAddress(hL, "midiOutGetErrorTextA");
	p[70] = GetProcAddress(hL, "midiOutGetErrorTextW");
	p[71] = GetProcAddress(hL, "midiOutGetID");
	p[72] = GetProcAddress(hL, "midiOutGetNumDevs");
	p[73] = GetProcAddress(hL, "midiOutGetVolume");
	p[74] = GetProcAddress(hL, "midiOutLongMsg");
	p[75] = GetProcAddress(hL, "midiOutMessage");
	p[76] = GetProcAddress(hL, "midiOutOpen");
	p[77] = GetProcAddress(hL, "midiOutPrepareHeader");
	p[78] = GetProcAddress(hL, "midiOutReset");
	p[79] = GetProcAddress(hL, "midiOutSetVolume");
	p[80] = GetProcAddress(hL, "midiOutShortMsg");
	p[81] = GetProcAddress(hL, "midiOutUnprepareHeader");
	p[82] = GetProcAddress(hL, "midiStreamClose");
	p[83] = GetProcAddress(hL, "midiStreamOpen");
	p[84] = GetProcAddress(hL, "midiStreamOut");
	p[85] = GetProcAddress(hL, "midiStreamPause");
	p[86] = GetProcAddress(hL, "midiStreamPosition");
	p[87] = GetProcAddress(hL, "midiStreamProperty");
	p[88] = GetProcAddress(hL, "midiStreamRestart");
	p[89] = GetProcAddress(hL, "midiStreamStop");
	p[90] = GetProcAddress(hL, "mixerClose");
	p[91] = GetProcAddress(hL, "mixerGetControlDetailsA");
	p[92] = GetProcAddress(hL, "mixerGetControlDetailsW");
	p[93] = GetProcAddress(hL, "mixerGetDevCapsA");
	p[94] = GetProcAddress(hL, "mixerGetDevCapsW");
	p[95] = GetProcAddress(hL, "mixerGetID");
	p[96] = GetProcAddress(hL, "mixerGetLineControlsA");
	p[97] = GetProcAddress(hL, "mixerGetLineControlsW");
	p[98] = GetProcAddress(hL, "mixerGetLineInfoA");
	p[99] = GetProcAddress(hL, "mixerGetLineInfoW");
	p[100] = GetProcAddress(hL, "mixerGetNumDevs");
	p[101] = GetProcAddress(hL, "mixerMessage");
	p[102] = GetProcAddress(hL, "mixerOpen");
	p[103] = GetProcAddress(hL, "mixerSetControlDetails");
	p[104] = GetProcAddress(hL, "mmDrvInstall");
	p[105] = GetProcAddress(hL, "mmGetCurrentTask");
	p[106] = GetProcAddress(hL, "mmTaskBlock");
	p[107] = GetProcAddress(hL, "mmTaskCreate");
	p[108] = GetProcAddress(hL, "mmTaskSignal");
	p[109] = GetProcAddress(hL, "mmTaskYield");
	p[110] = GetProcAddress(hL, "mmioAdvance");
	p[111] = GetProcAddress(hL, "mmioAscend");
	p[112] = GetProcAddress(hL, "mmioClose");
	p[113] = GetProcAddress(hL, "mmioCreateChunk");
	p[114] = GetProcAddress(hL, "mmioDescend");
	p[115] = GetProcAddress(hL, "mmioFlush");
	p[116] = GetProcAddress(hL, "mmioGetInfo");
	p[117] = GetProcAddress(hL, "mmioInstallIOProcA");
	p[118] = GetProcAddress(hL, "mmioInstallIOProcW");
	p[119] = GetProcAddress(hL, "mmioOpenA");
	p[120] = GetProcAddress(hL, "mmioOpenW");
	p[121] = GetProcAddress(hL, "mmioRead");
	p[122] = GetProcAddress(hL, "mmioRenameA");
	p[123] = GetProcAddress(hL, "mmioRenameW");
	p[124] = GetProcAddress(hL, "mmioSeek");
	p[125] = GetProcAddress(hL, "mmioSendMessage");
	p[126] = GetProcAddress(hL, "mmioSetBuffer");
	p[127] = GetProcAddress(hL, "mmioSetInfo");
	p[128] = GetProcAddress(hL, "mmioStringToFOURCCA");
	p[129] = GetProcAddress(hL, "mmioStringToFOURCCW");
	p[130] = GetProcAddress(hL, "mmioWrite");
	p[131] = GetProcAddress(hL, "mmsystemGetVersion");
	p[132] = GetProcAddress(hL, "sndPlaySoundA");
	p[133] = GetProcAddress(hL, "sndPlaySoundW");
	p[134] = GetProcAddress(hL, "timeBeginPeriod");
	p[135] = GetProcAddress(hL, "timeEndPeriod");
	p[136] = GetProcAddress(hL, "timeGetDevCaps");
	p[137] = GetProcAddress(hL, "timeGetSystemTime");
	p[138] = GetProcAddress(hL, "timeGetTime");
	p[139] = GetProcAddress(hL, "timeKillEvent");
	p[140] = GetProcAddress(hL, "timeSetEvent");
	p[141] = GetProcAddress(hL, "waveInAddBuffer");
	p[142] = GetProcAddress(hL, "waveInClose");
	p[143] = GetProcAddress(hL, "waveInGetDevCapsA");
	p[144] = GetProcAddress(hL, "waveInGetDevCapsW");
	p[145] = GetProcAddress(hL, "waveInGetErrorTextA");
	p[146] = GetProcAddress(hL, "waveInGetErrorTextW");
	p[147] = GetProcAddress(hL, "waveInGetID");
	p[148] = GetProcAddress(hL, "waveInGetNumDevs");
	p[149] = GetProcAddress(hL, "waveInGetPosition");
	p[150] = GetProcAddress(hL, "waveInMessage");
	p[151] = GetProcAddress(hL, "waveInOpen");
	p[152] = GetProcAddress(hL, "waveInPrepareHeader");
	p[153] = GetProcAddress(hL, "waveInReset");
	p[154] = GetProcAddress(hL, "waveInStart");
	p[155] = GetProcAddress(hL, "waveInStop");
	p[156] = GetProcAddress(hL, "waveInUnprepareHeader");
	p[157] = GetProcAddress(hL, "waveOutBreakLoop");
	p[158] = GetProcAddress(hL, "waveOutClose");
	p[159] = GetProcAddress(hL, "waveOutGetDevCapsA");
	p[160] = GetProcAddress(hL, "waveOutGetDevCapsW");
	p[161] = GetProcAddress(hL, "waveOutGetErrorTextA");
	p[162] = GetProcAddress(hL, "waveOutGetErrorTextW");
	p[163] = GetProcAddress(hL, "waveOutGetID");
	p[164] = GetProcAddress(hL, "waveOutGetNumDevs");
	p[165] = GetProcAddress(hL, "waveOutGetPitch");
	p[166] = GetProcAddress(hL, "waveOutGetPlaybackRate");
	p[167] = GetProcAddress(hL, "waveOutGetPosition");
	p[168] = GetProcAddress(hL, "waveOutGetVolume");
	p[169] = GetProcAddress(hL, "waveOutMessage");
	p[170] = GetProcAddress(hL, "waveOutOpen");
	p[171] = GetProcAddress(hL, "waveOutPause");
	p[172] = GetProcAddress(hL, "waveOutPrepareHeader");
	p[173] = GetProcAddress(hL, "waveOutReset");
	p[174] = GetProcAddress(hL, "waveOutRestart");
	p[175] = GetProcAddress(hL, "waveOutSetPitch");
	p[176] = GetProcAddress(hL, "waveOutSetPlaybackRate");
	p[177] = GetProcAddress(hL, "waveOutSetVolume");
	p[178] = GetProcAddress(hL, "waveOutUnprepareHeader");
	p[179] = GetProcAddress(hL, "waveOutWrite");
	p[180] = GetProcAddress(hL, (LPCSTR) "2");

	return TRUE;
}

BOOL WINAPI Winmm_Free(HINSTANCE hInst, DWORD reason)
{
	if (reason != DLL_PROCESS_DETACH)
		return FALSE;

	auto result = FreeLibrary(hL);
	if (result)
		hL = NULL;
	return result;
}

// CloseDriver
extern "C" __declspec(naked) void __stdcall __E__0__()
{
	__asm
	{
		jmp p[0*4];
	}
}

// DefDriverProc
extern "C" __declspec(naked) void __stdcall __E__1__()
{
	__asm
	{
		jmp p[1*4];
	}
}

// DriverCallback
extern "C" __declspec(naked) void __stdcall __E__2__()
{
	__asm
	{
		jmp p[2*4];
	}
}

// DrvGetModuleHandle
extern "C" __declspec(naked) void __stdcall __E__3__()
{
	__asm
	{
		jmp p[3*4];
	}
}

// GetDriverModuleHandle
extern "C" __declspec(naked) void __stdcall __E__4__()
{
	__asm
	{
		jmp p[4*4];
	}
}

// OpenDriver
extern "C" __declspec(naked) void __stdcall __E__5__()
{
	__asm
	{
		jmp p[5*4];
	}
}

// PlaySound
extern "C" __declspec(naked) void __stdcall __E__6__()
{
	__asm
	{
		jmp p[6*4];
	}
}

// PlaySoundA
extern "C" __declspec(naked) void __stdcall __E__7__()
{
	__asm
	{
		jmp p[7*4];
	}
}

// PlaySoundW
extern "C" __declspec(naked) void __stdcall __E__8__()
{
	__asm
	{
		jmp p[8*4];
	}
}

// SendDriverMessage
extern "C" __declspec(naked) void __stdcall __E__9__()
{
	__asm
	{
		jmp p[9*4];
	}
}

// WOWAppExit
extern "C" __declspec(naked) void __stdcall __E__10__()
{
	__asm
	{
		jmp p[10*4];
	}
}

// auxGetDevCapsA
extern "C" __declspec(naked) void __stdcall __E__11__()
{
	__asm
	{
		jmp p[11*4];
	}
}

// auxGetDevCapsW
extern "C" __declspec(naked) void __stdcall __E__12__()
{
	__asm
	{
		jmp p[12*4];
	}
}

// auxGetNumDevs
extern "C" __declspec(naked) void __stdcall __E__13__()
{
	__asm
	{
		jmp p[13*4];
	}
}

// auxGetVolume
extern "C" __declspec(naked) void __stdcall __E__14__()
{
	__asm
	{
		jmp p[14*4];
	}
}

// auxOutMessage
extern "C" __declspec(naked) void __stdcall __E__15__()
{
	__asm
	{
		jmp p[15*4];
	}
}

// auxSetVolume
extern "C" __declspec(naked) void __stdcall __E__16__()
{
	__asm
	{
		jmp p[16*4];
	}
}

// joyConfigChanged
extern "C" __declspec(naked) void __stdcall __E__17__()
{
	__asm
	{
		jmp p[17*4];
	}
}

// joyGetDevCapsA
extern "C" __declspec(naked) void __stdcall __E__18__()
{
	__asm
	{
		jmp p[18*4];
	}
}

// joyGetDevCapsW
extern "C" __declspec(naked) void __stdcall __E__19__()
{
	__asm
	{
		jmp p[19*4];
	}
}

// joyGetNumDevs
extern "C" __declspec(naked) void __stdcall __E__20__()
{
	__asm
	{
		jmp p[20*4];
	}
}

// joyGetPos
extern "C" __declspec(naked) void __stdcall __E__21__()
{
	__asm
	{
		jmp p[21*4];
	}
}

// joyGetPosEx
extern "C" __declspec(naked) void __stdcall __E__22__()
{
	__asm
	{
		jmp p[22*4];
	}
}

// joyGetThreshold
extern "C" __declspec(naked) void __stdcall __E__23__()
{
	__asm
	{
		jmp p[23*4];
	}
}

// joyReleaseCapture
extern "C" __declspec(naked) void __stdcall __E__24__()
{
	__asm
	{
		jmp p[24*4];
	}
}

// joySetCapture
extern "C" __declspec(naked) void __stdcall __E__25__()
{
	__asm
	{
		jmp p[25*4];
	}
}

// joySetThreshold
extern "C" __declspec(naked) void __stdcall __E__26__()
{
	__asm
	{
		jmp p[26*4];
	}
}

// mciDriverNotify
extern "C" __declspec(naked) void __stdcall __E__27__()
{
	__asm
	{
		jmp p[27*4];
	}
}

// mciDriverYield
extern "C" __declspec(naked) void __stdcall __E__28__()
{
	__asm
	{
		jmp p[28*4];
	}
}

// mciExecute
extern "C" __declspec(naked) void __stdcall __E__29__()
{
	__asm
	{
		jmp p[29*4];
	}
}

// mciFreeCommandResource
extern "C" __declspec(naked) void __stdcall __E__30__()
{
	__asm
	{
		jmp p[30*4];
	}
}

// mciGetCreatorTask
extern "C" __declspec(naked) void __stdcall __E__31__()
{
	__asm
	{
		jmp p[31*4];
	}
}

// mciGetDeviceIDA
extern "C" __declspec(naked) void __stdcall __E__32__()
{
	__asm
	{
		jmp p[32*4];
	}
}

// mciGetDeviceIDFromElementIDA
extern "C" __declspec(naked) void __stdcall __E__33__()
{
	__asm
	{
		jmp p[33*4];
	}
}

// mciGetDeviceIDFromElementIDW
extern "C" __declspec(naked) void __stdcall __E__34__()
{
	__asm
	{
		jmp p[34*4];
	}
}

// mciGetDeviceIDW
extern "C" __declspec(naked) void __stdcall __E__35__()
{
	__asm
	{
		jmp p[35*4];
	}
}

// mciGetDriverData
extern "C" __declspec(naked) void __stdcall __E__36__()
{
	__asm
	{
		jmp p[36*4];
	}
}

// mciGetErrorStringA
extern "C" __declspec(naked) void __stdcall __E__37__()
{
	__asm
	{
		jmp p[37*4];
	}
}

// mciGetErrorStringW
extern "C" __declspec(naked) void __stdcall __E__38__()
{
	__asm
	{
		jmp p[38*4];
	}
}

// mciGetYieldProc
extern "C" __declspec(naked) void __stdcall __E__39__()
{
	__asm
	{
		jmp p[39*4];
	}
}

// mciLoadCommandResource
extern "C" __declspec(naked) void __stdcall __E__40__()
{
	__asm
	{
		jmp p[40*4];
	}
}

// mciSendCommandA
extern "C" __declspec(naked) void __stdcall __E__41__()
{
	__asm
	{
		jmp p[41*4];
	}
}

// mciSendCommandW
extern "C" __declspec(naked) void __stdcall __E__42__()
{
	__asm
	{
		jmp p[42*4];
	}
}

// mciSendStringA
extern "C" __declspec(naked) void __stdcall __E__43__()
{
	__asm
	{
		jmp p[43*4];
	}
}

// mciSendStringW
extern "C" __declspec(naked) void __stdcall __E__44__()
{
	__asm
	{
		jmp p[44*4];
	}
}

// mciSetDriverData
extern "C" __declspec(naked) void __stdcall __E__45__()
{
	__asm
	{
		jmp p[45*4];
	}
}

// mciSetYieldProc
extern "C" __declspec(naked) void __stdcall __E__46__()
{
	__asm
	{
		jmp p[46*4];
	}
}

// midiConnect
extern "C" __declspec(naked) void __stdcall __E__47__()
{
	__asm
	{
		jmp p[47*4];
	}
}

// midiDisconnect
extern "C" __declspec(naked) void __stdcall __E__48__()
{
	__asm
	{
		jmp p[48*4];
	}
}

// midiInAddBuffer
extern "C" __declspec(naked) void __stdcall __E__49__()
{
	__asm
	{
		jmp p[49*4];
	}
}

// midiInClose
extern "C" __declspec(naked) void __stdcall __E__50__()
{
	__asm
	{
		jmp p[50*4];
	}
}

// midiInGetDevCapsA
extern "C" __declspec(naked) void __stdcall __E__51__()
{
	__asm
	{
		jmp p[51*4];
	}
}

// midiInGetDevCapsW
extern "C" __declspec(naked) void __stdcall __E__52__()
{
	__asm
	{
		jmp p[52*4];
	}
}

// midiInGetErrorTextA
extern "C" __declspec(naked) void __stdcall __E__53__()
{
	__asm
	{
		jmp p[53*4];
	}
}

// midiInGetErrorTextW
extern "C" __declspec(naked) void __stdcall __E__54__()
{
	__asm
	{
		jmp p[54*4];
	}
}

// midiInGetID
extern "C" __declspec(naked) void __stdcall __E__55__()
{
	__asm
	{
		jmp p[55*4];
	}
}

// midiInGetNumDevs
extern "C" __declspec(naked) void __stdcall __E__56__()
{
	__asm
	{
		jmp p[56*4];
	}
}

// midiInMessage
extern "C" __declspec(naked) void __stdcall __E__57__()
{
	__asm
	{
		jmp p[57*4];
	}
}

// midiInOpen
extern "C" __declspec(naked) void __stdcall __E__58__()
{
	__asm
	{
		jmp p[58*4];
	}
}

// midiInPrepareHeader
extern "C" __declspec(naked) void __stdcall __E__59__()
{
	__asm
	{
		jmp p[59*4];
	}
}

// midiInReset
extern "C" __declspec(naked) void __stdcall __E__60__()
{
	__asm
	{
		jmp p[60*4];
	}
}

// midiInStart
extern "C" __declspec(naked) void __stdcall __E__61__()
{
	__asm
	{
		jmp p[61*4];
	}
}

// midiInStop
extern "C" __declspec(naked) void __stdcall __E__62__()
{
	__asm
	{
		jmp p[62*4];
	}
}

// midiInUnprepareHeader
extern "C" __declspec(naked) void __stdcall __E__63__()
{
	__asm
	{
		jmp p[63*4];
	}
}

// midiOutCacheDrumPatches
extern "C" __declspec(naked) void __stdcall __E__64__()
{
	__asm
	{
		jmp p[64*4];
	}
}

// midiOutCachePatches
extern "C" __declspec(naked) void __stdcall __E__65__()
{
	__asm
	{
		jmp p[65*4];
	}
}

// midiOutClose
extern "C" __declspec(naked) void __stdcall __E__66__()
{
	__asm
	{
		jmp p[66*4];
	}
}

// midiOutGetDevCapsA
extern "C" __declspec(naked) void __stdcall __E__67__()
{
	__asm
	{
		jmp p[67*4];
	}
}

// midiOutGetDevCapsW
extern "C" __declspec(naked) void __stdcall __E__68__()
{
	__asm
	{
		jmp p[68*4];
	}
}

// midiOutGetErrorTextA
extern "C" __declspec(naked) void __stdcall __E__69__()
{
	__asm
	{
		jmp p[69*4];
	}
}

// midiOutGetErrorTextW
extern "C" __declspec(naked) void __stdcall __E__70__()
{
	__asm
	{
		jmp p[70*4];
	}
}

// midiOutGetID
extern "C" __declspec(naked) void __stdcall __E__71__()
{
	__asm
	{
		jmp p[71*4];
	}
}

// midiOutGetNumDevs
extern "C" __declspec(naked) void __stdcall __E__72__()
{
	__asm
	{
		jmp p[72*4];
	}
}

// midiOutGetVolume
extern "C" __declspec(naked) void __stdcall __E__73__()
{
	__asm
	{
		jmp p[73*4];
	}
}

// midiOutLongMsg
extern "C" __declspec(naked) void __stdcall __E__74__()
{
	__asm
	{
		jmp p[74*4];
	}
}

// midiOutMessage
extern "C" __declspec(naked) void __stdcall __E__75__()
{
	__asm
	{
		jmp p[75*4];
	}
}

// midiOutOpen
extern "C" __declspec(naked) void __stdcall __E__76__()
{
	__asm
	{
		jmp p[76*4];
	}
}

// midiOutPrepareHeader
extern "C" __declspec(naked) void __stdcall __E__77__()
{
	__asm
	{
		jmp p[77*4];
	}
}

// midiOutReset
extern "C" __declspec(naked) void __stdcall __E__78__()
{
	__asm
	{
		jmp p[78*4];
	}
}

// midiOutSetVolume
extern "C" __declspec(naked) void __stdcall __E__79__()
{
	__asm
	{
		jmp p[79*4];
	}
}

// midiOutShortMsg
extern "C" __declspec(naked) void __stdcall __E__80__()
{
	__asm
	{
		jmp p[80*4];
	}
}

// midiOutUnprepareHeader
extern "C" __declspec(naked) void __stdcall __E__81__()
{
	__asm
	{
		jmp p[81*4];
	}
}

// midiStreamClose
extern "C" __declspec(naked) void __stdcall __E__82__()
{
	__asm
	{
		jmp p[82*4];
	}
}

// midiStreamOpen
extern "C" __declspec(naked) void __stdcall __E__83__()
{
	__asm
	{
		jmp p[83*4];
	}
}

// midiStreamOut
extern "C" __declspec(naked) void __stdcall __E__84__()
{
	__asm
	{
		jmp p[84*4];
	}
}

// midiStreamPause
extern "C" __declspec(naked) void __stdcall __E__85__()
{
	__asm
	{
		jmp p[85*4];
	}
}

// midiStreamPosition
extern "C" __declspec(naked) void __stdcall __E__86__()
{
	__asm
	{
		jmp p[86*4];
	}
}

// midiStreamProperty
extern "C" __declspec(naked) void __stdcall __E__87__()
{
	__asm
	{
		jmp p[87*4];
	}
}

// midiStreamRestart
extern "C" __declspec(naked) void __stdcall __E__88__()
{
	__asm
	{
		jmp p[88*4];
	}
}

// midiStreamStop
extern "C" __declspec(naked) void __stdcall __E__89__()
{
	__asm
	{
		jmp p[89*4];
	}
}

// mixerClose
extern "C" __declspec(naked) void __stdcall __E__90__()
{
	__asm
	{
		jmp p[90*4];
	}
}

// mixerGetControlDetailsA
extern "C" __declspec(naked) void __stdcall __E__91__()
{
	__asm
	{
		jmp p[91*4];
	}
}

// mixerGetControlDetailsW
extern "C" __declspec(naked) void __stdcall __E__92__()
{
	__asm
	{
		jmp p[92*4];
	}
}

// mixerGetDevCapsA
extern "C" __declspec(naked) void __stdcall __E__93__()
{
	__asm
	{
		jmp p[93*4];
	}
}

// mixerGetDevCapsW
extern "C" __declspec(naked) void __stdcall __E__94__()
{
	__asm
	{
		jmp p[94*4];
	}
}

// mixerGetID
extern "C" __declspec(naked) void __stdcall __E__95__()
{
	__asm
	{
		jmp p[95*4];
	}
}

// mixerGetLineControlsA
extern "C" __declspec(naked) void __stdcall __E__96__()
{
	__asm
	{
		jmp p[96*4];
	}
}

// mixerGetLineControlsW
extern "C" __declspec(naked) void __stdcall __E__97__()
{
	__asm
	{
		jmp p[97*4];
	}
}

// mixerGetLineInfoA
extern "C" __declspec(naked) void __stdcall __E__98__()
{
	__asm
	{
		jmp p[98*4];
	}
}

// mixerGetLineInfoW
extern "C" __declspec(naked) void __stdcall __E__99__()
{
	__asm
	{
		jmp p[99*4];
	}
}

// mixerGetNumDevs
extern "C" __declspec(naked) void __stdcall __E__100__()
{
	__asm
	{
		jmp p[100*4];
	}
}

// mixerMessage
extern "C" __declspec(naked) void __stdcall __E__101__()
{
	__asm
	{
		jmp p[101*4];
	}
}

// mixerOpen
extern "C" __declspec(naked) void __stdcall __E__102__()
{
	__asm
	{
		jmp p[102*4];
	}
}

// mixerSetControlDetails
extern "C" __declspec(naked) void __stdcall __E__103__()
{
	__asm
	{
		jmp p[103*4];
	}
}

// mmDrvInstall
extern "C" __declspec(naked) void __stdcall __E__104__()
{
	__asm
	{
		jmp p[104*4];
	}
}

// mmGetCurrentTask
extern "C" __declspec(naked) void __stdcall __E__105__()
{
	__asm
	{
		jmp p[105*4];
	}
}

// mmTaskBlock
extern "C" __declspec(naked) void __stdcall __E__106__()
{
	__asm
	{
		jmp p[106*4];
	}
}

// mmTaskCreate
extern "C" __declspec(naked) void __stdcall __E__107__()
{
	__asm
	{
		jmp p[107*4];
	}
}

// mmTaskSignal
extern "C" __declspec(naked) void __stdcall __E__108__()
{
	__asm
	{
		jmp p[108*4];
	}
}

// mmTaskYield
extern "C" __declspec(naked) void __stdcall __E__109__()
{
	__asm
	{
		jmp p[109*4];
	}
}

// mmioAdvance
extern "C" __declspec(naked) void __stdcall __E__110__()
{
	__asm
	{
		jmp p[110*4];
	}
}

// mmioAscend
extern "C" __declspec(naked) void __stdcall __E__111__()
{
	__asm
	{
		jmp p[111*4];
	}
}

// mmioClose
extern "C" __declspec(naked) void __stdcall __E__112__()
{
	__asm
	{
		jmp p[112*4];
	}
}

// mmioCreateChunk
extern "C" __declspec(naked) void __stdcall __E__113__()
{
	__asm
	{
		jmp p[113*4];
	}
}

// mmioDescend
extern "C" __declspec(naked) void __stdcall __E__114__()
{
	__asm
	{
		jmp p[114*4];
	}
}

// mmioFlush
extern "C" __declspec(naked) void __stdcall __E__115__()
{
	__asm
	{
		jmp p[115*4];
	}
}

// mmioGetInfo
extern "C" __declspec(naked) void __stdcall __E__116__()
{
	__asm
	{
		jmp p[116*4];
	}
}

// mmioInstallIOProcA
extern "C" __declspec(naked) void __stdcall __E__117__()
{
	__asm
	{
		jmp p[117*4];
	}
}

// mmioInstallIOProcW
extern "C" __declspec(naked) void __stdcall __E__118__()
{
	__asm
	{
		jmp p[118*4];
	}
}

// mmioOpenA
extern "C" __declspec(naked) void __stdcall __E__119__()
{
	__asm
	{
		jmp p[119*4];
	}
}

// mmioOpenW
extern "C" __declspec(naked) void __stdcall __E__120__()
{
	__asm
	{
		jmp p[120*4];
	}
}

// mmioRead
extern "C" __declspec(naked) void __stdcall __E__121__()
{
	__asm
	{
		jmp p[121*4];
	}
}

// mmioRenameA
extern "C" __declspec(naked) void __stdcall __E__122__()
{
	__asm
	{
		jmp p[122*4];
	}
}

// mmioRenameW
extern "C" __declspec(naked) void __stdcall __E__123__()
{
	__asm
	{
		jmp p[123*4];
	}
}

// mmioSeek
extern "C" __declspec(naked) void __stdcall __E__124__()
{
	__asm
	{
		jmp p[124*4];
	}
}

// mmioSendMessage
extern "C" __declspec(naked) void __stdcall __E__125__()
{
	__asm
	{
		jmp p[125*4];
	}
}

// mmioSetBuffer
extern "C" __declspec(naked) void __stdcall __E__126__()
{
	__asm
	{
		jmp p[126*4];
	}
}

// mmioSetInfo
extern "C" __declspec(naked) void __stdcall __E__127__()
{
	__asm
	{
		jmp p[127*4];
	}
}

// mmioStringToFOURCCA
extern "C" __declspec(naked) void __stdcall __E__128__()
{
	__asm
	{
		jmp p[128*4];
	}
}

// mmioStringToFOURCCW
extern "C" __declspec(naked) void __stdcall __E__129__()
{
	__asm
	{
		jmp p[129*4];
	}
}

// mmioWrite
extern "C" __declspec(naked) void __stdcall __E__130__()
{
	__asm
	{
		jmp p[130*4];
	}
}

// mmsystemGetVersion
extern "C" __declspec(naked) void __stdcall __E__131__()
{
	__asm
	{
		jmp p[131*4];
	}
}

// sndPlaySoundA
extern "C" __declspec(naked) void __stdcall __E__132__()
{
	__asm
	{
		jmp p[132*4];
	}
}

// sndPlaySoundW
extern "C" __declspec(naked) void __stdcall __E__133__()
{
	__asm
	{
		jmp p[133*4];
	}
}

// timeBeginPeriod
extern "C" __declspec(naked) void __stdcall __E__134__()
{
	__asm
	{
		jmp p[134*4];
	}
}

// timeEndPeriod
extern "C" __declspec(naked) void __stdcall __E__135__()
{
	__asm
	{
		jmp p[135*4];
	}
}

// timeGetDevCaps
extern "C" __declspec(naked) void __stdcall __E__136__()
{
	__asm
	{
		jmp p[136*4];
	}
}

// timeGetSystemTime
extern "C" __declspec(naked) void __stdcall __E__137__()
{
	__asm
	{
		jmp p[137*4];
	}
}

// timeGetTime
extern "C" __declspec(naked) void __stdcall __E__138__()
{
	__asm
	{
		jmp p[138*4];
	}
}

// timeKillEvent
extern "C" __declspec(naked) void __stdcall __E__139__()
{
	__asm
	{
		jmp p[139*4];
	}
}

// timeSetEvent
extern "C" __declspec(naked) void __stdcall __E__140__()
{
	__asm
	{
		jmp p[140*4];
	}
}

// waveInAddBuffer
extern "C" __declspec(naked) void __stdcall __E__141__()
{
	__asm
	{
		jmp p[141*4];
	}
}

// waveInClose
extern "C" __declspec(naked) void __stdcall __E__142__()
{
	__asm
	{
		jmp p[142*4];
	}
}

// waveInGetDevCapsA
extern "C" __declspec(naked) void __stdcall __E__143__()
{
	__asm
	{
		jmp p[143*4];
	}
}

// waveInGetDevCapsW
extern "C" __declspec(naked) void __stdcall __E__144__()
{
	__asm
	{
		jmp p[144*4];
	}
}

// waveInGetErrorTextA
extern "C" __declspec(naked) void __stdcall __E__145__()
{
	__asm
	{
		jmp p[145*4];
	}
}

// waveInGetErrorTextW
extern "C" __declspec(naked) void __stdcall __E__146__()
{
	__asm
	{
		jmp p[146*4];
	}
}

// waveInGetID
extern "C" __declspec(naked) void __stdcall __E__147__()
{
	__asm
	{
		jmp p[147*4];
	}
}

// waveInGetNumDevs
extern "C" __declspec(naked) void __stdcall __E__148__()
{
	__asm
	{
		jmp p[148*4];
	}
}

// waveInGetPosition
extern "C" __declspec(naked) void __stdcall __E__149__()
{
	__asm
	{
		jmp p[149*4];
	}
}

// waveInMessage
extern "C" __declspec(naked) void __stdcall __E__150__()
{
	__asm
	{
		jmp p[150*4];
	}
}

// waveInOpen
extern "C" __declspec(naked) void __stdcall __E__151__()
{
	__asm
	{
		jmp p[151*4];
	}
}

// waveInPrepareHeader
extern "C" __declspec(naked) void __stdcall __E__152__()
{
	__asm
	{
		jmp p[152*4];
	}
}

// waveInReset
extern "C" __declspec(naked) void __stdcall __E__153__()
{
	__asm
	{
		jmp p[153*4];
	}
}

// waveInStart
extern "C" __declspec(naked) void __stdcall __E__154__()
{
	__asm
	{
		jmp p[154*4];
	}
}

// waveInStop
extern "C" __declspec(naked) void __stdcall __E__155__()
{
	__asm
	{
		jmp p[155*4];
	}
}

// waveInUnprepareHeader
extern "C" __declspec(naked) void __stdcall __E__156__()
{
	__asm
	{
		jmp p[156*4];
	}
}

// waveOutBreakLoop
extern "C" __declspec(naked) void __stdcall __E__157__()
{
	__asm
	{
		jmp p[157*4];
	}
}

// waveOutClose
extern "C" __declspec(naked) void __stdcall __E__158__()
{
	__asm
	{
		jmp p[158*4];
	}
}

// waveOutGetDevCapsA
extern "C" __declspec(naked) void __stdcall __E__159__()
{
	__asm
	{
		jmp p[159*4];
	}
}

// waveOutGetDevCapsW
extern "C" __declspec(naked) void __stdcall __E__160__()
{
	__asm
	{
		jmp p[160*4];
	}
}

// waveOutGetErrorTextA
extern "C" __declspec(naked) void __stdcall __E__161__()
{
	__asm
	{
		jmp p[161*4];
	}
}

// waveOutGetErrorTextW
extern "C" __declspec(naked) void __stdcall __E__162__()
{
	__asm
	{
		jmp p[162*4];
	}
}

// waveOutGetID
extern "C" __declspec(naked) void __stdcall __E__163__()
{
	__asm
	{
		jmp p[163*4];
	}
}

// waveOutGetNumDevs
extern "C" __declspec(naked) void __stdcall __E__164__()
{
	__asm
	{
		jmp p[164*4];
	}
}

// waveOutGetPitch
extern "C" __declspec(naked) void __stdcall __E__165__()
{
	__asm
	{
		jmp p[165*4];
	}
}

// waveOutGetPlaybackRate
extern "C" __declspec(naked) void __stdcall __E__166__()
{
	__asm
	{
		jmp p[166*4];
	}
}

// waveOutGetPosition
extern "C" __declspec(naked) void __stdcall __E__167__()
{
	__asm
	{
		jmp p[167*4];
	}
}

// waveOutGetVolume
extern "C" __declspec(naked) void __stdcall __E__168__()
{
	__asm
	{
		jmp p[168*4];
	}
}

// waveOutMessage
extern "C" __declspec(naked) void __stdcall __E__169__()
{
	__asm
	{
		jmp p[169*4];
	}
}

// waveOutOpen
extern "C" __declspec(naked) void __stdcall __E__170__()
{
	__asm
	{
		jmp p[170*4];
	}
}

// waveOutPause
extern "C" __declspec(naked) void __stdcall __E__171__()
{
	__asm
	{
		jmp p[171*4];
	}
}

// waveOutPrepareHeader
extern "C" __declspec(naked) void __stdcall __E__172__()
{
	__asm
	{
		jmp p[172*4];
	}
}

// waveOutReset
extern "C" __declspec(naked) void __stdcall __E__173__()
{
	__asm
	{
		jmp p[173*4];
	}
}

// waveOutRestart
extern "C" __declspec(naked) void __stdcall __E__174__()
{
	__asm
	{
		jmp p[174*4];
	}
}

// waveOutSetPitch
extern "C" __declspec(naked) void __stdcall __E__175__()
{
	__asm
	{
		jmp p[175*4];
	}
}

// waveOutSetPlaybackRate
extern "C" __declspec(naked) void __stdcall __E__176__()
{
	__asm
	{
		jmp p[176*4];
	}
}

// waveOutSetVolume
extern "C" __declspec(naked) void __stdcall __E__177__()
{
	__asm
	{
		jmp p[177*4];
	}
}

// waveOutUnprepareHeader
extern "C" __declspec(naked) void __stdcall __E__178__()
{
	__asm
	{
		jmp p[178*4];
	}
}

// waveOutWrite
extern "C" __declspec(naked) void __stdcall __E__179__()
{
	__asm
	{
		jmp p[179*4];
	}
}

// ___XXX___181
extern "C" __declspec(naked) void __stdcall __E__180__()
{
	__asm
	{
		jmp p[180*4];
	}
}
