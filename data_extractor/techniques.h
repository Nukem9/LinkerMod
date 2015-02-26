#include <Windows.h>

char* Material_TechniqueTypeForName(int techniqueType)
{
  char *techniqueNames[130];

  techniqueNames[0] = "\"depth prepass\"";
  techniqueNames[1] = "\"build floatz\"";
  techniqueNames[2] = "\"build shadowmap depth\"";
  techniqueNames[3] = "\"build shadowmap color\"";
  techniqueNames[4] = "\"unlit\"";
  techniqueNames[5] = "\"emissive\"";
  techniqueNames[6] = "\"emissive shadow\"";
  techniqueNames[7] = "\"emissive nvintz\"";
  techniqueNames[8] = "\"emissive shadow nvintz\"";
  techniqueNames[9] = "\"emissive reflected\"";
  techniqueNames[10] = "\"lit\"";
  techniqueNames[11] = "\"lit sun\"";
  techniqueNames[12] = "\"lit sun shadow\"";
  techniqueNames[13] = "\"lit spot\"";
  techniqueNames[14] = "\"lit spot shadow\"";
  techniqueNames[15] = "\"lit omni\"";
  techniqueNames[16] = "\"lit omni shadow\"";
  techniqueNames[17] = "\"lit dlight\"";
  techniqueNames[18] = "\"lit sun dlight\"";
  techniqueNames[19] = "\"lit sun shadow dlight\"";
  techniqueNames[20] = "\"lit spot dlight\"";
  techniqueNames[21] = "\"lit spot shadow dlight\"";
  techniqueNames[22] = "\"lit omni dlight\"";
  techniqueNames[23] = "\"lit omni shadow dlight\"";
  techniqueNames[24] = "\"lit glight\"";
  techniqueNames[25] = "\"lit sun glight\"";
  techniqueNames[26] = "\"lit sun shadow glight\"";
  techniqueNames[27] = "\"lit spot glight\"";
  techniqueNames[28] = "\"lit spot shadow glight\"";
  techniqueNames[29] = "\"lit omni glight\"";
  techniqueNames[30] = "\"lit omni shadow glight\"";
  techniqueNames[31] = "\"lit dlight glight\"";
  techniqueNames[32] = "\"lit sun dlight glight\"";
  techniqueNames[33] = "\"lit sun shadow dlight glight\"";
  techniqueNames[34] = "\"lit spot dlight glight\"";
  techniqueNames[35] = "\"lit spot shadow dlight glight\"";
  techniqueNames[36] = "\"lit omni dlight glight\"";
  techniqueNames[37] = "\"lit omni shadow dlight glight\"";
  techniqueNames[38] = "\"lit alpha\"";
  techniqueNames[39] = "\"lit sun alpha\"";
  techniqueNames[40] = "\"lit sun shadow alpha\"";
  techniqueNames[41] = "\"lit spot alpha\"";
  techniqueNames[42] = "\"lit spot shadow alpha\"";
  techniqueNames[43] = "\"lit omni alpha\"";
  techniqueNames[44] = "\"lit omni shadow alpha\"";
  techniqueNames[45] = "\"lit remap\"";
  techniqueNames[46] = "\"lit sun remap\"";
  techniqueNames[47] = "\"lit sun shadow remap\"";
  techniqueNames[48] = "\"lit spot remap\"";
  techniqueNames[49] = "\"lit spot shadow remap\"";
  techniqueNames[50] = "\"lit omni remap\"";
  techniqueNames[51] = "\"lit omni shadow remap\"";
  techniqueNames[52] = "\"lit fade\"";
  techniqueNames[53] = "\"lit sun fade\"";
  techniqueNames[54] = "\"lit sun shadow fade\"";
  techniqueNames[55] = "\"lit spot fade\"";
  techniqueNames[56] = "\"lit spot shadow fade\"";
  techniqueNames[57] = "\"lit omni fade\"";
  techniqueNames[58] = "\"lit omni shadow fade\"";
  techniqueNames[59] = "\"lit charred\"";
  techniqueNames[60] = "\"lit fade charred\"";
  techniqueNames[61] = "\"lit sun charred\"";
  techniqueNames[62] = "\"lit sun fade charred\"";
  techniqueNames[63] = "\"lit sun shadow charred\"";
  techniqueNames[64] = "\"lit sun shadow fade charred\"";
  techniqueNames[65] = "\"lit spot charred\"";
  techniqueNames[66] = "\"lit spot fade charred\"";
  techniqueNames[67] = "\"lit spot shadow charred\"";
  techniqueNames[68] = "\"lit spot shadow fade charred\"";
  techniqueNames[69] = "\"lit omni charred\"";
  techniqueNames[70] = "\"lit omni fade charred\"";
  techniqueNames[71] = "\"lit omni shadow charred\"";
  techniqueNames[72] = "\"lit omni shadow fade charred\"";
  techniqueNames[73] = "\"lit instanced\"";
  techniqueNames[74] = "\"lit instanced sun\"";
  techniqueNames[75] = "\"lit instanced sun shadow\"";
  techniqueNames[76] = "\"lit instanced spot\"";
  techniqueNames[77] = "\"lit instanced spot shadow\"";
  techniqueNames[78] = "\"lit instanced omni\"";
  techniqueNames[79] = "\"lit instanced omni shadow\"";
  techniqueNames[80] = "\"lit nvintz\"";
  techniqueNames[81] = "\"lit sun nvintz\"";
  techniqueNames[82] = "\"lit sun shadow nvintz\"";
  techniqueNames[83] = "\"lit spot nvintz\"";
  techniqueNames[84] = "\"lit spot shadow nvintz\"";
  techniqueNames[85] = "\"lit omni nvintz\"";
  techniqueNames[86] = "\"lit omni shadow nvintz\"";
  techniqueNames[87] = "\"lit dlight nvintz\"";
  techniqueNames[88] = "\"lit sun dlight nvintz\"";
  techniqueNames[89] = "\"lit sun shadow dlight nvintz\"";
  techniqueNames[90] = "\"lit spot dlight nvintz\"";
  techniqueNames[91] = "\"lit spot shadow dlight nvintz\"";
  techniqueNames[92] = "\"lit omni dlight nvintz\"";
  techniqueNames[93] = "\"lit omni shadow dlight nvintz\"";
  techniqueNames[94] = "\"lit glight nvintz\"";
  techniqueNames[95] = "\"lit sun glight nvintz\"";
  techniqueNames[96] = "\"lit sun shadow glight nvintz\"";
  techniqueNames[97] = "\"lit spot glight nvintz\"";
  techniqueNames[98] = "\"lit spot shadow glight nvintz\"";
  techniqueNames[99] = "\"lit omni glight nvintz\"";
  techniqueNames[100] = "\"lit omni shadow glight nvintz\"";
  techniqueNames[101] = "\"lit dlight glight nvintz\"";
  techniqueNames[102] = "\"lit sun dlight glight nvintz\"";
  techniqueNames[103] = "\"lit sun shadow dlight glight nvintz\"";
  techniqueNames[104] = "\"lit spot dlight glight nvintz\"";
  techniqueNames[105] = "\"lit spot shadow dlight glight nvintz\"";
  techniqueNames[106] = "\"lit omni dlight glight nvintz\"";
  techniqueNames[107] = "\"lit omni shadow dlight glight nvintz\"";
  techniqueNames[108] = "\"lit instanced nvintz\"";
  techniqueNames[109] = "\"lit instanced sun nvintz\"";
  techniqueNames[110] = "\"lit instanced sun shadow nvintz\"";
  techniqueNames[111] = "\"lit instanced spot nvintz\"";
  techniqueNames[112] = "\"lit instanced spot shadow nvintz\"";
  techniqueNames[113] = "\"lit instanced omni nvintz\"";
  techniqueNames[114] = "\"lit instanced omni shadow nvintz\"";
  techniqueNames[115] = "\"light spot\"";
  techniqueNames[116] = "\"light omni\"";
  techniqueNames[117] = "\"light spot shadow\"";
  techniqueNames[118] = "\"light spot charred\"";
  techniqueNames[119] = "\"light omni charred\"";
  techniqueNames[120] = "\"light spot shadow charred\"";
  techniqueNames[121] = "\"fakelight normal\"";
  techniqueNames[122] = "\"fakelight view\"";
  techniqueNames[123] = "\"sunlight preview\"";
  techniqueNames[124] = "\"case texture\"";
  techniqueNames[125] = "\"solid wireframe\"";
  techniqueNames[126] = "\"shaded wireframe\"";
  techniqueNames[127] = "\"debug bumpmap\"";
  techniqueNames[128] = "\"debug bumpmap instanced\"";
  techniqueNames[129] = "\"impact mask\"";
  
  return techniqueNames[techniqueType];
}


