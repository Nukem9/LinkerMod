#pragma once

struct vec4
{
	union
	{
		float data[4];
		struct
		{
			float x;
			float y;
			float z;
			float w;
		};
	};
};

typedef int(__cdecl* R_HW_SetPixelShaderConstant_t)(void* device, int dest, void* data, int rowCount);
extern R_HW_SetPixelShaderConstant_t R_HW_SetPixelShaderConstant;

void mfh_R_SetPassPixelShaderStableArguments();

void R_CalculateCodeConst_LightConeControl1(float roundness, vec4* dest);
void R_CalculateCodeConst_LightConeControl2(vec4* aAbB, vec4* dest);
void R_CalculateCodeConst_LightFallOffA(vec4* aAbB, vec4* fallOff, vec4* dest);
void R_CalculateCodeConst_LightFallOffB(vec4* aAbB, vec4* fallOff, vec4* dest);
