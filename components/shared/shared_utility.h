#pragma once

//
// Fix for typedef errors regarding int8_t etc.
//
#include <stdint.h>

#include <Windows.h>
#include <string>

#include "detours/Detours.h"

#include "../shared/shared_assert.h"

static void PatchMemory(ULONG_PTR Address, PBYTE Data, SIZE_T Size)
{
	DWORD d = 0;
	VirtualProtect((LPVOID)Address, Size, PAGE_EXECUTE_READWRITE, &d);

	for (SIZE_T i = 0; i < Size; i++)
		*(volatile BYTE *)(Address + i) = *Data++;

	VirtualProtect((LPVOID)Address, Size, d, &d);

	FlushInstructionCache(GetCurrentProcess(), (LPVOID)Address, Size);
}

template<typename T>
static inline void PatchPointer(ULONG_PTR Address, T* Value)
{
	PatchMemory(Address, (PBYTE)&Value, sizeof(Value));
}

//
// Only supports the 0xE8 opcode
//
static void PatchCall(ULONG_PTR instr, PBYTE dest)
{
	BYTE* opcode = (BYTE*)instr;
	ASSERT_MSG_VA(*opcode == 0xE8, "Incorrect opcode at 0x%.08X", instr);

	DWORD d = 0;
	VirtualProtect((LPVOID)instr, 5, PAGE_EXECUTE_READWRITE, &d);

	DWORD newOperand = reinterpret_cast<DWORD>(dest) - reinterpret_cast<DWORD>(opcode + 5);
	((DWORD*)(opcode + 1))[0] = newOperand;

	VirtualProtect((LPVOID)instr, 5, d, &d);

	FlushInstructionCache(GetCurrentProcess(), (LPVOID)instr, 5);
}

//
// Only supports the 0xE9 opcode
//
static void PatchJump(ULONG_PTR instr, PBYTE dest)
{
	BYTE* opcode = (BYTE*)instr;
	ASSERT(*opcode == 0xE9);

	DWORD d = 0;
	VirtualProtect((LPVOID)instr, 5, PAGE_EXECUTE_READWRITE, &d);

	DWORD newOperand = reinterpret_cast<DWORD>(dest)-reinterpret_cast<DWORD>(opcode + 5);
	((DWORD*)(opcode + 1))[0] = newOperand;

	VirtualProtect((LPVOID)instr, 5, d, &d);

	FlushInstructionCache(GetCurrentProcess(), (LPVOID)instr, 5);
}

//
// Only supports the 0x68 opcode
//
static void PatchPush(ULONG_PTR instr, PBYTE dest)
{
	BYTE* opcode = (BYTE*)instr;
	ASSERT_MSG_VA(*opcode == 0x68, "Incorrect opcode at 0x%.08X", instr);

	PatchMemory(instr + 1, (PBYTE)dest, sizeof(dest));
}

static void PatchMemory_WithNOP(ULONG_PTR Address, SIZE_T Size)
{
	DWORD d = 0;
	VirtualProtect((LPVOID)Address, Size, PAGE_EXECUTE_READWRITE, &d);

	for (SIZE_T i = 0; i < Size; i++)
		*(volatile BYTE *)(Address + i) = 0x90; //0x90 == opcode for NOP

	VirtualProtect((LPVOID)Address, Size, d, &d);

	FlushInstructionCache(GetCurrentProcess(), (LPVOID)Address, Size);
}

static void FixupFunction(ULONG_PTR Address, ULONG_PTR DestAddress)
{
	DWORD data = (DestAddress - Address - 5);

	PatchMemory(Address + 0, (PBYTE)"\xE9", 1);
	PatchMemory(Address + 1, (PBYTE)&data, 4);
}

static bool StrEndsWith (std::string str, std::string substr)
{
    if (str.length() >= substr.length())
        return (0 == str.compare (str.length() - substr.length(), substr.length(), substr));
	else
        return false;
}

