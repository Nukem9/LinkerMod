#pragma once

typedef int(__cdecl* R_HW_SetPixelShaderConstant_t)(void* device, int dest, void* data, int rowCount);
extern R_HW_SetPixelShaderConstant_t R_HW_SetPixelShaderConstant;

void mfh_R_SetPassPixelShaderStableArguments();
