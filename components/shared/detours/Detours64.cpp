#include "stdafx.h"

#ifdef _WIN64
namespace Detours
{
	namespace X64
	{
		#define HEADER_MAGIC		'@D64'

		#define MAX_INSTRUCT_SIZE	0x10

		#define RAX_JUMP_64			0x0C	// mov rax, <addr>; jmp rax;
		#define PUSH_RET_LENGTH_64	0x0E	// push <low 32 addr>; mov [rsp+4h], <hi 32 addr>; ret;

		#define ALIGN_64(x)			DetourAlignAddress((uint64_t)(x), 0x10);

		uint8_t *DetourFunction(uint8_t *Target, uint8_t *Detour, X64Option Options)
		{
			// Decode the actual assembly
			_DecodedInst decodedInstructions[DISASM_MAX_INSTRUCTIONS];
			uint32_t decodedInstructionsCount = 0;

			_DecodeResult res = distorm_decode64((_OffsetType)Target, (const unsigned char *)Target, 32, Decode64Bits, decodedInstructions, DISASM_MAX_INSTRUCTIONS, &decodedInstructionsCount);

			// Check if decoding failed
			if (res != DECRES_SUCCESS)
				return nullptr;

			// Calculate the hook length from options
			uint32_t totalInstrSize = 0;
			uint32_t neededSize		= DetourGetHookLength(Options);

			// Calculate how many instructions are needed to place the jump
			for (uint32_t i = 0; i < decodedInstructionsCount; i++)
			{
				totalInstrSize += decodedInstructions[i].size;

				if (totalInstrSize >= neededSize)
					break;
			}

			// Unable to find a needed length
			if (totalInstrSize < neededSize)
				return nullptr;

			// Allocate the trampoline data
			uint32_t allocSize = 0;
			allocSize += sizeof(JumpTrampolineHeader);	// Base structure
			allocSize += totalInstrSize;				// Size of the copied instructions
			allocSize += MAX_INSTRUCT_SIZE;				// See PUSH_RET_LENGTH_64
			allocSize += MAX_INSTRUCT_SIZE;				// See PUSH_RET_LENGTH_64
			allocSize += 0x64;							// Padding for any memory alignment

			uint8_t *jumpTrampolinePtr = (uint8_t *)VirtualAlloc(nullptr, allocSize, MEM_COMMIT, PAGE_EXECUTE_READWRITE);

			if (!jumpTrampolinePtr)
				return nullptr;

			// Fill out the header
			JumpTrampolineHeader *header = (JumpTrampolineHeader *)jumpTrampolinePtr;

			header->Magic				= HEADER_MAGIC;
			header->Random				= GetCurrentProcessId() + GetCurrentThreadId() + totalInstrSize;

			header->CodeOffset			= Target;
			header->DetourOffset		= Detour;

			header->InstructionLength	= totalInstrSize;
			header->InstructionOffset	= ALIGN_64(jumpTrampolinePtr + sizeof(JumpTrampolineHeader));

			header->TrampolineLength	= MAX_INSTRUCT_SIZE;
			header->TrampolineOffset	= ALIGN_64(header->InstructionOffset + header->InstructionLength + MAX_INSTRUCT_SIZE + header->TrampolineLength);

			// Copy the fixed instructions over
			memcpy(header->InstructionOffset, Target, header->InstructionLength);

			// Write the assembly in the allocation block
			DetourWriteStub(header);

			bool result = true;

			switch (Options)
			{
			case X64Option::USE_RAX_JUMP:	result = DetourWriteRaxJump(header);break;
			case X64Option::USE_PUSH_RET:	result = DetourWritePushRet(header);break;
			default:						result = false;						break;
			}

			// If an operation failed free the memory and exit
			if (!result)
			{
				VirtualFree(jumpTrampolinePtr, 0, MEM_RELEASE);
				return nullptr;
			}

			// Force flush any possible CPU cache
			DetourFlushCache(Target, totalInstrSize);
			DetourFlushCache(jumpTrampolinePtr, allocSize);

			// Set read/execution on the page
			DWORD dwOld = 0;
			VirtualProtect(jumpTrampolinePtr, allocSize, PAGE_EXECUTE_READ, &dwOld);

			return header->InstructionOffset;
		}

