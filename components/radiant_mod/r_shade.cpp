#include "stdafx.h"

float null[4] = { 0.0, 0.0, 0.0, 0.0 };
float not_null[4] = { 1.0, 1.0, 1.0, 1.0 };

float hdrControl0[4] = { 1.0, 0.0, 0.0, 1.0 };
float skyColorMultiplier[4] = { 1.0, 0.0, 0.0, 1.0 };

float red[4] = { 1.0, 0.0, 0.0, 1.0 };
float green[4] = { 0.0, 1.0, 0.0, 1.0 };
float blue[4] = { 0.0, 0.0, 1.0, 1.0 };

R_HW_SetPixelShaderConstant_t R_HW_SetPixelShaderConstant = (R_HW_SetPixelShaderConstant_t)0x0053EE00;

int __cdecl R_HW_RemapPixelShaderConstant(void* device, MaterialShaderArgument* arg)
{
	//todo -> create a full jump table

	if (arg->u.codeConst.index == 30) //colorMatrixR
	{
		R_HW_SetPixelShaderConstant(device, arg->dest, red, 1);
		return 1;
	}
	else if (arg->u.codeConst.index == 31) //colorMatrixG
	{
		R_HW_SetPixelShaderConstant(device, arg->dest, green, 1);
		return 1;
	}
	else if (arg->u.codeConst.index == 32) //colorMatrixB
	{
		R_HW_SetPixelShaderConstant(device, arg->dest, blue, 1);
		return 1;
	}

	if (arg->u.codeConst.index < 150)
		return 0;

	switch (arg->u.codeConst.index)
	{
		case 170: // hdrControl0
			R_HW_SetPixelShaderConstant(device, arg->dest, hdrControl0, 1);
			return 1;
		case 171: //skyColorMultiplier
			R_HW_SetPixelShaderConstant(device, arg->dest, skyColorMultiplier, 1);
			return 1;

		case 150: // lightAttenuation
			R_HW_SetPixelShaderConstant(device, arg->dest, &g_lightInfo.attenuation, 1);
			return 1;
		case 151: // lightConeControl1
			R_HW_SetPixelShaderConstant(device, arg->dest, &g_lightInfo.coneControl1, 1);
			return 1;
		case 152: // lightConeControl2
			R_HW_SetPixelShaderConstant(device, arg->dest, &g_lightInfo.coneControl2, 1);
			return 1;
		case 153: // lightFallOffA
			R_HW_SetPixelShaderConstant(device, arg->dest, &g_lightInfo.fallOffA, 1);
			return 1;
		case 154: // lightFallOffB
			R_HW_SetPixelShaderConstant(device, arg->dest, &g_lightInfo.fallOffB, 1);
			return 1;
		case 156: // lightSpotCookieSlideControl
			R_HW_SetPixelShaderConstant(device, arg->dest, &g_lightInfo.spotCookieControl, 1);
			return 1;
		case 155: // lightSpotAABB
			R_HW_SetPixelShaderConstant(device, arg->dest, &g_lightInfo.aAbB, 1);
			return 1;
		case 157: // lightSpotMatrix0
			R_HW_SetPixelShaderConstant(device, arg->dest, &g_lightInfo.spotMatrix[0], 1);
			return 1;
		case 158: // lightSpotMatrix1
			R_HW_SetPixelShaderConstant(device, arg->dest, &g_lightInfo.spotMatrix[4], 1);
			return 1;
		case 159: // lightSpotMatrix2
			R_HW_SetPixelShaderConstant(device, arg->dest, &g_lightInfo.spotMatrix[8], 1);
			return 1;
		case 160: // lightSpotMatrix3
			R_HW_SetPixelShaderConstant(device, arg->dest, &g_lightInfo.spotMatrix[12], 1);
			return 1;
		case 161: // lightSpotFactors
			R_HW_SetPixelShaderConstant(device, arg->dest, &g_lightInfo.spotFactors, 1);
			return 1;
	}

	return 0;
}
void * rtn2_R_SetPassPixelShaderStableArguments = (void*)0x0053F674;
void* rtn_R_SetPassPixelShaderStableArguments = (void*)0x0053F62C;
void __declspec(naked) mfh_R_SetPassPixelShaderStableArguments() //0x0053F625
{
	_asm
	{
		mov[esp + 14h], eax
		mov eax, [edi + 4] // context->state

		pushad
		push esi // arg
		mov ecx, [eax + 90h]
		push ecx // device

		call R_HW_RemapPixelShaderConstant
		add esp, 8
		cmp eax, 0
		jne SKIP

		popad
		jmp rtn_R_SetPassPixelShaderStableArguments

	SKIP:
		popad
		jmp rtn2_R_SetPassPixelShaderStableArguments
	}
}

void R_CalculateCodeConst_LightConeControl1(float roundness, vec4* dest)
{
	if (roundness == 0.0f)
		dest->x = 1.0f;
	else
		dest->x = 2.0f / roundness;

	dest->y = -1.0f / dest->x;
	dest->z = 1.0f;
	dest->w = roundness;
}

void R_CalculateCodeConst_LightConeControl2(vec4* aAbB, vec4* dest)
{
	dest->x = aAbB->x * aAbB->z;
	dest->y = aAbB->y * aAbB->w;
	dest->z = -2.0f;
	dest->w = 3.0f;
}

void R_CalculateCodeConst_LightFallOffA(vec4* aAbB, vec4* fallOff, vec4* dest)
{
	if (fallOff->z == fallOff->x)
		dest->x = 1.0f;
	else
		dest->x = 1.0f / (fallOff->z - fallOff->x);

	dest->y = 1.0f / (aAbB->z - aAbB->w);
	dest->z = 1.0f / (aAbB->x - aAbB->y);

	if (fallOff->y == fallOff->w)
		dest->w = -1.0f;
	else
		dest->w = 1.0f / (fallOff->w - fallOff->y);
}

void R_CalculateCodeConst_LightFallOffB(vec4* aAbB, vec4* fallOff, vec4* dest)
{
	R_CalculateCodeConst_LightFallOffA(aAbB, fallOff, dest);

	dest->x *= -fallOff->x;
	dest->y *= -aAbB->w;
	dest->z *= -aAbB->y;

	dest->w *= -fallOff->y;
}
