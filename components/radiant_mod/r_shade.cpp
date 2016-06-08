#include "stdafx.h"

float null[4] = { 0.0, 0.0, 0.0, 0.0 };
float not_null[4] = { 1.0, 1.0, 1.0, 1.0 };

float mat4x4_0[4] = { 1.0, 0.0, 0.0, 0.0 };
float mat4x4_1[4] = { 0.0, 1.0, 0.0, 0.0 };
float mat4x4_2[4] = { 0.0, 0.0, 1.0, 0.0 };
float mat4x4_3[4] = { 0.0, 0.0, 0.0, 1.0 };

float aAbB[4] = { 0.0, 1.0, 0.0, 1.0 };

float attenuation[4] = { 1.0, 0.0, 0.0, 0.0 };

R_HW_SetPixelShaderConstant_t R_HW_SetPixelShaderConstant = (R_HW_SetPixelShaderConstant_t)0x0053EE00;

int __cdecl R_HW_RemapPixelShaderConstant(void* device, MaterialShaderArgument* arg)
{
	if (arg->u.codeConst.index < 150)
		return 0;

	switch (arg->u.codeConst.index)
	{
	case 150: // lightAttenuation
		R_HW_SetPixelShaderConstant(device, arg->dest, attenuation, 1);
		return 1;
	case 151: // lightConeControl1
	case 152: // lightConeControl2
	case 153: // lightFallOffA
	case 154: // lightFallOffB
	case 156: // lightSpotCookieSlideControl
		R_HW_SetPixelShaderConstant(device, arg->dest, not_null, 1);
		return 1;
	case 155: // lightSpotAABB
		R_HW_SetPixelShaderConstant(device, arg->dest, aAbB, 1);
		return 0;
	case 157: // lightSpotMatrix0
		R_HW_SetPixelShaderConstant(device, arg->dest, mat4x4_0, 1);
		return 1;
	case 158: // lightSpotMatrix1
		R_HW_SetPixelShaderConstant(device, arg->dest, mat4x4_1, 1);
		return 1;
	case 159: // lightSpotMatrix2
		R_HW_SetPixelShaderConstant(device, arg->dest, mat4x4_2, 1);
		return 1;
	case 160: // lightSpotMatrix3
		R_HW_SetPixelShaderConstant(device, arg->dest, mat4x4_3, 1);
		return 1;
	}

	return 0;
}

void* rtn_R_SetPassPixelShaderStableArguments = (void*)0x0053F62C;
void __declspec(naked) mfh_R_SetPassPixelShaderStableArguments() //0x0053F625
{
	_asm
	{
		mov [esp + 14h], eax
		mov eax, [edi + 4] // context->state
		
		pushad
		push esi // arg
		mov ecx, [eax + 90h]
		push ecx // device

		call R_HW_RemapPixelShaderConstant
		add esp, 8

		popad

		jmp rtn_R_SetPassPixelShaderStableArguments
	}
}