		bool DetourRemove(uint8_t *Trampoline)
		{
			return false;
		}

		uint8_t *DetourVTable(uint8_t *Target, uint8_t *Detour, uint32_t TableIndex)
		{
			// Each function is stored in an array
			uint8_t *virtualPointer = (Target + (TableIndex * sizeof(sizeptr_t)));

			DWORD dwOld = 0;
			if (!VirtualProtect(virtualPointer, sizeof(sizeptr_t), PAGE_EXECUTE_READWRITE, &dwOld))
				return nullptr;

			uint8_t *original = (uint8_t *)InterlockedExchange64((volatile LONG64 *)virtualPointer, (LONG64)Detour);

			VirtualProtect(virtualPointer, sizeof(sizeptr_t), dwOld, &dwOld);

			return original;
		}

		bool VTableRemove(uint8_t *Target, uint8_t *Function, uint32_t TableIndex)
		{
			// Reverse VTable detour
			return DetourVTable(Target, Function, TableIndex) != nullptr;
		}

		void DetourWriteStub(JumpTrampolineHeader *Header)
		{
			/********** Allocated code block modifications **********/

			// Determine where the 'unhooked' part of the function starts
			uint8_t *unhookStart = (Header->CodeOffset + Header->InstructionLength);

			// Jump to hooked function (Backed up instructions)
			uint8_t *binstr_ptr = (Header->InstructionOffset + Header->InstructionLength);

			AsmGen hookGen(binstr_ptr, ASMGEN_64);
			hookGen.AddCode("push 0x%X",						((uint64_t)unhookStart) & 0xFFFFFFFF);
			hookGen.AddCode("mov dword ptr ss:[rsp+0x4], 0x%X", ((uint64_t)unhookStart) >> 32);
			hookGen.AddCode("retn");

			memcpy(binstr_ptr, hookGen.GetStream(), hookGen.GetStreamLength());

			// Jump to user function (Write the trampoline)
			AsmGen userGen(Header->TrampolineOffset, ASMGEN_64);
			userGen.AddCode("push 0x%X",						((uint64_t)Header->DetourOffset) & 0xFFFFFFFF);
			userGen.AddCode("mov dword ptr ss:[rsp+0x4], 0x%X", ((uint64_t)Header->DetourOffset) >> 32);
			userGen.AddCode("retn");

			memcpy(Header->TrampolineOffset, userGen.GetStream(), userGen.GetStreamLength());
		}

		bool DetourWriteRaxJump(JumpTrampolineHeader *Header)
		{
			AsmGen gen(Header->CodeOffset, ASMGEN_64);

			// Jump to trampoline (from hooked function)
			gen.AddCode("mov rax, 0x%llx", Header->TrampolineOffset);
			gen.AddCode("jmp rax");

			return DetourCopyMemory(Header->CodeOffset, gen.GetStream(), gen.GetStreamLength());
		}

		bool DetourWritePushRet(JumpTrampolineHeader *Header)
		{
			AsmGen gen(Header->CodeOffset, ASMGEN_64);

			// Jump with PUSH/RET (push low32, [rsp+4h] = hi32)
			gen.AddCode("push 0x%X",						((uint64_t)Header->TrampolineOffset) & 0xFFFFFFFF);
			gen.AddCode("mov dword ptr ss:[rsp+0x4], 0x%X", ((uint64_t)Header->TrampolineOffset) >> 32);
			gen.AddCode("retn");

			return DetourCopyMemory(Header->CodeOffset, gen.GetStream(), gen.GetStreamLength());
		}

		uint32_t DetourGetHookLength(X64Option Options)
		{
			uint32_t size = 0;

			switch(Options)
			{
			case X64Option::USE_RAX_JUMP:	size += RAX_JUMP_64;		break;
			case X64Option::USE_PUSH_RET:	size += PUSH_RET_LENGTH_64;	break;
			default:						size = 0;					break;
			}

			return size;
		}
	}
}
#endif // _WIN64