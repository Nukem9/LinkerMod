#pragma once

#ifdef _M_IX86
static
LONGLONG
_intrinInterlockedCompareExchange64(
	__inout LONGLONG volatile *Destination,
	__in    LONGLONG ExChange,
	__in    LONGLONG Comperand
)
{
	__asm
	{
		mov esi, [Destination]
		mov ebx, dword ptr [ExChange]
		mov ecx, dword ptr [ExChange + 4]
		mov eax, dword ptr [Comperand]
		mov edx, dword ptr [Comperand + 4]
		lock cmpxchg8b qword ptr [esi]
	}
}

FORCEINLINE
LONGLONG
_intrinInterlockedExchange64(
	__inout LONGLONG volatile *Target,
	__in    LONGLONG Value
)
{
	LONGLONG Old;

	do
	{
		Old = *Target;
	} while (_intrinInterlockedCompareExchange64(Target, Value, Old) != Old);

	return Old;
}
#else
#define _intrinInterlockedCompareExchange64 InterlockedCompareExchange64
#define _intrinInterlockedExchange64		InterlockedExchange64
#endif // _M_IX86