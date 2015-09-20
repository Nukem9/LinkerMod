#pragma once

const static DWORD* g_lightgridSampleCount = (DWORD*)0x153C91D0;
const static DWORD* g_diskLightgridSampleCount = (DWORD*)0x153C91E0;

typedef float SampleColorHDR[56][3];
typedef WORD DiskSampleColorHDR[56][3];
typedef BYTE DiskSampleColor[56][3];

typedef void(*R_Init_Lightgrid_t)();
extern R_Init_Lightgrid_t o_R_Init_Lightgrid;

typedef void(*R_Store_QuantizedLightGridSample_t)();
extern R_Store_QuantizedLightGridSample_t o_R_Store_QuantizedLightGridSample;

void mfh_R_Init_Lightgrid();
void mfh_R_Store_LightgridSample();
void mfh_R_Alloc_DiskLightGridColors();
void hk_R_Store_QuantizedLightGridSample();