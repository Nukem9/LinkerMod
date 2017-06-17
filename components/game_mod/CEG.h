#pragma once

void Patch_CEG();

void hk_inline_memcpy();
void hk_inline_memcpy2();
void *hk_memcpy(void *dest, const void *src, size_t size);

void *sub_8EE640(void *Nullptr1, void *Nullptr2);

DWORD sub_9A2860(PEXCEPTION_RECORD a1, int a2, PCONTEXT a3);
DWORD sub_9A2800(PEXCEPTION_RECORD a1, int a2, PCONTEXT a3);
DWORD sub_9A2900(PEXCEPTION_RECORD a1, int a2, PCONTEXT a3);

void __fastcall sub_5CBF00(void *thisptr, PVOID _EDX, DWORD address, size_t scanSize);
void __fastcall sub_661450(void *thisptr, PVOID _EDX, DWORD address, size_t scanSize);
void __fastcall sub_4E9880(void *thisptr, PVOID _EDX, DWORD address, size_t scanSize);
void __fastcall sub_640020(void *thisptr, PVOID _EDX, DWORD address, size_t scanSize);
void __fastcall sub_51A140(void *thisptr, PVOID _EDX, DWORD address, size_t scanSize);
void __fastcall sub_54A430(void *thisptr, PVOID _EDX, DWORD address, size_t scanSize);