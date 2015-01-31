#pragma once

namespace Detours
{
	const static uint32_t DISASM_MAX_INSTRUCTIONS	= 20;		// Maximum number of instructions to decode at once

	const static uint32_t OPT_MASK					= 0xFFF;	// Mask for all options
	const static uint32_t OPT_NONE					= 0x000;	// No options
	const static uint32_t OPT_INSTRUCTION_PADDING	= 0x100;	// Add useless instructions to help hide the detour

	enum class X86Option
	{
		USE_JUMP,		// jmp <address>;
		USE_CALL,		// call <address>;
		USE_JUMP_PTR,	// jmp dword ptr <address>;
		USE_PUSH_RET,	// push <address>; retn;
	};

#ifdef _WIN64
	enum class X64Option
	{
		USE_PUSH_RET,	// push <low 32 addr>; push <high 32 addr>; ret;
	};
#endif

	struct JumpTrampolineHeader
	{
		uint32_t Magic;				// Used to verify the header
		uint32_t Random;			// Variable to change the code/data hash

		uint8_t *CodeOffset;		// Code offset that was hooked
		uint8_t *DetourOffset;		// User function offset

		uint32_t InstructionLength;	// Length of the instructions that were replaced
		uint8_t *InstructionOffset;	// Where the backed-up instructions are placed

		uint32_t TrampolineLength;	// Length of the trampoline
		uint8_t *TrampolineOffset;	// Code offset where 'jmp (q/d)word ptr <user function>' occurs

		// Anything after this struct is null data or pure code (instructions/trampoline)
	};

	extern uint32_t GlobalOptions;

	void		SetGlobalOptions(uint32_t Options);

	uint8_t		*DetourAlignAddress(uint64_t Address, uint8_t Align);

	bool		DetourAtomicCopy4X8(uint8_t *Target, uint8_t *Memory, sizeptr_t Length);
	bool		DetourCopyMemory(uint8_t *Target, uint8_t *Memory, sizeptr_t Length);
	bool		DetourFlushCache(uint8_t *Target, sizeptr_t Length);

#if defined(_WIN32) || defined(_WIN64)
	namespace X86
	{
		// Redirects a single static function to another
		uint8_t		*DetourFunction(uint8_t *Target, uint8_t *Detour, X86Option Options = X86Option::USE_JUMP);

		// Redirects a class member function (__thiscall) to another
		template<typename T>
		uint8_t		*DetourClassFunction(uint8_t *Target, T Detour, X86Option Options = X86Option::USE_JUMP)
		{
			return DetourFunction(Target, *(uint8_t **)&Detour, Options);
		}

		// Removes a detoured function (Static or class member)
		bool		DetourRemove(uint8_t *Trampoline);

		// Redirects an index in a virtual table
		uint8_t		*DetourVTable(uint8_t *Target, uint8_t *Detour, uint32_t TableIndex);

		// Removes a detoured virtual table index
		bool		VTableRemove(uint8_t *Target, uint8_t *Function, uint32_t TableIndex);

		void		DetourWriteStub(JumpTrampolineHeader *Header);
		bool		DetourWriteJump(JumpTrampolineHeader *Header);
		bool		DetourWriteCall(JumpTrampolineHeader *Header);
		bool		DetourWriteJumpPtr(JumpTrampolineHeader *Header);
		bool		DetourWritePushRet(JumpTrampolineHeader *Header);

		uint32_t	DetourGetHookLength(X86Option Options);
	}
#endif // defined(_WIN32) || defined(_WIN64)

#ifdef _WIN64
	namespace X64
	{
		// Redirects a single static function to another
		uint8_t		*DetourFunction(uint8_t *Target, uint8_t *Detour, X64Option Options = X64Option::USE_PUSH_RET);

		// Redirects a class member function (__thiscall) to another
		template<typename T>
		uint8_t		*DetourClassFunction(uint8_t *Target, T Detour, X64Option Options = X64Option::USE_PUSH_RET)
		{
			return DetourFunction(Target, *(uint8_t **)&Detour, Options);
		}

		// Removes a detoured function (Static or class member)
		bool		DetourRemove(uint8_t *Trampoline);

		// Redirects an index in a virtual table
		uint8_t		*DetourVTable(uint8_t *Target, uint8_t *Detour, uint32_t TableIndex);

		// Removes a detoured virtual table index
		bool		VTableRemove(uint8_t *Target, uint8_t *Function, uint32_t TableIndex);

		uint32_t	DetourGetHookLength(X64Option Options);
	}
#endif // _WIN64
}