char* Technique_SourceToString(char source)
{
	char* sourceStrings[10];
	sourceStrings[0] = "position";
	sourceStrings[1] = "color";
	sourceStrings[2] = "texcoord[0]";
	sourceStrings[3] = "normal";
	sourceStrings[4] = "tangent";
	sourceStrings[5] = "texcoord[1]";
	sourceStrings[6] = "texcoord[2]";
	sourceStrings[7] = "normalTransform[0]";
	sourceStrings[8] = "normalTransform[1]";
	sourceStrings[9] = "blendweight";

	return sourceStrings[source];
}

char* Technique_DestToString(char dest)
{
	char* destStrings[20];

	destStrings[0] = "position";
	destStrings[1] = "normal";
	destStrings[2] = "color[0]";
	destStrings[3] = "color[1]";
	destStrings[4] = "depth";
	destStrings[5] = "texcoord[0]";
	destStrings[6] = "texcoord[1]";
	destStrings[7] = "texcoord[2]";
	destStrings[8] = "texcoord[3]";
	destStrings[9] = "texcoord[4]";
	destStrings[10] = "texcoord[5]";
	destStrings[11] = "texcoord[6]";
	destStrings[12] = "texcoord[7]";
	destStrings[13] = "texcoord[8]";
	destStrings[14] = "texcoord[9]";
	destStrings[15] = "texcoord[10]";
	destStrings[16] = "texcoord[11]";
	destStrings[17] = "texcoord[12]";
	destStrings[18] = "texcoord[13]";
	destStrings[19] = "blendweight";

	return destStrings[dest];
}