static bool GetGameDirectory(char *Buffer, DWORD BufferSize)
{
	// Zero initial value
	memset(Buffer, 0, sizeof(char) * BufferSize);

	// Using a lambda here makes it easier to organize the execution flow
	auto GetGameDirectoryFromRegistryKey = [&Buffer, &BufferSize] {
		//
		// 64: HKEY_LOCAL_MACHINE\SOFTWARE\[ Wow6432Node ]\activision\call of duty black ops
		// 32: HKEY_LOCAL_MACHINE\SOFTWARE\activision\call of duty black ops
		//
		// Subkey: installpath
		//
		HKEY regKey;
		if (RegOpenKeyA(HKEY_LOCAL_MACHINE, "SOFTWARE\\activision\\call of duty black ops", &regKey) != ERROR_SUCCESS)
			return false;

		if (RegQueryValueExA(regKey, "installpath", nullptr, nullptr, (LPBYTE)Buffer, &BufferSize) != ERROR_SUCCESS)
			return false;

		// Check that the key is filled and the directory exists
		if (strlen(Buffer) <= 0)
			return false;

		DWORD dirType = GetFileAttributesA(Buffer);

		if ((dirType == INVALID_FILE_ATTRIBUTES) ||
			!(dirType & FILE_ATTRIBUTE_DIRECTORY))
			return false;

		return true;
	};

	// Attempt to locate the game directory through known registry keys
	if (GetGameDirectoryFromRegistryKey())
		return true;

	// TODO
	// Fallback to searching backwards along the current file path
	return false;
}

namespace Detours
{
	namespace X86
	{
		class LogHook
		{
		private:
			void* rtn;
			BYTE* op;
			void* fnc;
			char* msg;

		public:
			LogHook(const char* message)
			{
				// Using new[] instead of malloc causes the VS2017 compiler to crash
				op = (BYTE*)malloc(23);
				ASSERT(op != NULL);

				PatchMemory((ULONG_PTR)op + 0,	(PBYTE)"\x60", 1);						//pusha
				PatchMemory((ULONG_PTR)op + 1,	(PBYTE)"\xFF\x35\x00\x00\x00\x00", 6);	//push str
				PatchMemory((ULONG_PTR)op + 7,	(PBYTE)"\xFF\x15\x00\x00\x00\x00", 6);	//call printf
				PatchMemory((ULONG_PTR)op + 13,	(PBYTE)"\x83\xC4\x04", 3);				//add esp, 4
				PatchMemory((ULONG_PTR)op + 16,	(PBYTE)"\x61", 1);						//popa
				PatchMemory((ULONG_PTR)op + 17,	(PBYTE)"\xFF\x25\x00\x00\x00\x00", 6);	//jmp rtn

				this->msg = (char*)message;
				this->fnc = (void*)&PrintMessage;
				void** str = (void**)&msg;
				
				void** prnt = &fnc;
				void** retn = &rtn;

				PatchMemory((ULONG_PTR)op + 3,	(PBYTE)&str, 4);	//fix str
				PatchMemory((ULONG_PTR)op + 9,	(PBYTE)&prnt, 4);	//fix printf
				PatchMemory((ULONG_PTR)op + 19,	(PBYTE)&retn, 4);	//fix rtn
			}

			~LogHook()
			{
				if (op)
					free(op);
				op = NULL;
			}

			static void __cdecl PrintMessage(const char* str)
			{
				OutputDebugStringA(str);
			}

			friend void AddLogHook(uint8_t * target, const char* msg);
			friend static void RemoveLogHook(uint8_t * target);
		};

		// Print a message when the code at the target address is executed
		static void AddLogHook(uint8_t * target, const char* msg)
		{
			LogHook* hk = new LogHook(msg);
			hk->rtn = DetourFunction(target, hk->op, Detours::X86Option::USE_JUMP);
		}

		static void* GetDest_JmpRel32(BYTE* instr)
		{
			ASSERT(*instr == 0xE9);
			int* target_rel32 = (int*)(instr + 1); 
			return instr + 5 + *target_rel32; // jmp instruction offset + the relative offset that it would jump to + size of jmp instruction
		}

		// Remove a log hook and restore the original code, where target matches the target argument used in AddLogHook
		static void RemoveLogHook(uint8_t * target)
		{
			void* trampoline = GetDest_JmpRel32(target);
			BYTE* op = (BYTE*)GetDest_JmpRel32((BYTE*)trampoline);

			LogHook* hk = *(LogHook**)(op + 19);

			DetourRemove((PBYTE)hk->rtn);
			delete[] hk;
		}
	}
}

template <typename _New_t, typename _Old_t>
static inline _New_t* ForceCastPointer(_Old_t* addr)
{
	union
	{
		_Old_t* _old;
		_New_t* _new;
	} u;

	u._old = addr;
	return u._new;
}
