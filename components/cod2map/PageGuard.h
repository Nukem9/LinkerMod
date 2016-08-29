#pragma once

void PageGuard_Monitor(ULONG_PTR VirtualAddress, ULONG Size);
LONG PageGuard_Check(PEXCEPTION_POINTERS ExceptionInfo);

DWORD Scan(ULONG_PTR Address);
void REMAP(ULONG_PTR Address, ULONG_PTR Target, ULONG_PTR New);