enum GfxBlend
{
  GFXS_BLEND_DISABLED = 0x0,
  GFXS_BLEND_ZERO = 0x1,
  GFXS_BLEND_ONE = 0x2,
  GFXS_BLEND_SRCCOLOR = 0x3,
  GFXS_BLEND_INVSRCCOLOR = 0x4,
  GFXS_BLEND_SRCALPHA = 0x5,
  GFXS_BLEND_INVSRCALPHA = 0x6,
  GFXS_BLEND_DESTALPHA = 0x7,
  GFXS_BLEND_INVDESTALPHA = 0x8,
  GFXS_BLEND_DESTCOLOR = 0x9,
  GFXS_BLEND_INVDESTCOLOR = 0xA,
  GFXS_BLEND_MASK = 0xF,
};


enum GfxBlendOp
{
  GFXS_BLENDOP_DISABLED = 0x0,
  GFXS_BLENDOP_ADD = 0x1,
  GFXS_BLENDOP_SUBTRACT = 0x2,
  GFXS_BLENDOP_REVSUBTRACT = 0x3,
  GFXS_BLENDOP_MIN = 0x4,
  GFXS_BLENDOP_MAX = 0x5,
  GFXS_BLENDOP_MASK = 0x7,
};

enum $B8A254328D12B632EF29D90876D26447
{
  GFXS0_SRCBLEND_RGB_SHIFT = 0x0,
  GFXS0_SRCBLEND_RGB_MASK = 0xF,
  GFXS0_DSTBLEND_RGB_SHIFT = 0x4,
  GFXS0_DSTBLEND_RGB_MASK = 0xF0,
  GFXS0_BLENDOP_RGB_SHIFT = 0x8,
  GFXS0_BLENDOP_RGB_MASK = 0x700,
  GFXS0_BLEND_RGB_MASK = 0x7FF,
  GFXS0_ATEST_DISABLE = 0x800,
  GFXS0_ATEST_GT_0 = 0x1000,
  GFXS0_ATEST_GE_255 = 0x2000,
  GFXS0_ATEST_GE_128 = 0x3000,
  GFXS0_ATEST_MASK = 0x3000,
  GFXS0_CULL_SHIFT = 0xE,
  GFXS0_CULL_NONE = 0x4000,
  GFXS0_CULL_BACK = 0x8000,
  GFXS0_CULL_FRONT = 0xC000,
  GFXS0_CULL_MASK = 0xC000,
  GFXS0_SRCBLEND_ALPHA_SHIFT = 0x10,
  GFXS0_SRCBLEND_ALPHA_MASK = 0xF0000,
  GFXS0_DSTBLEND_ALPHA_SHIFT = 0x14,
  GFXS0_DSTBLEND_ALPHA_MASK = 0xF00000,
  GFXS0_BLENDOP_ALPHA_SHIFT = 0x18,
  GFXS0_BLENDOP_ALPHA_MASK = 0x7000000,
  GFXS0_BLEND_ALPHA_MASK = 0x7FF0000,
  GFXS0_COLORWRITE_RGB = 0x8000000,
  GFXS0_COLORWRITE_ALPHA = 0x10000000,
  GFXS0_COLORWRITE_MASK = 0x18000000,
  GFXS0_POLYMODE_LINE = 0x80000000,
  GFXS1_DEPTHWRITE = 0x1,
  GFXS1_DEPTHTEST_DISABLE = 0x2,
  GFXS1_DEPTHTEST_SHIFT = 0x2,
  GFXS1_DEPTHTEST_ALWAYS = 0x0,
  GFXS1_DEPTHTEST_LESS = 0x4,
  GFXS1_DEPTHTEST_EQUAL = 0x8,
  GFXS1_DEPTHTEST_LESSEQUAL = 0xC,
  GFXS1_DEPTHTEST_MASK = 0xC,
  GFXS1_DEPTHFUNC_MASK = 0xF,
  GFXS1_POLYGON_OFFSET_SHIFT = 0x4,
  GFXS1_POLYGON_OFFSET_0 = 0x0,
  GFXS1_POLYGON_OFFSET_1 = 0x10,
  GFXS1_POLYGON_OFFSET_2 = 0x20,
  GFXS1_POLYGON_OFFSET_SHADOWMAP = 0x30,
  GFXS1_POLYGON_OFFSET_MASK = 0x30,
  GFXS1_STENCIL_FRONT_ENABLE = 0x40,
  GFXS1_STENCIL_BACK_ENABLE = 0x80,
  GFXS1_STENCIL_MASK = 0xC0,
  GFXS1_STENCIL_FRONT_PASS_SHIFT = 0x8,
  GFXS1_STENCIL_FRONT_FAIL_SHIFT = 0xB,
  GFXS1_STENCIL_FRONT_ZFAIL_SHIFT = 0xE,
  GFXS1_STENCIL_FRONT_FUNC_SHIFT = 0x11,
  GFXS1_STENCIL_FRONT_MASK = 0xFFF00,
  GFXS1_STENCIL_BACK_PASS_SHIFT = 0x14,
  GFXS1_STENCIL_BACK_FAIL_SHIFT = 0x17,
  GFXS1_STENCIL_BACK_ZFAIL_SHIFT = 0x1A,
  GFXS1_STENCIL_BACK_FUNC_SHIFT = 0x1D,
  GFXS1_STENCIL_BACK_MASK = 0xFFF00000,
  GFXS1_STENCILFUNC_FRONTBACK_MASK = 0xE00E0000,
  GFXS1_STENCILOP_FRONTBACK_MASK = 0x1FF1FF00,
};

