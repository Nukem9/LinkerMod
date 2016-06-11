#include "stdafx.h"

float null[4] = { 0.0, 0.0, 0.0, 0.0 };
float not_null[4] = { 1.0, 1.0, 1.0, 1.0 };

float hdrControl0[4] = { 1.0, 0.0, 0.0, 1.0 };
float skyColorMultiplier[4] = { 1.0, 0.0, 0.0, 1.0 };

float red[4] = { 1.0, 0.0, 0.0, 1.0 };
float green[4] = { 0.0, 1.0, 0.0, 1.0 };
float blue[4] = { 0.0, 0.0, 1.0, 1.0 };

float aAbB[4] = { 0.0, 1.0, 0.0, 1.0 };
float attenuation[4] = { 1.0, 0.0, 0.0, 0.0 };

float lightSpotMatrix0[4] = { 1.0, 0.0, 0.0, 0.0 };
float lightSpotMatrix1[4] = { 0.0, 1.0, 0.0, 0.0 };
float lightSpotMatrix2[4] = { 0.0, 0.0, 1.0, 0.0 };
float lightSpotMatrix3[4] = { 0.0, 0.0, 0.0, 1.0 };

vec4 lightConeControl1 = { 0.0, 0.0, 0.0, 0.0 };
vec4 lightConeControl2 = { 0.0, 0.0, 0.0, 0.0 };
vec4 lightFallOffA = { 0.0, 0.0, 0.0, 0.0 };
vec4 lightFallOffB = { 0.0, 0.0, 0.0, 0.0 };

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

	if (R_GetCurrentLight() && R_GetCurrentLight()->type == 2)
	{
		GfxLight* light = R_GetCurrentLight();
		float roundness = 1.0;

		float cutOn = 0.0f;
		float nearEdge = 0.0f;
		float farEdge = 0.0f;

		vec4 fallOff;
		fallOff.x = 0.0f;	//cutOn;
		fallOff.y = 200.0f;	//light->radius;
		fallOff.z = 0.0f;	//((light->radius - cutOn) * nearEdge) + cutOn;
		fallOff.w = 0.0f;	//((light->radius - cutOn) * farEdge) + cutOn;

		R_CalculateCodeConst_LightConeControl1(roundness, &lightConeControl1);
		R_CalculateCodeConst_LightConeControl2((vec4*)aAbB, &lightConeControl2);
		R_CalculateCodeConst_LightFallOffA((vec4*)aAbB, &fallOff, &lightFallOffA);
		R_CalculateCodeConst_LightFallOffB((vec4*)aAbB, &fallOff, &lightFallOffB);

		vec4 angles = { 0.0f, 0.0f, 0.0f, 0.0f };
		memcpy(&angles, light->angles, sizeof(float) * 3);
		
		float viewMatrix[16];
		SpotLightViewMatrix((float*)&angles, 0.0f, viewMatrix);

		float projMatrix[16];
		SpotLightProjectionMatrix(light->cosHalfFovOuter, fallOff.x, light->radius, projMatrix);

		float spotMatrixTransposed[16];
		MatrixMultiply44(viewMatrix, projMatrix, spotMatrixTransposed);
		
		float spotMatrix[16];
		MatrixTranspose44(spotMatrixTransposed, spotMatrix);
		
		memcpy(lightSpotMatrix0, &spotMatrix[0], sizeof(float) * 4);
		memcpy(lightSpotMatrix1, &spotMatrix[4], sizeof(float) * 4);
		memcpy(lightSpotMatrix2, &spotMatrix[8], sizeof(float) * 4);
		memcpy(lightSpotMatrix3, &spotMatrix[12], sizeof(float) * 4);
	}

	switch (arg->u.codeConst.index)
	{
		case 170: // hdrControl0
			R_HW_SetPixelShaderConstant(device, arg->dest, hdrControl0, 1);
			return 1;
		case 171: //skyColorMultiplier
			R_HW_SetPixelShaderConstant(device, arg->dest, skyColorMultiplier, 1);
			return 1;

		case 150: // lightAttenuation
			R_HW_SetPixelShaderConstant(device, arg->dest, attenuation, 1);
			return 1;
		case 151: // lightConeControl1
			R_HW_SetPixelShaderConstant(device, arg->dest, &lightConeControl1, 1);
			return 1;
		case 152: // lightConeControl2
			R_HW_SetPixelShaderConstant(device, arg->dest, &lightConeControl2, 1);
			return 1;
		case 153: // lightFallOffA
			R_HW_SetPixelShaderConstant(device, arg->dest, &lightFallOffA, 1);
			return 1;
		case 154: // lightFallOffB
			R_HW_SetPixelShaderConstant(device, arg->dest, &lightFallOffB, 1);
			return 1;
		case 156: // lightSpotCookieSlideControl
			R_HW_SetPixelShaderConstant(device, arg->dest, not_null, 1);
			return 1;
		case 155: // lightSpotAABB
			R_HW_SetPixelShaderConstant(device, arg->dest, aAbB, 1);
			return 1;
		case 157: // lightSpotMatrix0
			R_HW_SetPixelShaderConstant(device, arg->dest, lightSpotMatrix0, 1);
			return 1;
		case 158: // lightSpotMatrix1
			R_HW_SetPixelShaderConstant(device, arg->dest, lightSpotMatrix1, 1);
			return 1;
		case 159: // lightSpotMatrix2
			R_HW_SetPixelShaderConstant(device, arg->dest, lightSpotMatrix2, 1);
			return 1;
		case 160: // lightSpotMatrix3
			R_HW_SetPixelShaderConstant(device, arg->dest, lightSpotMatrix3, 1);
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

		SKIP :
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
