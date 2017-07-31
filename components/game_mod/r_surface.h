#pragma once

struct srfTriangles_t
{
	float mins[3];
	int vertexLayerData;
	float maxs[3];
	int firstVertex;
	unsigned __int16 vertexCount;
	unsigned __int16 triCount;
	int baseIndex;
	float himipRadiusSq;
	int stream2ByteOffset;
};

struct GfxSurface
{
	srfTriangles_t tris;
	Material *material;
	char lightmapIndex;
	char reflectionProbeIndex;
	char primaryLightIndex;
	char flags;
	float bounds[2][3];
};
STATIC_ASSERT_OFFSET(GfxSurface, material, 0x30);
STATIC_ASSERT_SIZE(GfxSurface, 0x50);