enum GfxTextureOp
{
  GFXS_TEXOP_DISABLE = 0x0,
  GFXS_TEXOP_SELECTARG1 = 0x1,
  GFXS_TEXOP_SELECTARG2 = 0x2,
  GFXS_TEXOP_MODULATE = 0x3,
  GFXS_TEXOP_MODULATE2X = 0x4,
  GFXS_TEXOP_MODULATE4X = 0x5,
  GFXS_TEXOP_ADD = 0x6,
  GFXS_TEXOP_ADDSIGNED = 0x7,
  GFXS_TEXOP_ADDSIGNED2X = 0x8,
  GFXS_TEXOP_SUBTRACT = 0x9,
  GFXS_TEXOP_ADDSMOOTH = 0xA,
  GFXS_TEXOP_BLENDDIFFUSEALPHA = 0xB,
  GFXS_TEXOP_BLENDTEXTUREALPHA = 0xC,
  GFXS_TEXOP_BLENDFACTORALPHA = 0xD,
  GFXS_TEXOP_BLENDTEXTUREALPHAPM = 0xE,
  GFXS_TEXOP_BLENDCURRENTALPHA = 0xF,
  GFXS_TEXOP_MODULATEALPHA_ADDCOLOR = 0x10,
  GFXS_TEXOP_MODULATECOLOR_ADDALPHA = 0x11,
  GFXS_TEXOP_MODULATEINVALPHA_ADDCOLOR = 0x12,
  GFXS_TEXOP_MODULATEINVCOLOR_ADDALPHA = 0x13,
  GFXS_TEXOP_DOTPRODUCT3 = 0x14,
  GFXS_TEXOP_MULTIPLYADD = 0x15,
  GFXS_TEXOP_LERP = 0x16,
  GFXS_TEXOP_COUNT = 0x17,
  GFXS_TEXOP_BITS = 0x5,
  GFXS_TEXOP_MASK = 0x1F,
};